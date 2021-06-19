
#pragma once

#include "tft_driver.h"

namespace lvgl_adapter {

void setup();
// Called once after the first LVGL screen was rendered.
void backlight_on();

// For developement.
void dump_stats();
void start_screen_capture();
void stop_screen_capture();

}  // namespace lvgl_adapter
