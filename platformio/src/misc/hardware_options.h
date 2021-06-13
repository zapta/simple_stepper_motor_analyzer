
#pragma once

namespace hardware_options {

// TODO: Assign meanigful names once actually using it.
enum HardwareOptions {
  OPTIONS_UNKNOWN,
  OPTIONS_DEFAULT,
  OPTIONS_ALT1,
  OPTIONS_ALT2
};

HardwareOptions determine();

// Should call determine() before calling these.
HardwareOptions get();
const char* get_name();

}  // namespace hardware_options