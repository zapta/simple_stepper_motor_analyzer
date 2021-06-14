
#pragma once

// Releasees of Wizio Pico SDK are listed at
// https://github.com/Wiz-IO/wizio-pico/wiki#last-news. As of June 2021 we use
// version 1.1.2.
#if PICO_SDK_VERSION_MAJOR != 1 || PICO_SDK_VERSION_MINOR != 1 || \
    PICO_SDK_VERSION_REVISION != 2
#error "Using an unexpected version of the SDK."
#endif

// Our own version.
#define VERSION_STRING "1.0.1"

namespace config {

// FOR DEBUGGING ONLY. Turn off for official releases.

// For UI debugging. Shows the boundaries of UI objects.
static constexpr bool kDebugBackgrounds = false;

// For developers only. When enabled, clicking on a
// screen's title field pauses the program and sends a screen
// dump over the USB/serial connection.
static constexpr bool kEnableScreenshots = false;

// For developers only. When enabled, clicking on a
// screen's title field generates a Debug event for that
// screen.
static constexpr bool kEnableDebugEvents = false;

}  // namespace config
