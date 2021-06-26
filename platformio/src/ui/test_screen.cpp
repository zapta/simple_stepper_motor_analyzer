#include "test_screen.h"

#include <stdio.h>

#include "acquisition/analyzer.h"
#include "io.h"
#include "ui.h"

static const ui::ChartAxisConfigs kAxisConfigsNormal{
    .y_range = {.min = 0, .max = 1000},
    .x = {.labels = "0\n2s\n4s\n6s\n8s\n10s", .num_ticks = 4, .dividers = 4},
    .y = {.labels = "1k\n750\n500\n250\n0", .num_ticks = 3, .dividers = 3}};

TestScreen::TestScreen() {}

void TestScreen::construct_test_pattern() {
  for (int i = 0; i < kNumPoints; i++) {
    const int j = i % 40;
    const int r = j % 10;
    lv_coord_t v = 0;
    switch (j / 10) {
      case 0:
        v = r;
        break;
      case 1:
        v = 10 - r;
        break;
      case 2:
        v = -r;
        break;
      case 3:
        v = -10 + r;
        break;
    }
    *test_pattern_.insert() = 500 + 40 * v;
  }
}

void TestScreen::setup(uint8_t screen_num) {
  construct_test_pattern();

  ui::create_screen(&screen_);
  ui::create_chart(screen_, kNumPoints, 1, kAxisConfigsNormal,
                   ui_events::UI_EVENT_SCALE, &chart_);
};

void TestScreen::on_load() {
  chart_.ser1.clear();
  // Force an immediate update.
  updates_elapsed_.set(1000);
};

void TestScreen::loop() {
  if (updates_elapsed_.elapsed_millis() < 50) {
    return;
  }

  updates_elapsed_.reset();

  LED2_ON;

  for (uint16_t i = 0; i < kNumPoints; i++) {
    const lv_coord_t chart_val = *test_pattern_.get(i);
    lv_chart_set_point_id(chart_.lv_chart, chart_.ser1.lv_series, chart_val, i);
  }

  // Move the first pattern point to the end.
  const lv_coord_t v = *test_pattern_.get(0);
  *test_pattern_.insert() = v;

  lv_chart_refresh(chart_.lv_chart);

  // Force screen rendering now rather than waiting for the next LVGL screen
  // update timeslot.
  lv_refr_now(NULL);

  LED2_OFF;
}