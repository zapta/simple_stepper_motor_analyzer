#include "misc/hardware_test.h"
// #include <unistd.h>
#include "io.h"
#include "pico/stdlib.h"

namespace hardware_test {

static inline void update_pin(uint32_t bit_mask, uint32_t pin_mask,
                              uint32_t ticks) {
  if (ticks & bit_mask) {
    gpio_set_mask(pin_mask);
  } else {
    gpio_clr_mask(pin_mask);
  }
}

void test_tft_data() {
  for (;;) {
    uint32_t ticks = to_us_since_boot(get_absolute_time()) >> 5;

    update_pin(1ul << 0, 1ul << TFT_D0_PIN, ticks);
    update_pin(1ul << 1, 1ul << TFT_D1_PIN, ticks);
    update_pin(1ul << 2, 1ul << TFT_D2_PIN, ticks);
    update_pin(1ul << 3, 1ul << TFT_D3_PIN, ticks);

    update_pin(1ul << 4, 1ul << TFT_D4_PIN, ticks);
    update_pin(1ul << 5, 1ul << TFT_D5_PIN, ticks);
    update_pin(1ul << 6, 1ul << TFT_D6_PIN, ticks);
    update_pin(1ul << 7, 1ul << TFT_D7_PIN, ticks);

    update_pin(1ul << 8, 1ul << TFT_D8_PIN, ticks);
    update_pin(1ul << 9, 1ul << TFT_D9_PIN, ticks);
    update_pin(1ul << 10, 1ul << TFT_D10_PIN, ticks);
    update_pin(1ul << 11, 1ul << TFT_D11_PIN, ticks);

    update_pin(1ul << 12, 1ul << TFT_D12_PIN, ticks);
    update_pin(1ul << 13, 1ul << TFT_D13_PIN, ticks);
    update_pin(1ul << 14, 1ul << TFT_D14_PIN, ticks);
    update_pin(1ul << 15, 1ul << TFT_D15_PIN, ticks);
  }
}

}  // namespace hardware_test