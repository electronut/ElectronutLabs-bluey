/*
	LSM6DS3.h

  nRF52 TWI interface for LSM6DS3 IMU.

  Electronut Labs
  electronut.in

 */

#include "stdlib.h"
#include "time.h"
#include "LSM6DS3.h"

static struct IMU_settings settings;

/*
 * function to test availablility of IMU
*/
void LSM6DS3_who_am_i(void)
{
  uint32_t err_code;
  uint8_t who_am_i = 0;
  err_code = read_register(p_twi_sensors, LSM6DS3_ADDR, WHO_AM_I, &who_am_i, 1, false);
  APP_ERROR_CHECK(err_code);
}


/*
 * init function
*/
void LSM6DS3_init(void)
{
  ret_code_t err_code;

  settings.accel_enable           = 1;      // 0 - Disable. 1 - Enable
  settings.accel_ODR              = 1;
  settings.accel_range            = 2;     // Full Scale(FS) range (in g). Select from: 2, 4, 8, 16
  settings.accel_samplerate       = 416;    // Hz. Select from: 13, 26, 52, 104, 208, 416, 833, 1666
  settings.accel_bandwidth        = 200;    // Hz. Select from: 50, 100, 200, 400
  settings.accel_FIFO_enable      = 0;      // Set to include accelerometer data in FIFO buffer
  settings.accel_FIFO_decimation  = 0;      // Set to activate.

  settings.gyro_enable            = 1;      // 0 - Disable. 1 - Enable
  settings.gyro_range             = 2000;   // Angular Rate range (in deg/s).  Can be: 125, 245, 500, 1000, 2000
	settings.gyro_samplerate        = 104;    // Hz. Select from: 13, 26, 52, 104, 208, 416, 833, 1666
	settings.gyro_bandwidth         = 400;    // Hz. Select from: 50, 100, 200, 400;
	settings.gyro_FIFO_enable       = 0;      // Set to include gyroscope data in FIFO buffer
	settings.gyro_FIFO_decimation   = 0;      // Set to activate.

	settings.temp_enable            = 0;      // Set to activate temperature measure

	//FIFO control data
	settings.FIFO_threshold 			  = 3000;    //Can be 0 to 4096 (16 bit bytes)
	settings.FIFO_samplerate 		    = 13;     //default 13Hz
	settings.FIFO_mode 			        = 6;      //Default off

  uint8_t tx_data[2] = {MASTER_CONFIG, 0x01};
  err_code = nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  // set IF_INC in CTRL3_C register.
  tx_data[0] = CTRL3_C;
  tx_data[1] = 0x04;
  err_code = nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  LSM6DS3_config();
}

/*
 * function to configure IMU parameters.
*/
void LSM6DS3_config(void)
{
  ret_code_t err_code;
  uint8_t tx_data[2];

  // configure accelerometer
  tx_data[0] = CTRL1_XL;
  tx_data[1] = 0;
  if(settings.accel_enable == 1) {
    // Bandwidth
    switch(settings.accel_bandwidth) {
      case 50:
              tx_data[1] |= LSM6DS3_IMU_BW_XL_50Hz;
              break;

      case 100:
              tx_data[1] |= LSM6DS3_IMU_BW_XL_100Hz;
              break;

      case 200:
              tx_data[1] |= LSM6DS3_IMU_BW_XL_200Hz;
              break;

      default:
      case 400:
              tx_data[1] |= LSM6DS3_IMU_BW_XL_400Hz;
              break;
    }

    // Full scale range
    switch(settings.accel_range) {
      case 2:
              tx_data[1] |= LSM6DS3_IMU_FS_XL_2g;
              break;

      case 4:
              tx_data[1] |= LSM6DS3_IMU_FS_XL_4g;
              break;

      case 8:
              tx_data[1] |= LSM6DS3_IMU_FS_XL_8g;
              break;

      default:
      case 16:
              tx_data[1] |= LSM6DS3_IMU_FS_XL_16g;
              break;
    }

    // ODR
    switch(settings.accel_samplerate) {
      case 13:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_13Hz;
              break;

      case 26:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_26Hz;
              break;

      case 52:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_52Hz;
              break;

      default:
      case 104:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_104Hz;
              break;

      case 208:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_208Hz;
              break;

      case 416:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_416Hz;
              break;

      case 833:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_833Hz;
              break;

      case 1660:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_1660Hz;
              break;

      case 3330:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_3330Hz;
              break;

      case 6660:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_6660Hz;
              break;

      case 13330:
              tx_data[1] |= LSM6DS3_IMU_ODR_XL_13330Hz;
              break;
    }
  }
  else {
    tx_data[1] = 0;
  }

  err_code = nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  // configure gyroscope
  tx_data[0] = CTRL2_G;
  tx_data[1] = 0;
  if(settings.gyro_enable == 1) {
    // range
    switch(settings.gyro_range) {
      case 125:
              tx_data[1] |= LSM6DS3_IMU_FS_125_ENABLED;
              break;

      case 245:
              tx_data[1] |= LSM6DS3_IMU_FS_G_245dps;
              break;

      case 500:
              tx_data[1] |= LSM6DS3_IMU_FS_G_500dps;
              break;

      case 1000:
              tx_data[1] |= LSM6DS3_IMU_FS_G_1000dps;
              break;

      default:
      case 2000:
              tx_data[1] |= LSM6DS3_IMU_FS_G_2000dps;
              break;
    }

    // ODR
    switch(settings.gyro_samplerate) {
      case 13:
              tx_data[1] |= LSM6DS3_IMU_ODR_G_13Hz;
              break;

      case 26:
              tx_data[1] |= LSM6DS3_IMU_ODR_G_26Hz;
              break;

      case 52:
              tx_data[1] |= LSM6DS3_IMU_ODR_G_52Hz;
              break;

      default:
      case 104:
              tx_data[1] |= LSM6DS3_IMU_ODR_G_104Hz;
              break;

      case 208:
              tx_data[1] |= LSM6DS3_IMU_ODR_G_208Hz;
              break;

      case 416:
              tx_data[1] |= LSM6DS3_IMU_ODR_G_416Hz;
              break;

      case 833:
              tx_data[1] |= LSM6DS3_IMU_ODR_G_833Hz;
              break;

      case 1660:
              tx_data[1] |= LSM6DS3_IMU_ODR_G_1660Hz;
              break;
    }
  }
  else {
    tx_data[1] = 0;
  }

  err_code = nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);
}

/*
 * function to read accelerometer data
*/
void LSM6DS3_read_accl_data(int16_t *x_axis, int16_t *y_axis, int16_t *z_axis)
{
  ret_code_t err_code;
  uint8_t status = 0;
  uint8_t data[6];

  do {
    err_code = read_register(p_twi_sensors, LSM6DS3_ADDR, STATUS_REG, &status, sizeof(status), false);
  } while(!(status & 0x01));

  err_code = read_register(p_twi_sensors, LSM6DS3_ADDR, OUTX_L_XL, data, sizeof(data), true);
  APP_ERROR_CHECK(err_code);
  *x_axis = (data[1] << 8) | data[0];
  *y_axis = (data[3] << 8) | data[2];
  *z_axis = (data[5] << 8) | data[4];
}

/*
 * function to compute raw accelerometer data in g
*/
float LSM6DS3_accelData_in_g(int16_t raw_data)
{
  return ((float) ((raw_data * 0.061 * (settings.accel_range >> 1)) / 1000 ));
}

/*
 * function to compute raw gyroscope data in degrees per second (dps)
*/
float LSM6DS3_gyroData_in_dps(int16_t raw_data)
{
  uint8_t gyro_range_divisor;

  if (settings.gyro_range == 245) {
    gyro_range_divisor = 2;
  }
  else {
    gyro_range_divisor = settings.gyro_range / 125;
  }

  return ((float) ((raw_data * 4.375 * gyro_range_divisor) / 1000 ));
}

/*
 * functioon to read gyroscope data
*/
void LSM6DS3_read_gyro_data(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z)
{
  ret_code_t err_code;
  uint8_t status = 0;
  uint8_t data[6];

  do {
    //err_code = read_register(LSM6DS3_ADDR, STATUS_REG, &status, sizeof(status));
    err_code = read_register(p_twi_sensors, LSM6DS3_ADDR, STATUS_REG, &status, sizeof(status), false);
  } while(!(status & 0x02));

  //err_code = LSM6DS3_read_register(LSM6DS3_ADDR, OUTX_L_XL, data, sizeof(data));
  err_code = read_register(p_twi_sensors, LSM6DS3_ADDR, OUTX_L_G, data, sizeof(data), true);
  APP_ERROR_CHECK(err_code);
  *gyro_x = (data[1] << 8) | data[0];
  *gyro_y = (data[3] << 8) | data[2];
  *gyro_z = (data[5] << 8) | data[4];
}
//#endif

/*
 * function to configure FIFO
*/
void LSM6DS3_FIFO_config(void)
{
  ret_code_t err_code;
  uint8_t tx_data[2];

  // masking the threshold value in FIFO_CTRL1 register.
  tx_data[0] = FIFO_CTRL1;
  tx_data[1] = settings.FIFO_threshold & 0x00FF;
  err_code = nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  // masking the threshold value in FIFO_CTRL2 register.
  tx_data[0] = FIFO_CTRL2;
  tx_data[1] = (settings.FIFO_threshold & 0x0F00) >> 8;
  err_code = nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  // set up decimation factor for accelerometer and gyroscope
  tx_data[0] = FIFO_CTRL3;
  tx_data[1] = 0;
  if(settings.accel_FIFO_enable == 1) {
    tx_data[1] |= (settings.accel_FIFO_decimation & 0x07);
  }

  if (settings.gyro_FIFO_enable == 1) {
    tx_data[1] |= ((settings.gyro_FIFO_decimation & 0x07) << 3);
  }
  err_code = nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  // configure sensor hub (if any)
  // set decimation and ONLY_HIGH_DATA bit here
  // tx_data[0] = FIFO_CTRL4;

  // configure FIFO_CTRL5 register
  tx_data[0] = FIFO_CTRL5;
  // set FIFO ODR
  tx_data[1] = 0;
  switch(settings.FIFO_samplerate) {
    default:
    case 13:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_13Hz;
            break;

    case 26:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_26Hz;
            break;

    case 52:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_52Hz;
            break;

    case 104:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_104Hz;
            break;

    case 208:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_208Hz;
            break;

    case 416:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_416Hz;
            break;

    case 833:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_833Hz;
            break;

    case 1660:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_1660Hz;
            break;

    case 3330:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_3330Hz;
            break;

    case 6660:
            tx_data[1] |= LSM6DS3_IMU_ODR_FIFO_6660Hz;
            break;
  }

  // set FIFO mode
  switch(settings.FIFO_mode) {
    default:
    case 0:
          tx_data[1] |= LSM6DS3_IMU_FIFO_MODE_BYPASS;
          break;

    case 1:
            tx_data[1] |= LSM6DS3_IMU_FIFO_MODE_FIFO;
            break;

    case 3:
            tx_data[1] |= LSM6DS3_IMU_FIFO_MODE_STF;
            break;

    case 4:
            tx_data[1] |= LSM6DS3_IMU_FIFO_MODE_BTS;
            break;

    case 6:
            tx_data[1] |= LSM6DS3_IMU_FIFO_MODE_STREAM;
            break;
  }
  err_code = nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);
}

/*
 * function to read FIFO status
*/
uint16_t LSM6DS3_read_FIFO_status(void)
{
  ret_code_t err_code;
  uint8_t data[2];
  //uint8_t tx_data[2];
  uint16_t status = 0;

  /*// set BDU bit in CTRL3_C registers
  tx_data[0] = CTRL3_C;
  tx_data[1] = 0x40;
  err_code = nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);*/

  // read FIFO_STATUS1 and FIFO_STATUS2 registers
  err_code = read_register(p_twi_sensors, LSM6DS3_ADDR, FIFO_STATUS1, data, sizeof(data), true);
  APP_ERROR_CHECK(err_code);
  status = (data[1] << 8) | data[0];

  return status;
}

/*
 * function to read FIFO buffer
*/
int16_t LSM6DS3_read_FIFO_buffer()
{
  ret_code_t err_code;
  uint8_t read_data[2];
  int16_t data = 0;

  // read FIFO_DATA_OUT_L and FIFO_DATA_OUT_H registers
  err_code = read_register(p_twi_sensors, LSM6DS3_ADDR, FIFO_DATA_OUT_L, read_data, sizeof(read_data), true);
  APP_ERROR_CHECK(err_code);
  data = (read_data[1] << 8) | read_data[0];

  return data;
}

/*
 * function to empty FIFO buffer
*/
void LSM6DS3_clear_FIFO_buffer(void)
{
  // Read FIFO data and dump it.
  while((LSM6DS3_read_FIFO_status() & 0x1000) == 0) {
    LSM6DS3_read_FIFO_buffer();
  }
}

/*
 * function to configure tap functionality
*/
void LSM6DS3_tap_detect_config()
{
  ret_code_t err_code;
  uint8_t tx_data[2];

  // Enable tap detection on X, Y, Z axis, but do not latch output
  tx_data[0] = TAP_CFG;
  tx_data[1] = 0x0E;
  err_code =  nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  // Set tap threshold
  tx_data[0] = TAP_THRS_6D;
  tx_data[1] = 0x03;
  err_code =  nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  // Set Duration, Quiet and Shock time windows
  tx_data[0] = INT_DUR2;
  tx_data[1] = 0x7F;
  err_code =  nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  // Single & Double tap enabled (SINGLE_DOUBLE_TAP = 1)
  tx_data[0] = WAKE_UP_THRS;
  tx_data[1] = 0x80;
  err_code =  nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

  // Single tap interrupt driven to INT1 pin -- enable latch
  tx_data[0] = MD2_CFG;
  tx_data[1] = 0x48;
  err_code =  nrf_drv_twi_tx(&p_twi_sensors, LSM6DS3_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);
}
