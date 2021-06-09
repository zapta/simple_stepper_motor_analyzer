
#include "touch_driver.h"

#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"


#define FT6236_I2C_SLAVE_ADDR 0x38

// Dimensions of the touch panel
#define MAX_X ((uint16_t)480)
#define MAX_Y ((uint16_t)320)

/* Max detectable simultaneous touch points */
#define FT6X36_MAX_TOUCH_PNTS 2

/* Register of the current mode */
#define FT6X36_DEV_MODE_REG 0x00

/* Possible modes as of FT6X36_DEV_MODE_REG */
#define FT6X36_DEV_MODE_WORKING 0x00
#define FT6X36_DEV_MODE_FACTORY 0x04

#define FT6X36_DEV_MODE_MASK 0x70
#define FT6X36_DEV_MODE_SHIFT 4

/* Gesture ID register */
#define FT6X36_GEST_ID_REG 0x01

/* Possible values returned by FT6X36_GEST_ID_REG */
#define FT6X36_GEST_ID_NO_GESTURE 0x00
#define FT6X36_GEST_ID_MOVE_UP 0x10
#define FT6X36_GEST_ID_MOVE_RIGHT 0x14
#define FT6X36_GEST_ID_MOVE_DOWN 0x18
#define FT6X36_GEST_ID_MOVE_LEFT 0x1C
#define FT6X36_GEST_ID_ZOOM_IN 0x48
#define FT6X36_GEST_ID_ZOOM_OUT 0x49

/* Status register: stores number of active touch points (0, 1, 2) */
#define FT6X36_TD_STAT_REG 0x02
#define FT6X36_TD_STAT_MASK 0x0F
#define FT6X36_TD_STAT_SHIFT 0x00

/* Touch events */
#define FT6X36_TOUCH_EVT_FLAG_PRESS_DOWN 0x00
#define FT6X36_TOUCH_EVT_FLAG_LIFT_UP 0x01
#define FT6X36_TOUCH_EVT_FLAG_CONTACT 0x02
#define FT6X36_TOUCH_EVT_FLAG_NO_EVENT 0x03

#define FT6X36_TOUCH_EVT_FLAG_SHIFT 6
#define FT6X36_TOUCH_EVT_FLAG_MASK (3 << FT6X36_TOUCH_EVT_FLAG_SHIFT)

#define FT6X36_MSB_MASK 0x0F
#define FT6X36_MSB_SHIFT 0
#define FT6X36_LSB_MASK 0xFF
#define FT6X36_LSB_SHIFT 0

#define FT6X36_P1_XH_REG 0x03
#define FT6X36_P1_XL_REG 0x04
#define FT6X36_P1_YH_REG 0x05
#define FT6X36_P1_YL_REG 0x06

#define FT6X36_P1_WEIGHT_REG \
  0x07 /* Register reporting touch pressure - read only */
#define FT6X36_TOUCH_WEIGHT_MASK 0xFF
#define FT6X36_TOUCH_WEIGHT_SHIFT 0

#define FT6X36_P1_MISC_REG 0x08 /* Touch area register */

#define FT6X36_TOUCH_AREA_MASK \
  (0x04 << 4) /* Values related to FT6X36_Pn_MISC_REG */
#define FT6X36_TOUCH_AREA_SHIFT 0x04

#define FT6X36_P2_XH_REG 0x09
#define FT6X36_P2_XL_REG 0x0A
#define FT6X36_P2_YH_REG 0x0B
#define FT6X36_P2_YL_REG 0x0C
#define FT6X36_P2_WEIGHT_REG 0x0D
#define FT6X36_P2_MISC_REG 0x0E

/* Threshold for touch detection */
#define FT6X36_TH_GROUP_REG 0x80
#define FT6X36_THRESHOLD_MASK \
  0xFF /* Values FT6X36_TH_GROUP_REG : threshold related  */
#define FT6X36_THRESHOLD_SHIFT 0

#define FT6X36_TH_DIFF_REG 0x85 /* Filter function coefficients */

#define FT6X36_CTRL_REG 0x86 /* Control register */

#define FT6X36_CTRL_KEEP_ACTIVE_MODE \
  0x00 /* Will keep the Active mode when there is no touching */
#define FT6X36_CTRL_KEEP_AUTO_SWITCH_MONITOR_MODE                             \
  0x01 /* Switching from Active mode to Monitor mode automatically when there \
          is no touching */

#define FT6X36_TIME_ENTER_MONITOR_REG                                        \
  0x87 /* The time period of switching from Active mode to Monitor mode when \
          there is no touching */

#define FT6X36_PERIOD_ACTIVE_REG 0x88  /* Report rate in Active mode */
#define FT6X36_PERIOD_MONITOR_REG 0x89 /* Report rate in Monitor mode */

#define FT6X36_RADIAN_VALUE_REG                                              \
  0x91 /* The value of the minimum allowed angle while Rotating gesture mode \
        */

#define FT6X36_OFFSET_LEFT_RIGHT_REG \
  0x92 /* Maximum offset while Moving Left and Moving Right gesture */
#define FT6X36_OFFSET_UP_DOWN_REG \
  0x93 /* Maximum offset while Moving Up and Moving Down gesture */

#define FT6X36_DISTANCE_LEFT_RIGHT_REG \
  0x94 /* Minimum distance while Moving Left and Moving Right gesture */
#define FT6X36_DISTANCE_UP_DOWN_REG \
  0x95 /* Minimum distance while Moving Up and Moving Down gesture */

#define FT6X36_LIB_VER_H_REG 0xA1 /* High 8-bit of LIB Version info */
#define FT6X36_LIB_VER_L_REG 0xA2 /* Low 8-bit of LIB Version info */

#define FT6X36_CHIPSELECT_REG 0xA3 /* 0x36 for ft6236; 0x06 for ft6206 */

#define FT6X36_POWER_MODE_REG 0xA5
#define FT6X36_FIRMWARE_ID_REG 0xA6
#define FT6X36_RELEASECODE_REG 0xAF
#define FT6X36_PANEL_ID_REG 0xA8
#define FT6X36_OPMODE_REG 0xBC

// // Assuming landscape mode per memory access command 0x36.

namespace touch_driver {

bool ft6x06_i2c_read_8bit_register_until(uint8_t register_addr, uint8_t* data_buf, absolute_time_t deadline) {
  // uint8_t val = 0x3B;

  // TODO: Add timeout using i2c_read_blocking_until
  // i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep
  // master control of bus
  const int byte_written = i2c_write_blocking_until(
      i2c_default, FT6236_I2C_SLAVE_ADDR, &register_addr, 1, true, deadline);
  if (byte_written != 1) {
    return false;
  }
  const int bytes_read =
      i2c_read_blocking_until(i2c_default, FT6236_I2C_SLAVE_ADDR, data_buf, 1, false, deadline);
  return bytes_read == 1;
}

void test() {
    absolute_time_t deadline = make_timeout_time_ms(100);
  printf("\n");
  // Serial.println();
  uint8_t data_buf = 0;

  bool ok = ft6x06_i2c_read_8bit_register_until(FT6X36_PANEL_ID_REG, &data_buf, deadline);
  printf("FT6X36_PANEL_ID_REG: status=%d, value=%02hhx\n", (int)ok, data_buf);

  ok = ft6x06_i2c_read_8bit_register_until(FT6X36_CHIPSELECT_REG, &data_buf, deadline);
  printf("FT6X36_CHIPSELECT_REG: status=%d, value=%02hhx\n", (int)ok, data_buf);

  ok = ft6x06_i2c_read_8bit_register_until(FT6X36_DEV_MODE_REG, &data_buf, deadline);
  printf("FT6X36_DEV_MODE_REG: status=%d, value=%02hhx\n", (int)ok, data_buf);

  ok = ft6x06_i2c_read_8bit_register_until(FT6X36_FIRMWARE_ID_REG, &data_buf, deadline);
  printf("FT6X36_FIRMWARE_ID_REG: status=%d, value=%02hhx\n", (int)ok, data_buf);

  ok = ft6x06_i2c_read_8bit_register_until(FT6X36_RELEASECODE_REG, &data_buf, deadline);
  printf("FT6X36_RELEASECODE_REG: status=%d, value=%02hhx\n", (int)ok, data_buf);
}

// TODO: return last x, y in case of an error, as the ESP32 port does.
void touch_read(uint16_t* x, uint16_t* y, bool* is_pressed) {

const absolute_time_t deadline = make_timeout_time_ms(5);

  // uint8_t data_xy[4];         // 2 bytes X | 2 bytes Y

  static int16_t last_x = 0;
  static int16_t last_y = 0;

  // Set default for cases of an early exit.
  *x = last_x;
  *y = last_y;
  *is_pressed = false;

  uint8_t touch_pnt_cnt;  // Number of detected touch points

  // ft6x06_i2c_read8(current_dev_addr, FT6X36_TD_STAT_REG, &touch_pnt_cnt);
  ft6x06_i2c_read_8bit_register_until(FT6X36_TD_STAT_REG, &touch_pnt_cnt, deadline);
  if (touch_pnt_cnt != 1) {  // ignore no touch & multi touch
  
    return;
  }

  // Continue to read the x,y

  uint8_t tx_bfr1[] = {FT6X36_P1_XH_REG};

  const int bytes_written1 = i2c_write_blocking_until(
      i2c_default, FT6236_I2C_SLAVE_ADDR, tx_bfr1, sizeof(tx_bfr1), true, deadline);
  if (bytes_written1 != sizeof(tx_bfr1)) {
    return;
  }


  uint8_t rx_bfr1[2];

  const int bytes_read1 = i2c_read_blocking_until(i2c_default, FT6236_I2C_SLAVE_ADDR,
                                            rx_bfr1, sizeof(rx_bfr1), false, deadline);

  if (bytes_read1 != sizeof(rx_bfr1)) {
    return;
  }
 

  uint8_t tx_bfr2[] = {FT6X36_P1_YH_REG};


  const int bytes_written2 = i2c_write_blocking_until(
      i2c_default, FT6236_I2C_SLAVE_ADDR, tx_bfr2, sizeof(tx_bfr2), true, deadline);

  if (bytes_written2 != sizeof(tx_bfr2)) {
    return;
  }

  uint8_t rx_bfr2[2];



  const int bytes_read2 = i2c_read_blocking_until(i2c_default, FT6236_I2C_SLAVE_ADDR,
                                            rx_bfr2, sizeof(rx_bfr2), false, deadline);

  if (bytes_read2 != sizeof(rx_bfr2)) {
    return;
  }

  // Note that we swap x, y.
  last_y =
      ((rx_bfr1[0] & FT6X36_MSB_MASK) << 8) | (rx_bfr1[1] & FT6X36_LSB_MASK);
  last_x =
      ((rx_bfr2[0] & FT6X36_MSB_MASK) << 8) | (rx_bfr2[1] & FT6X36_LSB_MASK);

  // invert x.
  last_x = MAX_X - last_x;

  *x = last_x;
  *y = last_y;
  *is_pressed = true;
}

void setup() {
  // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
  i2c_init(i2c0, 100 * 1000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);

  gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);


}

}  // namespace touch_driver
