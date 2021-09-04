#pragma once

#include "misc/circular_buffer.h"
#include "screen_manager.h"

class StepsChartScreen : public screen_manager::Screen {
 public:
  StepsChartScreen();
  virtual void setup(uint8_t screen_num) override;
  virtual void on_load() override;
  virtual void on_unload() override;
  virtual void loop() override;
  virtual void on_event(ui_events::UiEventId ui_event_id) override;

  // NOTE: Chart's area width is 400 pixels. Note that
  // the more points we have, the slower is the LVGL 
  // in memory rendering.
  static constexpr int16_t kNumPoints = 200;

 private:
  // Counter to update the steps field once every N chart updates.
  uint8_t field_update_divider_ = 0;
  ui::Label steps_field_;
  ui::Chart chart_;
  // We add this value to the steps value to make the
  // chart scroll vertically in case of a Y over/underflow.
  int32_t y_offset_ = 0;
  // We keep our own buffer of point values as int32. This
  // way we don't risk an over/underflow if we will use the
  // Chart's int16 point values when we rebase the Y range.
  CircularBuffer<int32_t, kNumPoints> points_buffer_;
  // Index into kYAxisConfigs.
  int scale_index_ = 0;
};