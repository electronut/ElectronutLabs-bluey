![bluey](bluey-1.0.png)

## What is Bluey?

**Bluey** is a BLE (Bluetooth Low Energy) development board with Temperature/Humidity, Ambient Light and
Accelerometer sensors.

Bluey uses the Nordic nRF52832 BLE
SoC (System on a Chip) which has an ARM Cortex-M4F CPU and a 2.4 GHz radio
that supports BLE and other proprietary wireless protocols. It also supports
NFC and in fact the board comes with a built-in NFC PCB antenna.

Bluey is ideal for a whole bunch of low-power IoT (Internet Of Things) projects.

### Specifications

- Nordic nRF52832 QFAA BLE SoC (512k RAM / 64k Flash)
- TI HDC1010 Temperature/Humidity sensor
- APDS-9300-020 ambient light sensor
- ST Micro LSM6DS3 accelerometer
- CREE RGB LED
- CP2104 USB interface
- 2 push buttons
- Coin cell holder
- Micro SD slot
- 2.4 GHz PCB antenna
- NFC PCB antenna

## Getting Started with Bluey


## Bluey project on hackaday.io

https://hackaday.io/project/19962-bluey

## What can you do with Bluey?

Well, it's up to your imagination. :-) But here are some demo projects we have built using **bluey**.

- [SensorTest][1] - outputs all sensor data via UART/USB.
- [SensorTestBLE][2] - defines custom service to send sensor data via BLE.
- [BlueyBotSimple][3] - a simple phone controlled 2WD robot.
- [Blueybeacon][4] - displaying sensor data on web using a Raspberry Pi as gateway.

## Coming Soon...

Some of the additional things that we are working on...

- Data logging
- HID on Raspberry Pi
- Pedometer
- Object Tracker
- nRF IoT SDK 6LowPAN demo
- Quadcopter
