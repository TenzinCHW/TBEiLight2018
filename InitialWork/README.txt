### Star network setup ###

1. Set up DHCP server on one Raspberry Pi and broadcast its standalone network.
2. Run drums.py on Raspberry Pi and turn on all ESP8266 chips.
3. All nodes will know the SSID and password of this standalone network, connect to it and send in their chip id and IP address.
4. Raspberry Pi will send the physical locations to every node (pre-configured).
5. A range extender will extend the range of the network.
6. Transmitter node will broadcast UDP packets to announce when a drum is hit (user input 1 - 10 to indicate which drum and 0 - 1 to indicate loudness, drum locations stored in dictionary)
7. Nodes will flash LED based on a wave equation and timestamp of drum hit.

Files:
drums.py -> run on Raspberry Pi
ESP8266StarNode.ino -> compile and flash onto all ESP8266 chips

### Mesh network setup ###

1. Connect the root node of the mesh to the Raspberry Pi via UART (may need to change location of UART port in drumsMesh.py)
2. Start drumsMesh.py and turn on all ESP8266 non-root node chips.
3. Messages will be passed from the Raspberry Pi to the root node ESP8266 of the mesh via UART.
4. Root node will broadcast any messages from Raspberry pi unless a chip ID is provided in the message, in which case a single packet will be sent to the chip with stated chip ID.
5. Any messages sent to the root node ESP8266 will be relayed to Raspberry Pi for processing.

Files:
drumsMesh.py -> run on Rasperry Pi
ESP8266MeshRoot.ino -> compile and flash onto ESP8266 chip that is connected to Raspberry Pi UART
ESP8266Mesh.ino -> compile and flash onto all other ESP8266 chips

e.g.
drum_loc = ((1.5,3.5))

Input to transmitter node
1,1023
(drum index,hit loudness)

Receiving node will receive
1.5,3.5:1023:timestamp
(drum_x,drum_y:loudness:timestamp)