/**
 * HDC1010.c
 *
 * nRF52 TWI interface for HDC1010 Temperature/Humidity sensor.
 *
 * Electronut Labs
 * electronut.in
*/

#include "HDC1010.h"

const float pow_16 = 65536.0f;

// uncomment for printf traces
// #define ENABLE_PRINTF_TRACES

/**
 * @brief write 16-bit value to the sensor.
 */
void HDC1010_write_word(uint8_t device_addr, uint8_t register_addr, uint16_t *data)
{
  ret_code_t err_code;
  uint8_t data_buf[3];
  data_buf[0] = register_addr;
  data_buf[1] = *data >> 8;                 // send MSB first
  data_buf[2] = *data & 0xff;               // followed by LSB

  err_code = nrf_drv_twi_tx(&p_twi_sensors, device_addr, data_buf, sizeof(data_buf), false);
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief initialize the sensor.
 */
void HDC1010_init(uint16_t reg_data)
{
  // configure sensor to measure temperature and humidity with 14 bit resolution each.
  HDC1010_write_word(HDC1010_ADDR, CONFIG_REG, &reg_data);
}

/**
 * @brief read sensor configuration
 */
void HDC1010_read_config(uint16_t* data)
{
  uint32_t err_code;
  uint8_t tx_buffer = CONFIG_REG;
  uint8_t reg_data[2];

  err_code = nrf_drv_twi_tx(&p_twi_sensors, HDC1010_ADDR, &tx_buffer, 1, false);
  APP_ERROR_CHECK(err_code);

  err_code = nrf_drv_twi_rx(&p_twi_sensors, HDC1010_ADDR, reg_data, sizeof(reg_data));
  APP_ERROR_CHECK(err_code);

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
  ret_code_t err_code;
  uint16_t dev_id = 0;
  uint8_t data[2];
  err_code = read_register(p_twi_sensors, HDC1010_ADDR, DEV_ID_REG, data, sizeof(data), false);
  APP_ERROR_CHECK(err_code);

  dev_id = (data[0] << 8) | data[1];      // sensor outputs MSB first and LSB next.

  #ifdef ENABLE_PRINTF_TRACES
    printf("%u", dev_id);
  #endif

  return dev_id;
}

/**
 * @brief read Manufacturer ID register.
 */
uint16_t HDC1010_get_manuf_id(void)
{
  ret_code_t err_code;
  uint16_t manuf_id = 0;
  uint8_t data[2];
  err_code = read_register(p_twi_sensors, HDC1010_ADDR, MANUF_ID_REG, data, sizeof(data), false);
  APP_ERROR_CHECK(err_code);

  manuf_id = (data[0] << 8) | data[1];      // sensor outputs MSB first and LSB next.

  #ifdef ENABLE_PRINTF_TRACES
    printf("%u", manuf_id);
  #endif

  return manuf_id;
}

/**
 * @brief read temperature values only. (raw)
 */
void HDC1010_get_temp_raw(uint16_t* temp_val)
{
  ret_code_t err_code;
  uint8_t tx_buffer = TEMP_REG;
  uint8_t temp_data[2];
    // trigger measurement.
  err_code = nrf_drv_twi_tx(&p_twi_sensors, HDC1010_ADDR, &tx_buffer, 1, false);
  APP_ERROR_CHECK(err_code);
  // wait for measurement to complete
  nrf_delay_ms(10);
  // read temperature
  err_code = nrf_drv_twi_rx(&p_twi_sensors, HDC1010_ADDR, temp_data, sizeof(temp_data));
  APP_ERROR_CHECK(err_code);

  *temp_val = (temp_data[0] << 8) | temp_data[1];
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
  ret_code_t err_code;
  uint8_t tx_buffer = HUMIDITY_REG;
  uint8_t humid_data[2];
  // trigger measurement.
  err_code = nrf_drv_twi_tx(&p_twi_sensors, HDC1010_ADDR, &tx_buffer, 1, false);
  APP_ERROR_CHECK(err_code);
  // wait for measurement to complete
  nrf_delay_ms(10);
  // read temperature
  err_code = nrf_drv_twi_rx(&p_twi_sensors, HDC1010_ADDR, humid_data, sizeof(humid_data));
  APP_ERROR_CHECK(err_code);

  *humid_val = (humid_data[0] << 8) | humid_data[1];
}

/**
 * @brief  read humidity values only.
 */
float HDC1010_get_humid(void)
{
  uint16_t humid_val;
  HDC1010_get_humid_raw(&humid_val);

  // temperature (in C)
  float humid = ((humid_val / pow_16) * 100);
  return humid;
}
