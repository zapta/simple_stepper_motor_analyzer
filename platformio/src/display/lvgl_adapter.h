
#pragma once

#include "tft_driver.h"

namespace lvgl_adapter {

extern void setup(TftDriver* driver);

// For developement.
extern void dump_stats();
extern void start_screen_capture();
extern void stop_screen_capture();


}  // namespace lvgl_adapter
