import re
import sys
import os

from ble_communication import BleCommunicationNix

ble = BleCommunicationNix()
deviceId = u'FE:18:BB:3C:1E:76'

data = None
for ble_data in ble.get_datas():
    if ble_data[0] == deviceId:
        tmp = ble_data[1]
        if len(tmp) == 66:
            if u'626C756579' in tmp:
                 data = tmp[24:50]
            elif data:
                 data += tmp[10:64]
                 print(data)
                 print('\n')
            else:
                data = None
    else:
        pass
