// Provides common buffer and functionality to capture screens.

#pragma once

#include "acquisition/analyzer.h"
#include "misc/elapsed.h"
#include "ui.h"

namespace adc_capture_util {

// Common ASC capture screen controls.
struct AdcCaptureControls {
  // A button to toggle run/stop.
  ui::Button run_button;
  // Run/stop status text.
  ui::Label status_label;

  // Call once on initialization.
  void setup(ui::Screen& screen);

  // Update the button and text label based on common
  // capture state.
  void update_display_from_state();

  void sync_button_to_state();
};

// Toggle between normal (20ms) and alternative (100ms) scale.
void toggle_scale();

bool alternative_scale();

void clear_data();

bool has_data();

// Returns true if a new data was captured.
bool maybe_update_capture_data();

// If has_data() is true, this contains the data.
const analyzer::AdcCaptureBuffer* capture_buffer();

// Given capture controls, update capture enabled/disabled
// if needed.
bool maybe_update_state_from_controls(
    const AdcCaptureControls& capture_controls);

extern bool capture_enabled();

void set_capture_enabled(bool val);

}  // namespace adc_capture_util
