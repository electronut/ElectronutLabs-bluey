#include "nrf.h"
#include "bsp.h"
#include "ff.h"
#include "diskio_blkdev.h"
#include "nrf_block_dev_sdc.h"

#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define SDC_SCK_PIN     25 //ARDUINO_13_PIN  ///< SDC serial clock (SCK) pin.
#define SDC_MOSI_PIN    23 //ARDUINO_11_PIN  ///< SDC serial data in (DI) pin.
#define SDC_MISO_PIN    24 //ARDUINO_12_PIN  ///< SDC serial data out (DO) pin.
#define SDC_CS_PIN      22 //ARDUINO_10_PIN  ///< SDC chip select (CS) pin.

#define FILE_NAME   "BLUEY.TXT"
#define TEST_STRING "Sensor Data Logger Example.\r\n\n"

/*
 * function to log data in SD card.
*/
void sdcard_sensor_update_data(float temperature, float humidity, float lux, float accel_X, float accel_Y, float accel_Z, float gyro_X, float gyro_Y, float gyro_Z);
