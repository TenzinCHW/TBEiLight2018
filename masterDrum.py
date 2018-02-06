import time
from time import sleep, time, gmtime
import config
import serial

ser = serial.Serial('/dev/cu.usbmodem1421', 115200, timeout=None)

# get data from files 
drum_loc = config.drum_loc
drum_rgb = config.drum_rgb
drums = config.drums
lamps = config.lamps
wavelength = config.wavelength
period = config.period
expiry = config.expiry

start_time = time()
lamp_acks = []

def setup_network():
    # init variables
    done = False
    last_global = 0.0
    counter = 0

    while not done and not all_acked(): 
        messages = []
        # block while listening for msg
        msg = ser.readline()
        if msg: 
            parsed = msg.decode('utf-8').strip('\r\n')
            messages = parsed.split('$')
    
            # if message is global SR
            if messages[0] == 'G' and messages[1] == 'SR':
                last_global = send_global_si(last_global)
            # if message is non-global SR
            elif messages[0] == 'g' and messages[1] == 'SR':
                lamp_id = messages[2]
                send_si(lamp_id)
            # if message is non-global SA 
            elif messages[0] == 'g' and messages[1] == 'SA':
                lamp_id = messages[2]
                register_ack(lamp_id)

            ser.flush()
    return True

def send_global_si(last_global):
    # message = G $ SI $ dloc1:dloc2:dloc3 $ drgb1:drgb2:drgb3 $ wavelength $ period $ expiry \n
    if time() - last_global > 10:
        last_global = time()
        message = 'G$SI'
        for loc in drum_loc: 
            message += str(loc[0]) + ',' + str(loc[1]) + ':'
        message += '$'
        for rgb in drum_rgb: 
            message += str(rgb[0]) + ',' + str(rgb[1]) + ',' + str(rgb[2]) + ':'
        message += '$' + str(wavelength) + '$'
        message += str(period) + '$'
        message += str(expiry) + '\n'
        send_msg(message)
        print('global si: ', message)
        return last_global

def send_si(lamp_id):
    # message = g $ SI $ lamp_id $ lamp_loc \n
    if lamp_id not in lamp_acks:
        message = 'g$SI$' + lamp_id + '$' + str(lamps[lamp_id]['loc'][0]) + ',' + str(lamps[lamp_id]['loc'][0]) + '\n' 
        send_msg(message)
        print('non-global si: ', message)

def register_ack(lamp_id):
    if lamp_id not in lamp_acks:
        lamp_acks.append(lamp_id)
        print('acked: ', lamp_acks)

def all_acked(): 
    return len(lamp_acks) == len(lamps)


def send_msg(msg):
    out = bytearray(msg, 'utf8')
    ser.write(out)


if __name__ == '__main__':
    while (time() - start_time) < 20: 
        setup_network()

    # while True:
    #     listening() 



