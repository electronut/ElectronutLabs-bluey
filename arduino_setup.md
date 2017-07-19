# Using arduino IDE with bluey

#### Tools installation

* Download a newer release of Arduino IDE (tested with arduino 1.8.3, but at least v1.6.12) from https://www.arduino.cc/en/Main/Software

* We are still awaiting our pull request to add bluey to be merged into [sandeepmistry/arduino-nRF5](https://github.com/sandeepmistry/arduino-nRF5) repository. Once that is done, bluey will be in the supported boards section on the README at [https://github.com/sandeepmistry/arduino-nRF5]. At this point there is no special step apart from installing this

* For now, please follow installation instructions on [sandeepmistry/arduino-nRF5](https://github.com/sandeepmistry/arduino-nRF5) for installion. Once installed, please open your file manager and navigate to arduino boards boards installtion directory, which is `~/.arduino15/packages/` on Linux, and `C:\Users\(username)\AppData\Local\Arduino15\packages\` on Windows
    - if `arduino-nRF5` board was installed correctly, you can see navigate to `sandeepmistry/hardware/nRF5/0.3.0/` in this directory
    - download [arduino-nRF5_0.3.0_with_bluey.zip](arduino/arduino-nRF5_0.3.0_with_bluey.zip) and extract into this folder, replacing all current files

#### Usage

* After tools installation, you can now select "Electronut labs bluey" from the tools menu, and other related options. Currently (from arduino IDE), only 'ST-link v2', 'CMSIS-DAP', and 'Jlink' are the supported programmers.

* As mentioned in [sandeepmistry/arduino-nRF5](https://github.com/sandeepmistry/arduino-nRF5) `README`, you may need to program softdevice once before programming any sketches that use BLE capabilities (i.e. depend on softdevice eing programmed).

* _NOTE_: You can use [bumpy](https://www.tindie.com/products/ElectronutLabs/bumpy-blackmagic-probe-compatible-swd-debugger/), or Blackmagicprobe compatible programmer/debugger (or others) still with arduino's `.elf` output file (or hex)

#### Pin mapping

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

#### BLEPeriphial Arduino Library

* [arduino-BLEPeripheral](https://github.com/sandeepmistry/arduino-BLEPeripheral) library can be used creating BLE applications with bluey. Follow installation steps on it's `README`. See the library's [API](https://github.com/sandeepmistry/arduino-BLEPeripheral/blob/master/API.md) for more details on usage.

* _NOTE_: Although the description says "nRF8001 or nRF51822", but nRF52832 is also supported because despite changes in CPU and peripherals, the radio interface is identical in both (API is same, but there might be changes in power/sensitivity etc.)

* Once installed, you can see examples for this in examples menu. You can run most examples without modifications, unless some specify LED or button pins different from bluey's, in which case, just replace pin nnumbers with `LED_BUILTIN`, or `PIN_BUTTON` etc.

* Use [nRF connect](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en) (and other apps from nordic on play store) to scan, connect, interact with BLE peripherals created with these examples.
