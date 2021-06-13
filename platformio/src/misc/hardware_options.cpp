#include "hardware_options.h"

#include "pico/stdlib.h"

namespace hardware_options {

constexpr uint8_t OPTIONS_PIN = 14;

static HardwareOptions options = OPTIONS_UNKNOWN;

HardwareOptions determine() {
  gpio_init(OPTIONS_PIN);
  gpio_set_dir(OPTIONS_PIN, false);
  gpio_pull_down(OPTIONS_PIN);
  sleep_ms(01);
  const bool state_down = gpio_get(OPTIONS_PIN);
  gpio_pull_up(OPTIONS_PIN);
  sleep_ms(10);
  const bool state_up = gpio_get(OPTIONS_PIN);
  options = state_up ? (state_down ? OPTIONS_ALT2 : OPTIONS_DEFAULT)
                     : (state_down ? OPTIONS_UNKNOWN : OPTIONS_ALT1);
  return options;
}

HardwareOptions get() { return options; }

const char* get_name() {
  switch (options) {
    case OPTIONS_DEFAULT:
      return "DEFAULT";
    case OPTIONS_ALT1:
      return "ALT1";
    case OPTIONS_ALT2:
      return "ALT2";
    default:
      return "UNKNOWN";
  }
}

}  // namespace hardware_options
