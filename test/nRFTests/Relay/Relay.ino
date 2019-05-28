#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

#define PLOAD_WIDTH 32

byte byte1, byte2, byte3, byte4;

RF24 radio (9, 10);

byte rx_buf[PLOAD_WIDTH];

unsigned char ADDRESS1[5]  = {0xb1, 0x43, 0x88, 0x99, 0x45}; // Define a static TX address
unsigned char ADDRESS0[5]  = {0xb0, 0x43, 0x88, 0x99, 0x45}; // Define a static TX address
//just change b1 to b2 or b3 to send to other pip on reciever

void setup() {
  Serial.begin(115200);
  radio.begin();
  printf_begin();
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
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
  read_rebroadcast(rx_buf);
}

void read_rebroadcast(uint8_t* buf) {
  if (radio.available()) {
    if (radio.getDynamicPayloadSize() < 1) {
      return;
    }
    radio.read(buf, PLOAD_WIDTH);
    print_buffer(buf, PLOAD_WIDTH);
    if ((buf[0] >> 7) & 1) broadcast(buf, ADDRESS1);
    else broadcast(buf, ADDRESS0);
//    switch ((buf[0] >> 7) & 1) {
//      case 1: broadcast(buf, ADDRESS1);
//      case 0: broadcast(buf, ADDRESS0);
//    }
  }
}

void broadcast(uint8_t* buf, unsigned char* address) {
  radio.stopListening();
  radio.openWritingPipe(address); // No need to close, just change the address. Only 1 address can be written to at the same time.
  radio.startWrite(buf, PLOAD_WIDTH, true);
  radio.txStandBy();
  radio.startListening();
}

void print_buffer(uint8_t* buf, uint8_t len) {
  for (int i = 0; i < len; i++) Serial.print(buf[i]);
  Serial.println();
}
