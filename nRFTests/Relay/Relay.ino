#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

#define PLOAD_WIDTH 32

int address = 0x28; // 28 is the address
byte byte1, byte2, byte3, byte4;

RF24 radio (9, 10);

struct dataStruct {
  byte stuff[PLOAD_WIDTH];
} transmitter1_data;

byte rx_buf[PLOAD_WIDTH];

unsigned char ADDRESS1[5]  = {0xb1, 0x43, 0x88, 0x99, 0x45}; // Define a static TX address
unsigned char ADDRESS0[5]  = {0xb0, 0x43, 0x88, 0x99, 0x45}; // Define a static TX address
//just change b1 to b2 or b3 to send to other pip on reciever

void setup() {
  // Wire.begin();
  Serial.begin(115200);
  radio.begin();
  printf_begin();
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  //  turn off acknowledgements
  radio.setAddressWidth(5); //  5 byte addresses
  radio.setRetries(1, 5);
  radio.setChannel(50);
  radio.setPALevel(RF24_PA_MIN);  // TODO change to RF24_PA_MAX for actual one
  radio.openReadingPipe(0, ADDRESS0);
  radio.openReadingPipe(1, ADDRESS1);
  radio.startListening();
  radio.printDetails();
}

void loop() {
  read_rebroadcast(0, rx_buf);
  delay(4);
  read_rebroadcast(1, rx_buf);
  delay(4);
}

void read_rebroadcast(uint8_t pipe_num, uint8_t* buf) {
  if (radio.available()) {  //if (radio.available(pipe_num)) {
    if (radio.getDynamicPayloadSize() < 1) {
      return;
    }
    radio.read(buf, PLOAD_WIDTH);
//    print_buffer(buf, PLOAD_WIDTH);
    switch (pipe_num) {
      case 0: rebroadcast(buf, ADDRESS1);
      case 1: rebroadcast(buf, ADDRESS0);
    }
  }
}

void rebroadcast(uint8_t* buf, unsigned char* address) {
  buf[0] |= 1 << 7;
  broadcast(buf, address);
}

void broadcast(uint8_t* buf, unsigned char* address) {
  radio.stopListening();
  radio.openWritingPipe(address); // No need to close, just change the address. Only 1 address can be written to at the same time.
  radio.write(buf, PLOAD_WIDTH);
  radio.startListening();
}
