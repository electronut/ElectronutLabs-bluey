/*
	LSM6DS3.h

  nRF52 TWI interface for LSM6DS3 IMU.

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

/*Pin Definitions*/
#define INT1                    15
#define INT2                    20
#define SA0                     31

/*Device Address*/
//#define LSM6DS3_ADDR           0x6BU     // SA0 HIGH.
#define LSM6DS3_ADDR           0x6AU     // SA0 LOW.

/*Register Addresses*/

#define FIFO_CTRL1							0x06
#define FIFO_CTRL2							0x07
#define FIFO_CTRL3							0x08
#define FIFO_CTRL4							0x09
#define FIFO_CTRL5							0x0A

#define INT1_CTRL               0x0D
#define INT2_CTRL               0x0E

#define WHO_AM_I                0x0F

#define CTRL1_XL                0x10
#define CTRL2_G                 0x11
#define CTRL3_C                 0x12
#define CTRL4_C                 0x13
#define CTRL5_C                 0x14
#define CTRL6_C                 0x15
#define CTRL7_G                 0x16
#define CTRL8_XL                0x17
#define CTRL9_XL                0x18
#define CTRL10_C                0x19

#define MASTER_CONFIG           0x1A

#define STATUS_REG              0x1E

#define OUT_TEMP_L              0x20
#define OUT_TEMP                0x21
#define OUTX_L_G                0x22
#define OUTX_H_G                0x23
#define OUTY_L_G                0x24
#define OUTY_H_G                0x25
#define OUTZ_L_G                0x26
#define OUTZ_H_G                0x27

#define OUTX_L_XL               0x28
#define OUTX_H_XL               0x29
#define OUTY_L_XL               0x2A
#define OUTY_H_XL               0x2B
#define OUTZ_L_XL               0x2C
#define OUTZ_H_XL               0x2D

#define FIFO_STATUS1            0x3A
#define FIFO_STATUS2            0x3B
#define FIFO_STATUS3            0x3C
#define FIFO_STATUS4            0x3D
#define FIFO_DATA_OUT_L         0x3E
#define FIFO_DATA_OUT_H         0x3F

/*******************************************************************************
* Register      : FIFO_CTRL1
* Address       : 0X06
* Bit Group Name: WTM_FIFO
* Permission    : R/W
*******************************************************************************/
#define  	LSM6DS3_IMU_WTM_FIFO_CTRL1_MASK  			0xFF
#define  	LSM6DS3_IMU_WTM_FIFO_CTRL1_POSITION 	0
#define  	LSM6DS3_IMU_WTM_FIFO_CTRL2_MASK  			0x0F
#define  	LSM6DS3_IMU_WTM_FIFO_CTRL2_POSITION 	0

/*******************************************************************************
* Register      : FIFO_CTRL2
* Address       : 0X07
* Bit Group Name: TIM_PEDO_FIFO_DRDY
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TIM_PEDO_FIFO_DRDY_DISABLED 		= 0x00,
	LSM6DS3_IMU_TIM_PEDO_FIFO_DRDY_ENABLED 		  = 0x40,
} LSM6DS3_IMU_TIM_PEDO_FIFO_DRDY_t;

/*******************************************************************************
* Register      : FIFO_CTRL2
* Address       : 0X07
* Bit Group Name: TIM_PEDO_FIFO_EN
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TIM_PEDO_FIFO_EN_DISABLED 		 	= 0x00,
	LSM6DS3_IMU_TIM_PEDO_FIFO_EN_ENABLED 		 		= 0x80,
} LSM6DS3_IMU_TIM_PEDO_FIFO_EN_t;

/*******************************************************************************
* Register      : FIFO_CTRL3
* Address       : 0X08
* Bit Group Name: DEC_FIFO_XL
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DEC_FIFO_XL_DATA_NOT_IN_FIFO 		= 0x00,
	LSM6DS3_IMU_DEC_FIFO_XL_NO_DECIMATION 		  = 0x01,
	LSM6DS3_IMU_DEC_FIFO_XL_DECIMATION_BY_2 		= 0x02,
	LSM6DS3_IMU_DEC_FIFO_XL_DECIMATION_BY_3 		= 0x03,
	LSM6DS3_IMU_DEC_FIFO_XL_DECIMATION_BY_4 		= 0x04,
	LSM6DS3_IMU_DEC_FIFO_XL_DECIMATION_BY_8 		= 0x05,
	LSM6DS3_IMU_DEC_FIFO_XL_DECIMATION_BY_16 		= 0x06,
	LSM6DS3_IMU_DEC_FIFO_XL_DECIMATION_BY_32 		= 0x07,
} LSM6DS3_IMU_DEC_FIFO_XL_t;

/*******************************************************************************
* Register      : FIFO_CTRL3
* Address       : 0X08
* Bit Group Name: DEC_FIFO_G
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DEC_FIFO_G_DATA_NOT_IN_FIFO 		= 0x00,
	LSM6DS3_IMU_DEC_FIFO_G_NO_DECIMATION 		 		= 0x08,
	LSM6DS3_IMU_DEC_FIFO_G_DECIMATION_BY_2 		 	= 0x10,
	LSM6DS3_IMU_DEC_FIFO_G_DECIMATION_BY_3 		 	= 0x18,
	LSM6DS3_IMU_DEC_FIFO_G_DECIMATION_BY_4 		 	= 0x20,
	LSM6DS3_IMU_DEC_FIFO_G_DECIMATION_BY_8 		 	= 0x28,
	LSM6DS3_IMU_DEC_FIFO_G_DECIMATION_BY_16 		= 0x30,
	LSM6DS3_IMU_DEC_FIFO_G_DECIMATION_BY_32 		= 0x38,
} LSM6DS3_IMU_DEC_FIFO_G_t;

/*******************************************************************************
* Register      : FIFO_CTRL4
* Address       : 0X09
* Bit Group Name: DEC_FIFO_SLV0
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DEC_FIFO_SLV0_DATA_NOT_IN_FIFO 	= 0x00,
	LSM6DS3_IMU_DEC_FIFO_SLV0_NO_DECIMATION 		= 0x01,
	LSM6DS3_IMU_DEC_FIFO_SLV0_DECIMATION_BY_2 	= 0x02,
	LSM6DS3_IMU_DEC_FIFO_SLV0_DECIMATION_BY_3 	= 0x03,
	LSM6DS3_IMU_DEC_FIFO_SLV0_DECIMATION_BY_4 	= 0x04,
	LSM6DS3_IMU_DEC_FIFO_SLV0_DECIMATION_BY_8 	= 0x05,
	LSM6DS3_IMU_DEC_FIFO_SLV0_DECIMATION_BY_16 	= 0x06,
	LSM6DS3_IMU_DEC_FIFO_SLV0_DECIMATION_BY_32 	= 0x07,
} LSM6DS3_IMU_DEC_FIFO_SLV0_t;

/*******************************************************************************
* Register      : FIFO_CTRL4
* Address       : 0X09
* Bit Group Name: DEC_FIFO_SLV1
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DEC_FIFO_SLV1_DATA_NOT_IN_FIFO 	= 0x00,
	LSM6DS3_IMU_DEC_FIFO_SLV1_NO_DECIMATION 		= 0x08,
	LSM6DS3_IMU_DEC_FIFO_SLV1_DECIMATION_BY_2 	= 0x10,
	LSM6DS3_IMU_DEC_FIFO_SLV1_DECIMATION_BY_3 	= 0x18,
	LSM6DS3_IMU_DEC_FIFO_SLV1_DECIMATION_BY_4 	= 0x20,
	LSM6DS3_IMU_DEC_FIFO_SLV1_DECIMATION_BY_8 	= 0x28,
	LSM6DS3_IMU_DEC_FIFO_SLV1_DECIMATION_BY_16 	= 0x30,
	LSM6DS3_IMU_DEC_FIFO_SLV1_DECIMATION_BY_32 	= 0x38,
} LSM6DS3_IMU_DEC_FIFO_SLV1_t;

/*******************************************************************************
* Register      : FIFO_CTRL4
* Address       : 0X09
* Bit Group Name: HI_DATA_ONLY
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_HI_DATA_ONLY_DISABLED = 0x00,
	LSM6DS3_IMU_HI_DATA_ONLY_ENABLED 	= 0x40,
} LSM6DS3_IMU_HI_DATA_ONLY_t;

/*******************************************************************************
* Register      : FIFO_CTRL5
* Address       : 0X0A
* Bit Group Name: FIFO_MODE
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FIFO_MODE_BYPASS 		 	= 0x00,
	LSM6DS3_IMU_FIFO_MODE_FIFO 		 		= 0x01,
	LSM6DS3_IMU_FIFO_MODE_STF			 		= 0x03,
	LSM6DS3_IMU_FIFO_MODE_BTS 		 		= 0x04,
	LSM6DS3_IMU_FIFO_MODE_STREAM			= 0x06,
} LSM6DS3_IMU_FIFO_MODE_t;

/*******************************************************************************
* Register      : FIFO_CTRL5
* Address       : 0X0A
* Bit Group Name: ODR_FIFO
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_ODR_FIFO_13Hz 		 		= 0x08,
	LSM6DS3_IMU_ODR_FIFO_26Hz 		 		= 0x10,
	LSM6DS3_IMU_ODR_FIFO_52Hz 		 		= 0x18,
	LSM6DS3_IMU_ODR_FIFO_104Hz 		 		= 0x20,
	LSM6DS3_IMU_ODR_FIFO_208Hz 		 		= 0x28,
	LSM6DS3_IMU_ODR_FIFO_416Hz 		 		= 0x30,
	LSM6DS3_IMU_ODR_FIFO_833Hz 		 		= 0x38,
	LSM6DS3_IMU_ODR_FIFO_1660Hz 		 	= 0x40,
	LSM6DS3_IMU_ODR_FIFO_3330Hz 		 	= 0x48,
	LSM6DS3_IMU_ODR_FIFO_6660Hz 		 	= 0x50,
} LSM6DS3_IMU_ODR_FIFO_t;

/*******************************************************************************
* Register      : CTRL1_XL
* Address       : 0X10
* Bit Group Name: BW_XL
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_BW_XL_400Hz 		      = 0x00,
	LSM6DS3_IMU_BW_XL_200Hz 		      = 0x01,
	LSM6DS3_IMU_BW_XL_100Hz 		      = 0x02,
	LSM6DS3_IMU_BW_XL_50Hz 		        = 0x03,
} LSM6DS3_IMU_BW_XL_t;

/*******************************************************************************
* Register      : CTRL1_XL
* Address       : 0X10
* Bit Group Name: FS_XL
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FS_XL_2g 		          = 0x00,
	LSM6DS3_IMU_FS_XL_16g 		        = 0x04,
	LSM6DS3_IMU_FS_XL_4g 		          = 0x08,
	LSM6DS3_IMU_FS_XL_8g 		          = 0x0C,
} LSM6DS3_IMU_FS_XL_t;

/*******************************************************************************
* Register      : CTRL1_XL
* Address       : 0X10
* Bit Group Name: ODR_XL
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_ODR_XL_POWER_DOWN 	  = 0x00,
	LSM6DS3_IMU_ODR_XL_13Hz 		      = 0x10,
	LSM6DS3_IMU_ODR_XL_26Hz 		      = 0x20,
	LSM6DS3_IMU_ODR_XL_52Hz 		      = 0x30,
	LSM6DS3_IMU_ODR_XL_104Hz 		      = 0x40,
	LSM6DS3_IMU_ODR_XL_208Hz 		      = 0x50,
	LSM6DS3_IMU_ODR_XL_416Hz 		      = 0x60,
	LSM6DS3_IMU_ODR_XL_833Hz 		      = 0x70,
	LSM6DS3_IMU_ODR_XL_1660Hz 		    = 0x80,
	LSM6DS3_IMU_ODR_XL_3330Hz 		    = 0x90,
	LSM6DS3_IMU_ODR_XL_6660Hz 		    = 0xA0,
	LSM6DS3_IMU_ODR_XL_13330Hz 		    = 0xB0,
} LSM6DS3_IMU_ODR_XL_t;

/*******************************************************************************
* Register      : CTRL2_G
* Address       : 0X11
* Bit Group Name: FS_125
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FS_125_DISABLED 		  = 0x00,
	LSM6DS3_IMU_FS_125_ENABLED 		    = 0x02,
} LSM6DS3_IMU_FS_125_t;

/*******************************************************************************
* Register      : CTRL2_G
* Address       : 0X11
* Bit Group Name: FS_G
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FS_G_245dps 		      = 0x00,
	LSM6DS3_IMU_FS_G_500dps 		      = 0x04,
	LSM6DS3_IMU_FS_G_1000dps 		      = 0x08,
	LSM6DS3_IMU_FS_G_2000dps 		      = 0x0C,
} LSM6DS3_IMU_FS_G_t;

/*******************************************************************************
* Register      : CTRL2_G
* Address       : 0X11
* Bit Group Name: ODR_G
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_ODR_G_POWER_DOWN 	    = 0x00,
	LSM6DS3_IMU_ODR_G_13Hz 		        = 0x10,
	LSM6DS3_IMU_ODR_G_26Hz 		        = 0x20,
	LSM6DS3_IMU_ODR_G_52Hz 		        = 0x30,
	LSM6DS3_IMU_ODR_G_104Hz 		      = 0x40,
	LSM6DS3_IMU_ODR_G_208Hz 		      = 0x50,
	LSM6DS3_IMU_ODR_G_416Hz 		      = 0x60,
	LSM6DS3_IMU_ODR_G_833Hz 		      = 0x70,
	LSM6DS3_IMU_ODR_G_1660Hz 		      = 0x80,
} LSM6DS3_IMU_ODR_G_t;

/*******************************************************************************
* Register      : CTRL9_XL
* Address       : 0X18
* Bit Group Name: XEN_XL
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_XEN_XL_DISABLED 		  = 0x00,
	LSM6DS3_IMU_XEN_XL_ENABLED 		    = 0x08,
} LSM6DS3_IMU_XEN_XL_t;

/*******************************************************************************
* Register      : CTRL9_XL
* Address       : 0X18
* Bit Group Name: YEN_XL
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_YEN_XL_DISABLED 		  = 0x00,
	LSM6DS3_IMU_YEN_XL_ENABLED 		    = 0x10,
} LSM6DS3_IMU_YEN_XL_t;

/*******************************************************************************
* Register      : CTRL9_XL
* Address       : 0X18
* Bit Group Name: ZEN_XL
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_ZEN_XL_DISABLED 		  = 0x00,
	LSM6DS3_IMU_ZEN_XL_ENABLED 		    = 0x20,
} LSM6DS3_IMU_ZEN_XL_t;

/*******************************************************************************
* Register      : CTRL10_C
* Address       : 0X19
* Bit Group Name: SIGN_MOTION_EN
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SIGN_MOTION_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_SIGN_MOTION_EN_ENABLED 		 = 0x01,
} LSM6DS3_IMU_SIGN_MOTION_EN_t;

/*******************************************************************************
* Register      : CTRL10_C
* Address       : 0X19
* Bit Group Name: PEDO_RST_STEP
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_PEDO_RST_STEP_DISABLED 		 = 0x00,
	LSM6DS3_IMU_PEDO_RST_STEP_ENABLED 		 = 0x02,
} LSM6DS3_IMU_PEDO_RST_STEP_t;

/*******************************************************************************
* Register      : CTRL10_C
* Address       : 0X19
* Bit Group Name: XEN_G
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_XEN_G_DISABLED 		 = 0x00,
	LSM6DS3_IMU_XEN_G_ENABLED 		 = 0x08,
} LSM6DS3_IMU_XEN_G_t;

/*******************************************************************************
* Register      : CTRL10_C
* Address       : 0X19
* Bit Group Name: YEN_G
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_YEN_G_DISABLED 		 = 0x00,
	LSM6DS3_IMU_YEN_G_ENABLED 		 = 0x10,
} LSM6DS3_IMU_YEN_G_t;

/*******************************************************************************
* Register      : CTRL10_C
* Address       : 0X19
* Bit Group Name: ZEN_G
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_ZEN_G_DISABLED 		 = 0x00,
	LSM6DS3_IMU_ZEN_G_ENABLED 		 = 0x20,
} LSM6DS3_IMU_ZEN_G_t;

/*******************************************************************************
* Register      : CTRL10_C
* Address       : 0X19
* Bit Group Name: FUNC_EN
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FUNC_EN_DISABLED 	 = 0x00,
	LSM6DS3_IMU_FUNC_EN_ENABLED 	 = 0x04,
} LSM6DS3_IMU_FUNC_EN_t;

/*******************************************************************************
* Register      : FIFO_STATUS1
* Address       : 0X3A
* Bit Group Name: DIFF_FIFO
* Permission    : RO
*******************************************************************************/
#define  	LSM6DS3_ACC_GYRO_DIFF_FIFO_STATUS1_MASK  	0xFF
#define  	LSM6DS3_ACC_GYRO_DIFF_FIFO_STATUS1_POSITION  	0
#define  	LSM6DS3_ACC_GYRO_DIFF_FIFO_STATUS2_MASK  0xF
#define  	LSM6DS3_ACC_GYRO_DIFF_FIFO_STATUS2_POSITION  	0

/*******************************************************************************
* Register      : FIFO_STATUS2
* Address       : 0X3B
* Bit Group Name: FIFO_EMPTY
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_ACC_GYRO_FIFO_EMPTY_FIFO_NOT_EMPTY 	= 0x00,
	LSM6DS3_ACC_GYRO_FIFO_EMPTY_FIFO_EMPTY 		 	= 0x10,
} LSM6DS3_ACC_GYRO_FIFO_EMPTY_t;

/*******************************************************************************
* Register      : FIFO_STATUS2
* Address       : 0X3B
* Bit Group Name: FIFO_FULL
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_ACC_GYRO_FIFO_FULL_FIFO_NOT_FULL 		= 0x00,
	LSM6DS3_ACC_GYRO_FIFO_FULL_FIFO_FULL 		 		= 0x20,
} LSM6DS3_ACC_GYRO_FIFO_FULL_t;

/*******************************************************************************
* Register      : FIFO_STATUS2
* Address       : 0X3B
* Bit Group Name: OVERRUN
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_ACC_GYRO_OVERRUN_NO_OVERRUN 		 		= 0x00,
	LSM6DS3_ACC_GYRO_OVERRUN_OVERRUN 		 				= 0x40,
} LSM6DS3_ACC_GYRO_OVERRUN_t;

/*******************************************************************************
* Register      : FIFO_STATUS2
* Address       : 0X3B
* Bit Group Name: WTM
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_ACC_GYRO_WTM_BELOW_WTM 		 					= 0x00,
	LSM6DS3_ACC_GYRO_WTM_ABOVE_OR_EQUAL_WTM 		= 0x80,
} LSM6DS3_ACC_GYRO_WTM_t;

/*******************************************************************************
* Register      : FIFO_STATUS3
* Address       : 0X3C
* Bit Group Name: FIFO_PATTERN
* Permission    : RO
*******************************************************************************/
#define  	LSM6DS3_ACC_GYRO_FIFO_STATUS3_PATTERN_MASK  		0xFF
#define  	LSM6DS3_ACC_GYRO_FIFO_STATUS3_PATTERN_POSITION  0
#define  	LSM6DS3_ACC_GYRO_FIFO_STATUS4_PATTERN_MASK  		0x03
#define  	LSM6DS3_ACC_GYRO_FIFO_STATUS4_PATTERN_POSITION  0





struct IMU_settings {
    // Accelerometer settings
    uint8_t accel_enable;
    uint8_t accel_ODR;
    uint16_t accel_range;
    uint16_t accel_samplerate;
    uint16_t accel_bandwidth;

    uint8_t accel_FIFO_enable;
    uint8_t accel_FIFO_decimation;

    // Gyroscope settings
    uint16_t gyro_enable;
    uint16_t gyro_range;
    uint16_t gyro_samplerate;
    uint16_t gyro_bandwidth;

    uint8_t gyro_FIFO_enable;
    uint8_t gyro_FIFO_decimation;

    // Temperature settings
    uint8_t temp_enable;

    // FIFO control data
    uint16_t FIFO_threshold;
    uint16_t FIFO_samplerate;
    uint8_t FIFO_mode;
};

/*
 * function to initialize IMU sensor.
*/
void LSM6DS3_init(void);

/*
 * function to read sensor registers.
*/
ret_code_t read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop);

/*
 * function to test availablility of accelerometer
*/
void LSM6DS3_who_am_i(void);

/*
 * function to configure accelerometer, gyroscope and temperature sensors
*/
void LSM6DS3_config();

/*
 * function to read raw accelerometer data
*/
void LSM6DS3_read_accl_data(int16_t *x_axis, int16_t *y_axis, int16_t *z_axis);


/*
 * function to calculate acceleration value (in g)
*/
float LSM6DS3_accelData_in_g(int16_t raw_data);

/*
 * function to read raw gyroscope data
*/
void LSM6DS3_read_gyro_data(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);

/*
 * function to calculate gyroscope value (in dps)
*/
float LSM6DS3_gyroData_in_dps(int16_t raw_data);

/*
 * function to configure FIFO
*/
void LSM6DS3_FIFO_config(void);

/*
 * function to empty FIFO buffer
*/
void LSM6DS3_clear_FIFO_buffer(void);

/*
 * function to read FIFO status
*/
uint16_t LSM6DS3_read_FIFO_status(void);

/*
 * function to read FIFO buffer
*/
int16_t LSM6DS3_read_FIFO_buffer(void);
