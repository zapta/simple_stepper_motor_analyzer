
#pragma once

#include "acquisition/analyzer.h"

namespace config_eeprom {

extern void test() ;

// Returns true if read ok. Otherwise default settings are returned.
extern bool read_acquisition_settings(analyzer::Settings* settings);

// Returns true if written ok.
extern bool write_acquisition_settings(const analyzer::Settings& settings);

// Returns a human readable status from last read/write operation. For 
// Debugging.
extern const char* last_status;
}  // namespace