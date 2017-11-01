## Bluey-Sensor-Test

This example implements a custom BLE service. The service includes four characteristics, each containing data from the on-board sensors.

In order to proceed with this example, a little background information on custom BLE services and characteristics is essential. Use the links for explanation on [custom service](https://devzone.nordicsemi.com/tutorials/8/) and [custom characteristic](https://devzone.nordicsemi.com/tutorials/17/).

In this example, we define a custom 128-bit base UUID, 16-bit service UUID and four 16-bit characteristic UUIDs.  
The four characteristics contain sensor data and can be recognized using the following table:

| **S. No** | **16-bit Characteristic UUID** | **Description** |
| --------- | ------------------------------ | --------------- |
| 1         | 0x1339                         | Temperature and Humidity data |  
| 2         | 0x7C10                         | Ambient Light data |
| 3         | 0xACC1                         | Accelerometer data |
| 4         | 0xC420                         | Gyroscope data |

*Note!* The 16-bit characteristic UUID is part of the 128-bit base UUID.

The custom service and characteristics are defined in **bluey_service.c** file available in **bluey-common** directory.

The *main()* includes function calls for setting up BLE in connection-oriented mode and initiate advertising. HDC1010 (temperature and humidity sensor), APDS9301 (ambient light sensor) and LSM6DS3 (6-axis IMU) are initialized for data acquisition.

A timer instance (m_our_sensor_timer_id) is created for the custom service that acquires new data from sensors and transmits the data over BLE at an interval of 1 second. This functionality is implemented in *timer_timeout_sensor_handler()*.


#### Characteristics

  1. Temperature and Humidity - 0x1339
    The characteristic contains temperature in degree centigrade and relative humidity data in percentage.

  2. Ambient light - 0x7C10
    The characteristic contains data from adc channel 0 (ADC_CH0) and adc channel 1 (ADC_CH1) from APDS9301.
    In order to obtain ambient light data in lux (SI unit), refer *getlux(uint16_t adc_ch0, uint16_t adc_ch1)* function in **bluey-common/APDS9301.c**.

  3. Accelerometer - 0xACC1
    The characteristic contains 16-bit accelerometer data each, along X, Y and Z axes.
    In order to obtain data in terms of g, refer *LSM6DS3_accelData_in_g(int16_t raw_data)* function in **bluey-common/LSM6DS3.c**.

  4. Gyroscope - 0xC420
    The characteristic contains 16-bit gyroscope data each, along X, Y and Z axes.
    In order to obtain data in terms of dps (degrees per second), refer *LSM6DS3_gyroData_in_dps(int16_t raw_data)* function in **bluey-common/LSM6DS3.c**.


#### Interpreting the data

The data processed and stored by nRF52832 is in Little Endian (Least Significant Byte is stored at the lowest address) format. However, the data transmitted via BLE is in Big Endian (Most Significant Byte is stored at the lowest address) format.

For example: The 0x1339 Characteristic contains temperature and humidity data. The temperature and humidity data, bot are 16-bit values, transmitted in 8-bit packets. So, the first two bytes are temperature value while the last two bytes are humidity values.
So, if the data field contains data in following format **1B-00-47-00**, then **1B-00** is temperature data and **47-00** is humidity data.

As mentioned earlier, the transmitted data is in Big Endian format. Thus, on reversing the byte order temperature data is **00-1B** or **0x001B** which in decimal is **27**. Similarly, humidity data is **00-47** or **0047** which in decimal is **71**.


#### Testing

  * Install nRF Connect on your smartphone.
  * Power Bluey using CR2032 battery or external power supply (not more than 6 V) or connect it to PC via USB.
  * Open nRF Connect on your smartphone.
  * Select: Scan devices
  * Connect: Bluey_BLE
  * Select: Unknown Service
  * Select: Notification Enable under each Unknown Characteristic.
