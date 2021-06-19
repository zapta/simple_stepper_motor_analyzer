
#pragma once

#include <stdint.h>

namespace touch_driver {
    
void setup();
void test();
void touch_read(uint16_t* x, uint16_t* y, bool* is_pressed);

}  // namespace touch_driver
