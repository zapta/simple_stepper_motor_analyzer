#include "mk3_tft_driver.h"

#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "io.h"
#include "mk3_lookup_tables.h"
#include "mk3_tft.pio.h"
#include "pico/stdlib.h"

// Plain GPIO output pins.
#define TFT_RST_PIN 1  // Active low.
#define TFT_DC_PIN  2  // 1: data, 0: command.
#define TFT_BL_PIN 15  // Active high

// Outputs managed by PioTft.
#define TFT_D0_PIN 6   // First of 8 data pins, [GPIO_6, GPIO_13]
#define TFT_WR_PIN 28  // Active low

#define TFT_RST_HIGH gpio_set_mask(1ul << TFT_RST_PIN)
#define TFT_DC_HIGH gpio_set_mask(1ul << TFT_DC_PIN)
#define TFT_BL_HIGH gpio_set_mask(1ul << TFT_BL_PIN)

#define TFT_RST_LOW gpio_clr_mask(1ul << TFT_RST_PIN)
#define TFT_DC_LOW gpio_clr_mask(1ul << TFT_DC_PIN)
#define TFT_BL_LOW gpio_clr_mask(1ul << TFT_BL_PIN)

// // Assuming landscape mode per memory access command 0x36.
#define WIDTH 480
#define HEIGHT 320

#define ILI9488_SLPOUT 0x11

#define ILI9488_DISPON 0x29

#define ILI9488_CASET 0x2A
#define ILI9488_PASET 0x2B
#define ILI9488_RAMWR 0x2C
#define ILI9488_RAMRD 0x2E

// namespace tft_driver {

constexpr uint16_t PIO_CLOCK_DIV = 1;

#define PIO (pio0)
#define SM 0

// namespace pio_tft {

// This is a trickey thing. It provides a mask for pio->flevel register
// to test if the TX Fifo of sm0 has atleast 5 free words or not. It
// is done by testing that the four bits of fifo level has the pattern
// 00xx, which means no more than 3 occupied entries in the 8 entries
// TX buffer.
static constexpr uint32_t SM_FLEVEL_FREE_5_MASK = 0x000c << (SM * 8);

// Masks for the FDEBUG register.
static constexpr uint32_t SM_STALL_MASK = 1u << (PIO_FDEBUG_TXSTALL_LSB + SM);
static constexpr uint32_t SM_OVERRUN_MASK = 1u << (PIO_FDEBUG_TXOVER_LSB + SM);

// Updated later with the loading offset of the PIO program.
uint program_offset = 0;

static void init_gpio() {
  // A mask with all gpio output pins we use.
  constexpr uint kOutputMask =
      1ul << TFT_RST_PIN | 1ul << TFT_DC_PIN | 1ul << TFT_BL_PIN;

  gpio_init_mask(kOutputMask);

  // Start with backlight non active, efore we set it as an
  // output, to avoid startup flicker.
  TFT_BL_LOW;

  gpio_set_dir_out_masked(kOutputMask);
}

static void init_pio() {
  // Make sure nobody else uses this state machine.
  pio_sm_claim(PIO, SM);

  // Load the PIO program. Starting by default with 16 bits mode
  // since it's at the begining of the PIO program.
  program_offset = pio_add_program(PIO, &tft_io_program);

  // Associate pins with the PIO.
  pio_gpio_init(PIO, TFT_WR_PIN);
  for (int i = 0; i < 8; i++) {
    pio_gpio_init(PIO, TFT_D0_PIN + i);
  }

  // Configure the pins to be outputs.
  pio_sm_set_consecutive_pindirs(PIO, SM, TFT_WR_PIN, 1, true);
  pio_sm_set_consecutive_pindirs(PIO, SM, TFT_D0_PIN, 8, true);

  // Configure the state machine.
  pio_sm_config c = tft_io_program_get_default_config(program_offset);
  // The pio program declares that a single sideset pin is used.
  // Define it.
  sm_config_set_sideset_pins(&c, TFT_WR_PIN);
  // The 8 consecutive pins that are used for data outout.
  sm_config_set_out_pins(&c, TFT_D0_PIN, 8);
  // Set clock divider. Value of 1 for max speed.
  sm_config_set_clkdiv_int_frac(&c, PIO_CLOCK_DIV, 0);
  // Make a single 8 words FIFO from the 4 words TX and RX FIFOs.
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
  // The OSR register shifts to the right, sending the MSB byte
  // first, in a double bytes transfers.
  sm_config_set_out_shift(&c, true, false, 0);
  // Set the SM with the configuration we constructed above.
  // Default mode is single byte.
  pio_sm_init(PIO, SM, program_offset + tft_io_offset_start_8, &c);

  // Start the state machine.
  pio_sm_set_enabled(PIO, SM, true);
}

static void flush() {
  // Clear the sticky stall status.
  PIO->fdebug = SM_STALL_MASK;
  // Wait until the stall flag is up again.
  while (!(PIO->fdebug & SM_STALL_MASK)) {
  }
}

static void set_mode_single_byte() {
  flush();
  // Force a SM jump.
  pio_sm_exec(PIO, SM, pio_encode_jmp(program_offset + tft_io_offset_start_8));
}

static void set_mode_double_byte() {
  flush();
  // Force a SM jump.
  pio_sm_exec(PIO, SM, pio_encode_jmp(program_offset + tft_io_offset_start_16));
}

// For testing.
static bool is_overrun() {
  const bool result = PIO->fdebug & SM_OVERRUN_MASK;
  // Clear for next time.
  PIO->fdebug = SM_OVERRUN_MASK;
  return result;
}
static void write(uint16_t value) { pio_sm_put_blocking(PIO, SM, value); }

inline void write_command_byte(uint8_t c) {
  // This also flushes any pending writes.
  set_mode_single_byte();
  TFT_DC_LOW;
  write(c);
  // Prepear for data bytes that will follow
  flush();
  TFT_DC_HIGH;
}

// Assuming already in 8 bits data mode.
inline void write_data_byte(uint8_t c) {
  write(c);
  // No need to flush. Ok to data bytes being queued.
}

void Mk3TftDriver::begin() {
  init_gpio();
  init_pio();

  // pio_tft::begin();

  sleep_ms(5);
  TFT_RST_LOW;
  sleep_ms(20);
  TFT_RST_HIGH;
  sleep_ms(150);

  write_command_byte(0xE0);
  write_data_byte(0x00);
  write_data_byte(0x03);
  write_data_byte(0x09);
  write_data_byte(0x08);
  write_data_byte(0x16);
  write_data_byte(0x0A);
  write_data_byte(0x3F);
  write_data_byte(0x78);
  write_data_byte(0x4C);
  write_data_byte(0x09);
  write_data_byte(0x0A);
  write_data_byte(0x08);
  write_data_byte(0x16);
  write_data_byte(0x1A);
  write_data_byte(0x0F);

  write_command_byte(0XE1);
  write_data_byte(0x00);
  write_data_byte(0x16);
  write_data_byte(0x19);
  write_data_byte(0x03);
  write_data_byte(0x0F);
  write_data_byte(0x05);
  write_data_byte(0x32);
  write_data_byte(0x45);
  write_data_byte(0x46);
  write_data_byte(0x04);
  write_data_byte(0x0E);
  write_data_byte(0x0D);
  write_data_byte(0x35);
  write_data_byte(0x37);
  write_data_byte(0x0F);

  write_command_byte(0XC0);  // Power Control 1
  write_data_byte(0x17);     // Vreg1out
  write_data_byte(0x15);     // Verg2out

  write_command_byte(0xC1);  // Power Control 2
  write_data_byte(0x41);     // VGH,VGL

  write_command_byte(0xC5);  // Power Control 3
  write_data_byte(0x00);
  write_data_byte(0x12);  // Vcom
  write_data_byte(0x80);

  write_command_byte(0x36);  // Memory Access
  write_data_byte(0xe8);     // landscape mode. Swapping and mirroring x, y.

  // NOTE: For 16bit parallel transfer, IM jumpers need to be set
  // as 010.
  write_command_byte(0x3A);  // Interface Pixel Format
  write_data_byte(0x55);     // 16 bit

  write_command_byte(0XB0);  // Interface Mode Control
  write_data_byte(0x80);     // SDO NOT USE

  write_command_byte(0xB1);  // Frame rate
  write_data_byte(0xA0);     // 60Hz

  write_command_byte(0xB4);  // Display Inversion Control
  write_data_byte(0x02);     // 2-dot

  write_command_byte(
      0XB6);  // Display Function Control  RGB/MCU Interface Control

  write_data_byte(0x02);  // MCU
  write_data_byte(0x02);  // Source,Gate scan direction

  write_command_byte(0XE9);  // Set Image Function
  write_data_byte(0x00);     // Disable 24 bit data

  write_command_byte(0xF7);  // Adjust Control
  write_data_byte(0xA9);
  write_data_byte(0x51);
  write_data_byte(0x2C);
  write_data_byte(0x82);  // D7 stream, loose

  write_command_byte(ILI9488_SLPOUT);  // Exit Sleep
  sleep_ms(120);
  write_command_byte(ILI9488_DISPON);  // Display on

  // fill_rect(0, 0, 479, 319, 0x1234);
  // sleep_ms(50);
  // TFT_BL_HIGH;  // Backlight on
}

// This is followed by a stream of pixels to render in this
// rectangle.
static void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  write_command_byte(ILI9488_CASET);  // Column addr set
  write_data_byte(x0 >> 8);
  write_data_byte(x0 & 0xFF);  // XSTART
  write_data_byte(x1 >> 8);
  write_data_byte(x1 & 0xFF);  // XEND

  write_command_byte(ILI9488_PASET);  // Row addr set
  write_data_byte(y0 >> 8);
  write_data_byte(y0 & 0xff);  // YSTART
  write_data_byte(y1 >> 8);
  write_data_byte(y1 & 0xff);  // YEND

  // Should follow by pixels.
  write_command_byte(ILI9488_RAMWR);  // write to RAM
  set_mode_double_byte();
  TFT_DC_HIGH;
}

// A macro to wait until the TX FIFO has at least
// 5 free words.
#define WAIT_FOR_FIFO_5_FREE                      \
  while ((PIO->flevel) & SM_FLEVEL_FREE_5_MASK) { \
  }

#define SEND_PIXEL(x) \
  PIO->txf[SM] = mk3_lookup_tables::color8_to_color16_table[x];

void Mk3TftDriver::render_buffer(uint16_t x1, uint16_t y1, uint16_t x2,
                                 uint16_t y2, const uint8_t* color8_p) {
  setAddrWindow(x1, y1, x2, y2);

  const int32_t w_pixels = x2 - x1 + 1;
  const int32_t h_pixels = y2 - y1 + 1;
  const uint32_t n = w_pixels * h_pixels;

  const uint8_t* p = color8_p;
  const uint8_t* const p_limit = p + n;
  const uint8_t* const p_limit_minus_20 = p_limit - 20;

  while (p < p_limit_minus_20) {
    WAIT_FOR_FIFO_5_FREE;
    SEND_PIXEL(p[0]);
    SEND_PIXEL(p[1]);
    SEND_PIXEL(p[2]);
    SEND_PIXEL(p[3]);
    SEND_PIXEL(p[4]);

    WAIT_FOR_FIFO_5_FREE;
    SEND_PIXEL(p[5]);
    SEND_PIXEL(p[6]);
    SEND_PIXEL(p[7]);
    SEND_PIXEL(p[8]);
    SEND_PIXEL(p[9]);

    WAIT_FOR_FIFO_5_FREE;
    SEND_PIXEL(p[10]);
    SEND_PIXEL(p[11]);
    SEND_PIXEL(p[12]);
    SEND_PIXEL(p[13]);
    SEND_PIXEL(p[14]);

    WAIT_FOR_FIFO_5_FREE;
    SEND_PIXEL(p[15]);
    SEND_PIXEL(p[16]);
    SEND_PIXEL(p[17]);
    SEND_PIXEL(p[18]);
    SEND_PIXEL(p[19]);

    p += 20;
  }

  while (p < p_limit) {
    // NOTE: 1 free would be sufficient.
    WAIT_FOR_FIFO_5_FREE;
    SEND_PIXEL(*p++);
  }
}

void Mk3TftDriver::backlight_on() { TFT_BL_HIGH; }
