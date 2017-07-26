# Using arduino IDE with bluey

## Installation

[More detailed instructions will be over at: [electronut/arduino-bluey](https://github.com/electronut/arduino-bluey)]

### Board Manager

 1. [Download and install the Arduino IDE](https://www.arduino.cc/en/Main/Software) (At least v1.6.12)
 2. Start the Arduino IDE
 3. Go into Preferences
 4. Add ```https://raw.githubusercontent.com/electronut/arduino-bluey/master/docs/package_electronutlabs_boards_index.json``` as an "Additional Board Manager URL"
 5. Open the Boards Manager from the Tools -> Board menu and install "Electronut labs nRF5 Boards"
 6. Select 'Electronut labs bluey' from the Tools -> Board menu

__NOTE:__ During installation it takes the Arduino IDE a few minutes to extract the tools after they have been downloaded, please be patient.

## Usage

* After tools installation, you can now select "Electronut labs bluey" from the tools menu, and other related options. 

* Bluey comes preprogrammed with [serial DFU bootloader], which Nordic's nrfjprog utility can program using serial connection. To trigger bootloader, press and hold button1, press the Reset button and then release button1.

* Bluey's blue LED will start blinking in a 'timebomb' pattern, indicating that it is in DFU bootloader mode. Now you can press upload button in arduino IDE to program your code.

* If your bluey does not have a bootloader preinstalled, it can be programmed via the 'Tools->Burn bootloader' menu (you need one of the supported programmers). The source for the bootloader is [here](https://github.com/electronut/bluey_serial_dfu_bootloader).

* As mentioned in [electronut/arduino-bluey](https://github.com/electronut/arduino-bluey) `README`, you may need to program softdevice once before programming any sketches that use BLE capabilities (i.e. depend on softdevice being programmed).

* _NOTE_: You can use [bumpy](https://www.tindie.com/products/ElectronutLabs/bumpy-blackmagic-probe-compatible-swd-debugger/), or Blackmagicprobe compatible programmer/debugger (or others) to program the bootloader .hex file, or with arduino IDE's .hex output file too. (We're working on adding bumpy/BMP support to it).

## Pin mapping

```
                    Connector P3 on bluey

     A0       P0.02/AIN0   | 1   2 | P0.26        D0
     A1       P0.03/AIN1   | 3   4 | P0.27        D1
     A2       P0.04/AIN2   | 5   6 | P0.24/AIN4   A3
     D2            P0.22   | 7   8 | P0.29/AIN5   A4
     D3            P0.23   | 9  10 | P0.30/AIN6   A5
     D4            P0.24   | 11 12 | P0.31/AIN7   A6
     D5            P0.25   | 13 14 | Ex_vin
                     GND   | 15 16 | 3v3
                     GND   | 17 18 | GND


          button D6  - P0.16
              R  D7  - P0.19
              G  D8  - P0.18
              B  D9  - P0.17

      D10     SCL - P0.11    DRDYn - P0.12   D11
      D12     SDA - P0.13      INT - P0.14   D13
      D14    INT1 - P0.15     INT2 - P0.20   D15

              RXD - P0.08    TXD - P0.06
              RTS - P0.05    CTS - P0.07

              SS - P0.22    SCK - P0.25
            MISO - P0.24   MOSI - P0.23
```

* LED and button pins can be referenced by `PIN_LEDR`, `PIN_LEDG`, `PIN_LEDB` ,`LED_BUILTIN` (red one), `PIN_BUTTON` in arduino code.

## BLEPeriphial Arduino Library

* [arduino-BLEPeripheral](https://github.com/sandeepmistry/arduino-BLEPeripheral) library can be used creating BLE applications with bluey. Follow installation steps on it's `README`. See the library's [API](https://github.com/sandeepmistry/arduino-BLEPeripheral/blob/master/API.md) for more details on usage.

* _NOTE_: Although the description says "nRF8001 or nRF51822", but nRF52832 is also supported because despite changes in CPU and peripherals, the radio interface is identical in both (API is same, but there might be changes in power/sensitivity etc.)

* Once installed, you can see examples for this in examples menu. You can run most examples without modifications, unless some specify LED or button pins different from bluey's, in which case, just replace pin nnumbers with `LED_BUILTIN`, or `PIN_BUTTON` etc.

* Use [nRF connect](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en) (and other apps from nordic on play store) to scan, connect, interact with BLE peripherals created with these examples.
