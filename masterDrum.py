import time
from time import sleep, time
import config
import serial

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=0)
ser.flush()

# get data from files 
drum_loc = config.drum_loc
drum_rgb = config.drum_rgb
drums = config.drums
lamps = config.lamps
wavelength = config.wavelength
period = config.period
expiry = config.expiry

# init variables
start_time = time()
last_global = ''
counter = 0
lamp_acks = []

def setup_network():
    # listen for message
    # msg = get_msg()
    # ok gonna mock non-global SR message from lamp 2
    read = 'g:SR$2'
    messages = read.strip('!')
    messages = messages.split('$') 
    # messages = [['g', 'SR'], ['2']]
    # if message is global SR
    if messages[0][0] == 'G' and messages[0][1] == 'SR':
        send_global_sr()
    # if message is non-global SR
    elif messages[0][0] == 'g' and messages[0][1] == 'SR':
        lamp_id = messages([1][0])
        send_sr(lamp_id)
    # if message is non-global SA 
    elif messages[0][0] == 'g' and messages[0][1] == 'SA':
        lamp_id = messages([1][0])
        register_ack(lamp_id)

def send_global_sr():
    # message = dloc1:dloc2:dloc3 $ drgb1:drgb2:drgb3 $ wavelength $ period $ expiry !
    if time() - last_global > 10:
        last_global = time()
        message = ''
        for loc in drum_loc: 
            message += str(loc[0]) + ',' + str(loc[1]) + ':'
        message += '$'
        for rgb in drum_loc: 
            message += str(rgb[0]) + ',' + str(loc[1]) + ',' + str(loc[2]) + ':'
        message += '$' + str(wavelength) + '$'
        message += str(period) + '$'
        message += str(expiry) + '!'
        send_msg(message)

def send_sr(lamp_id):
    # message = lamp_id $ lamp_loc !
    if lamp_id not in lamp_acks:
        message = lamp_id + '$' str(lamps.lamp_id[0]) + ',' + str(lamps.lamp_id[0]) + '!' 
        send_msg(message)

def register_ack(lamp_id):
    if lamp_id not in lamp_acks:
        lamp_acks.append(lamp_id)

# listen for drum hits 
def listening():
    # message = DrumHit $ drum_id $ intensity $ counter
    # msg = get_msg
    read = 'DH$3$20.3$2!'
    messages = read.strip('!')
    messages = messages.split('$') 
    # check if msg type is DH 
    if messages[0][0] == 'DH':
        drum_id = messages[0][1]


def get_msg():
    return ser.read()

def send_msg(msg):
    ser.write(msg)

if __name__ == '__main__':
    while (time() - start_time) < 20: 
        setup_network()

    # while True:
    #     listening() 



