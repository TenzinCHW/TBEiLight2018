#include "comms.h"

byte msg_buf[32];

RF24 radio(7, 8);

void startup_nRF() {
  radio.begin();
  radio.setRetries(15,15);  // TODO find out what all this stuff means
  radio.enableDynamicPayloads();
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(ADDRESS0);
  radio.openReadingPipe(0,ADDRESS0);
  radio.stopListening();
  radio.printDetails();
}

void send_msg(byte* msg) {  // TODO figure out a system for sending outward and inward on diff channels because can't read and write to/from channel at same time?
  // TODO stop listening
  radio.write(&msg, sizeof(msg));
  // TODO start listening
}

