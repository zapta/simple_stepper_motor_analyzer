
#pragma once

#include "tft_driver.h"

namespace lvgl_adapter {

extern void setup();
// Called once after the first LVGL screen was rendered.
extern void backlight_on();

// For developement.
extern void dump_stats();
extern void start_screen_capture();
extern void stop_screen_capture();


}  // namespace lvgl_adapter
