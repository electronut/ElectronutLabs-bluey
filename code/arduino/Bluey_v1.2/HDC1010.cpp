/**
 * HDC1010.c
 *
 * nRF52 TWI interface for HDC1010 Temperature/Humidity sensor.
 *
 * Electronut Labs
 * electronut.in
*/
#include "bluey_wire.h"
#include "HDC1010.h"

const float pow_16 = 65536.0f;

// uncomment for printf traces
// #define ENABLE_PRINTF_TRACES

uint16_t HDC1010_readData(uint8_t dev_addr, uint8_t reg_addr, uint8_t num_bytes)
{
  uint8_t msb, lsb;

  Wire.beginTransmission(dev_addr);
	Wire.write(reg_addr);
	Wire.endTransmission();

	delay(10);

  Wire.requestFrom(dev_addr, (uint8_t)2);

	msb = Wire.read();
	lsb = Wire.read();

	return (msb << 8 | lsb);
}

/**
 * @brief initialize the sensor.
 */
void HDC1010_init(uint16_t reg_data)
{
  uint8_t tx_data[2];
  tx_data[0] = reg_data >> 8;      // higher byte
  tx_data[1] = reg_data & 0xff;    // lower byte
  // configure sensor to measure temperature and humidity with 14 bit resolution each.
  writeRegister(HDC1010_ADDR, CONFIG_REG, tx_data);
}

/**
 * @brief read sensor configuration
 */
void HDC1010_read_config(uint16_t* data)
{
  uint8_t reg_data[2];
/*
  err_code = nrf_drv_twi_tx(&p_twi_sensors, HDC1010_ADDR, &tx_buffer, 1, false);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_twi_rx(&p_twi_sensors, HDC1010_ADDR, reg_data, sizeof(reg_data));
  APP_ERROR_CHECK(err_code);
*/

  readRegister(HDC1010_ADDR, CONFIG_REG, 2, reg_data);

  *data = (reg_data[0] << 8) | reg_data[1];

  #ifdef ENABLE_PRINTF_TRACES
    printf("%d", data);
  #endif
}

/**
 * @brief read Device ID register.
 */
uint16_t HDC1010_get_dev_id(void)
{
  uint16_t dev_id = 0;
  uint8_t data[2];

  readRegister(HDC1010_ADDR, DEV_ID_REG, 2, data);

  dev_id = (data[0] << 8) | data[1];      // sensor outputs MSB first and LSB next.

  return dev_id;
}


/**
 * @brief read Manufacturer ID register.
 */
uint16_t HDC1010_get_manuf_id(void)
{
  uint16_t manuf_id = 0;
  uint8_t data[2];

  readRegister(HDC1010_ADDR, MANUF_ID_REG, 2, data);

  manuf_id = (data[0] << 8) | data[1];      // sensor outputs MSB first and LSB next.

  return manuf_id;
}


/**
 * @brief read temperature values only. (raw)
 */
void HDC1010_get_temp_raw(uint16_t* temp_val)
{
  *temp_val = HDC1010_readData(HDC1010_ADDR, TEMP_REG, 2);
}


/**
 * @brief read temperature values only.
 */
float HDC1010_get_temp(void)
{
  uint16_t temp_val;

  HDC1010_get_temp_raw(&temp_val);

  // temperature (in C)
  float temp = ((temp_val / pow_16) * 165 - 40);
  return temp;
}

/**
 * @brief read humidity values only (RAW values)
 */
void HDC1010_get_humid_raw(uint16_t* humid_val)
{
  *humid_val = HDC1010_readData(HDC1010_ADDR, HUMIDITY_REG, 2);
}

/**
 * @brief  read humidity values only.
 */
float HDC1010_get_humid(void)
{
  uint16_t humid_val;

  HDC1010_get_humid_raw(&humid_val);

  // relative humidity (in %)
  float humid = ((humid_val / pow_16) * 100);
  return humid;
}
