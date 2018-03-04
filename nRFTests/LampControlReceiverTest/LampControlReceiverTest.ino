#include <SPI.h>
#include <nRF24L01.h>
#include "printf.h"
#include <RF24.h>

RF24 radio (10, 9);

#define PLOAD_WIDTH  32  // 32 unsigned chars TX payload

unsigned char rx_buf[PLOAD_WIDTH] = {0};
const unsigned char ADDRESS1[5]  = {0xb1, 0x41, 0x29, 0x75, 0x93};
const unsigned char ADDRESS0[5]  = {0xb0, 0x41, 0x29, 0x75, 0x93};

void setup() {
  Serial.begin(115200);
  radio.begin();
  printf_begin();
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  //  turn off acknowledgements
  radio.setRetries(1, 15);
  radio.setChannel(50);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(0, ADDRESS0);
  radio.openReadingPipe(1, ADDRESS1);
  radio.startListening();
  radio.printDetails();
  Serial.println("Receiver");
  delay(1000);
}

void loop() {
  //  read_and_reply(0, rx_buf);
  read_and_reply(1, rx_buf);
  //  read_if_avail(rx_buf);
}

void read_if_avail(uint8_t* buf) {
  if (radio.available()) {
    if (radio.getDynamicPayloadSize() < 1) {
      return;
    }
    read_and_flush(buf);
    print_buffer(buf, 32);
  }
}

void read_and_reply(uint8_t pipe_num, uint8_t* buf) {
  if (radio.available()) {
    if (radio.getDynamicPayloadSize() < 1) return;
    read_and_flush(buf);
    print_buffer(buf, PLOAD_WIDTH); // Needed if you are using relay
    buf[0] |= 1 << 7;
    switch (pipe_num) {
      case 0: broadcast(buf, ADDRESS0); break;
      case 1: broadcast(buf, ADDRESS1); break;
    }
  }
}

void read_and_flush(uint8_t* buf) {
  radio.read(buf, PLOAD_WIDTH);
  radio.flush_rx();
}

void broadcast(uint8_t* buf, unsigned char* address) {
  radio.stopListening();
  radio.openWritingPipe(address); // No need to close, just change the address. Only 1 address can be written to at the same time.
  radio.startWrite(buf, PLOAD_WIDTH, true);
  //  Serial.println(F("Wrote liao"));
  radio.txStandBy();
  radio.startListening();
}

void print_buffer(uint8_t* buf, uint8_t len) {
  for (int i = 0; i < len; i++) Serial.print(buf[i]);
  Serial.println();
}
