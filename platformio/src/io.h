// API of the data acquisition and step decoder modole. This
// is the core of the analysis software and it is executed
// in an interrupt routine so handle with care.

#pragma once

#include "pico.h"

// TODO: Convert macros to inline functions.

// Diagnostics LEDs.
#define LED0_PIN PICO_DEFAULT_LED_PIN
#define LED1_PIN 17
#define LED2_PIN 16

#define LED0_ON gpio_set_mask(1ul << LED0_PIN)
#define LED1_ON gpio_set_mask(1ul << LED1_PIN)
#define LED2_ON gpio_set_mask(1ul << LED2_PIN)

#define LED0_OFF gpio_clr_mask(1ul << LED0_PIN)
#define LED1_OFF gpio_clr_mask(1ul << LED1_PIN)
#define LED2_OFF gpio_clr_mask(1ul << LED2_PIN)

#define LED0_TOGGLE gpio_xor_mask(1ul << LED0_PIN)
#define LED1_TOGGLE gpio_xor_mask(1ul << LED1_PIN)
#define LED2_TOGGLE gpio_xor_mask(1ul << LED2_PIN)

#define TFT_D0_PIN 28
#define TFT_D1_PIN 3
#define TFT_D2_PIN 6
#define TFT_D3_PIN 7

#define TFT_D4_PIN 8
#define TFT_D5_PIN 9
#define TFT_D6_PIN 22
#define TFT_D7_PIN 10

#define TFT_D8_PIN 11
#define TFT_D9_PIN 21
#define TFT_D10_PIN 12
#define TFT_D11_PIN 20

#define TFT_D12_PIN 13
#define TFT_D13_PIN 19
#define TFT_D14_PIN 14
#define TFT_D15_PIN 18

#define TFT_RST_PIN 2
#define TFT_WR_PIN 1
#define TFT_DC_PIN 0
#define TFT_BL_PIN 15

#define TFT_RST_HIGH gpio_set_mask(1ul << TFT_RST_PIN)
#define TFT_WR_HIGH gpio_set_mask(1ul << TFT_WR_PIN)
#define TFT_DC_HIGH gpio_set_mask(1ul << TFT_DC_PIN)
#define TFT_BL_HIGH gpio_set_mask(1ul << TFT_BL_PIN)

#define TFT_RST_LOW gpio_clr_mask(1ul << TFT_RST_PIN)
#define TFT_WR_LOW gpio_clr_mask(1ul << TFT_WR_PIN)
#define TFT_DC_LOW gpio_clr_mask(1ul << TFT_DC_PIN)
#define TFT_BL_LOW gpio_clr_mask(1ul << TFT_BL_PIN)

namespace io {

void setup();

// void printf(const char* format, ...);
// void printfln(const char* format, ...);
// void println();
// void println(const char* s);

}  // namespace io
