<img src="images/bluey-beacon.png" alt="Bluey Beacon" width= "600"/>

## Bluey Beacon

In this project we will build an IoT (Internet Of Things) device that displays
real-time Temperature, Humidity and Ambient Light level day on the internet.

## Architecture

The firmware on bluey sends out non-connectable non-scannable advertisement
packets with sensor data. These adv packets are picked up with a Python script
running on a Raspberry Pi 3. The script uses Linux *bluez* tools *hcidump*
and *hcitool* to parse the sensor data. The data is then posted online to a
*freeboard* dashboard via the *dweet.io* service.

## Power Supply

We're using a 3.7 V LiPo batter to power the beacon.

## Enclosure

We have build a simple laser-cut acrylic enclosure for our beacon. The design
files are available in the repository.

<img src="images/bluey-beacon-enclosure.png" alt="Bluey Beacon" width= "600"/>
