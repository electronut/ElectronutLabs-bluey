<img src="images/bluey1.2-sm.jpg" alt="Bluey 1.1" />

## What is Bluey?

**Bluey** is an Open Source BLE (Bluetooth Low Energy) development board with Temperature, Humidity, Ambient Light and
Accelerometer sensors.

Bluey uses the Nordic nRF52832 BLE
SoC (System on a Chip) which has an ARM Cortex-M4F CPU and a 2.4 GHz radio
that supports BLE and other proprietary wireless protocols. It also supports
NFC, and in fact the board comes with a built-in NFC PCB antenna.

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

To get a project up and running with **bluey**, you need to first build the code to generate a *.hex*
file and then upload it to the board.

### Building code for Bluey

If you are just getting started with the nRF52832, we recommend that you set up and
use the Nordic nRF5 SDK for programming **bluey**. [Click here](nrf5-sdk-setup.md) to
read our *Getting Started with nRF5 SDK* guide.

### Uploading code to Bluey

There are two main ways of uploading code into **bluey**:

1. Use the built-in OTA (over the air) bootloader which lets you upload code
using a phone. [Click here](ota-prog.md) to read about this option.

2. Use an external programmer to upload the code. Here are a few ways of doing this:
   1. Using a cheap programmer loaded with *Black Magic Probe* firmware. [Click here](blackmagic-prog.md) to learn about this option.
   2. Using the Nordic nRF52-DK. [Click here](nRF52-DK-prog.md) to explore this option.

### Powering Bluey

You can power **bluey** in several ways:

1. Using the 5V micro USB connector (which also gives you the
  option to print debug messages via UART).
2. The + / -  power supply pins which can take regular 2.54 mm header pins,
a JST connector for a 3.7 V LiPo battery, or a 3.5 mm terminal block.
3. A CR2032 coin cell for low power applications.

The sensors on bluey require a minimum of 2.7 Volts to function properly.
Please note that the *maximum power supply to bluey is 6 Volts*. Do not
exceed it! We also have reverse polarity protection on all power supplies,
but better not to tempt fate. :-)

## What can you do with Bluey?

You can use **bluey** for a wide range of projects. The BLE part is ideal for IoT
projects, or if you want to control something with your phone. The nRF52832 SoC
has a powerful ARM Cortex-M4F CPU, so you can use this board for general purpose
microcontroller projects as well.

With BLE and useful built-in sensors, **bluey** is a great platform for learning
IoT, BLE programming, ARM, and microcontrollers in general. In fact, it's being
currently used in Embedded Systems training courses.

The [code](code/) folder
in this repository has a bunch of interesting demo projects that we have built using
**bluey** - so do take a look!

## Buy a Bluey!

Bluey is available for purchase for international customers from our [Tindie store][1]. Customers in India can purchase Bluey from our [Instamojo store][2]. We also provide discounts for bulk purchases. Do email us at **info@electronut.in** if you have any questions.

<a href="https://www.tindie.com/stores/ElectronutLabs/?ref=offsite_badges&utm_source=sellers_ElectronutLabs&utm_medium=badges&utm_campaign=badge_large"><img src="https://d2ss6ovg47m0r5.cloudfront.net/badges/tindie-larges.png" alt="I sell on Tindie" width="200" height="104"></a>

[1]: https://www.tindie.com/stores/ElectronutLabs/
[2]: https://www.instamojo.com/ElectronutLabs/bluey-nrf52832-ble-development-board-with-se/
