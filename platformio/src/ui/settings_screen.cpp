#include "settings_screen.h"

#include <stdio.h>

#include "acquisition/analyzer.h"
#include "config.h"
#include "display/lvgl_adapter.h"
#include "misc/config_eeprom.h"
#include "misc/hardware_options.h"
#include "ui.h"
#include "ui_events.h"

static constexpr uint32_t kUpdateIntervalMillis = 350;

static constexpr const char* kFootnotFormat =
    "To calibrate, disconnect the stepper motors and\n"
    "press SET ZERO. Options: %s, firmware: %s.\n"
    "Pico SDK: %s. LVLG: %s" ;

// Must be static. LV keeps a reference to it.
static lv_style_t style;

void SettingsScreen::maybe_update_eeprom() {
  analyzer::Settings new_analyzer_settings;
  analyzer::get_settings(&new_analyzer_settings);
  const uint8_t new_backlight_percents = backlight_slider_.get_value();
  config_eeprom::write_settings(new_analyzer_settings, new_backlight_percents);
  printf("%s\n", config_eeprom::last_status);
  // Since the flash writing disable interrupts temporarily, analyzer
  // may loose a few samples so reset it just in case.
  analyzer::reset_state();
}

static bool is_reversed_direction() {
  analyzer::Settings acq_settings;
  analyzer::get_settings(&acq_settings);
  return acq_settings.reverse_direction;
}

static uint8_t get_backlight_percents() {
  uint8_t backlight_percents;
  config_eeprom::read_settings(nullptr, &backlight_percents);
  return backlight_percents;
}

// TODO: generalize and move to ui.cpp.
static void create_set_zero_button(lv_obj_t* lv_screen) {
  lv_obj_t* lv_button = lv_btn_create(lv_screen, NULL);
  lv_obj_set_size(lv_button, 130, 60);
  lv_obj_set_pos(lv_button, 320, 43);

  lv_style_init(&style);
  lv_style_set_radius(&style, LV_STATE_DEFAULT, 10);

  lv_style_set_outline_width(&style, LV_STATE_DEFAULT, 0);

  lv_obj_add_style(lv_button, LV_OBJ_PART_MAIN, &style);

  lv_obj_t* lv_label = lv_label_create(lv_button, NULL);
  lv_label_set_text(lv_label, "SET ZERO");

  lv_label_set_align(lv_label, LV_ALIGN_CENTER);

  const lv_event_cb_t event_cb =
      ui_events::get_event_handler(ui_events::UI_EVENT_ZERO_CALIBRATION);

  // Nulls are trapped.
  lv_obj_set_event_cb(lv_button, event_cb);

  lv_obj_set_style_local_bg_color(lv_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                  LV_COLOR_ORANGE);

  lv_obj_set_style_local_bg_color(lv_button, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,
                                  LV_COLOR_GREEN);

  lv_obj_set_style_local_text_color(lv_label, LV_LABEL_PART_MAIN,
                                    LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_border_color(lv_button, LV_LABEL_PART_MAIN,
                                      LV_STATE_DEFAULT, LV_COLOR_GRAY);
  lv_obj_set_style_local_border_width(lv_button, LV_LABEL_PART_MAIN,
                                      LV_STATE_DEFAULT, 4);
}

SettingsScreen::SettingsScreen(){};

void SettingsScreen::setup(uint8_t screen_num) {
  ui::create_screen(&screen_);

  ui::create_page_title(screen_, "SETTINGS", nullptr);

  create_set_zero_button(screen_.lv_screen);

  ui::create_button(screen_, 50, 410, ui::kBottomButtonsPosY - 10,
                    ui::kSymbolOk, LV_COLOR_GREEN,
                    ui_events::UI_EVENT_HOME_PAGE, nullptr);

  // We don't bother to keep references to the fixed labels.
  constexpr lv_coord_t w1 = 100;
  constexpr lv_coord_t w2 = 130;
  constexpr lv_coord_t x1 = 40;
  constexpr lv_coord_t x2 = 150;
  // constexpr lv_coord_t dy = 36;
  lv_coord_t y = 47;

  // NOTE: adding 1 to the y of the numeric fields to better align
  // with the font of the text fields.
  ui::create_label(screen_, w1, x1, y, "COIL A", ui::kFontDataFields,
                   LV_LABEL_ALIGN_LEFT, LV_COLOR_SILVER, nullptr);

  ui::create_label(screen_, w2, x2, y + 1, "", ui::kFontNumericDataFields,
                   LV_LABEL_ALIGN_RIGHT, LV_COLOR_SILVER, &ch_a_field_);
  y += 36;

  ui::create_label(screen_, w1, x1, y + 1, "COIL B", ui::kFontDataFields,
                   LV_LABEL_ALIGN_LEFT, LV_COLOR_SILVER, nullptr);
  ui::create_label(screen_, w2, x2, y, "", ui::kFontNumericDataFields,
                   LV_LABEL_ALIGN_RIGHT, LV_COLOR_SILVER, &ch_b_field_);

  ui::create_label(screen_, w2, x2, y, "", ui::kFontNumericDataFields,
                   LV_LABEL_ALIGN_RIGHT, LV_COLOR_SILVER, &ch_b_field_);

  y += 60;
  ui::create_checkbox(screen_, x1, y, "  REVERSED  STEPS  DIRECTION",
                      ui::kFontDataFields, LV_COLOR_SILVER,
                      ui_events::UI_EVENT_DIRECTION, &reverse_checkbox_);
  reverse_checkbox_.set_is_checked(is_reversed_direction());

  y += 70;
  // We don't let the brightness go all the way to zero to completly
  // disable the display.
  ui::create_slider(screen_, 200, x1 + 5, y, 5, 100, get_backlight_percents(),
                    &backlight_slider_);
  ui::create_label(screen_, 0, 270, y - 5, "BRIGHTNESS", ui::kFontDataFields,
                   LV_LABEL_ALIGN_LEFT, LV_COLOR_SILVER, nullptr);

  const char* footnote_text =
      format(kFootnotFormat, hardware_options::get_name(), VERSION_STRING,
             PICO_SDK_VERSION_STRING, LVGL_VERSION_INFO);
  ui::create_label(screen_, 0, 5, 275, footnote_text, ui::kFontSmallText,
                   LV_LABEL_ALIGN_LEFT, LV_COLOR_OLIVE, nullptr);
};

void SettingsScreen::on_load() {
  // Force display update on first loop.
  display_update_elapsed_.set(kUpdateIntervalMillis + 1);
  // Force the slider to the current settings.
  backlight_slider_.set_value(get_backlight_percents());
};

void SettingsScreen::on_unload() { maybe_update_eeprom(); };

// For the intensity slider we don't use events, just 
// polling.
void SettingsScreen::on_event(ui_events::UiEventId ui_event_id) {
  switch (ui_event_id) {
    case ui_events::UI_EVENT_ZERO_CALIBRATION:
      analyzer::calibrate_zeros();
      // update_eeprom();
      break;

    case ui_events::UI_EVENT_DIRECTION:
      analyzer::set_direction(reverse_checkbox_.is_checked());
      // update_eeprom();
      break;

    default:
      break;
  }
}

void SettingsScreen::loop() {
  // Force the display intensify follow the slider. No harm
  // in setting it too often since it's updated synchronosly
  // at the begining of each PWM cycle.
  lvgl_adapter::set_backlight(backlight_slider_.get_value());

  // We update the currents display at a fixed rate.
  if (display_update_elapsed_.elapsed_millis() < kUpdateIntervalMillis) {
    return;
  }
  display_update_elapsed_.reset();

  // Sample data and update screen.
  const analyzer::State* state = analyzer::sample_state();
  ch_a_field_.set_text_float(analyzer::adc_value_to_amps(state->v1), 2);
  ch_b_field_.set_text_float(analyzer::adc_value_to_amps(state->v2), 2);
}