import time
from time import sleep, time
import serial

ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=0)
ser.flush()

drum_loc = ((0,0),(0,30.6))
node_loc = {843321:(0,7.3)} #:(0,20.4), 4
node_registered = {}
time_limit = 20
start_time = time()

for chip_id in node_loc:
    node_registered[chip_id] = False

def register_chip(chip_id):
    # add chip_id to node_ip using the ip address of the node
    # check location from node_loc and send the 'x,y' location back
    chip_id = int(chip_id)
    loc = [str(loc) for loc in node_loc[chip_id]]
    loc_data = 'L' + str(chip_id) + ':' + ','.join(loc)
    ser.write(str.encode(loc_data))
    print(chip_id)
    start_time = time() 

## parses all incoming setup requests (exclusive of drum hits)
def setup_network():
    data = ser.readline()
    data = data.decode('utf-8')
    data = data.strip()

    # if data is 1 byte and it is a global SR 
    if data.len() == 8 and data[5] == '1':
        # check if <10s since last send_global()
        send_global_si()
    # if data is 1 byte and it is a non-global SR 
    elif data.len() == 8 and data[5] == '0':
        # read the next msg, which is the non-global SR 
        sr = ser.readline
        digits = [str(int(sr[i:i+2], 2)) for i in range(0, len(sr), 2)]
        lamp_id = ''.join(digits)
        send_si(lamp_id)

    # elif SA 

def send_global_si():
    # 4 drum locations
    byte_drum_loc = [str(l[0]).encode() + str(l[1]).encode() for l in drum_loc]
    ser.write(','.join(byte_drum_loc))
    ## TODO hmm does the comma take up extra bits?? and do all the attributes go in one ser.write 
    ## or is there a ser.write for each attribute? 
    # 4 drum colors - hard coded? like loc? 
    # wavelength
    # wave period
    # expiry_time
    

def send_si(lamp_id): 


    # # Wait for node to send a string 'c_id:<chip_id>'
    # if data[:5] == 'c_id:':
    #     register_chip(data[5:])
    # # then wait for reply 'ok' from same ip and set node_registered[node_ip[ipaddress_of_packet]] = True
    # elif data[:2] == 'ok':
    #     node_registered[data[2:]] = True


if __name__ == '__main__':
    # while not all(node_registered.values()):
    while (time() - start_time) < 20: 
        setup_network()
        print(node_registered)
    while True:
        drum_loud = input('Enter a drum (1 or 2) followed by a comma and loudness between 0 and 1\n')
        var = drum_loud.split(',')
        if len(var) != 2:
            print('Invalid input format')
            continue

        drum_index = int(var[0])
        loud = int(var[1])

        if not 0 < drum_index <= len(drum_loc):
            print('Invalid drum index (1 or 2)')
            continue

        if not 0 <= loud <= 1023:
            print('value out of range, should be between 0 and 1023')
            continue

        drum = [str(loc) for loc in drum_loc[drum_index-1]]

        try:
            data = ':'.join((','.join(drum), str(loud)))
            ts = time.time()
            data = ':'.join((data,str(ts)))
            ser.write(str.encode(data))

        except (KeyboardInterrupt, SystemExit):
            raise
        except:
            traceback.print_exc()

