#include "steps_chart_screen.h"

#include <stdio.h>

#include "acquisition/analyzer.h"
#include "io.h"
#include "ui.h"
#include "ui_events.h"

// Axis configuration below assumes 20 samples/secs and 
// 200 data point on the x axis.
static_assert(analyzer::kStepsCaptursPerSec == 20);
static_assert(StepsChartScreen::kNumPoints == 200);

// Update the steps field once every N time the chart is updated.
static constexpr int kFieldUpdateRatio = 5;

static const ui::ChartAxisConfig kXAxisConfig{
    .range = {.min = 0, .max = 10},
    .labels = "0\n2s\n4s\n6s\n8s\n10s",
    .num_ticks = 4,
    .dividers = 4};

const ui::ChartAxisConfig kYAxisConfigs[] = {
    // Scale 0 (default)
    {.range = {.min = 0, .max = 200},
     .labels = "200\n150\n100\n50\n0",
     .num_ticks = 3,
     .dividers = 3},

    // Scale 1
    {.range = {.min = 0, .max = 1000},
     .labels = "1k\n750\n500\n250\n0",
     .num_ticks = 3,
     .dividers = 3},

    // Scale 2
    {.range = {.min = 0, .max = 5000},
     .labels = "5k\n4k\n3k\n2k\n1k\n0",
     .num_ticks = 4,
     .dividers = 4}};

constexpr int kNumYScales = sizeof(kYAxisConfigs) / sizeof(kYAxisConfigs[0]);

StepsChartScreen::StepsChartScreen(){};

void StepsChartScreen::setup(uint8_t screen_num) {
  y_offset_ = 0;
  // Default Y scale.
  scale_index_ = 0;
  field_update_divider_ = kFieldUpdateRatio;
  points_buffer_.clear();
  ui::create_screen(&screen_);
  ui::create_page_elements(screen_, "STEPS  CHART", screen_num, nullptr);
  ui::create_chart(screen_, kNumPoints, 1, kXAxisConfig,
                   kYAxisConfigs[scale_index_], ui_events::UI_EVENT_SCALE,
                   &chart_);

  ui::create_label(screen_, 110, 120, 293, "", ui::kFontNumericDataFields,
                   LV_LABEL_ALIGN_CENTER, LV_COLOR_YELLOW, &steps_field_);
};

void StepsChartScreen::on_load() {
  chart_.ser1.clear();
  points_buffer_.clear();
};

void StepsChartScreen::on_unload(){};

void StepsChartScreen::on_event(ui_events::UiEventId ui_event_id) {
  switch (ui_event_id) {
    case ui_events::UI_EVENT_RESET:
      // TODO: define a clear() method and use also in setup().
      y_offset_ = 0;
      field_update_divider_ = kFieldUpdateRatio;
      steps_field_.set_text("");
      points_buffer_.clear();
      chart_.ser1.clear();
      break;

    case ui_events::UI_EVENT_SCALE:
      scale_index_++;
      if (scale_index_ >= kNumYScales) {
        scale_index_ = 0;
      }
      // Data will be scaled on the next loop().
      chart_.set_scale(kXAxisConfig, kYAxisConfigs[scale_index_]);
      lv_chart_refresh(chart_.lv_chart);
    default:
      break;
  }
}

void StepsChartScreen::loop() {
  const analyzer::StepsCaptureBuffer* steps_sample =
      analyzer::sample_steps_capture();

  if (steps_sample->is_empty()) {
    return;
  }

  LED2_ON;

  // Add the new items to points_buffer_, adjusting y range
  // as needed.
  const int new_items = steps_sample->size();
  for (int i = 0; i < new_items; i++) {
    // int32_t abs_steps = state->full_steps;
    int32_t abs_steps = steps_sample->get(i)->full_steps;

    // Start from the zero line.
    if (points_buffer_.is_empty()) {
      y_offset_ = -abs_steps;
    }

    // Shift the point into our local buffer.
    *points_buffer_.insert() = abs_steps;

    const ui::Range& y_range = kYAxisConfigs[scale_index_].range;

    // Adjust offset if value got out of chart range.
    const int32_t rel_steps = abs_steps + y_offset_;
    if (rel_steps > y_range.max) {
      y_offset_ -= (rel_steps - y_range.max);
    } else if (rel_steps < y_range.min) {
      y_offset_ += (y_range.min - rel_steps);
    }

    // Every n points we update the steps numeric display.
    if (++field_update_divider_ >= kFieldUpdateRatio) {
      field_update_divider_ = 0;
      steps_field_.set_text_int(abs_steps);
    }
  }

  // Update the chart with points in points_buffer_
  const uint16_t n = points_buffer_.size();

  for (uint16_t i = 0; i < n; i++) {
    const int32_t rel_point_steps = *points_buffer_.get(i) + y_offset_;

    const lv_coord_t chart_val = rel_point_steps;

    lv_chart_set_point_id(chart_.lv_chart, chart_.ser1.lv_series, chart_val, i);
  }

  lv_chart_refresh(chart_.lv_chart);

  // Force screen rendering now rather than waiting for the next LVGL screen
  // update timeslot.
  lv_refr_now(NULL);

  LED2_OFF;
}