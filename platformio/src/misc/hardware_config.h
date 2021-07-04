#pragma once

#include <stdint.h>

namespace hardware_config {

// PCB compatibility level.
enum Level {
  LEVEL_UNKNOWN,
  LEVEL_MK3,
  LEVEL_MK4,
  LEVEL_MK5,
};

struct SensorSpec {
  const char* name;
  const uint16_t range_milliamps;
  const float volts_per_amp;
  // These computed values are cached to speed up unit conversions.
  const float counts_per_amp;
  const float amps_per_count;
  const float milliamps_per_count;

  SensorSpec(const char* name_, uint16_t range_milliamps_, float volts_per_amp_)
      : name(name_),
        range_milliamps(range_milliamps_),
        volts_per_amp(volts_per_amp_),
        counts_per_amp(volts_per_amp * 4096 / 3.3),
        amps_per_count(1 / counts_per_amp),
        milliamps_per_count(1000 / counts_per_amp) {}

  // Convert adc value to milliamps.
  int adc_value_to_milliamps(int adc_value) const {
    return (int)(adc_value * milliamps_per_count);
  }

  // Convert adc value to amps.
  float adc_value_to_amps(int adc_value) const {
    return ((float)adc_value) * amps_per_count;
  }
};

struct HardwareConfig {
  Level level;
  const SensorSpec* sensor_spec;
  HardwareConfig(Level l, const SensorSpec* s) : level(l), sensor_spec(s) {}
  /// HardwareConfig() : HardwareConfig(LEVEL_UNKNOWN, SENSOR_UNKNOWN) {}
};

HardwareConfig determine();

// Should call determine() before calling these.
const HardwareConfig& config();

inline const SensorSpec* sensor_spec() { return config().sensor_spec; }
inline Level level() { return config().level; }
const char* level_name();

inline const char* sensor_name() { return config().sensor_spec->name; }

inline uint16_t range_milliamps() {
  return config().sensor_spec->range_milliamps;
}

}  // namespace hardware_config