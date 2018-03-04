#include "comms.h"
#include "printf.h"

RF24 radio(10, 9);

const unsigned char ADDR1[5]  = {0xb1, 0x41, 0x29, 0x75, 0x93};
const unsigned char ADDR0[5]  = {0xb0, 0x41, 0x29, 0x75, 0x93};

void startup_nRF() {
  radio.begin();
  printf_begin();
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  //  turn off acknowledgements
  radio.setRetries(1, 15);
  radio.setChannel(50);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(0, ADDR0);
  radio.openReadingPipe(1, ADDR1);
  radio.startListening();
  radio.printDetails();
}

bool read_if_avail(uint8_t* buf) {
  if (radio.available()) {
    if (radio.getDynamicPayloadSize() < 1) {
      return false;
    }
    read_and_flush(buf);
    print_buffer(buf, PACKET_SZ);
    return true;
  } else {
    return false;
  }
}

void read_and_flush(uint8_t* buf) {
  radio.read(buf, PACKET_SZ);
  radio.flush_rx();
}

void broadcast(uint8_t addr, byte* msg) {
  radio.stopListening();
  switch (addr) {
    case 0 : radio.openWritingPipe(ADDR0);
    case 1 : radio.openWritingPipe(ADDR1);
    default: radio.openWritingPipe(ADDR0);
  }
  radio.startWrite(msg, PACKET_SZ, true);
  radio.txStandBy();
  radio.startListening();
}

void print_buffer(uint8_t* buf, uint8_t len) {
  for (int i = 0; i < len; i++) Serial.print(buf[i]);
  Serial.println();
}
