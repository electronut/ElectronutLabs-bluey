/*
APDS9300.c

nRF52 TWI interface for APDS9300 ambient light sensor.

Electronut Labs
electronut.in

*/

#include <math.h>

#include "APDS9300.h"

/*
power on/off the sensor
*/
void APDS9300_power_control(void)
{
  ret_code_t err_code;

  uint8_t tx_data[2] = {(CMD | CTRL_REG), 0x03};  // default

  err_code = nrf_drv_twi_tx(&p_twi_sensors, APDS9300_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);

}

/*
set time and gain parameters
*/
void APDS9300_set_params(void)
{
  ret_code_t err_code;
  // change tx_data[1] as per the requirements (refer datasheet: pg 10).
  uint8_t tx_data[2] = {CMD | TIMING_REG, 0x02};  // default

  err_code = nrf_drv_twi_tx(&p_twi_sensors, APDS9300_ADDR, tx_data, sizeof(tx_data), false);
  APP_ERROR_CHECK(err_code);
}

/*
check if device is ON or OFF.
*/
void APDS9300_power_status(void)
{
  ret_code_t err_code;
  uint8_t status = 99;
  uint8_t reg = CMD | CTRL_REG;
  err_code = read_register(p_twi_sensors, APDS9300_ADDR, reg, &status, sizeof(status), true);
  APP_ERROR_CHECK(err_code);
}

/*
test sensor id
*/
void APDS9300_id(void)
{
  ret_code_t err_code;
  uint8_t id = 0;
  uint8_t reg = CMD | ID_REG;
  //err_code = APDS9300_read_register(APDS9300_ADDR, ID_REG, &id, 1);
  err_code = read_register(p_twi_sensors, APDS9300_ADDR, reg, &id, sizeof(id), false);
  APP_ERROR_CHECK(err_code);
}

/*
read ADC value
*/
//#if 0
void APDS9300_read_adc_data(uint16_t *adc_ch0, uint16_t *adc_ch1)
{
  ret_code_t err_code;
  uint8_t adc_ch0_lower, adc_ch0_upper, adc_ch1_lower, adc_ch1_upper;

  err_code = read_register(p_twi_sensors, APDS9300_ADDR, (CMD | DATA0_LOW), &adc_ch0_lower, 1, false);
  APP_ERROR_CHECK(err_code);

  err_code = read_register(p_twi_sensors, APDS9300_ADDR, (CMD | DATA0_HIGH), &adc_ch0_upper, 1, false);
  APP_ERROR_CHECK(err_code);

  err_code = read_register(p_twi_sensors, APDS9300_ADDR, (CMD | DATA1_LOW), &adc_ch1_lower, 1, false);
  APP_ERROR_CHECK(err_code);

  err_code = read_register(p_twi_sensors, APDS9300_ADDR, (CMD | DATA1_HIGH), &adc_ch1_upper, 1, false);
  APP_ERROR_CHECK(err_code);

  *adc_ch0 = (adc_ch0_upper << 8) | adc_ch0_lower;
  *adc_ch1 = (adc_ch1_upper << 8) | adc_ch1_lower;
}

// compute brightness (lux) bases on ADC values
float getlux(uint16_t adc_ch0, uint16_t adc_ch1)
{
  float result;
  float channelRatio = (adc_ch1)/(float)(adc_ch0);

  // below formula is from datasheet

  if(channelRatio >= 0 && channelRatio <= 0.52f) {
    result = (0.0315 * adc_ch0) - (0.0593 * adc_ch0 * pow(channelRatio, 1.4));
  }
  else if(channelRatio > 0.52f && channelRatio <= 0.65f) {
    result = (0.0229 * adc_ch0) - (0.0291 * adc_ch1);
  }
  else if(channelRatio > 0.65f && channelRatio <= 0.80f) {
    result = (0.0157 * adc_ch0) - (0.0180 * adc_ch1);
  }
  else if(channelRatio > 0.80f && channelRatio <= 1.30f) {
    result = (0.00338 * adc_ch0) - (0.00260 * adc_ch1);
  }
  else if(channelRatio > 1.30f) {
    result = 0;
  }
  else {
    result = 999.99;
  }

  return result;
}
//#endif
