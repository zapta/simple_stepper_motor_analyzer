
#pragma once

// Our own version.
#define VERSION_STRING "1.0.4"

namespace config {

// FLAGS ARE FOR DEBUGGING ONLY. Turn off for official releases.

// TODO: convert kEnableScreenshots and kEnableDebugEvents to a single enum.

// For developers only. When enabled, clicking on a
// screen's title field pauses the program and sends a screen
// dump over the USB/serial connection.
static constexpr bool kEnableScreenshots = false;

// For developers only. When enabled, clicking on a
// screen's title field generates a Debug event for that
// screen.
static constexpr bool kEnableDebugEvents = false;

// For UI debugging. Shows the boundaries of UI objects.
static constexpr bool kDebugBackgrounds = false;

}  // namespace config
