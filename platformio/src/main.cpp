#include <stdarg.h>
#include <stdio.h>

#include "acquisition/adc_dma.h"
#include "acquisition/analyzer.h"
#include "display/lvgl_adapter.h"
#include "display/tft_driver.h"
#include "display/touch_driver.h"
#include "io.h"
#include "lvgl.h"
#include "misc/config_eeprom.h"
#include "misc/elapsed.h"
#include "misc/memory.h"
#include "pico/stdlib.h"
#include "ui/screen_manager.h"

constexpr uint8_t kColor8Blue = 0x3;

// TODO: move the lvgl tick timer to the lv_adapter files.
//
// Doc: https://github.com/stm32duino/wiki/wiki/HardwareTimer-library
// HardwareTimer tim2(TIM2);

static Elapsed elapsed_from_last_dump;

static repeating_timer_t lvgl_ticker;

// LVGL time ticks IRQ.
static bool timer_callback(repeating_timer_t *rt) {
  lv_tick_inc(5);
  return true;  // keep repeating
}

static Elapsed timer;

void setup() {
  stdio_init_all();

  io::setup();

  LED0_ON;

  // TODO: Read settings from EEPROM
  analyzer::Settings acq_settings;
  config_eeprom::read_acquisition_settings(&acq_settings);
  // acq_settings.offset1 = 1900;
  // acq_settings.offset2 = 1900;
  // acq_settings.reverse_direction = false;

  // Must setup analyzer before adc_dma.
  analyzer::setup(acq_settings);
  adc_dma::setup();

  touch_driver::setup();

  tft_driver::begin();

  lvgl_adapter::setup();

  analyzer::reset_state();

  screen_manager::setup();

  const bool timer_ok =
      add_repeating_timer_ms(5, timer_callback, NULL, &lvgl_ticker);

  // Force a blocking screen redraw.
  lv_refr_now(NULL);

  //lv_task_handler();

  sleep_ms(50);  // let the screen process the data to avoid initial flicker.

  // Turn on the backlight. User can now see the screen.
  TFT_BL_LOW;
}

void loop() {
  // LVGL processing and rendering.
  lv_task_handler();

  // sleep_ms(5);

  // Screen updates.
  screen_manager::loop();

  // Heartbeat.
  if (to_ms_since_boot(get_absolute_time()) % 1000 < 50) {
    LED0_ON;
  } else {
    LED0_OFF;
  }

  // Periodic report over USB/Serial.
  if (elapsed_from_last_dump.elapsed_millis() > 5000) {
    elapsed_from_last_dump.reset();
    
    static uint print_cycle = 0;

    switch (print_cycle) {
      default:
      case 0:
        printf("\nFree memory: %d\n", memory::free_memory());
        print_cycle = 1;
        break;
      case 1:
        printf("\n");
        lvgl_adapter::dump_stats();
        print_cycle = 2;
        break;
      case 2:
        printf("\n");
        analyzer::sample_state();
        analyzer::dump_sampled_state();
        print_cycle = 0;
        break;
    }
  }
}

// Arduino like main.
int main() {
  setup();
  for (;;) {
    loop();
  }
}
