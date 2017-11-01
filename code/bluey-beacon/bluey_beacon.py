"""

bluey_beacon.py

This program connects to the bluey 1.1 Nordic nRF52 dev board 
made by Electronut Labs. The firmware on Bluey has to be running the 
"bluey beacon" example. 

This code is expected to be run on a Raspverry Pi 3.

It assumes that you have bluez installed - it uses hcidump and hcitool.

The code reads advertisement packets from bluey, parses the 
Temperature, Humidity, and Ambient Light informationb and 
posts that to freeboard.io dashboard via dweet.io.

Electronut Labs
electronut.in

References:

1. Ruuvi Project.

https://github.com/ttu/ruuvitag-sensor/

"""

import re
import sys
import os
import urllib2
import time
import subprocess


# constant
pow_16 = 65536.0

# decode temperature
def decodeT(temp_val):
    return ((temp_val / pow_16) * 165 - 40)

# decode humidity
def decodeH(humid_val):
    return ((humid_val / pow_16) * 100)

# decode ambient light
def decodeL(adc_ch0, adc_ch1):
    result = 999.99
    channelRatio = (adc_ch1)/(float)(adc_ch0);
    # below formula is from datasheet
    if(channelRatio >= 0 and channelRatio <= 0.52):
        result = (0.0315 * adc_ch0) - (0.0593 * adc_ch0 * pow(channelRatio, 1.4))
    elif(channelRatio > 0.52 and channelRatio <= 0.65):
        result = (0.0229 * adc_ch0) - (0.0291 * adc_ch1)
    elif(channelRatio > 0.65 and channelRatio <= 0.80):
        result = (0.0157 * adc_ch0) - (0.0180 * adc_ch1)
    elif(channelRatio > 0.80 and channelRatio <= 1.30):
        result = (0.00338 * adc_ch0) - (0.00260 * adc_ch1)
    elif(channelRatio > 1.30):
        result = 0;
    return result

# decode T/H/L data
def decodeData(x1, x2, x3, x4):
    T = decodeT(x1)
    H = decodeH(x2)
    L = decodeL(x3, x4)
    return (T, H, L)

"""
This class uses hctool and hcidump to parse BLE adv data.

"""
class BLEScanner:

    hcitool = None
    hcidump = None
    
    def start(self):
        print('Start receiving broadcasts')
        DEVNULL = subprocess.DEVNULL if sys.version_info > (3, 0) else open(os.devnull, 'wb')

        subprocess.call('sudo hciconfig hci0 reset', shell = True, stdout = DEVNULL)
        self.hcitool = subprocess.Popen(['sudo', '-n', 'hcitool', 'lescan', '--duplicates'], stdout = DEVNULL)
        self.hcidump = subprocess.Popen(['sudo', '-n', 'hcidump', '--raw'], stdout=subprocess.PIPE)

    def stop(self):
        print('Stop receiving broadcasts')
        subprocess.call(['sudo', 'kill', str(self.hcidump.pid), '-s', 'SIGINT'])
        subprocess.call(['sudo', '-n', 'kill', str(self.hcitool.pid), '-s', "SIGINT"])

    def get_lines(self):
        data = None
        try:
            print("reading hcidump...\n")
            #for line in hcidump.stdout:
            while True:
                line = self.hcidump.stdout.readline()
                line = line.decode()
                if line.startswith('> '):
                    yield data
                    data = line[2:].strip().replace(' ', '')
                elif line.startswith('< '):
                    data = None
                else:
                    if data:
                        data += line.strip().replace(' ', '')
        except KeyboardInterrupt as ex:
            print("kbi")
            return
        except Exception as ex:
            print(ex)
            return

 
# main() function
def main():
    # use sys.argv if needed
    if len(sys.argv) < 2:
        print('Usage: python bluey-beacon.py MACADDR')
        exit(0)
    print 'starting...'

    deviceId = sys.argv[1]
    scanner = BLEScanner()
    scanner.start()
    
    # dweet.io base URL
    baseURL = "https://dweet.io/dweet/for/bluey-beacon-0001?"

    data = None
    while True:
        for line in scanner.get_lines():
            if line:
                found_mac = line[14:][:12]
                reversed_mac = ''.join(
                    reversed([found_mac[i:i + 2] for i in range(0, len(found_mac), 2)]))
                mac = ':'.join(a+b for a,b in zip(reversed_mac[::2], reversed_mac[1::2]))
                data = line[26:]
                if mac == deviceId and len(data) == 66:
                    #print(mac, data)
                    if u'626C756579' in data:
                        data2 = data[24:50]
                        #print(data)
                        x1 = int(data2[0:4], 16)
                        x2 = int(data2[4:8], 16)
                        x3 = int(data2[8:12], 16)
                        x4 = int(data2[12:16], 16)
                        #print("%x %x %x %x\n" % (x1, x2, x3, x4))
                        T, H, L = decodeData(x1, x2, x3, x4)
                        dweetURL = baseURL + "T=%.2f&&H=%.1f&&L=%d" % (T, H, L)
                        print(dweetURL)
                        try:
                            f = urllib2.urlopen(dweetURL)
                            res = f.read()
                            print(res)
                            f.close()
                        except:
                            print("dweet failed!")
        scanner.stop()
        exit(0)

# call main
if __name__ == '__main__':
    main()
