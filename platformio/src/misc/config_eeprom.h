
#pragma once

#include "acquisition/analyzer.h"

namespace config_eeprom {

void test();

// Returns true if read ok. Otherwise default settings are returned.
// Both parameters are nullable.
bool read_settings(analyzer::Settings* settings, uint8_t* backlight_percents);

// Returns true if written ok. Does nothing if no change.
bool write_settings(const analyzer::Settings& settings,
                    uint8_t backlight_percents);

// Returns a human readable status from last read/write operation. For
// Debugging.
extern const char* last_status;

// For testing.
void check_packet_size();

}  // namespace config_eeprom