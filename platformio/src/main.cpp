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
#include "misc/hardware_config.h"
#include "misc/memory.h"
#include "pico/stdlib.h"
#include "ui/screen_manager.h"

constexpr uint8_t kColor8Blue = 0x3;

// TODO: move the lvgl tick timer to the lv_adapter file.

static Elapsed elapsed_from_last_dump;

static repeating_timer_t lvgl_ticker;

// LVGL time ticks IRQ.
static bool timer_callback(repeating_timer_t* rt) {
  lv_tick_inc(5);
  return true;  // keep repeating
}

static Elapsed timer;

void setup() {
  stdio_init_all();
  hardware_config::determine();

  io::setup();

  LED0_ON;

  // TODO: Read settings from EEPROM
  analyzer::Settings acq_settings;
  uint8_t backlight_percents;
  config_eeprom::read_settings(&acq_settings, &backlight_percents);

  // Must setup analyzer before adc_dma.
  analyzer::setup(acq_settings);
  adc_dma::setup();

  touch_driver::setup();

  // ver.begin();

  lvgl_adapter::setup();

  analyzer::reset_state();

  screen_manager::setup();

  const bool timer_ok =
      add_repeating_timer_ms(5, timer_callback, NULL, &lvgl_ticker);

  // Render the first page.
  // lv_task_handler();
  screen_manager::loop();
  lv_refr_now(NULL);
  sleep_ms(30);  // let the screen process the data to avoid initial flicker.

  // Turn on the backlight. User can now see the screen.
  lvgl_adapter::set_backlight(backlight_percents);
  // millis_to_first_screen = to_ms_since_boot(get_absolute_time());

  // Now that the first screen is display. We can spend some
  // time setting up proactivly the other screens. Otherwise
  // they will be setup on demand on first invocation.
  screen_manager::setup_screens_ahead();
}

void loop() {
  // LVGL processing and rendering.
  lv_task_handler();

  // Screen updates.
  screen_manager::loop();

  // Heartbeat.
  if (to_ms_since_boot(get_absolute_time()) % 1000 < 50) {
    LED0_ON;
  } else {
    LED0_OFF;
  }

  // Periodic report over USB/Serial. For debugging.
  if (elapsed_from_last_dump.elapsed_millis() > 3000) {
    elapsed_from_last_dump.reset();

    static uint print_cycle = 0;

    switch (print_cycle) {
      default:
      case 0:
        printf("\nFree memory: %d\n", memory::free_memory());
        printf("Config: [%s] [%s]\n", hardware_config::level_name(),
               hardware_config::sensor_name());
        puts("Pico SDK version: " PICO_SDK_VERSION_STRING);
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
        adc_dma::dump_state();
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
