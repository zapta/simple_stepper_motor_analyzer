
#pragma once

namespace hardware_version {

enum Version { HARDWARE_UNKNOWN, HARDWARE_MK2, HARDWARE_MK3 };

Version determine();

// Should call determine() before calling these.
Version get();
const char* get_name();

}  // namespace hardware_version