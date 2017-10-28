# Using arduino IDE with bluey

## Installation

Uploading without programmer requires bootloader programmed on the board. The repo [electronut/bluey_serial_dfu_bootloader](https://github.com/electronut/bluey_serial_dfu_bootloader/) 
contains the source and precompiled hex file of the bootloader as `hex/s132_nrf52_2.0.0_softdevice.hex`.

The bootloader has the softdevice s132 in it, so no other action is needed to use the BLEPeripheral library.

Bootloader mode is triggered by pressing and holding both prss buttons on bluey and releasing the reset button. The blue LED will start blinkng at an accelerating rate rpeatedly to 
indicate that the bootloader mode is active.

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

## Low Frequency Clock Source (LFCLKSRC)

If the selected board has an external 32 kHz crystal connected, it will be used as the source for the low frequency clock. Otherwise the internal 32 kHz RC oscillator will be used. The low frequency clock is used by the `delay(ms)` and `millis()` Arduino API's.

Bluey has an additional menu item under `Tools -> Low Frequency Clock` that allows you to select the low frequency clock source.

## BLEPeriphial Arduino Library

* [arduino-BLEPeripheral](https://github.com/sandeepmistry/arduino-BLEPeripheral) library can be used creating BLE applications with bluey. Follow installation steps on it's `README`. See the library's [API](https://github.com/sandeepmistry/arduino-BLEPeripheral/blob/master/API.md) for more details on usage.

* Ensure that Softdevice:"S132" is selected under tools menu. This is essential to compile BLE examples.

* _NOTE_: Although the description says "nRF8001 or nRF51822", but nRF52832 is also supported because despite changes in CPU and peripherals, the radio interface is identical in both (API is same, but there might be changes in power/sensitivity etc.)

* Once installed, you can see examples for this in examples menu. You can run most examples without modifications, unless some specify LED or button pins different from bluey's, in which case, just replace pin numbers with `LED_BUILTIN`, or `PIN_BUTTON` etc.

* Use [nRF connect](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en) (and other apps from nordic on play store) to scan, connect, interact with BLE peripherals created with these examples.
