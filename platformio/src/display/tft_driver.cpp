#include "tft_driver.h"

#include <stdio.h>
#include <stdlib.h>

#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "io.h"
#include "pico/stdlib.h"
#include "tft_driver.pio.h"

namespace tft_driver {

// Plain GPIO output pins.
#define TFT_RST_PIN 1  // Active low.
#define TFT_DC_PIN 2   // 1: data, 0: command.
#define TFT_BL_PIN 15  // Active high

// Outputs managed by PioTft.
#define TFT_D0_PIN 6   // First of 8 data pins, [GPIO_6, GPIO_13]
#define TFT_WR_PIN 28  // Active low
// RD is not connected in MK3.
#define TFT_RD_PIN 18  // Active low

#define TFT_RST_HIGH gpio_set_mask(1ul << TFT_RST_PIN)
#define TFT_DC_HIGH gpio_set_mask(1ul << TFT_DC_PIN)

#define TFT_RST_LOW gpio_clr_mask(1ul << TFT_RST_PIN)
#define TFT_DC_LOW gpio_clr_mask(1ul << TFT_DC_PIN)

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

// Masks for the FDEBUG register.
static constexpr uint32_t SM_STALL_MASK = 1u << (PIO_FDEBUG_TXSTALL_LSB + SM);
static constexpr uint32_t SM_OVERRUN_MASK = 1u << (PIO_FDEBUG_TXOVER_LSB + SM);

// Updated later with the loading offset of the PIO program.
static uint pio_program_offset = 0;

// Info about the PWM channel used for the backlight output.
static uint bl_pwm_slice_num;
static uint bl_pwm_channel;

static void pio_set_x(uint32_t v) {
  static const uint instr_shift = pio_encode_in(pio_x, 4);
  static const uint instr_mov = pio_encode_mov(pio_x, pio_isr);
  for (int i = 7; i >= 0; i--) {
    const uint32_t nibble = (v >> (i * 4)) & 0xf;
    pio_sm_exec(PIO, SM, pio_encode_set(pio_x, nibble));
    pio_sm_exec(PIO, SM, instr_shift);
  }
  pio_sm_exec(PIO, SM, instr_mov);
}

void set_backlight(uint8_t percents) {
  // Don't go beyond a minimum level to make sure some
  // display is visible.
  percents = MAX(5, percents);
  percents = MIN(100, percents);
  pwm_set_chan_level(bl_pwm_slice_num, bl_pwm_channel, percents);
}

// BL output is PWM driven.
static void init_pwm() {
  bl_pwm_slice_num = pwm_gpio_to_slice_num(TFT_BL_PIN);
  bl_pwm_channel = pwm_gpio_to_channel(TFT_BL_PIN);
  gpio_set_function(TFT_BL_PIN, GPIO_FUNC_PWM);
  pwm_set_wrap(bl_pwm_slice_num, 99);
  // Reduce PWM freq to ~4.8Khz
  pwm_set_clkdiv_int_frac(bl_pwm_slice_num, 255, 0);
  // We start with BL off.
  pwm_set_chan_level(bl_pwm_slice_num, bl_pwm_channel, 0);
  pwm_set_enabled(bl_pwm_slice_num, true);
}

static void init_gpio() {
  // A mask with all gpio output pins we use.
  constexpr uint kOutputMask = 1ul << TFT_RST_PIN | 1ul << TFT_DC_PIN;
  gpio_init_mask(kOutputMask);
  gpio_set_dir_out_masked(kOutputMask);
}

static void init_pio() {
  // Make sure nobody else uses this state machine.
  pio_sm_claim(PIO, SM);

  // Load the PIO program. Starting by default with 16 bits mode
  // since it's at the begining of the PIO program.
  pio_program_offset = pio_add_program(PIO, &tft_driver_pio_program);

  // Associate pins with the PIO.
  pio_gpio_init(PIO, TFT_WR_PIN);
  pio_gpio_init(PIO, TFT_RD_PIN);
  for (int i = 0; i < 8; i++) {
    pio_gpio_init(PIO, TFT_D0_PIN + i);
  }

  // Configure the pins to be outputs.
  pio_sm_set_consecutive_pindirs(PIO, SM, TFT_WR_PIN, 1, true);
  pio_sm_set_consecutive_pindirs(PIO, SM, TFT_RD_PIN, 1, true);
  // Initially outputs. The PIO program latter toggle these to be
  // input or output as needed.
  pio_sm_set_consecutive_pindirs(PIO, SM, TFT_D0_PIN, 8, true);

  // Configure the state machine.
  pio_sm_config c =
      tft_driver_pio_program_get_default_config(pio_program_offset);
  // The pio program declares that a single sideset pin is used.
  // Define it.
  sm_config_set_sideset_pins(&c, TFT_WR_PIN);
  sm_config_set_set_pins(&c, TFT_RD_PIN, 1);

  // The 8 consecutive pins that are used for data outout.
  sm_config_set_out_pins(&c, TFT_D0_PIN, 8);

  sm_config_set_in_pins(&c, TFT_D0_PIN);

  // Set clock divider. Value of 1 for max speed.
  sm_config_set_clkdiv_int_frac(&c, PIO_CLOCK_DIV, 0);


  // The OSR register shifts to the right, sending the MSB byte
  // first, in a double bytes transfers.
  sm_config_set_out_shift(&c, true, false, 0);
  sm_config_set_in_shift(&c, false, false, 0);
  // Set the SM with the configuration we constructed above.
  // Default mode is single byte.
  pio_sm_init(PIO, SM, pio_program_offset + tft_driver_pio_offset_start_wr8,
              &c);

  // Start the state machine.
  pio_sm_set_enabled(PIO, SM, true);
}

static void flush() {
  // Clear the sticky stall status.
  PIO->fdebug = SM_STALL_MASK;
  // Wait until the stall flag is up again.
  while (!(PIO->fdebug & SM_STALL_MASK)) {
  }

  const uint rx_count = pio_sm_get_rx_fifo_level(PIO, SM);
  const uint tx_count = pio_sm_get_tx_fifo_level(PIO, SM);
  if (rx_count || tx_count) {
    printf("WARNING: Dropping FIFOs, rx=%u, tx=%u\n", rx_count, tx_count);
  }
  pio_sm_clear_fifos(PIO, SM);
}

static const uint wr8_init_table[] = {
    0xf801,  // set    pins, 1         side 1
    0xe020,  // set    x, 0
    0xa0e9,  // mov    osr, !x
    0x6088,  // out    pindirs, 8
};

static void set_mode_write_8() {
  flush();

  pio_sm_set_enabled(PIO, SM, false);

  constexpr int N = sizeof(wr8_init_table) / sizeof(wr8_init_table[0]);
  for (int i = 0; i < N; i++) {
    pio_sm_exec(PIO, SM, wr8_init_table[i]);
  }

  // Force jump to start address.
  // pio_program_offset is a variable so can't be used in the static init table.
  pio_sm_exec(
      PIO, SM,
      pio_encode_jmp(pio_program_offset + tft_driver_pio_offset_start_wr8));

  pio_sm_set_enabled(PIO, SM, true);
}

static const uint wr16_init_table[] = {
    0xf801,  // set    pins, 1         side 1
    0xe020,  // set    x, 0
    0xa0e9,  // mov    osr, !x
    0x6088,  // out    pindirs, 8
};

static void set_mode_write_16() {
  flush();
  pio_sm_set_enabled(PIO, SM, false);

  constexpr int N = sizeof(wr16_init_table) / sizeof(wr16_init_table[0]);
  for (int i = 0; i < N; i++) {
    pio_sm_exec(PIO, SM, wr16_init_table[i]);
  }

  // Force jump to start address.
  // pio_program_offset is a variable so can't be used in the static init table.
  pio_sm_exec(
      PIO, SM,
      pio_encode_jmp(pio_program_offset + tft_driver_pio_offset_start_wr16));
  pio_sm_set_enabled(PIO, SM, true);
}

static const uint rd8_init_table[] = {
    0xf801,  // 18: set    pins, 1         side 1
    0xe040,  // 19: set    y, 0
    0xa0e2,  // 20: mov    osr, y
    0x6088,  // 21: out    pindirs, 8
};

// Not available in MK3 (RD signal not connected).
static void set_mode_read_8(uint count) {
  flush();

  pio_sm_set_enabled(PIO, SM, false);

  constexpr int N = sizeof(rd8_init_table) / sizeof(rd8_init_table[0]);
  for (int i = 0; i < N; i++) {
    pio_sm_exec(PIO, SM, rd8_init_table[i]);
  }

  pio_set_x(count);

  // Force jump to start address.
  // pio_program_offset is a variable so can't be used in the static init table.
  pio_sm_exec(
      PIO, SM,
      pio_encode_jmp(pio_program_offset + tft_driver_pio_offset_start_rd8));

  pio_sm_set_enabled(PIO, SM, true);
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
  set_mode_write_8();
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

static volatile uint32_t irq_counter;
static uint dma_chan;
static dma_channel_config dma_config;

static void dma_irq_handler() {
  // TODO: Add verification that this is always true.
  if (dma_hw->ints1 & 1u << dma_chan) {
    irq_counter++;
    // Clear the interrupt request.
    dma_hw->ints1 = 1u << dma_chan;
    // Call the client's completion callback.
    lvgl_adapter::dma_completion_irq_cb();
  }
}

static void init_dma() {
  LED1_OFF;
  dma_chan = dma_claim_unused_channel(true);

  dma_config = dma_channel_get_default_config(dma_chan);
  channel_config_set_transfer_data_size(&dma_config, DMA_SIZE_16);
  channel_config_set_read_increment(&dma_config, true);    // Pixel buffer
  channel_config_set_write_increment(&dma_config, false);  // PIO.
  // TODO: Derive the target from the PIO and SM specification.
  channel_config_set_dreq(&dma_config, DREQ_PIO0_TX0);
  dma_channel_set_irq1_enabled(dma_chan, true);
  irq_set_exclusive_handler(DMA_IRQ_1, dma_irq_handler);
  irq_set_mask_enabled(1 << DMA_IRQ_1, true);
}

void begin() {
  init_gpio();
  init_pwm();
  init_pio();
  init_dma();

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
  // write_data_byte(0x00);     // 30Hz
  write_data_byte(0x10);  // NOTE: Was missing originally. RTNA, Default 0x11.

  // // Optimized for a long vertical blank for sync update.
  // write_command_byte(0xb5);
  // write_data_byte(0x02);    // LOW VFP
  // write_data_byte(0x1C);    // HIGH VPB
  // write_data_byte(0x02);    // LOW HFP
  // write_data_byte(0x02);    // Low HBP

  write_command_byte(0x35);  // Tearing effect signal on.
  write_data_byte(0x00);     // Vsync only.
  // write_data_byte(0x01);     // Vsync + hsync.

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
  set_mode_write_16();
  TFT_DC_HIGH;
}

void render_buffer(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                   const uint16_t* color16_p) {
  setAddrWindow(x1, y1, x2, y2);

  const int32_t w_pixels = x2 - x1 + 1;
  const int32_t h_pixels = y2 - y1 + 1;
  const uint32_t n = w_pixels * h_pixels;

  // Start the DMA transfer
  dma_channel_configure(dma_chan, &dma_config,
                        &PIO->txf[SM],  // dst
                        color16_p,      // src
                        n,              // transfer count
                        true            // start immediately
  );

  // For testing, simulated blocking DMA transfer.
  // if (false) {
  //  while (dma_channel_is_busy(dma_chan)) {
  //}
}

// Temp for testing. Reads ILI9488 data.
void test(void) {
  printf("Reading TFT\n");
  write_command_byte(0xd3);
  constexpr int N = 4;
  set_mode_read_8(N);
  uint32_t bfr[N];
  for (int i = 0; i < N; i++) {
    bfr[i] = pio_sm_get_blocking(PIO, SM);
  }
  for (int i = 0; i < N; i++) {
    printf("Byte %d: 0x%08lx\n", i, bfr[i]);
  }
}

}  // namespace tft_driver
