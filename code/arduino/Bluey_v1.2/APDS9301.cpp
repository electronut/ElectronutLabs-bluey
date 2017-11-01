/**
 * APDS9301.c
 *
 * nRF52 TWI interface for APDS9301 ambient light sensor.
 *
 * Electronut Labs
 * electronut.in
 *
 */

#include <math.h>

#include "bluey_wire.h"
#include "APDS9301.h"

static struct APDS9301_settings settings;


/**
 *  @brief function to configure sensor parameters.
 * These parameters are defined in TIMING register.
 * Bit[1:0] defines Integration Time.
 * Bit[3] defines Manual Timing Control. Write Bit[1:0] as '11' and set Bit[3] to enable manual timing control.
 * Bit[4] defines Gain. Low gain = 0, High gain = 1.
 */
void APDS9301_config(void)
{
  uint8_t tx_data[2];

  if(settings.APDS9301_enable == 1) {
    tx_data[0] = (CMD | TIMING_REG);
    tx_data[1] = 0;

    switch(settings.APDS9301_gain) {
      default:
      case 0:
            tx_data[1] |= APDS9301_SET_GAIN_LOW_GAIN;
            break;

      case 1:
            tx_data[1] |= APDS9301_SET_GAIN_HIGH_GAIN;
            break;
    }

    switch(settings.APDS9301_integration_time) {
      case 0:
            tx_data[1] |= APDS9301_INTG_TIME_13_7_MS;
            break;

      case 1:
            tx_data[1] |= APDS9301_INTG_TIME_101_MS;
            break;

      default:
      case 2:
            tx_data[1] |= APDS9301_INTG_TIME_402_MS;
            break;

      case 3:
            tx_data[1] |= APDS9301_INTG_TIME_MANUAL;
            break;
    }

    if(settings.APDS9301_integration_time == 3) {
      switch(settings.APDS9301_manual_time) {
        case 0:
              tx_data[1] |= APDS9301_MANUAL_TIME_DISABLE;
              break;

        case 1:
              tx_data[1] |= APDS9301_MANUAL_TIME_ENABLE;
              break;
      }
    }
    // err_code = nrf_drv_twi_tx(&p_twi_sensors, APDS9301_ADDR, tx_data, sizeof(tx_data), false);
    // APP_ERROR_CHECK(err_code);
    writeRegister(APDS9301_ADDR, tx_data[0], &tx_data[1]);
  }
}

/**
 * @brief function to turn the device ON
 * Bit[1:0] is Power control bit. Set Bit[1:0] to power up.
 */
void APDS9301_power_up(void)
{
  uint8_t tx_data[2];
  tx_data[0] = (CMD | CTRL_REG);
  tx_data[1] = 0x03;

  writeRegister(APDS9301_ADDR, tx_data[0], &tx_data[1]);
  // err_code = nrf_drv_twi_tx(&p_twi_sensors, APDS9301_ADDR, tx_data, sizeof(tx_data), false);
  // APP_ERROR_CHECK(err_code);
}

/**
 * @brief function to turn the device OFF
 * Bit[1:0] is Power control bit. Reset Bit[1:0] to power down.
 */
void APDS9301_power_down(void)
{
  uint8_t tx_data[2];
  tx_data[0] = (CMD | CTRL_REG);
  tx_data[1] = 0x00;

  writeRegister(APDS9301_ADDR, tx_data[0], &tx_data[1]);
  // err_code = nrf_drv_twi_tx(&p_twi_sensors, APDS9301_ADDR, tx_data, sizeof(tx_data), false);
  // APP_ERROR_CHECK(err_code);
}

/**
 * @brief function to initialize and power up the sensor.
 */
void APDS9301_init(void)
{
  settings.APDS9301_enable            = 1;      // enable = 1, disable = 0.
  settings.APDS9301_gain              = 0;      // low gain = 0, high gain = 1.
  settings.APDS9301_manual_time       = 0;      // start integration cycle = 1, stop integration cycle = 0.
                                                // applicable only if APDS9301_integratation_time = 3.
  settings.APDS9301_integration_time  = 2;      // 13.7 ms = 0, 101 ms = 1, 402 ms = 2, custom integration time = 3.
  settings.APDS9301_level_interrupt   = 0;      // level interrupt enable = 1, leven interrupt disable = 0.
  settings.APDS9301_persistence_value = 0;

  if (settings.APDS9301_enable == 1) {
    APDS9301_power_up();
  }
  APDS9301_config();
}

/**
 * @brief function to check if device is ON or OFF.
 * Read Bit[1:0] to get current power status
 */
uint8_t APDS9301_get_power_status(void)
{
  uint8_t status = 99;
  uint8_t reg = CMD | CTRL_REG;

  readRegister(APDS9301_ADDR, reg, 1, &status);
  // status = readRegister(APDS9301_ADDR, reg);
  // err_code = read_register(p_twi_sensors, APDS9301_ADDR, reg, &status, sizeof(status), true);
  // APP_ERROR_CHECK(err_code);

  return status;
}

/**
 * @brief function to test sensor id
 */
uint8_t APDS9301_id(void)
{
  uint8_t id = 0;
  uint8_t reg = CMD | ID_REG;

  readRegister(APDS9301_ADDR, reg, 1, &id);
  // err_code = read_register(p_twi_sensors, APDS9301_ADDR, reg, &id, sizeof(id), false);
  // APP_ERROR_CHECK(err_code);

  return id;
}


/**
 * @brief function to read ADC value
 * Data is present in two channels, namely, DATA0 and DATA1, where each includes two 8 bit registers.
 * So each channel provides a 16-bit ADC data.
 */
void APDS9301_read_adc_data(uint16_t *adc_ch0, uint16_t *adc_ch1)
{
  uint8_t adc_ch0_lower, adc_ch0_upper, adc_ch1_lower, adc_ch1_upper;

  readRegister(APDS9301_ADDR, (CMD | DATA0_LOW), 1, &adc_ch0_lower);
  readRegister(APDS9301_ADDR, (CMD | DATA0_HIGH), 1, &adc_ch0_upper);
  readRegister(APDS9301_ADDR, (CMD | DATA1_LOW), 1, &adc_ch1_lower);
  readRegister(APDS9301_ADDR, (CMD | DATA1_HIGH), 1, &adc_ch1_upper);
/*
  err_code = read_register(p_twi_sensors, APDS9301_ADDR, (CMD | DATA0_LOW), &adc_ch0_lower, 1, false);
  APP_ERROR_CHECK(err_code);

  err_code = read_register(p_twi_sensors, APDS9301_ADDR, (CMD | DATA0_HIGH), &adc_ch0_upper, 1, false);
  APP_ERROR_CHECK(err_code);

  err_code = read_register(p_twi_sensors, APDS9301_ADDR, (CMD | DATA1_LOW), &adc_ch1_lower, 1, false);
  APP_ERROR_CHECK(err_code);

  err_code = read_register(p_twi_sensors, APDS9301_ADDR, (CMD | DATA1_HIGH), &adc_ch1_upper, 1, false);
  APP_ERROR_CHECK(err_code);
*/

  *adc_ch0 = (adc_ch0_upper << 8) | adc_ch0_lower;
  *adc_ch1 = (adc_ch1_upper << 8) | adc_ch1_lower;
}

/**
 * @brief function to compute brightness (lux) bases on ADC values
 * The equations and parameters are from APDS9301 datasheet - page 4.
 */
float getlux(void)
{
  float result;
  uint16_t adc_ch0, adc_ch1;
  APDS9301_read_adc_data(&adc_ch0, &adc_ch1);

  float channelRatio = (adc_ch1)/(float)(adc_ch0);

  // below formula is from datasheet

  if(channelRatio >= 0 && channelRatio <= 0.50f) {
    result = (0.0304 * adc_ch0) - (0.062 * adc_ch0 * pow(channelRatio, 1.4));
  }
  else if(channelRatio > 0.50f && channelRatio <= 0.61f) {
    result = (0.0224 * adc_ch0) - (0.031 * adc_ch1);
  }
  else if(channelRatio > 0.61f && channelRatio <= 0.80f) {
    result = (0.0128 * adc_ch0) - (0.0153 * adc_ch1);
  }
  else if(channelRatio > 0.80f && channelRatio <= 1.30f) {
    result = (0.00146 * adc_ch0) - (0.00112 * adc_ch1);
  }
  else if(channelRatio > 1.30f) {
    result = 0;
  }
  else {
    result = 999.99;
  }

  return result;
}
