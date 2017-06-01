/**
 * LSM6DS3.h
 *
 * nRF52 TWI interface for LSM6DS3 IMU.
 *
 * Electronut Labs
 * electronut.in
 */

#include <stdint.h>
#include <stdbool.h>
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bluey_twi.h"

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

#define FUNC_SRC								0x53
#define TAP_CFG									0x58
#define TAP_THRS_6D							0X59
#define INT_DUR2								0X5A
#define WAKE_UP_THRS						0X5B
#define WAKE_UP_DUR 						0x5C
#define FREE_FALL 							0x5D
#define MD1_CFG 								0x5E
#define MD2_CFG 								0x5F

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
* Register      : INT1_CTRL
* Address       : 0X0D
* Bit Group Name: INT1_DRDY_XL
* Permission    : R/W
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_DRDY_XL_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_DRDY_XL_ENABLED 		   = 0x01,
} LSM6DS3_IMU_INT1_DRDY_XL_t;

/*******************************************************************************
* Register      : INT1_CTRL
* Address       : 0X0D
* Bit Group Name: INT1_DRDY_G
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_DRDY_G_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_DRDY_G_ENABLED 		 = 0x02,
} LSM6DS3_IMU_INT1_DRDY_G_t;

/*******************************************************************************
* Register      : INT1_CTRL
* Address       : 0X0D
* Bit Group Name: INT1_BOOT
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_BOOT_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_BOOT_ENABLED 		 = 0x04,
} LSM6DS3_IMU_INT1_BOOT_t;

/*******************************************************************************
* Register      : INT1_CTRL
* Address       : 0X0D
* Bit Group Name: INT1_FTH
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_FTH_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_FTH_ENABLED 		   = 0x08,
} LSM6DS3_IMU_INT1_FTH_t;

/*******************************************************************************
* Register      : INT1_CTRL
* Address       : 0X0D
* Bit Group Name: INT1_OVR
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_OVR_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_OVR_ENABLED 		   = 0x10,
} LSM6DS3_IMU_INT1_OVR_t;

/*******************************************************************************
* Register      : INT1_CTRL
* Address       : 0X0D
* Bit Group Name: INT1_FSS5
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_FSS5_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_FSS5_ENABLED 		 = 0x20,
} LSM6DS3_IMU_INT1_FSS5_t;

/*******************************************************************************
* Register      : INT1_CTRL
* Address       : 0X0D
* Bit Group Name: INT1_SIGN_MOT
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_SIGN_MOT_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_SIGN_MOT_ENABLED 		 = 0x40,
} LSM6DS3_IMU_INT1_SIGN_MOT_t;

/*******************************************************************************
* Register      : INT1_CTRL
* Address       : 0X0D
* Bit Group Name: INT1_PEDO
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_PEDO_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_PEDO_ENABLED 		 = 0x80,
} LSM6DS3_IMU_INT1_PEDO_t;

/*******************************************************************************
* Register      : INT2_CTRL
* Address       : 0X0E
* Bit Group Name: INT2_DRDY_XL
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_DRDY_XL_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_DRDY_XL_ENABLED 		   = 0x01,
} LSM6DS3_IMU_INT2_DRDY_XL_t;

/*******************************************************************************
* Register      : INT2_CTRL
* Address       : 0X0E
* Bit Group Name: INT2_DRDY_G
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_DRDY_G_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_DRDY_G_ENABLED 		 = 0x02,
} LSM6DS3_IMU_INT2_DRDY_G_t;

/*******************************************************************************
* Register      : INT2_CTRL
* Address       : 0X0E
* Bit Group Name: INT2_FTH
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_FTH_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_FTH_ENABLED 		   = 0x08,
} LSM6DS3_IMU_INT2_FTH_t;

/*******************************************************************************
* Register      : INT2_CTRL
* Address       : 0X0E
* Bit Group Name: INT2_OVR
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_OVR_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_OVR_ENABLED 		   = 0x10,
} LSM6DS3_IMU_INT2_OVR_t;

/*******************************************************************************
* Register      : INT2_CTRL
* Address       : 0X0E
* Bit Group Name: INT2_FSS5
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_FSS5_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_FSS5_ENABLED 		 = 0x20,
} LSM6DS3_IMU_INT2_FSS5_t;

/*******************************************************************************
* Register      : INT2_CTRL
* Address       : 0X0E
* Bit Group Name: INT2_SIGN_MOT
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_SIGN_MOT_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_SIGN_MOT_ENABLED 		 = 0x40,
} LSM6DS3_IMU_INT2_SIGN_MOT_t;

/*******************************************************************************
* Register      : INT2_CTRL
* Address       : 0X0E
* Bit Group Name: INT2_PEDO
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_PEDO_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_PEDO_ENABLED 		 = 0x80,
} LSM6DS3_IMU_INT2_PEDO_t;


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
* Register      : CTRL3_C
* Address       : 0X12
* Bit Group Name: SW_RESET
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SW_RESET_NORMAL_MODE 		 = 0x00,
	LSM6DS3_IMU_SW_RESET_RESET_DEVICE 		 = 0x01,
} LSM6DS3_IMU_SW_RESET_t;

/*******************************************************************************
* Register      : CTRL3_C
* Address       : 0X12
* Bit Group Name: BLE
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_BLE_LSB 		 = 0x00,
	LSM6DS3_IMU_BLE_MSB 		 = 0x02,
} LSM6DS3_IMU_BLE_t;

/*******************************************************************************
* Register      : CTRL3_C
* Address       : 0X12
* Bit Group Name: IF_INC
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_IF_INC_DISABLED 		 = 0x00,
	LSM6DS3_IMU_IF_INC_ENABLED 		 = 0x04,
} LSM6DS3_IMU_IF_INC_t;

/*******************************************************************************
* Register      : CTRL3_C
* Address       : 0X12
* Bit Group Name: SIM
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SIM_4_WIRE 		 = 0x00,
	LSM6DS3_IMU_SIM_3_WIRE 		 = 0x08,
} LSM6DS3_IMU_SIM_t;

/*******************************************************************************
* Register      : CTRL3_C
* Address       : 0X12
* Bit Group Name: PP_OD
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_PP_OD_PUSH_PULL 		 = 0x00,
	LSM6DS3_IMU_PP_OD_OPEN_DRAIN 		 = 0x10,
} LSM6DS3_IMU_PP_OD_t;

/*******************************************************************************
* Register      : CTRL3_C
* Address       : 0X12
* Bit Group Name: INT_ACT_LEVEL
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT_ACT_LEVEL_ACTIVE_HI 		 = 0x00,
	LSM6DS3_IMU_INT_ACT_LEVEL_ACTIVE_LO 		 = 0x20,
} LSM6DS3_IMU_INT_ACT_LEVEL_t;

/*******************************************************************************
* Register      : CTRL3_C
* Address       : 0X12
* Bit Group Name: BDU
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_BDU_CONTINUOS 		 = 0x00,
	LSM6DS3_IMU_BDU_BLOCK_UPDATE 		 = 0x40,
} LSM6DS3_IMU_BDU_t;

/*******************************************************************************
* Register      : CTRL3_C
* Address       : 0X12
* Bit Group Name: BOOT
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_BOOT_NORMAL_MODE 		 = 0x00,
	LSM6DS3_IMU_BOOT_REBOOT_MODE 		 = 0x80,
} LSM6DS3_IMU_BOOT_t;

/*******************************************************************************
* Register      : CTRL4_C
* Address       : 0X13
* Bit Group Name: STOP_ON_FTH
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_STOP_ON_FTH_DISABLED 		 = 0x00,
	LSM6DS3_IMU_STOP_ON_FTH_ENABLED 		 = 0x01,
} LSM6DS3_IMU_STOP_ON_FTH_t;

/*******************************************************************************
* Register      : CTRL4_C
* Address       : 0X13
* Bit Group Name: MODE3_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_MODE3_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_MODE3_EN_ENABLED 		 = 0x02,
} LSM6DS3_IMU_MODE3_EN_t;

/*******************************************************************************
* Register      : CTRL4_C
* Address       : 0X13
* Bit Group Name: I2C_DISABLE
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_I2C_DISABLE_I2C_AND_SPI 		 = 0x00,
	LSM6DS3_IMU_I2C_DISABLE_SPI_ONLY 		 = 0x04,
} LSM6DS3_IMU_I2C_DISABLE_t;

/*******************************************************************************
* Register      : CTRL4_C
* Address       : 0X13
* Bit Group Name: DRDY_MSK
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DRDY_MSK_DISABLED 		 = 0x00,
	LSM6DS3_IMU_DRDY_MSK_ENABLED 		 = 0x08,
} LSM6DS3_IMU_DRDY_MSK_t;

/*******************************************************************************
* Register      : CTRL4_C
* Address       : 0X13
* Bit Group Name: FIFO_TEMP_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FIFO_TEMP_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_FIFO_TEMP_EN_ENABLED 		 = 0x10,
} LSM6DS3_IMU_FIFO_TEMP_EN_t;

/*******************************************************************************
* Register      : CTRL4_C
* Address       : 0X13
* Bit Group Name: INT2_ON_INT1
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_ON_INT1_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_ON_INT1_ENABLED 		 = 0x20,
} LSM6DS3_IMU_INT2_ON_INT1_t;

/*******************************************************************************
* Register      : CTRL4_C
* Address       : 0X13
* Bit Group Name: SLEEP_G
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SLEEP_G_DISABLED 		 = 0x00,
	LSM6DS3_IMU_SLEEP_G_ENABLED 		 = 0x40,
} LSM6DS3_IMU_SLEEP_G_t;

/*******************************************************************************
* Register      : CTRL4_C
* Address       : 0X13
* Bit Group Name: BW_SCAL_ODR
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_BW_SCAL_ODR_DISABLED 		 = 0x00,
	LSM6DS3_IMU_BW_SCAL_ODR_ENABLED 		 = 0x80,
} LSM6DS3_IMU_BW_SCAL_ODR_t;

/*******************************************************************************
* Register      : CTRL5_C
* Address       : 0X14
* Bit Group Name: ST_XL
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_ST_XL_NORMAL_MODE 		 = 0x00,
	LSM6DS3_IMU_ST_XL_POS_SIGN_TEST 		 = 0x01,
	LSM6DS3_IMU_ST_XL_NEG_SIGN_TEST 		 = 0x02,
	LSM6DS3_IMU_ST_XL_NA 		 = 0x03,
} LSM6DS3_IMU_ST_XL_t;

/*******************************************************************************
* Register      : CTRL5_C
* Address       : 0X14
* Bit Group Name: ST_G
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_ST_G_NORMAL_MODE 		 = 0x00,
	LSM6DS3_IMU_ST_G_POS_SIGN_TEST 		 = 0x04,
	LSM6DS3_IMU_ST_G_NA 		 = 0x08,
	LSM6DS3_IMU_ST_G_NEG_SIGN_TEST 		 = 0x0C,
} LSM6DS3_IMU_ST_G_t;

/*******************************************************************************
* Register      : CTRL6_G
* Address       : 0X15
* Bit Group Name: LP_XL
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_LP_XL_DISABLED 		 = 0x00,
	LSM6DS3_IMU_LP_XL_ENABLED 		 = 0x10,
} LSM6DS3_IMU_LP_XL_t;

/*******************************************************************************
* Register      : CTRL6_G
* Address       : 0X15
* Bit Group Name: DEN_LVL2_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DEN_LVL2_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_DEN_LVL2_EN_ENABLED 		 = 0x20,
} LSM6DS3_IMU_DEN_LVL2_EN_t;

/*******************************************************************************
* Register      : CTRL6_G
* Address       : 0X15
* Bit Group Name: DEN_LVL_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DEN_LVL_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_DEN_LVL_EN_ENABLED 		 = 0x40,
} LSM6DS3_IMU_DEN_LVL_EN_t;

/*******************************************************************************
* Register      : CTRL6_G
* Address       : 0X15
* Bit Group Name: DEN_EDGE_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DEN_EDGE_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_DEN_EDGE_EN_ENABLED 		 = 0x80,
} LSM6DS3_IMU_DEN_EDGE_EN_t;

/*******************************************************************************
* Register      : CTRL7_G
* Address       : 0X16
* Bit Group Name: HPM_G
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_HPM_G_NORMAL_MODE 		 = 0x00,
	LSM6DS3_IMU_HPM_G_REF_SIGNAL 		 = 0x10,
	LSM6DS3_IMU_HPM_G_NORMAL_MODE_2 		 = 0x20,
	LSM6DS3_IMU_HPM_G_AUTO_RESET_ON_INT 		 = 0x30,
} LSM6DS3_IMU_HPM_G_t;

/*******************************************************************************
* Register      : CTRL7_G
* Address       : 0X16
* Bit Group Name: HP_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_HP_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_HP_EN_ENABLED 		 = 0x40,
} LSM6DS3_IMU_HP_EN_t;

/*******************************************************************************
* Register      : CTRL7_G
* Address       : 0X16
* Bit Group Name: LP_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_LP_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_LP_EN_ENABLED 		 = 0x80,
} LSM6DS3_IMU_LP_EN_t;

/*******************************************************************************
* Register      : CTRL8_XL
* Address       : 0X17
* Bit Group Name: FDS
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FDS_FILTER_OFF 		 = 0x00,
	LSM6DS3_IMU_FDS_FILTER_ON 		 = 0x04,
} LSM6DS3_IMU_FDS_t;

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
	LSM6DS3_IMU_SIGN_MOTION_EN_ENABLED 		   = 0x01,
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
* Register      : MASTER_CONFIG
* Address       : 0X1A
* Bit Group Name: MASTER_ON
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_MASTER_ON_DISABLED 		 = 0x00,
	LSM6DS3_IMU_MASTER_ON_ENABLED 		 = 0x01,
} LSM6DS3_IMU_MASTER_ON_t;

/*******************************************************************************
* Register      : MASTER_CONFIG
* Address       : 0X1A
* Bit Group Name: IRON_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_IRON_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_IRON_EN_ENABLED 		 = 0x02,
} LSM6DS3_IMU_IRON_EN_t;

/*******************************************************************************
* Register      : MASTER_CONFIG
* Address       : 0X1A
* Bit Group Name: PASS_THRU_MODE
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_PASS_THRU_MODE_DISABLED 		 = 0x00,
	LSM6DS3_IMU_PASS_THRU_MODE_ENABLED 		 = 0x04,
} LSM6DS3_IMU_PASS_THRU_MODE_t;

/*******************************************************************************
* Register      : MASTER_CONFIG
* Address       : 0X1A
* Bit Group Name: PULL_UP_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_PULL_UP_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_PULL_UP_EN_ENABLED 		 = 0x08,
} LSM6DS3_IMU_PULL_UP_EN_t;

/*******************************************************************************
* Register      : MASTER_CONFIG
* Address       : 0X1A
* Bit Group Name: START_CONFIG
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_START_CONFIG_XL_G_DRDY 		 = 0x00,
	LSM6DS3_IMU_START_CONFIG_EXT_INT2 		 = 0x10,
} LSM6DS3_IMU_START_CONFIG_t;

/*******************************************************************************
* Register      : MASTER_CONFIG
* Address       : 0X1A
* Bit Group Name: DATA_VAL_SEL_FIFO
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DATA_VAL_SEL_FIFO_XL_G_DRDY 		 = 0x00,
	LSM6DS3_IMU_DATA_VAL_SEL_FIFO_SHUB_DRDY 		 = 0x40,
} LSM6DS3_IMU_DATA_VAL_SEL_FIFO_t;

/*******************************************************************************
* Register      : MASTER_CONFIG
* Address       : 0X1A
* Bit Group Name: DRDY_ON_INT1
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DRDY_ON_INT1_DISABLED 		 = 0x00,
	LSM6DS3_IMU_DRDY_ON_INT1_ENABLED 		 = 0x80,
} LSM6DS3_IMU_DRDY_ON_INT1_t;

/*******************************************************************************
* Register      : WAKE_UP_SRC
* Address       : 0X1B
* Bit Group Name: Z_WU
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_Z_WU_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_Z_WU_DETECTED 		 = 0x01,
} LSM6DS3_IMU_Z_WU_t;

/*******************************************************************************
* Register      : WAKE_UP_SRC
* Address       : 0X1B
* Bit Group Name: Y_WU
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_Y_WU_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_Y_WU_DETECTED 		 = 0x02,
} LSM6DS3_IMU_Y_WU_t;

/*******************************************************************************
* Register      : WAKE_UP_SRC
* Address       : 0X1B
* Bit Group Name: X_WU
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_X_WU_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_X_WU_DETECTED 		 = 0x04,
} LSM6DS3_IMU_X_WU_t;

/*******************************************************************************
* Register      : WAKE_UP_SRC
* Address       : 0X1B
* Bit Group Name: WU_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_WU_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_WU_EV_STATUS_DETECTED 		 = 0x08,
} LSM6DS3_IMU_WU_EV_STATUS_t;

/*******************************************************************************
* Register      : WAKE_UP_SRC
* Address       : 0X1B
* Bit Group Name: SLEEP_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SLEEP_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_SLEEP_EV_STATUS_DETECTED 		 = 0x10,
} LSM6DS3_IMU_SLEEP_EV_STATUS_t;

/*******************************************************************************
* Register      : WAKE_UP_SRC
* Address       : 0X1B
* Bit Group Name: FF_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FF_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_FF_EV_STATUS_DETECTED 		 = 0x20,
} LSM6DS3_IMU_FF_EV_STATUS_t;

/*******************************************************************************
* Register      : TAP_SRC
* Address       : 0X1C
* Bit Group Name: Z_TAP
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_Z_TAP_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_Z_TAP_DETECTED 		 = 0x01,
} LSM6DS3_IMU_Z_TAP_t;

/*******************************************************************************
* Register      : TAP_SRC
* Address       : 0X1C
* Bit Group Name: Y_TAP
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_Y_TAP_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_Y_TAP_DETECTED 		 = 0x02,
} LSM6DS3_IMU_Y_TAP_t;

/*******************************************************************************
* Register      : TAP_SRC
* Address       : 0X1C
* Bit Group Name: X_TAP
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_X_TAP_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_X_TAP_DETECTED 		 = 0x04,
} LSM6DS3_IMU_X_TAP_t;

/*******************************************************************************
* Register      : TAP_SRC
* Address       : 0X1C
* Bit Group Name: TAP_SIGN
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TAP_SIGN_POS_SIGN 		 = 0x00,
	LSM6DS3_IMU_TAP_SIGN_NEG_SIGN 		 = 0x08,
} LSM6DS3_IMU_TAP_SIGN_t;

/*******************************************************************************
* Register      : TAP_SRC
* Address       : 0X1C
* Bit Group Name: DOUBLE_TAP_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DOUBLE_TAP_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_DOUBLE_TAP_EV_STATUS_DETECTED 		 = 0x10,
} LSM6DS3_IMU_DOUBLE_TAP_EV_STATUS_t;

/*******************************************************************************
* Register      : TAP_SRC
* Address       : 0X1C
* Bit Group Name: SINGLE_TAP_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SINGLE_TAP_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_SINGLE_TAP_EV_STATUS_DETECTED 		 = 0x20,
} LSM6DS3_IMU_SINGLE_TAP_EV_STATUS_t;

/*******************************************************************************
* Register      : TAP_SRC
* Address       : 0X1C
* Bit Group Name: TAP_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TAP_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_TAP_EV_STATUS_DETECTED 		 = 0x40,
} LSM6DS3_IMU_TAP_EV_STATUS_t;

/*******************************************************************************
* Register      : D6D_SRC
* Address       : 0X1D
* Bit Group Name: DSD_XL
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DSD_XL_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_DSD_XL_DETECTED 		 = 0x01,
} LSM6DS3_IMU_DSD_XL_t;

/*******************************************************************************
* Register      : D6D_SRC
* Address       : 0X1D
* Bit Group Name: DSD_XH
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DSD_XH_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_DSD_XH_DETECTED 		 = 0x02,
} LSM6DS3_IMU_DSD_XH_t;

/*******************************************************************************
* Register      : D6D_SRC
* Address       : 0X1D
* Bit Group Name: DSD_YL
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DSD_YL_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_DSD_YL_DETECTED 		 = 0x04,
} LSM6DS3_IMU_DSD_YL_t;

/*******************************************************************************
* Register      : D6D_SRC
* Address       : 0X1D
* Bit Group Name: DSD_YH
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DSD_YH_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_DSD_YH_DETECTED 		 = 0x08,
} LSM6DS3_IMU_DSD_YH_t;

/*******************************************************************************
* Register      : D6D_SRC
* Address       : 0X1D
* Bit Group Name: DSD_ZL
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DSD_ZL_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_DSD_ZL_DETECTED 		 = 0x10,
} LSM6DS3_IMU_DSD_ZL_t;

/*******************************************************************************
* Register      : D6D_SRC
* Address       : 0X1D
* Bit Group Name: DSD_ZH
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_DSD_ZH_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_DSD_ZH_DETECTED 		 = 0x20,
} LSM6DS3_IMU_DSD_ZH_t;

/*******************************************************************************
* Register      : D6D_SRC
* Address       : 0X1D
* Bit Group Name: D6D_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_D6D_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_D6D_EV_STATUS_DETECTED 		 = 0x40,
} LSM6DS3_IMU_D6D_EV_STATUS_t;

/*******************************************************************************
* Register      : STATUS_REG
* Address       : 0X1E
* Bit Group Name: XLDA
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_XLDA_NO_DATA_AVAIL 		 = 0x00,
	LSM6DS3_IMU_XLDA_DATA_AVAIL 		 = 0x01,
} LSM6DS3_IMU_XLDA_t;

/*******************************************************************************
* Register      : STATUS_REG
* Address       : 0X1E
* Bit Group Name: GDA
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_GDA_NO_DATA_AVAIL 		 = 0x00,
	LSM6DS3_IMU_GDA_DATA_AVAIL 		 = 0x02,
} LSM6DS3_IMU_GDA_t;

/*******************************************************************************
* Register      : STATUS_REG
* Address       : 0X1E
* Bit Group Name: EV_BOOT
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_EV_BOOT_NO_BOOT_RUNNING 		 = 0x00,
	LSM6DS3_IMU_EV_BOOT_BOOT_IS_RUNNING 		 = 0x08,
} LSM6DS3_IMU_EV_BOOT_t;

/*******************************************************************************
* Register      : FIFO_STATUS1
* Address       : 0X3A
* Bit Group Name: DIFF_FIFO
* Permission    : RO
*******************************************************************************/
#define  	LSM6DS3_IMU_DIFF_FIFO_STATUS1_MASK  	0xFF
#define  	LSM6DS3_IMU_DIFF_FIFO_STATUS1_POSITION  	0
#define  	LSM6DS3_IMU_DIFF_FIFO_STATUS2_MASK  0xF
#define  	LSM6DS3_IMU_DIFF_FIFO_STATUS2_POSITION  	0

/*******************************************************************************
* Register      : FIFO_STATUS2
* Address       : 0X3B
* Bit Group Name: FIFO_EMPTY
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FIFO_EMPTY_FIFO_NOT_EMPTY 		 = 0x00,
	LSM6DS3_IMU_FIFO_EMPTY_FIFO_EMPTY 		 = 0x10,
} LSM6DS3_IMU_FIFO_EMPTY_t;

/*******************************************************************************
* Register      : FIFO_STATUS2
* Address       : 0X3B
* Bit Group Name: FIFO_FULL
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FIFO_FULL_FIFO_NOT_FULL 		 = 0x00,
	LSM6DS3_IMU_FIFO_FULL_FIFO_FULL 		 = 0x20,
} LSM6DS3_IMU_FIFO_FULL_t;

/*******************************************************************************
* Register      : FIFO_STATUS2
* Address       : 0X3B
* Bit Group Name: OVERRUN
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_OVERRUN_NO_OVERRUN 		 = 0x00,
	LSM6DS3_IMU_OVERRUN_OVERRUN 		 = 0x40,
} LSM6DS3_IMU_OVERRUN_t;

/*******************************************************************************
* Register      : FIFO_STATUS2
* Address       : 0X3B
* Bit Group Name: WTM
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_WTM_BELOW_WTM 		 = 0x00,
	LSM6DS3_IMU_WTM_ABOVE_OR_EQUAL_WTM 		 = 0x80,
} LSM6DS3_IMU_WTM_t;

/*******************************************************************************
* Register      : FIFO_STATUS3
* Address       : 0X3C
* Bit Group Name: FIFO_PATTERN
* Permission    : RO
*******************************************************************************/
#define  	LSM6DS3_IMU_FIFO_STATUS3_PATTERN_MASK  	0xFF
#define  	LSM6DS3_IMU_FIFO_STATUS3_PATTERN_POSITION  	0
#define  	LSM6DS3_IMU_FIFO_STATUS4_PATTERN_MASK  	0x03
#define  	LSM6DS3_IMU_FIFO_STATUS4_PATTERN_POSITION  	0

/*******************************************************************************
* Register      : FUNC_SRC
* Address       : 0X53
* Bit Group Name: SENS_HUB_END
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SENS_HUB_END_STILL_ONGOING 		 = 0x00,
	LSM6DS3_IMU_SENS_HUB_END_OP_COMPLETED 		 = 0x01,
} LSM6DS3_IMU_SENS_HUB_END_t;

/*******************************************************************************
* Register      : FUNC_SRC
* Address       : 0X53
* Bit Group Name: SOFT_IRON_END
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SOFT_IRON_END_NOT_COMPLETED 		 = 0x00,
	LSM6DS3_IMU_SOFT_IRON_END_COMPLETED 		 = 0x02,
} LSM6DS3_IMU_SOFT_IRON_END_t;

/*******************************************************************************
* Register      : FUNC_SRC
* Address       : 0X53
* Bit Group Name: PEDO_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_PEDO_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_PEDO_EV_STATUS_DETECTED 		 = 0x10,
} LSM6DS3_IMU_PEDO_EV_STATUS_t;

/*******************************************************************************
* Register      : FUNC_SRC
* Address       : 0X53
* Bit Group Name: TILT_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TILT_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_TILT_EV_STATUS_DETECTED 		 = 0x20,
} LSM6DS3_IMU_TILT_EV_STATUS_t;

/*******************************************************************************
* Register      : FUNC_SRC
* Address       : 0X53
* Bit Group Name: SIGN_MOT_EV_STATUS
* Permission    : RO
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SIGN_MOT_EV_STATUS_NOT_DETECTED 		 = 0x00,
	LSM6DS3_IMU_SIGN_MOT_EV_STATUS_DETECTED 		 = 0x40,
} LSM6DS3_IMU_SIGN_MOT_EV_STATUS_t;

/*******************************************************************************
* Register      : TAP_CFG
* Address       : 0X58
* Bit Group Name: LIR
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_LIR_DISABLED 		 = 0x00,
	LSM6DS3_IMU_LIR_ENABLED 		 = 0x01,
} LSM6DS3_IMU_LIR_t;

/*******************************************************************************
* Register      : TAP_CFG
* Address       : 0X58
* Bit Group Name: TAP_Z_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TAP_Z_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_TAP_Z_EN_ENABLED 		 = 0x02,
} LSM6DS3_IMU_TAP_Z_EN_t;

/*******************************************************************************
* Register      : TAP_CFG
* Address       : 0X58
* Bit Group Name: TAP_Y_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TAP_Y_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_TAP_Y_EN_ENABLED 		 = 0x04,
} LSM6DS3_IMU_TAP_Y_EN_t;

/*******************************************************************************
* Register      : TAP_CFG
* Address       : 0X58
* Bit Group Name: TAP_X_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TAP_X_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_TAP_X_EN_ENABLED 		 = 0x08,
} LSM6DS3_IMU_TAP_X_EN_t;

/*******************************************************************************
* Register      : TAP_CFG
* Address       : 0X58
* Bit Group Name: TILT_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TILT_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_TILT_EN_ENABLED 		 = 0x20,
} LSM6DS3_IMU_TILT_EN_t;

/*******************************************************************************
* Register      : TAP_CFG
* Address       : 0X58
* Bit Group Name: PEDO_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_PEDO_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_PEDO_EN_ENABLED 		 = 0x40,
} LSM6DS3_IMU_PEDO_EN_t;

/*******************************************************************************
* Register      : TAP_CFG
* Address       : 0X58
* Bit Group Name: TIMER_EN
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TIMER_EN_DISABLED 		 = 0x00,
	LSM6DS3_IMU_TIMER_EN_ENABLED 		 = 0x80,
} LSM6DS3_IMU_TIMER_EN_t;

/*******************************************************************************
* Register      : TAP_THS_6D
* Address       : 0X59
* Bit Group Name: TAP_THS
* Permission    : RW
*******************************************************************************/
#define  	LSM6DS3_IMU_TAP_THS_MASK  	0x1F
#define  	LSM6DS3_IMU_TAP_THS_POSITION  	0

/*******************************************************************************
* Register      : TAP_THS_6D
* Address       : 0X59
* Bit Group Name: SIXD_THS
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SIXD_THS_80_degree 		 = 0x00,
	LSM6DS3_IMU_SIXD_THS_70_degree 		 = 0x20,
	LSM6DS3_IMU_SIXD_THS_60_degree 		 = 0x40,
	LSM6DS3_IMU_SIXD_THS_50_degree 		 = 0x60,
} LSM6DS3_IMU_SIXD_THS_t;

/*******************************************************************************
* Register      : INT_DUR2
* Address       : 0X5A
* Bit Group Name: SHOCK
* Permission    : RW
*******************************************************************************/
#define  	LSM6DS3_IMU_SHOCK_MASK  	0x03
#define  	LSM6DS3_IMU_SHOCK_POSITION  	0

/*******************************************************************************
* Register      : INT_DUR2
* Address       : 0X5A
* Bit Group Name: QUIET
* Permission    : RW
*******************************************************************************/
#define  	LSM6DS3_IMU_QUIET_MASK  	0x0C
#define  	LSM6DS3_IMU_QUIET_POSITION  	2

/*******************************************************************************
* Register      : INT_DUR2
* Address       : 0X5A
* Bit Group Name: DUR
* Permission    : RW
*******************************************************************************/
#define  	LSM6DS3_IMU_DUR_MASK  	0xF0
#define  	LSM6DS3_IMU_DUR_POSITION  	4

/*******************************************************************************
* Register      : WAKE_UP_THS
* Address       : 0X5B
* Bit Group Name: WK_THS
* Permission    : RW
*******************************************************************************/
#define  	LSM6DS3_IMU_WK_THS_MASK  	0x3F
#define  	LSM6DS3_IMU_WK_THS_POSITION  	0

/*******************************************************************************
* Register      : WAKE_UP_THS
* Address       : 0X5B
* Bit Group Name: INACTIVITY_ON
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INACTIVITY_ON_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INACTIVITY_ON_ENABLED 		 = 0x40,
} LSM6DS3_IMU_INACTIVITY_ON_t;

/*******************************************************************************
* Register      : WAKE_UP_THS
* Address       : 0X5B
* Bit Group Name: SINGLE_DOUBLE_TAP
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_SINGLE_DOUBLE_TAP_DOUBLE_TAP 		 = 0x00,
	LSM6DS3_IMU_SINGLE_DOUBLE_TAP_SINGLE_TAP 		 = 0x80,
} LSM6DS3_IMU_SINGLE_DOUBLE_TAP_t;

/*******************************************************************************
* Register      : WAKE_UP_DUR
* Address       : 0X5C
* Bit Group Name: SLEEP_DUR
* Permission    : RW
*******************************************************************************/
#define  	LSM6DS3_IMU_SLEEP_DUR_MASK  	0x0F
#define  	LSM6DS3_IMU_SLEEP_DUR_POSITION  	0

/*******************************************************************************
* Register      : WAKE_UP_DUR
* Address       : 0X5C
* Bit Group Name: TIMER_HR
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_TIMER_HR_6_4ms 		 = 0x00,
	LSM6DS3_IMU_TIMER_HR_25us 		 = 0x10,
} LSM6DS3_IMU_TIMER_HR_t;

/*******************************************************************************
* Register      : WAKE_UP_DUR
* Address       : 0X5C
* Bit Group Name: WAKE_DUR
* Permission    : RW
*******************************************************************************/
#define  	LSM6DS3_IMU_WAKE_DUR_MASK  	0x60
#define  	LSM6DS3_IMU_WAKE_DUR_POSITION  	5

/*******************************************************************************
* Register      : FREE_FALL
* Address       : 0X5D
* Bit Group Name: FF_DUR
* Permission    : RW
*******************************************************************************/
#define  	LSM6DS3_IMU_FF_FREE_FALL_DUR_MASK  		0xF8
#define  	LSM6DS3_IMU_FF_FREE_FALL_DUR_POSITION  	3
#define  	LSM6DS3_IMU_FF_WAKE_UP_DUR_MASK  			0x80
#define  	LSM6DS3_IMU_FF_WAKE_UP_DUR_POSITION  	7


/*******************************************************************************
* Register      : FREE_FALL
* Address       : 0X5D
* Bit Group Name: FF_THS
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_FF_THS_5 		   = 0x00,
	LSM6DS3_IMU_FF_THS_7 		   = 0x01,
	LSM6DS3_IMU_FF_THS_8 		 	 = 0x02,
	LSM6DS3_IMU_FF_THS_10 		 = 0x03,
	LSM6DS3_IMU_FF_THS_11 		 = 0x04,
	LSM6DS3_IMU_FF_THS_13 		 = 0x05,
	LSM6DS3_IMU_FF_THS_15 		 = 0x06,
	LSM6DS3_IMU_FF_THS_16 		 = 0x07,
} LSM6DS3_IMU_FF_THS_t;

/*******************************************************************************
* Register      : MD1_CFG
* Address       : 0X5E
* Bit Group Name: INT1_TIMER
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_TIMER_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_TIMER_ENABLED 		 = 0x01,
} LSM6DS3_IMU_INT1_TIMER_t;

/*******************************************************************************
* Register      : MD1_CFG
* Address       : 0X5E
* Bit Group Name: INT1_TILT
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_TILT_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_TILT_ENABLED 		 = 0x02,
} LSM6DS3_IMU_INT1_TILT_t;

/*******************************************************************************
* Register      : MD1_CFG
* Address       : 0X5E
* Bit Group Name: INT1_6D
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_6D_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_6D_ENABLED 		 = 0x04,
} LSM6DS3_IMU_INT1_6D_t;

/*******************************************************************************
* Register      : MD1_CFG
* Address       : 0X5E
* Bit Group Name: INT1_TAP
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_TAP_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_TAP_ENABLED 		 = 0x08,
} LSM6DS3_IMU_INT1_TAP_t;

/*******************************************************************************
* Register      : MD1_CFG
* Address       : 0X5E
* Bit Group Name: INT1_FF
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_FF_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_FF_ENABLED 		 = 0x10,
} LSM6DS3_IMU_INT1_FF_t;

/*******************************************************************************
* Register      : MD1_CFG
* Address       : 0X5E
* Bit Group Name: INT1_WU
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_WU_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_WU_ENABLED 		 = 0x20,
} LSM6DS3_IMU_INT1_WU_t;

/*******************************************************************************
* Register      : MD1_CFG
* Address       : 0X5E
* Bit Group Name: INT1_SINGLE_TAP
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_SINGLE_TAP_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_SINGLE_TAP_ENABLED 		 = 0x40,
} LSM6DS3_IMU_INT1_SINGLE_TAP_t;

/*******************************************************************************
* Register      : MD1_CFG
* Address       : 0X5E
* Bit Group Name: INT1_SLEEP
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT1_SLEEP_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT1_SLEEP_ENABLED 		 = 0x80,
} LSM6DS3_IMU_INT1_SLEEP_t;

/*******************************************************************************
* Register      : MD2_CFG
* Address       : 0X5F
* Bit Group Name: INT2_TIMER
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_TIMER_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_TIMER_ENABLED 		 = 0x01,
} LSM6DS3_IMU_INT2_TIMER_t;

/*******************************************************************************
* Register      : MD2_CFG
* Address       : 0X5F
* Bit Group Name: INT2_TILT
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_TILT_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_TILT_ENABLED 		 = 0x02,
} LSM6DS3_IMU_INT2_TILT_t;

/*******************************************************************************
* Register      : MD2_CFG
* Address       : 0X5F
* Bit Group Name: INT2_6D
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_6D_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_6D_ENABLED 		 = 0x04,
} LSM6DS3_IMU_INT2_6D_t;

/*******************************************************************************
* Register      : MD2_CFG
* Address       : 0X5F
* Bit Group Name: INT2_TAP
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_TAP_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_TAP_ENABLED 		 = 0x08,
} LSM6DS3_IMU_INT2_TAP_t;

/*******************************************************************************
* Register      : MD2_CFG
* Address       : 0X5F
* Bit Group Name: INT2_FF
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_FF_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_FF_ENABLED 		 = 0x10,
} LSM6DS3_IMU_INT2_FF_t;

/*******************************************************************************
* Register      : MD2_CFG
* Address       : 0X5F
* Bit Group Name: INT2_WU
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_WU_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_WU_ENABLED 		 = 0x20,
} LSM6DS3_IMU_INT2_WU_t;

/*******************************************************************************
* Register      : MD2_CFG
* Address       : 0X5F
* Bit Group Name: INT2_SINGLE_TAP
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_SINGLE_TAP_DISABLED 		 = 0x00,
	LSM6DS3_IMU_INT2_SINGLE_TAP_ENABLED 		 = 0x40,
} LSM6DS3_IMU_INT2_SINGLE_TAP_t;

/*******************************************************************************
* Register      : MD2_CFG
* Address       : 0X5F
* Bit Group Name: INT2_SLEEP
* Permission    : RW
*******************************************************************************/
typedef enum {
	LSM6DS3_IMU_INT2_SLEEP_DISABLED 		 		 = 0x00,
	LSM6DS3_IMU_INT2_SLEEP_ENABLED 		 			 = 0x80,
} LSM6DS3_IMU_INT2_SLEEP_t;


struct IMU_settings {
    // Accelerometer settings
    uint8_t accel_enable;
    uint16_t accel_range;
    uint16_t accel_samplerate;
    uint16_t accel_bandwidth;

    uint8_t accel_FIFO_enable;
    uint8_t accel_FIFO_decimation;

    // Gyroscope settings
    uint8_t gyro_enable;
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

/**
 * @brief function to initialize IMU sensor.
 */
void LSM6DS3_init(void);

/**
 * @brief function to read sensor registers.
 */
ret_code_t read_register(nrf_drv_twi_t twi_instance, uint8_t device_addr, uint8_t register_addr, uint8_t *p_data, uint8_t bytes, bool no_stop);

/**
 * @brief function to test availablility of IMU by reading WHO_AM_I register.
 */
void LSM6DS3_who_am_i(void);

/**
 * @brief function to configure IMU parameters.
 *
 * Accelerometer parameters are configured using CTRL1_XL register.
 * Bit[1:0] are reserved for filter bandwidth selection. Values can be 400 Hz, 200 Hz, 100 Hz and 50 Hz
 * Bit[3:2] are reserved for full scale range selection. Values can be (+/-)2g, (+/-)4g, (+/-)8g and (+/-)16g
 * Bit[7:4] are reserved for output data rate and power mode selection.
 *
 * Gyroscope parameters are configured using CTRL2_G registers.
 * Bit[1] is reserved for full scale range selection at 125 dps.
 * Bit[3:2] are reserved for full scale range sselection from 245 dps, 500 dps, 1000 dps and 2000 dps
 * Bit[7:4] are reserved for output data rate selection.
 */
void LSM6DS3_config();

/**
 * @brief function to enable power down mode in accelerometer
 *
 * setting ODR_XL[3:0] in CTRL1_XL register to 0 enables power-down mode.
 */
void LSM6DS3_set_accel_power_down_mode();

/**
 * @brief function to enable low power mode in accelerometer
 *
 * Low power mode is enabled by setting one of the following ODR values: 13 Hz, 26Hz and 52 Hz
 */
void LSM6DS3_set_accel_low_power_mode(uint16_t value);

/**
 * @brief function to enable normal mode in accelerometer
 *
 * Normal mode is enabled by setting one of the following ODR values: 104 Hz and 208 Hz
 */
void LSM6DS3_set_accel_normal_mode(uint16_t value);

/**
 * @brief function to enable high performance mode in accelerometer
 *
 * High performance mode is enabled by setting one of the following ODR values: 416 Hz, 833 Hz, 1.66 kHz, 3.33 kHz and 6.66 kHz
 */
void LSM6DS3_set_accel_high_performance_mode(uint16_t value);

/**
 * @brief function to read accelerometer data
 */
void LSM6DS3_read_accl_data(int16_t *x_axis, int16_t *y_axis, int16_t *z_axis);


/**
 * @brief function to compute raw accelerometer data in g.
 *
 * Multiply linear acceleration sensitivity with raw_data. Divide the product by 1000 to obtain value in g.
 * Refer table 4.1 on page 19 of LSM6DS3 datasheet.
 */
float LSM6DS3_accelData_in_g(int16_t raw_data);

/**
 * @brief function to disable sleep mode in gyroscope
 */
void LSM6DS3_set_gyro_active_mode();

/**
 * @brief function to enable sleep mode in gyroscope
 */
void LSM6DS3_set_gyro_sleep_mode();

/**
 * @brief functioon to read gyroscope data.
 */
void LSM6DS3_read_gyro_data(int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);

/**
 * @brief function to compute raw gyroscope data in degrees per second (dps).
 *
 * Multiply raw data with angular rate sensitivity.
 * Refer table 4.1 on page 19 of LSM6DS3 datasheet
 */
float LSM6DS3_gyroData_in_dps(int16_t raw_data);

/**
 * @brief function to configure FIFO
 */
void LSM6DS3_FIFO_config(void);

/**
 * @brief function to empty FIFO buffer
 */
void LSM6DS3_clear_FIFO_buffer(void);

/**
 * @brief function to read FIFO status
 */
uint16_t LSM6DS3_read_FIFO_status(void);

/**
 * @brief function to read FIFO buffer
 */
int16_t LSM6DS3_read_FIFO_buffer(void);

/**
 * @brief function to configure tap functionality
 */
void LSM6DS3_tap_detect_config();
