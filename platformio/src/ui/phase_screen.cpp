#include "phase_screen.h"

#include "acquisition/analyzer.h"
#include "misc/hardware_config.h"
#include "ui.h"

// NOTE: Capture deviders are configured in capture_util.cpp.

// TODO: Make class member? Share with other screen?
static lv_point_t points[analyzer::kAdcCaptureBufferSize];

static const ui::ChartAxisConfig kXAxisConfig_2500ma{
    .range = {.min = -2500, .max = 2500},  // ignored ?
    .labels = "-2.5A\n0\n2.5A",
    .num_ticks = 3,
    .dividers = 9,
    .minor_div_lines_mask = 0x03de};

static const ui::ChartAxisConfig kXAxisConfig_3000ma{
    .range = {.min = -3000, .max = 3000},  // ignored ?
    .labels = "-3A\n-2\n-1\n0\n1\n2\n3A",
    .num_ticks = 7,
    .dividers = 11,
    .minor_div_lines_mask = 0x0aaa};

static const ui::ChartAxisConfig kYAxisConfig_2500ma{
    .range = {.min = -2500, .max = 2500},  // ignored ?
    .labels = "2.5A\n0\n-2.5A",
    .num_ticks = 3,
    .dividers = 9,
    .minor_div_lines_mask = 0x03de};

static const ui::ChartAxisConfig kYAxisConfig_3000ma{
    .range = {.min = -3000, .max = 3000},  // ignored ?
    .labels = "3A\n2\n1\n0\n-1\n-2\n-3A",
    .num_ticks = 7,
    .dividers = 11,
    .minor_div_lines_mask = 0x0aaa};

void PhaseScreen::setup(uint8_t screen_num) {
  ui::create_screen(&screen_);
  ui::create_page_elements(screen_, "PHASE PATTERNS", screen_num, nullptr);
  // Select axis configs based on current sensor used.
  const ui::ChartAxisConfig* x_axis_config = &kXAxisConfig_2500ma;
  const ui::ChartAxisConfig* y_axis_config = &kYAxisConfig_2500ma;
  if (hardware_config::sensor_spec()->range_milliamps > 2500) {
    x_axis_config = &kXAxisConfig_3000ma;
    y_axis_config = &kYAxisConfig_3000ma;
  }
  xy_max_milliamps_ = x_axis_config->range.max;
  ui::create_polar_chart(screen_, *x_axis_config, *y_axis_config,
                         ui_events::UI_EVENT_SCALE, &polar_chart_);
  adc_capture_controls_.setup(screen_);
  // We set the text dynamically when updating the display.
  ui::create_label(screen_, 0, 350, 180, "??", ui::kFontSmallText,
                   LV_LABEL_ALIGN_CENTER, LV_COLOR_SILVER, &scale_label_);
  lv_label_set_long_mode(scale_label_.lv_label, LV_LABEL_LONG_EXPAND);
  scale_label_.set_click_event(ui_events::UI_EVENT_SCALE);
};

void PhaseScreen::on_load() {
  adc_capture_controls_.sync_button_to_state();
  update_display();
};

void PhaseScreen::on_event(ui_events::UiEventId ui_event_id) {
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

// Maps -2500 to +2500 ma to 0 to 2*max_radius.
static lv_coord_t map_line_coord(int milliamps, int max_milliamps,
                                 lv_coord_t max_radius) {
  return max_radius + (lv_coord_t)((milliamps * max_radius) / max_milliamps);
}

void PhaseScreen::update_display() {
  adc_capture_controls_.update_display_from_state();

  scale_label_.set_text(adc_capture_util::alternative_scale()
                            ? "Long Capture\ntime:  100ms"
                            : "Short Capture\ntime:  20ms");

  if (!adc_capture_util::has_data()) {
    lv_line_set_points(polar_chart_.lv_line, points, 0);
    return;
  }

  // Update both chart series with the new captured data.
  const hardware_config::SensorSpec* sensor_spec =
      hardware_config::sensor_spec();
  for (int i = 0; i < analyzer::kAdcCaptureBufferSize; i++) {
    const analyzer::AdcCaptureItem* item =
        adc_capture_util::capture_buffer()->items.get(i);
    // Currents in millamps.
    const int milliamps1 = sensor_spec->adc_value_to_milliamps(item->v1);
    const int milliamps2 = sensor_spec->adc_value_to_milliamps(item->v2);

    points[i].x = map_line_coord(milliamps1, xy_max_milliamps_,
                                 polar_chart_.max_radius_in_pixels);
    points[i].y = map_line_coord(milliamps2, xy_max_milliamps_,
                                 polar_chart_.max_radius_in_pixels);
  }

  // The line keeps a reference to our points buffer.
  lv_line_set_points(polar_chart_.lv_line, points,
                     analyzer::kAdcCaptureBufferSize);
}

void PhaseScreen::loop() {
  // Update capture enabled from button if needed.
  if (adc_capture_util::maybe_update_state_from_controls(
          adc_capture_controls_)) {
    update_display();
    return;
  }

  if (adc_capture_util::maybe_update_capture_data()) {
    update_display();
  }
}