#include "speed_gauge_screen.h"

#include <stdio.h>

#include "acquisition/analyzer.h"
//#include "io.h"
#include "ui.h"

constexpr uint16_t kUpdateIntervalMillis = 75;

// Update speed label only every this number of gauge
// updates. Should allow more time to read the text.
constexpr uint8_t kLabelUpdateRatio = 500 / kUpdateIntervalMillis;

static const ui::GaugeAxisConfig kAxisConfigNormal = {
    .min = -2000, .max = 2000, .major_intervals = 4, .minor_intervals = 5};

static const ui::GaugeAxisConfig kAxisConfigAlternative = {
    .min = -500, .max = 500, .major_intervals = 4, .minor_intervals = 5};

SpeedGaugeScreen::SpeedGaugeScreen(){};

void SpeedGaugeScreen::setup(uint8_t screen_num) {
  ui::create_screen(&screen_);
  ui::create_page_elements(screen_, "STEPS / SEC", screen_num, nullptr);
  ui::create_gauge(screen_, kAxisConfigNormal, ui_events::UI_EVENT_SCALE,
                   &gauge_);
  ui::create_label(screen_, 180, 270, 200, "", ui::kFontLargeNumericFields,
                   LV_LABEL_ALIGN_RIGHT, LV_COLOR_YELLOW, &speed_field_);
};

void SpeedGaugeScreen::on_load() {
  recent_full_step_samples_.clear();
  // This avoids value flicker on screen switch.
  speed_field_.set_text("");
  // Force label update on first guage update.
  label_update_divider_ = kLabelUpdateRatio;
};

void SpeedGaugeScreen::on_unload(){};

void SpeedGaugeScreen::on_event(ui_events::UiEventId ui_event_id) {
  switch (ui_event_id) {
    case ui_events::UI_EVENT_SCALE:
      printf("Gauge scale\n");
      alternative_scale_ = !alternative_scale_;
      gauge_.set_scale(alternative_scale_ ? kAxisConfigAlternative
                                          : kAxisConfigNormal);
      break;

    // This makes the compiler happy.
    default:
      break;
  }
}

void SpeedGaugeScreen::loop() {
  const analyzer::StepsCaptureBuffer* new_samples =
      analyzer::sample_steps_capture();
  if (new_samples->is_empty()) {
    return;
  }
 

  // Append new samples to the local buffer.
  const int new_samples_count = new_samples->size();
  for (int i = 0; i < new_samples_count; i++) {
    *recent_full_step_samples_.insert() = new_samples->get(i)->full_steps;
  }

  // We need at least two readings to estimate speed.
  const int samples_count = recent_full_step_samples_.size();
  if (samples_count < 2) {
    return;
  }

  //LED2_ON;


  // Should be >= 1 per the condition above.
  // TODO: Consider to change to weighted average, giving more weight to recent
  // readings.
  const int interval_count = MIN(5, samples_count - 1);
  const int delta_steps =
      *recent_full_step_samples_.get(samples_count - 1) -
      *recent_full_step_samples_.get(samples_count - 1 - interval_count);
  const int steps_per_sec =
      (delta_steps * (int)analyzer::kStepsCaptursPerSec) / interval_count;
  

  lv_gauge_set_value(gauge_.lv_gauge, 0, steps_per_sec);

  label_update_divider_++;
  if (label_update_divider_ >= kLabelUpdateRatio) {
    speed_field_.set_text_int(steps_per_sec);
    label_update_divider_ = 0;
  }

  // Force screen update for consistent update rate.
  lv_refr_now(NULL);

  //LED2_OFF;
};