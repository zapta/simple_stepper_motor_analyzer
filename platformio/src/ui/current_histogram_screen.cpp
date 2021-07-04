#include "current_histogram_screen.h"

#include "acquisition/analyzer.h"
#include "misc/hardware_config.h"
#include "ui.h"

static constexpr uint32_t kUpdateIntervalMillis = 200;

static const ui::ChartAxisConfig kXAxisConfig_2500ma = {
    .range = {.min = 0, .max = 2000},  // ignored
    .labels = "0\n500\n1000\n1500\n2000",
    .num_ticks = 5,
    .dividers = 3};

static const ui::ChartAxisConfig kYAxisConfig_2500ma = {
    .range = {.min = 0, .max = 2500},
    .labels = "2.5A\n2.0\n1.5\n1.0\n0.5\n0.0",
    .num_ticks = 6,
    .dividers = 4};

static const ui::ChartAxisConfig kYAxisConfig_3000ma = {
    .range = {.min = 0, .max = 3000},
    .labels = "3A\n2\n1\n0",
    .num_ticks = 4,
    .dividers = 5};

CurrentHistogramScreen::CurrentHistogramScreen(){};

void CurrentHistogramScreen::setup(uint8_t screen_num) {
  ui::create_screen(&screen_);
  ui::create_page_elements(screen_, "CURRENT BY STEPS/SEC", screen_num,
                           nullptr);
  const uint16_t sensor_max_milliamps =
      hardware_config::sensor_spec()->range_milliamps;
  const ui::ChartAxisConfig* y_axis_config = (sensor_max_milliamps > 2500)
                                                 ? &kYAxisConfig_3000ma
                                                 : &kYAxisConfig_2500ma;
  ui::create_histogram(screen_, analyzer::kNumHistogramBuckets,
                       kXAxisConfig_2500ma, *y_axis_config, &histogram_);
};

void CurrentHistogramScreen::on_load() {
  // Force display update on first loop.
  display_update_elapsed_.set(kUpdateIntervalMillis + 1);
};

void CurrentHistogramScreen::on_unload(){};

void CurrentHistogramScreen::on_event(ui_events::UiEventId ui_event_id) {
  switch (ui_event_id) {
    case ui_events::UI_EVENT_RESET:
      analyzer::reset_state();
      break;
    default:
      break;
  }
}

void CurrentHistogramScreen::loop() {
  // We update at a fixed rate.
  if (display_update_elapsed_.elapsed_millis() < kUpdateIntervalMillis) {
    return;
  }
  display_update_elapsed_.reset();

  // Sample acquisition state and update display.
  const analyzer::State* state = analyzer::sample_state();

  // Update all the histogram points.
  const hardware_config::SensorSpec* sensor_spec =
      hardware_config::sensor_spec();
  for (int i = 0; i < analyzer::kNumHistogramBuckets; i++) {
    uint64_t total_current_ticks = state->buckets[i].total_step_peak_currents;
    uint64_t steps = state->buckets[i].total_steps;
    uint16_t avg_peak_milliamps =
        steps > 0
            ? sensor_spec->adc_value_to_milliamps(total_current_ticks / steps)
            : 0;

    histogram_.lv_series->points[i] = avg_peak_milliamps;
  }

  lv_chart_refresh(histogram_.lv_chart);
}