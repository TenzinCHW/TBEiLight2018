import time
import socket, traceback

#host = '<broadcast>'
src_host = ''
src_port = 5010
dest_host = '192.168.255.255'
dest_port = 4210

src_address = (src_host, src_port)
dest_address = (dest_host, dest_port)

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST,1)
s.bind(src_address)

drum_loc = ((0,0),(0,30.6))
node_loc = {4843321:(0,7.3), 4842972:(0,14.7), 4843059:(0, 25.4)}   #4843090:(0,20.4), 
node_ip = {}
node_registered = {}
for chip_id in node_loc:
    node_registered[chip_id] = False

def register_chip(chip_id, address):
    # add chip_id to node_ip using the ip address of the node
    # check location from node_loc and send the 'x,y' location back
    chip_id = int(chip_id)
    node_ip[address] = chip_id
    loc = [str(loc) for loc in node_loc[chip_id]]
    loc_data = 'L' + ','.join(loc)
    s.sendto(loc_data.encode('utf-8'), address)
    print(chip_id, address)

def setup_network():
    data, address = s.recvfrom(65535)
    data = data.decode('utf-8')

    # Wait for node to send a string 'c_id:<chip_id>'
    if data[:5] == 'c_id:':
        register_chip(data[5:], address)
    # then wait for reply 'ok' from same ip and set node_registered[node_ip[ipaddress_of_packet]] = True
    elif data == 'ok':
        node_registered[node_ip[address]] = True


if __name__ == '__main__':
    while not all(node_registered.values()):
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

            s.sendto(data.encode('utf-8'), dest_address)
        except (KeyboardInterrupt, SystemExit):
            raise
        except:
            traceback.print_exc()

