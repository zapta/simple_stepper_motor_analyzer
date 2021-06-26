// See display porting here:
// https://docs.lvgl.io/v7/en/html/porting/display.html

#include "lvgl_adapter.h"

#include <stdio.h>

#include "io.h"
#include "lvgl.h"
#include "pico/stdlib.h"
#include "tft_driver.h"
#include "touch_driver.h"

#if LV_COLOR_DEPTH != 16
#error "Expecting LVGL color depth of 16"
#endif

namespace lvgl_adapter {

//#define TFT_SYNC_PIN 0

#define MY_DISP_HOR_RES (480)

// A static variable to store the buffers.
static lv_disp_buf_t disp_buf;
static lv_disp_drv_t disp_drv;

// LVGL renders up to this number of pixels at a time.
// We use two buffers, each with a 1/4 screen size and alternate
// render/DMA.
static constexpr uint32_t kBufferSize = MY_DISP_HOR_RES * 80;
//
static lv_color_t buf_1[kBufferSize];
static lv_color_t buf_2[kBufferSize];

// For developer's usage. Eatables screen capture for
// documentation. Do not release with this flag set.
static bool screen_capture_enabled = false;

// static inline void wait_for_tft_sync() {
//   uint32_t start = to_us_since_boot(get_absolute_time());

//   while (!gpio_get(TFT_SYNC_PIN)) {
//   };
//     uint32_t end = to_us_since_boot(get_absolute_time());

//   //printf("%lu\n", end - start);
//   sleep_ms(20);
// }

// Experimental.
//static bool sync_next_update_flag = false;

// void sync_next_update() {
//   sync_next_update_flag = true;
// }

// NOTE: Capture the dumpped text using an external terminal emularot.
// Platformio's own terminal drops line seperators in some cases.
//
// Used during debugging to dump the screen. Enabled by
// ui::kEnableScreenshots.
static void capture_buffer(const lv_area_t* area, lv_color_t* bfr) {
  const int32_t w_pixels = area->x2 - area->x1 + 1;
  const int32_t h_pixels = area->y2 - area->y1 + 1;

  // x,y are relative to the buffer rect.
  for (int y = 0; y < h_pixels; y++) {
    printf("#%d,%d,%ld", area->x1, area->y1 + y, w_pixels);
    uint16_t pending_pixels_count = 0;
    uint16_t pending_pixel_color = 0;
    for (int x = 0; x < w_pixels; x++) {
      uint16_t pixel_color = bfr[(uint32_t)y * w_pixels + x].full;

      // Case 0: no pending.
      if (pending_pixels_count == 0) {
        pending_pixels_count = 1;
        pending_pixel_color = pixel_color;
        continue;
      }

      // Case 1: pending exists, append to pending.
      if (pending_pixel_color == pixel_color) {
        pending_pixels_count++;
        continue;
      }

      // Case 2: pending exist, flushing pending.
      printf(",%hu:%hx", pending_pixels_count, pending_pixel_color);
      pending_pixel_color = pixel_color;
      pending_pixels_count = 1;
    }
    // Flush end of line.
    if (pending_pixels_count > 0) {
      printf(",%hu:%hx", pending_pixels_count, pending_pixel_color);
    }
    printf("\n");
    // This prevents loss of data(?).
    // sleep_ms(25);
  }
}

// Called by LV_GL to flush a buffer to the display. Per our LVGL config,
// color is uint16_t RGB565.
static void my_flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area,
                        lv_color_t* color_p) {
  //LED2_ON;
  if (screen_capture_enabled) {
    capture_buffer(area, color_p);
  }

  // if (sync_next_update_flag) {
  //   wait_for_tft_sync();
  //   sync_next_update_flag = false;
  // }

  // Per our lv config settings, LVGL uses 16 bits colors.
  const lv_color16_t* lv_color16 = static_cast<lv_color16_t*>(color_p);
  tft_driver::render_buffer(area->x1, area->y1, area->x2, area->y2,
                            (uint16_t*)lv_color16);

  //LED2_OFF;
  // NOTE: The DMA completion callback will call lv_disp_flush_ready(disp_drv)
  // once the DMA to the TFT is completed.
}

void static init_display_driver() {
  // Initialize `disp_buf` with the buffer. We pass NULL for
  // the second (optional) buffer since we don't use DMA.
  lv_disp_buf_init(&disp_buf, buf_1, buf_2, kBufferSize);

  lv_disp_drv_init(&disp_drv);

  // Sets an initialized buffer.
  disp_drv.buffer = &disp_buf;
  // Sets a flush callback to draw to the display.
  disp_drv.flush_cb = my_flush_cb;

  // Register the driver and save the created display objects.
  lv_disp_drv_register(&disp_drv);
}

// This is how LVGL reads the touch screen's status.
bool my_touch_read_cb(lv_indev_drv_t* drv, lv_indev_data_t* data) {
  uint16_t x, y;
  bool is_pressed;
  touch_driver::touch_read(&x, &y, &is_pressed);
  data->point.x = x;
  data->point.y = y;
  data->state = is_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
  // No buffering now so no more data read.
  return false;
}

void static init_touch_driver() {
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touch_read_cb;
  // Register the driver in LVGL and save the created input device object.
  // TODO: verify result is not null (error)
  lv_indev_drv_register(&indev_drv);
}

// DMA completion callback from the tft_driver.
void dma_completion_irq_cb() { lv_disp_flush_ready(&disp_drv); }

// Called once from main on program start.
void setup() {
  tft_driver::begin();

  // Init gpio
  // constexpr uint kInputMask = 1ul << TFT_SYNC_PIN;
  // gpio_init_mask(kInputMask);
  // gpio_set_dir_in_masked(kInputMask);

  lv_init();

  init_display_driver();
  init_touch_driver();
}

void dump_stats() {
  lv_mem_monitor_t lv_info;
  lv_mem_monitor(&lv_info);
  printf("total_size=%lu, free_cnt=%lu, free_size=%lu, free_bigest_size=%lu\n",
         lv_info.total_size, lv_info.free_cnt, lv_info.free_size,
         lv_info.free_biggest_size);

  printf("used_cnt=%lu, max_used=%lu, used_pct=%hu, frag_pct=%hu\n",
         lv_info.used_cnt, lv_info.max_used, lv_info.used_pct,
         lv_info.frag_pct);
}

// For developer's usage. Dump the current screen.
void start_screen_capture() {
  screen_capture_enabled = true;
  printf("###BEGIN screen capture\n");
}

void stop_screen_capture() {
  screen_capture_enabled = false;
  printf("###END screen capture\n");
}

void set_backlight(uint8_t percents) { tft_driver::set_backlight(percents); }

}  // namespace lvgl_adapter
