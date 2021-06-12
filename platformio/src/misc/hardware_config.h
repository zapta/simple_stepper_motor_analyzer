
#pragma once

namespace hardware_config {

// TODO: Assign meanigful names once actually using it.
enum HardwareConfig { CONFIG_UNKNOWN, CONFIG_DEFAULT, CONFIG_ALT1, CONFIG_ALT2 };

HardwareConfig determine();

// Should call determine() before calling these.
HardwareConfig get();
const char* get_name();

}  // namespace hardware_config