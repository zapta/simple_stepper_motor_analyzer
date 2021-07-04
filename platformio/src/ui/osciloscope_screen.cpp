#include "osciloscope_screen.h"

#include "acquisition/analyzer.h"
#include "misc/hardware_config.h"
#include "ui.h"

// NOTE: Capture deviders are configured in adc_capture_util.cpp.

static const ui::ChartAxisConfig kYAxisConfig{
    .range = {.min = -2500, .max = 2500},

    .labels = "2.5A\n0\n-2.5A",
    .num_ticks = 3,
    .dividers = 9,
    .minor_div_lines_mask = 0x03de};

static const ui::ChartAxisConfig kXAxisConfigNormal{
    .range = {.min = 0, .max = 20},  // ignored
    .labels = "0\n5ms\n10ms\n15ms\n20ms",
    .num_ticks = 5,
    .dividers = 19,
    .minor_div_lines_mask = 0xf7bde};

static const ui::ChartAxisConfig kXAxisConfigAlternative{
    .range = {.min = 0, .max = 100},  // ignored
    .labels = "0\n20ms\n40ms\n60ms\n80ms\n100ms",
    .num_ticks = 6,
    .dividers = 19,
    .minor_div_lines_mask = 0xeeeee};

void OsciloscopeScreen::setup(uint8_t screen_num) {
  ui::create_screen(&screen_);
  ui::create_page_elements(screen_, "CURRENT PATTERNS", screen_num, nullptr);
  ui::create_chart(screen_, analyzer::kAdcCaptureBufferSize, 2,
                   kXAxisConfigNormal, kYAxisConfig, ui_events::UI_EVENT_SCALE,
                   &chart_);
  adc_capture_controls_.setup(screen_);
};

void OsciloscopeScreen::on_load() {
  adc_capture_controls_.sync_button_to_state();
  update_display();
};

void OsciloscopeScreen::on_event(ui_events::UiEventId ui_event_id) {
  switch (ui_event_id) {
    case ui_events::UI_EVENT_RESET:
      adc_capture_util::clear_data();
      adc_capture_controls_.sync_button_to_state();
      update_display();
      break;

    case ui_events::UI_EVENT_SCALE: {
      adc_capture_util::toggle_scale();
      adc_capture_controls_.sync_button_to_state();
      update_display();
    } break;

    // This makes the compiler happy.
    default:
      break;
  }
}

// Update chart from shared state.
void OsciloscopeScreen::update_display() {
  // TODO: can we skip this most of the times? Is it expensive?
  chart_.set_scale(adc_capture_util::alternative_scale()
                       ? kXAxisConfigAlternative
                       : kXAxisConfigNormal,
                   kYAxisConfig);
  adc_capture_controls_.update_display_from_state();

  // No capture data.
  if (!adc_capture_util::has_data()) {
    chart_.ser1.clear();
    chart_.ser2.clear();
    lv_chart_refresh(chart_.lv_chart);
    return;
  }

  // Has capture data.
  const analyzer::AdcCaptureBuffer* capture_buffer =
      adc_capture_util::capture_buffer();
  const hardware_config::SensorSpec* sensor_spec =
      hardware_config::sensor_spec();
  for (int i = 0; i < analyzer::kAdcCaptureBufferSize; i++) {
    const analyzer::AdcCaptureItem* item = capture_buffer->items.get(i);
    // Currents in millamps [-2500, 2500].
    const int milliamps1 = sensor_spec->adc_value_to_milliamps(item->v1);
    const int milliamps2 = sensor_spec->adc_value_to_milliamps(item->v2);

    lv_chart_set_point_id(chart_.lv_chart, chart_.ser1.lv_series, milliamps1,
                          i);
    lv_chart_set_point_id(chart_.lv_chart, chart_.ser2.lv_series, milliamps2,
                          i);
  }

  // Chart is dirty. Mark it for refresh.
  lv_chart_refresh(chart_.lv_chart);
}

void OsciloscopeScreen::loop() {
  // Update capture enabled if needed.
  if (adc_capture_util::maybe_update_state_from_controls(
          adc_capture_controls_)) {
    update_display();
    return;
  }

  if (adc_capture_util::maybe_update_capture_data()) {
    update_display();
  }
}