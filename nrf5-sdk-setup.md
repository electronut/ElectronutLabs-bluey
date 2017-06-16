# Getting Started with Nordic nRF5 SDK

Jump To:

### [Installation on Windows](#installation-on-windows)

### [Installation on Linux](#installation-on-linux)

<hr />

## Installation on Windows

This guide covers installation on Windows.

#### Get MinGW

* Download [MinGW](https://sourceforge.net/projects/mingw/files/) for windows for linux command line utilities.

* Run mingw-get-setup.exe

* Install at default location: C:\MinGW

* Select following packages under Basic Setup:

![](images/mingw.png)

* Select: Installation > Apply Changes > Apply

* Add: `C:\MinGW\bin` and `C:\MinGW\msys\1.0\bin` path to environment variable separated by a semi-colon(;).

#### Get nRF SDK12.2
* Download [nRF5 SDK12.2 ](https://www.nordicsemi.com/eng/nordic/Products/nRF5-SDK/nRF5-SDK-v12-zip/54291)

* Extract: nRF5_SDK_12.2.0_f012efa.zip.

#### Get nRFGo Studio
* Download [nRFGo Studio](https://www.nordicsemi.com/chi/node_176/2.4GHz-RF/nRFgo-Studio).

* Go to: Downloads and select **nRFgo-Studio-Winxx** as per your installed Windows OS.

* Run: nrfgostudio_win-64-xxx_installer

* Install the software with "typical" configurations.

* Post installation, Install: **nRF5x-Command-Line-Tools**

#### Get ARM-GCC
* Download [GNU-ARM-GCC](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads) (version 6-2016-q4) compiler.

* Run: gcc-arm-none-eabi.exe

* Check "Add path to environment variable" and click on "Finish"

#### Get Eclipse Mars 2
* Download Eclipse Mars 2 for C/C++ (32-bit) [package](http://www.eclipse.org/downloads/packages/release/Mars/2)

* Extract: eclipse-cpp-mars-2-win32.zip

* Install [GNU Arm Eclipse Plug-in](http://gnuarmeclipse.github.io/plugins/install/)

## Set up the environment to compile code

* Using Windows Explorer, open arm-gcc install directory. In my system, it is installed in `C:\Program Files (x86)\GNU Tools ARM Embedded`.

**GNU Tools ARM Embedded** includes installed ARM-GCC compilers. In this case, `6.2 2016q4`. This folder tells you the compiler version.

For exact version of currently installed ARM-GCC compiler, run:
` arm-none-eabi-gcc --version `

![](images/arm-none-eabi.png)

* Go to: `nRF5_SDK_12.2.0_f012efa\components\toolchain\gcc`

* Open Makefile.Windows in a text editor.

* Update:

`GNU_INSTALL_ROOT := C:/Program Files (x86)/GNU Tools ARM Embedded/6.2 2016q4`

and

`GNU_VERSION := 6.2.1`

* Go to: `<nRF5_Install_Directory>\examples\ble_peripheral\ble_app_hrs\pca10040\s132\armgcc`

* Issue `make` command to compile the project.

![](images/make.png)

* Add `SHELL=C:/Windows/System32/cmd.exe` in Makefile.Windows if following error occurs.

![](images/shell.png)

<hr />

## Installation on Linux

...
