#include "retraction_chart_screen.h"

#include <limits>

#include "acquisition/analyzer.h"
#include "ui.h"
#include "ui_events.h"

// Axis configuration below assumes 20 samples/secs.
static_assert(analyzer::kStepsCaptursPerSec == 20);

// Update the retraction field once every N times the chart is updated.
static constexpr int kFieldUpdateRatio = 2;

static const ui::ChartAxisConfigs kScaleAxisConfigs[] = {
    {.y_range = {.min = 0, .max = 100},
     .x = {.labels = "0\n2s\n4s\n6s\n8s\n10s", .num_ticks = 4, .dividers = 4},
     .y = {.labels = "100\n80\n60\n40\n20\n0", .num_ticks = 4, .dividers = 4}},

    {.y_range = {.min = 0, .max = 500},
     .x = {.labels = "0\n2s\n4s\n6s\n8s\n10s", .num_ticks = 4, .dividers = 4},
     .y = {.labels = "500\n400\n300\n200\n100\n0",
           .num_ticks = 4,
           .dividers = 4}},

    {.y_range = {.min = 0, .max = 30},
     .x = {.labels = "0\n2s\n4s\n6s\n8s\n10s", .num_ticks = 4, .dividers = 4},
     .y = {.labels = "30\n20\n10\n0", .num_ticks = 2, .dividers = 2}}};

void RetractionChartScreen::setup(uint8_t screen_num) {
  // y_offset_ = 0;
  field_update_divider_ = kFieldUpdateRatio;
  // points_buffer_.clear();
  ui::create_screen(&screen_);
  ui::create_page_elements(screen_, "RETRACTION CHART", screen_num, nullptr);
  ui::create_chart(screen_, kNumPoints, 1, kScaleAxisConfigs[0],
                   ui_events::UI_EVENT_SCALE, &chart_);

  ui::create_label(screen_, 110, 120, 293, "", ui::kFontNumericDataFields,
                   LV_LABEL_ALIGN_CENTER, LV_COLOR_YELLOW, &retraction_field_);
};

void RetractionChartScreen::on_load() {
  chart_.ser1.clear();
};

void RetractionChartScreen::on_unload(){};

void RetractionChartScreen::on_event(ui_events::UiEventId ui_event_id) {
  switch (ui_event_id) {
    case ui_events::UI_EVENT_RESET:
      field_update_divider_ = kFieldUpdateRatio;
      retraction_field_.set_text("");

      chart_.ser1.clear();
      break;

    case ui_events::UI_EVENT_SCALE:
      // TODO: make 3 a const (derive from the config table size).
      scale_ = (scale_ + 1) % 3;
      chart_.set_scale(kScaleAxisConfigs[scale_]);
      lv_chart_refresh(chart_.lv_chart);

    default:
      break;
  }
}

void RetractionChartScreen::loop() {
  const analyzer::StepsCaptureBuffer* steps_sample =
      analyzer::sample_steps_capture();

  if (steps_sample->is_empty()) {
    return;
  }

  // Add the new samples to the chart.
  const int new_items = steps_sample->size();
  for (int i = 0; i < new_items; i++) {
    const analyzer::StepsCaptureItem* sample = steps_sample->get(i);

    int retraction_steps = sample->max_full_steps - sample->full_steps;

    // Limit range to avoid over/underflow.
    constexpr lv_coord_t kMaxLvCoord = std::numeric_limits<lv_coord_t>::max();
    constexpr lv_coord_t kMinLvCoord = std::numeric_limits<lv_coord_t>::min();
    if (retraction_steps > kMaxLvCoord) {
      retraction_steps = kMaxLvCoord;
    } else if (retraction_steps < kMinLvCoord) {
      retraction_steps = kMinLvCoord;
    }

    // Add a data point to the chart.
    chart_.ser1.set_next((lv_coord_t)retraction_steps);

    // We first update the chart and then the occasional field
    // update. This affects the ordering of the rendering and results in
    // somewhat more consistent chart rendering intervals (?).
    if (++field_update_divider_ >= kFieldUpdateRatio) {
      field_update_divider_ = 0;
      retraction_field_.set_text_int(retraction_steps);
    }
  }

  // Force screen rendering now rather than waiting for the next LVGL screen
  // update timeslot.
  lv_refr_now(NULL);
}