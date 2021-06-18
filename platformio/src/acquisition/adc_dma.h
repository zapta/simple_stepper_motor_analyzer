

#pragma once

namespace adc_dma {
void disable_irq();
void enable_irq();
void setup();
// For debugging.
void dump_state();
}  // namespace adc_dma
