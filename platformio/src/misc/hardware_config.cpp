#include "hardware_config.h"

#include "pico/stdlib.h"

namespace hardware_config {

constexpr uint8_t LEVELS_PIN = 21;
constexpr uint8_t SENSORS_PIN = 14;

// We use arbitrary values to make sure things do
// not break.
static const SensorSpec UNKNOWN_SENSOR("????", 2500, 0.4);

// ACS70331EOLCTR-2P5B3
static const SensorSpec GMR_2P5_SENSOR("G2P5A", 2500, 0.4);

// ACS712ELCTR-05B-T
// We limit the range to 3A since this is the
// most 3D printer use cases fall here.
static SensorSpec HAUL_5A_SENSOR("H5A", 3000, 0.185);

static HardwareConfig hardware_config(LEVEL_UNKNOWN, &UNKNOWN_SENSOR);

enum PinState { STATE_ERROR, STATE_FLOAT, STATE_DOWN, STATE_UP };

static PinState determine_config_pin_state(uint pin) {
  gpio_init(pin);
  gpio_set_dir(pin, false);
  gpio_pull_down(pin);
  sleep_ms(10);
  const bool state_down = gpio_get(pin);
  gpio_pull_up(pin);
  sleep_ms(10);
  const bool state_up = gpio_get(pin);
  return state_up ? (state_down ? STATE_UP : STATE_FLOAT)
                  : (state_down ? STATE_ERROR : STATE_DOWN);
}

static Level determine_level() {
  const PinState pin_state = determine_config_pin_state(LEVELS_PIN);
  switch (pin_state) {
    case STATE_FLOAT:
      return LEVEL_MK3;
    case STATE_DOWN:
      return LEVEL_MK4;
    case STATE_UP:
      return LEVEL_MK5;
    default:
      return LEVEL_UNKNOWN;
  }
}

static const SensorSpec* determine_sensor() {
  //return &HAUL_5A_SENSOR;
  const PinState pin_state = determine_config_pin_state(SENSORS_PIN);
  switch (pin_state) {
    case STATE_FLOAT:
      return &GMR_2P5_SENSOR;
    case STATE_DOWN:
      return &HAUL_5A_SENSOR;
    case STATE_UP:
    default:
      return &UNKNOWN_SENSOR;
  }
}

HardwareConfig determine() {
  hardware_config = HardwareConfig(determine_level(), determine_sensor());
  return hardware_config;
}

const HardwareConfig& config() { return hardware_config; }

const char* level_name() {
  switch (hardware_config.level) {
    case LEVEL_MK3:
      return "MK3";
    case LEVEL_MK4:
      return "MK4";
    case LEVEL_MK5:
      return "MK5";
    default:
      return "UNKNOWN";
  }
}

}  // namespace hardware_config
