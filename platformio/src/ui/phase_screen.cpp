#include "phase_screen.h"

#include "acquisition/analyzer.h"
#include "misc/hardware_config.h"
#include "ui.h"

// NOTE: Capture deviders are configured in capture_util.cpp.

// TODO: Make class member? Share with other screen?
static lv_point_t points[analyzer::kAdcCaptureBufferSize];

static const ui::ChartAxisConfigs kAxisConfigs{
    .y_range = {.min = -2500, .max = 2500},
    .x = {.labels = "-2.5A\n0\n2.5A",
          .num_ticks = 3,
          .dividers = 9,
          .minor_div_lines_mask = 0x03de},
    .y = {.labels = "2.5A\n0\n-2.5A",
          .num_ticks = 3,
          .dividers = 9,
          .minor_div_lines_mask = 0x03de}};

void PhaseScreen::setup(uint8_t screen_num) {
  ui::create_screen(&screen_);
  ui::create_page_elements(screen_, "PHASE PATTERNS", screen_num, nullptr);
  ui::create_polar_chart(screen_, kAxisConfigs, ui_events::UI_EVENT_SCALE,
                         &polar_chart_);
  adc_capture_controls_.setup(screen_);
  // We set the text dynamically when updating the display.
  ui::create_label(screen_, 0, 350, 180, "??", ui::kFontSmallText,
                   LV_LABEL_ALIGN_CENTER, LV_COLOR_SILVER, &scale_lable_);
  lv_label_set_long_mode(scale_lable_.lv_label, LV_LABEL_LONG_EXPAND);
  scale_lable_.set_click_event(ui_events::UI_EVENT_SCALE);
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
static lv_coord_t map_line_coord(int milliamps, lv_coord_t max_radius) {
  constexpr int kMaxScale = 2500;
  return max_radius + (lv_coord_t)((milliamps * max_radius) / kMaxScale);
}

void PhaseScreen::update_display() {
  adc_capture_controls_.update_display_from_state();

  scale_lable_.set_text(adc_capture_util::alternative_scale()
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
    // Currents in millamps [-2500, 2500].
    const int milliamps1 = sensor_spec->adc_value_to_milliamps(item->v1);
    const int milliamps2 = sensor_spec->adc_value_to_milliamps(item->v2);

    points[i].x = map_line_coord(milliamps1, polar_chart_.max_radius);
    points[i].y = map_line_coord(milliamps2, polar_chart_.max_radius);
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