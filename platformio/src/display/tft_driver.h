
#pragma once

#include <stdint.h>

namespace tft_driver {

void begin();

// Sets the DMA and returns. On actual completion, dma_completion_irq_cb
// is called, at an IRQ level.
void render_buffer(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                   const uint16_t* color16_p);

void set_backlight(uint8_t percents);

}  // namespace tft_driver

namespace lvgl_adapter {
// DMA completion handler. Implmeented by lvgl_adapter. Called at
// IRQ level.
void dma_completion_irq_cb();
}  // namespace lvgl_adapter
