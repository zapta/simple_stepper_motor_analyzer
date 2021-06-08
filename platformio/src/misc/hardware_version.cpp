#include "hardware_version.h"

#include "pico/stdlib.h"

namespace hardware_version {

constexpr uint8_t CONFIG_PIN = 18;

static Version version = HARDWARE_UNKNOWN;

// Using pin 18.
Version determine() {
  gpio_init(CONFIG_PIN);
  gpio_set_dir(CONFIG_PIN, false);
  gpio_pull_down(CONFIG_PIN);
  sleep_ms(01);
  const bool state_down = gpio_get(CONFIG_PIN);
  gpio_pull_up(CONFIG_PIN);
  sleep_ms(10);
  const bool state_up = gpio_get(CONFIG_PIN);
  version = state_up ? (state_down ? HARDWARE_UNKNOWN : HARDWARE_MK2)
                     : (state_down ? HARDWARE_UNKNOWN : HARDWARE_MK3);
  return version;
}

Version get() { return version; }

const char* get_name() {
  switch (version) {
    case HARDWARE_MK2:
      return "MK2";
    case HARDWARE_MK3:
      return "MK3";
    default:
      return "Unknown";
  }
}

}  // namespace hardware_version
