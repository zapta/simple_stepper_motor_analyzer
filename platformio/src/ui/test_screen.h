// Not a user visible screen. For testing only.
#pragma once

#include "misc/circular_buffer.h"
#include "misc/elapsed.h"
#include "screen_manager.h"

class TestScreen : public screen_manager::Screen {
 public:
  TestScreen();
  virtual void setup(uint8_t screen_num) override;
  virtual void on_load() override;
  virtual void loop() override;

 private:
  static constexpr int16_t kNumPoints = 200;
  ui::Chart chart_;
  // TODO: Consider to allocate this from the heap to avoid
  // wasting this memory when not testing. (~400 bytes)
  CircularBuffer<lv_coord_t, kNumPoints> test_pattern_;
  Elapsed updates_elapsed_;

  void construct_test_pattern();
};