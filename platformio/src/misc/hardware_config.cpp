#include "hardware_config.h"

#include "pico/stdlib.h"

namespace hardware_config {

constexpr uint8_t CONFIG_PIN = 19;

static HardwareConfig config = CONFIG_UNKNOWN;

HardwareConfig determine() {
  gpio_init(CONFIG_PIN);
  gpio_set_dir(CONFIG_PIN, false);
  gpio_pull_down(CONFIG_PIN);
  sleep_ms(01);
  const bool state_down = gpio_get(CONFIG_PIN);
  gpio_pull_up(CONFIG_PIN);
  sleep_ms(10);
  const bool state_up = gpio_get(CONFIG_PIN);
  config = state_up ? (state_down ? CONFIG_ALT2 : CONFIG_DEFAULT)
                     : (state_down ? CONFIG_UNKNOWN : CONFIG_ALT1);
  return config;
}

HardwareConfig get() { return config; }

const char* get_name() {
  switch (config) {
    case CONFIG_DEFAULT:
      return "DEFAULT";
    case CONFIG_ALT1:
      return "ALT1";
    case CONFIG_ALT2:
      return "ALT2";
    default:
      return "UNKNOWN";
  }
}

}  // namespace hardware_config
