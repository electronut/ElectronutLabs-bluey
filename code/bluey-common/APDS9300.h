/*
  APDS9300.h

  nRF52 TWI interface for APDS9300 ambient light sensor.

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
 #include "bluey-twi.h"

 #define APDS_INT_PIN            14

 #define APDS9300_ADDR           0x39

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

/*
 * function to read sensor registers.
*/
ret_code_t read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop);

/*
* function to power on/off the sensor
*/
void APDS9300_power_control(void);

/*
* function to test sensor id
*/
void APDS9300_id(void);

/*
* function to read accelerometer registers.
*/
//ret_code_t APDS9300_read_register(uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint32_t bytes);

/*
* function to set time and gain parameters
*/
void APDS9300_set_params(void);

/*
* function to check if device is ON or OFF.
*/
void APDS9300_power_status(void);

/*
* function to read ADC value
*/
void APDS9300_read_adc_data(uint16_t *adc_ch0, uint16_t *adc_ch1);


float getlux(uint16_t adc_ch0, uint16_t adc_ch1);
