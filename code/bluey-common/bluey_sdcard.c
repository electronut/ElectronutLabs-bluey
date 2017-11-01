/**
 * bluey-sdcard.c
 *
 * nRF52 SD card interface to log data.
 *
 * Electronut Labs
 * electronut.in
 */

#include <stdio.h>
#include "bluey_sdcard.h"

/**
 * @brief SDC block device definition
 */
NRF_BLOCK_DEV_SDC_DEFINE(
        m_block_dev_sdc,
        NRF_BLOCK_DEV_SDC_CONFIG(
                SDC_SECTOR_SIZE,
                APP_SDCARD_CONFIG(SDC_MOSI_PIN, SDC_MISO_PIN, SDC_SCK_PIN, SDC_CS_PIN)
         ),
         NFR_BLOCK_DEV_INFO_CONFIG("Bluey", "SDC", "1.00")
);



static volatile bool heading = true;
unsigned char str[100];

/**
 * @brief function to log data in SD card.
 */
void sdcard_sensor_update_data(float temperature, float humidity, float lux, float accel_X, float accel_Y, float accel_Z, float gyro_X, float gyro_Y, float gyro_Z)
{
    static FATFS fs;
    static DIR dir;
    static FILINFO fno;
    static FIL file;

    uint32_t bytes_written;
    FRESULT ff_result;
    DSTATUS disk_state = STA_NOINIT;

    // Initialize FATFS disk I/O interface by providing the block device.
    static diskio_blkdev_t drives[] =
    {
            DISKIO_BLOCKDEV_CONFIG(NRF_BLOCKDEV_BASE_ADDR(m_block_dev_sdc, block_dev), NULL)
    };

    diskio_blockdev_register(drives, ARRAY_SIZE(drives));

    NRF_LOG_INFO("Initializing disk 0 (SDC)...\r\n");
    for (uint32_t retries = 3; retries && disk_state; --retries)
    {
        disk_state = disk_initialize(0);
    }
    if (disk_state)
    {
        NRF_LOG_INFO("Disk initialization failed.\r\n");
        return;
    }

    uint32_t blocks_per_mb = (1024uL * 1024uL) / m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_size;
    uint32_t capacity = m_block_dev_sdc.block_dev.p_ops->geometry(&m_block_dev_sdc.block_dev)->blk_count / blocks_per_mb;
    NRF_LOG_INFO("Capacity: %d MB\r\n", capacity);

    NRF_LOG_INFO("Mounting volume...\r\n");
    ff_result = f_mount(&fs, "", 1);
    if (ff_result)
    {
        NRF_LOG_INFO("Mount failed.\r\n");
        return;
    }

    NRF_LOG_INFO("\r\n Listing directory: /\r\n");
    ff_result = f_opendir(&dir, "/");
    if (ff_result)
    {
        NRF_LOG_INFO("Directory listing failed!\r\n");
        return;
    }

    do
    {
        ff_result = f_readdir(&dir, &fno);
        if (ff_result != FR_OK)
        {
            NRF_LOG_INFO("Directory read failed.");
            return;
        }

        if (fno.fname[0])
        {
            if (fno.fattrib & AM_DIR)
            {
                NRF_LOG_RAW_INFO("   <DIR>   %s\r\n",(uint32_t)fno.fname);
            }
            else
            {
                NRF_LOG_RAW_INFO("%9lu  %s\r\n", fno.fsize, (uint32_t)fno.fname);
            }
        }
    }
    while (fno.fname[0]);
    NRF_LOG_RAW_INFO("\r\n");

    NRF_LOG_INFO("Writing to file " FILE_NAME "...\r\n");
    ff_result = f_open(&file, FILE_NAME, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Unable to open or create file: " FILE_NAME ".\r\n");
        return;
    }

    if(heading) {
      ff_result = f_write(&file, TEST_STRING, sizeof(TEST_STRING) - 1, (UINT *) &bytes_written);
      heading = false;
    }


    sprintf((char *)str, "Temperature = %.2f C\nHumidity = %.2f %%\nAmbient Light = %.2f lux\n\r\n",
                          (float)temperature, (float)humidity, (float)lux);

    ff_result = f_write(&file, str, sizeof(str) - 1, (UINT *) &bytes_written);

    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Write failed\r\n.");
    }
    else
    {
        NRF_LOG_INFO("%d bytes written.\r\n", bytes_written);
    }

    // flush cached information
    ff_result = f_sync(&file);

    sprintf((char *)str, "Accelerometer: X-axis = %.2f g, Y-axis = %.2f g, Z-axis = %.2f g\n\r\n",
                          (float)accel_X, (float)accel_Y, (float)accel_Z);

    ff_result = f_write(&file, str, sizeof(str) - 1, (UINT *) &bytes_written);

    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Write failed\r\n.");
    }
    else
    {
        NRF_LOG_INFO("%d bytes written.\r\n", bytes_written);
    }

    // flush cached information
    ff_result = f_sync(&file);

    sprintf((char *)str, "Gyroscope: X-axis = %.2f dps, Y-axis = %.2f dps, Z-axis = %.2f dps\n\r\n",
                          (float)gyro_X, (float)gyro_Y, (float)gyro_Z);

    ff_result = f_write(&file, str, sizeof(str) - 1, (UINT *) &bytes_written);
    if (ff_result != FR_OK)
    {
        NRF_LOG_INFO("Write failed\r\n.");
    }
    else
    {
        NRF_LOG_INFO("%d bytes written.\r\n", bytes_written);
    }

    // flush cached information
    ff_result = f_sync(&file);

    (void) f_close(&file);
    return;
}
