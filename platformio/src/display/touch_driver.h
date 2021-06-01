
#pragma once

#include <stdint.h>

namespace touch_driver {
extern void setup();
extern void test();
extern void touch_read(uint16_t* x, uint16_t* y, bool* is_pressed);

}  // namespace touch_driver
