## Bluey-Sensor-Test

This is an introductory example including all the interface of temperature and humidity sensor **(HDC1010)**, ambient light sensor **(APDS9301)** and 6-axis IMU **(LSM6DS3)**. The data can be viewed on a serial monitor at **115200** baud rate and via nRF Connect or nRF Toolbox using Nordic UART Service (NUS).

Testing:

1. Using serial monitor;

  * Connect Bluey to PC via USB.
  * Open a serial monitor of your choice. (e.g. Coolterm, Putty etc.)
  * Select appropriate COM port.
  * Set __baud rate__ = 115200, __data bits__ = 8, __parity__ = none, __stop bits__ = 1.
  * Set __DTR__ - ON and __RTS__ - ON
  * Click: **Connect** (Coolterm) or **Open** (Putty)
  
  You should observe following result:

  ![](../../images/serial.png)
  
2. Using NUS;

  * Power Bluey using CR2032 battery or external power supply (not more than 6 V) or connect it to PC via USB.
  * Open nRF Connect on your smartphone.
  * Select: Scan devices
  * Connect: Bluey_ST
  * Select: Nordic UART Service
  * Select: Notification Enable under TX Characteristic.

  You should observe following result:

  ![](../../images/nus_data.png)

Alternatively,

  * Open nRF Toolbox on your smartphone.
  * Select: UART
  * Connect: Bluey_ST
  * Select: Options (beside EDIT)
  * Select: Show log.

NFC functionality displays temperature and humidity data when in contact with a smartphone NFC.

Testing:

  * Enable NFC on your smartphone.
  * Bring the NFC antenna close to the NFC antenna of your smartphone.
  * Hold until RGB LED turns **blue**.
  * Data should be available on your screen, as shown below.
  
  ![](../../images/nfc.png)

SD card functionality logs data from al the sensors available on board.

Testing:

  * Insert SD card in the SD card holder.
  * Power up Bluey.
  * Press SW3 to log data.
  * Power down Bluey.
  * Unmount SD card.
  * Insert SD card in SD card adapter and plug it in your PC.
  * Open *Bluey.txt*
  
  ![](../../images/sd_card.png)

*Note-* The functionalities are selected by defining certain parameters in the main.c file to enable them.__

|Functionality   |         Definition|
--------------- | ------------------
|HDC1010         |         #define TEMP_HUMID_DATA|
|APDS9301        |         #define AMBIENT_LIGHT_DATA|
|IMU_DATA        |         #define IMU_DATA|
|NFC_UPDATE      |         #define NFC_UPDATE|
