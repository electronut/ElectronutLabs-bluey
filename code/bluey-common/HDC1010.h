/*
  HDC1010.h

  nRF52 TWI interface for HDC1010 Temperature/Humidity sensor.

  Electronut Labs
  electronut.in
*/

#include <stdint.h>
#include <stdbool.h>
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bluey-twi.h"

// pins
#define DRDY_PIN                 12

// registers
#define HDC1010_ADDR            0x40
#define TEMP_REG                0x00
#define HUMIDITY_REG            0x01
#define CONFIG_REG              0x02
#define SERIAL_ID1              0XFB
#define SERIAL_ID2              0XFC
#define SERIAL_ID3              0XFD
#define MANUF_ID_REG            0XFE
#define DEV_ID_REG              0XFF

#define TEMP_OR_HUMID           0x0000        // CONFIG_REG value to measure either temperature or humidity
#define TEMP_AND_HUMID          0x1000        // CONFIG_REG value to measure both temperature and humidity

/*
 * read sensor registers.
*/
ret_code_t read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop);

// read Device ID register.
uint16_t HDC1010_dev_id(void);

// read Manufacturer ID register.
uint16_t HDC1010_get_manuf_id(void);

// initialize the sensor.
void HDC1010_init(uint16_t value);

// read sensor configuration
void HDC1010_read_config(uint16_t* data);

// read temperature values only.
float HDC1010_get_temp(void);

// read temperature values only. (raw)
void HDC1010_get_temp_raw(uint16_t* temp_val);

// read humidity values only.
float HDC1010_get_humid(void);

// read humidity values only (RAW values)
void HDC1010_get_humid_raw(uint16_t* humid_val);

// read temperature and humidity values (raw)
void HDC1010_get_temp_humid_raw(uint16_t* temp_val, uint16_t* humid_val);

// read temperature and humidity values.
void HDC1010_get_temp_humid(float *temperature, float *humidity);
