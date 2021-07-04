
#pragma once

#include "tft_driver.h"

namespace lvgl_adapter {

void setup();
// Called once after the first LVGL screen was rendered.
void set_backlight(uint8_t percents);

// For developement.
void dump_stats();
void dump_screen();

}  // namespace lvgl_adapter
