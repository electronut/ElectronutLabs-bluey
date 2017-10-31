## Arduino support for Bluey

The library is a basic project to demonstrate the use of Bluey with Arduino IDE. It includes files for HDC1010 and APDS9301 sensors to 
obtain temperature/humidity and ambient light data respectively. 

Sparkfun's library for LSM6DS3 is used to obtain Accelerometer and Gyroscope data.

Sandeep Mistry's BLEPeripheral library is used to implement Nordic UART Service.


## Examples

### bluey_sensor_test

Example to transmit sensor data as per user's request using Nordic UART Service and nRF Connect application.

Nordic UART Service consists of Rx Characteristic and Tx Characteristic.

These characteristics are defined with respect to the BLE Client i.e. BLE device containing nRF52832 SoC.

Thus, Tx characteristic contains data to be transmitted by Bluey and Rx characteristic contains data received by Bluey.

The sensor data is grouped into three sets, where each set is the value to be written to Bluey. These sets are as follows:

| Set Value | Sensor Data |
| --------- | ----------- |
| 1 | Ambient light, Temperature, Humidity |
| 2 | Accelerometer: X, Y, Z |
| 3 | Gyroscope: X, Y, Z |


## Testing

* Setup Arduino environment for Bluey as described in the **Arduino Bootloader** section [here](../../../../README.md)

* Download nRF Connect mobile application.

* Enable bluetooth and scan for devices.

* Connect: **Bluey**

* Click: **Nordic UART Service**

* Enable notifications by clicking on the *enable notification icon* shown by three downward arrows in the Tx characteristic. 
The characteristic value should be **Request data** indicating Bluey is expecting the user to request for a  set of data.

* Click on *write characteristic icon* in the Rx characteristic, indicated by a single upward arrow. Write 1, 2 or 3 as per 
the data you want to receive. Refer to the data above to see the relation between set number and data contained in the set.

* The value of Tx characteristic should now contain relevant sensor data.

