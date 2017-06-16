## Bluey-Sensor-Test

This example implements a custom BLE service. The service includes four characteristic each containing data from the on-board sensors.

In order to proceed with this example, a little background information on custom BLE services and characteristics is essential. Use the links for explanation on [custom service](https://devzone.nordicsemi.com/tutorials/8/) and [custom characteristic](https://devzone.nordicsemi.com/tutorials/17/).

The four characteristics contain sensor data and can be recognized using the following table:

| **S. No** | **16-bit Characteristic UUID** | **Description** |
| --------- | ------------------------------ | --------------- |
| 1         | 0x1339                         | Temperature and Humidity data |  
| 2         | 0x7C10                         | Ambient Light data |
| 3         | 0xACC1                         | Accelerometer data |
| 4         | 0xC420                         | Gyroscope data |

*Note!* The 16-bit characteristic UUID is part of the 128-bit base UUID.

Testing:

1. Using nRF Connect;

  * Power Bluey using CR2032 battery or external power supply (not more than 6 V) or connect it to PC via USB.
  * Open nRF Connect on your smartphone.
  * Select: Scan devices
  * Connect: Bluey_BLE
  * Select: Unknown Service
  * Select: Notification Enable under each Unknown Characteristic.

Interpreting the data:

The data processed and stored by nRF52832 is in Little Endian (Least Significant Byte is stored at the lowest address) format. However, the data transmitted via BLE is in Big Endian (Most Significant Byte is stored at the lowest address) format.

For example: The 0x1339 Characteristic contains temperature and humidity data. The temperature and humidity data, bot are 16-bit values, transmitted in 8-bit packets. So, the first two bytes are temperature value while the last two bytes are humidity values.
So, if the data field contains data in following format **1B-00-47-00**, then **1B-00** is temperature data and **47-00** is humidity data.

As mentioned earlier, the transmitted data is in Big Endian format. Thus, on reversing the byte order temperature data is **00-1B** or **0x001B** which in decimal is **27**. Temperature is measure in degree centigrade while humidity is the relative humidity in percentage.
