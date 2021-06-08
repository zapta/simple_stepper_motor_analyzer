#include "io.h"

#include "hardware/gpio.h"

namespace io {

void setup() {
  // A mask with all gpio output pins we use.
  constexpr uint kOutputMask =
      1ul << LED0_PIN | 1ul << LED1_PIN | 1ul << LED2_PIN ;
      // | 1ul << TFT_D0_PIN |
      // 1ul << TFT_D1_PIN | 1ul << TFT_D2_PIN | 1ul << TFT_D3_PIN |
      // 1ul << TFT_D4_PIN | 1ul << TFT_D5_PIN | 1ul << TFT_D6_PIN |
      // 1ul << TFT_D7_PIN | 1ul << TFT_D8_PIN | 1ul << TFT_D9_PIN |
      // 1ul << TFT_D10_PIN | 1ul << TFT_D11_PIN | 1ul << TFT_D12_PIN |
      // 1ul << TFT_D13_PIN | 1ul << TFT_D14_PIN | 1ul << TFT_D15_PIN |
      // 1ul << TFT_RST_PIN | 1ul << TFT_WR_PIN | 1ul << TFT_DC_PIN |
      // 1ul << TFT_BL_PIN;


  gpio_init_mask(kOutputMask);
  
  // Start with backlight non active. Before we set it as a direction,
  // an external pullup resistor makes sure  it's high.
  //TFT_BL_HIGH;

  gpio_set_dir_out_masked(kOutputMask);
}

 }  // namespace io
