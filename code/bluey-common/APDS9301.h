/*
  APDS9301.h

  nRF52 TWI interface for APDS9301 ambient light sensor.

  Electronut Labs
  electronut.in
 */

 #include <stdint.h>
 #include <stdbool.h>
 #include "nrf_drv_twi.h"
 #include "nrf_delay.h"
 #include "app_util_platform.h"
 #include "app_error.h"
 #include "boards.h"
 #include "nrf_drv_twi.h"
 #include "bluey_twi.h"

 #define APDS_INT_PIN            14

 #define APDS9301_ADDR           0x39

 #define CMD                     0x80

 #define CTRL_REG                0x0
 #define TIMING_REG              0x1
 #define THRES_LOW_LOW           0x2    // Interrupt Threshold Register - ADC channel 0 lower byte of the low threshold
 #define THRES_LOW_HIGH          0x3    // Interrupt Threshold Register - ADC channel 0 upper byte of the low threshold
 #define THRES_HIGH_LOW          0x4    // Interrupt Threshold Register - ADC channel 0 lower byte of the high threshold
 #define THRES_HIGH_HIGH         0x5    // Interrupt Threshold Register - ADC channel 0 upper byte of the high threshold
 #define INT_CTRL_REG            0x6    // Interrupt Control Register
 #define ID_REG                  0xA    // ID Register for identifiation
 #define DATA0_LOW               0xC    // ADC Channel Data Register - ADC channel 0 lower byte
 #define DATA0_HIGH              0xD    // ADC Channel Data Register - ADC channel 0 upper byte
 #define DATA1_LOW               0xE    // ADC Channel Data Register - ADC channel 1 lower byte
 #define DATA1_HIGH              0xF    // ADC Channel Data Register - ADC channel 1 upper byte

/*******************************************************************************
* Register      : CTRL_REG
* Address       : 0h
* Bit Group Name: POWER
*******************************************************************************/
typedef enum {
	APDS9301_PWR_DISABLE 		= 0x00,
	APDS9301_PWR_ENABLE 		= 0x03,
} APDS9301_PWR_t;

/*******************************************************************************
* Register      : TIMING_REG
* Address       : 1h
* Bit Group Name: GAIN
*******************************************************************************/
typedef enum {
	APDS9301_SET_GAIN_LOW_GAIN 		= 0x00,
	APDS9301_SET_GAIN_HIGH_GAIN 	= 0x10,
} APDS9301_SET_GAIN_t;

/*******************************************************************************
* Register      : TIMING_REG
* Address       : 1h
* Bit Group Name: MANUAL TIME
*******************************************************************************/
typedef enum {
	APDS9301_MANUAL_TIME_DISABLE 	= 0x00,
	APDS9301_MANUAL_TIME_ENABLE 	= 0x08,
} APDS9301_MANUAL_TIME_t;

/*******************************************************************************
* Register      : CTRL_REG
* Address       : 1h
* Bit Group Name: INTEGRATION TIME
*******************************************************************************/
typedef enum {
	APDS9301_INTG_TIME_13_7_MS 		= 0x00,
	APDS9301_INTG_TIME_101_MS 		= 0x01,
  APDS9301_INTG_TIME_402_MS     = 0x02,
  APDS9301_INTG_TIME_MANUAL     = 0x03,
} APDS9301_INTG_TIME_t;

/*******************************************************************************
* Register      : INT_CTRL_REG
* Address       : 6h
* Bit Group Name: INTERRUPT CONTROL SELECT
*******************************************************************************/
typedef enum {
	APDS9301_INTR_CTRL_LEVEL_INTR_DISABLED 	= 0x00,
	APDS9301_INTR_CTRL_LEVEL_INTR_ENABLED 	= 0x10,
} APDS9301_INTR_CTRL_LEVEL_INTR_t;

/*******************************************************************************
* Register      : INT_CTRL_REG
* Address       : 6h
* Bit Group Name: INTERRUPT PERSISTENCE SELECT
*******************************************************************************/
typedef enum {
	APDS9301_INTR_PRSIST_ADC_VALUE 	  = 0x00,
	APDS9301_INTR_PRSIST_THRESHOLD	  = 0x01,
  APDS9301_INTR_PRSIST_INTG_TIME_2	= 0x02,
  APDS9301_INTR_PRSIST_INTG_TIME_3	= 0x03,
  APDS9301_INTR_PRSIST_INTG_TIME_4	= 0x04,
  APDS9301_INTR_PRSIST_INTG_TIME_5	= 0x05,
  APDS9301_INTR_PRSIST_INTG_TIME_6	= 0x06,
  APDS9301_INTR_PRSIST_INTG_TIME_7	= 0x07,
  APDS9301_INTR_PRSIST_INTG_TIME_8	= 0x08,
  APDS9301_INTR_PRSIST_INTG_TIME_9	= 0x09,
  APDS9301_INTR_PRSIST_INTG_TIME_10	= 0x0A,
  APDS9301_INTR_PRSIST_INTG_TIME_11	= 0x0B,
  APDS9301_INTR_PRSIST_INTG_TIME_12	= 0x0C,
  APDS9301_INTR_PRSIST_INTG_TIME_13	= 0x0D,
  APDS9301_INTR_PRSIST_INTG_TIME_14	= 0x0E,
  APDS9301_INTR_PRSIST_INTG_TIME_15	= 0x0F,
} APDS9301_INTR_PERSIST_INTG_TIME_t;


struct APDS9301_settings {
    // Accelerometer settings
    uint8_t APDS9301_enable;
    uint8_t APDS9301_gain;
    uint8_t APDS9301_manual_time;
    uint8_t APDS9301_integration_time;
    uint8_t APDS9301_level_interrupt;
    uint8_t APDS9301_persistence_value;
};

/*
 * function to read sensor registers.
*/
ret_code_t read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop);

/*
* function to test sensor id
*/
void APDS9301_id(void);

/*
 * function to enable APDS9301
*/
void APDS9301_init(void);

/*
* function to set time and gain parameters
*/
void APDS9301_set_params(void);

/*
* function to check if device is ON or OFF.
*/
uint8_t APDS9301_get_power_status(void);

/*
* function to read ADC value
*/
void APDS9301_read_adc_data(uint16_t *adc_ch0, uint16_t *adc_ch1);

/*
 * function to compute brightness (lux) bases on ADC values
*/
float getlux(uint16_t adc_ch0, uint16_t adc_ch1);

/*
 * function to configure Interrupt Control register
*/
void APDS9301_interrupt_config(void);

/*
 * function to clear interrupt by setting CLEAR bit in COMMAND register.
*/
void clearInterrupt(void);

/*
 * function to set lower interrupt threshold
*/
void APDS9301_set_low_threshold(uint16_t value);

/*
 * function to set higher interrupt threshold
*/
void APDS9301_set_high_threshold(uint16_t value);
