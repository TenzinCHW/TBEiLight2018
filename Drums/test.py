import time
from time import sleep, time, gmtime
import config
import serial

ser = serial.Serial('/dev/cu.usbmodem1411', 115200, timeout=None)

def loop():
    out = bytearray("I$33$10,20:15,25$100,200,100:0,0,255$90$90\n", 'utf-8')
    ser.write(out)

if __name__ == '__main__':
    
    new_input = False

    while not new_input:
        msg = ser.readline() 
        if msg: 
            print(msg)
            parsed = msg.decode('utf-8').strip('\r\n')
            print(parsed)
            new_input = False

    loop()
