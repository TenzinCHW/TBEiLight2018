#include<DigitalIO.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

#define PLOAD_WIDTH 32

byte byte1, byte2, byte3, byte4;

RF24 radio (7, 8);

byte tx_buf[PLOAD_WIDTH];
byte rx_buf[PLOAD_WIDTH];

unsigned char ADDRESS1[5]  = {0xb1, 0x43, 0x88, 0x99, 0x45}; // Define a static TX address
unsigned char ADDRESS0[5]  = {0xb0, 0x43, 0x88, 0x99, 0x45}; // Define a static TX address
//just change b1 to b2 or b3 to send to other pipe on reciever

long start; // For timing

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
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
  radio.openReadingPipe(1, ADDRESS1);
  radio.stopListening();
  radio.printDetails();
  Serial.println("Transmitter relay");

  for (int i = 0; i < PLOAD_WIDTH - 1; i++) {
    tx_buf[i] = i;
  }

  wait_for_reply();
}

void loop() {
  delay(10);
}

void broadcast(uint8_t* buf, unsigned char* address) {
  radio.stopListening();
  radio.openWritingPipe(address); // No need to close, just change the address. Only 1 address can be written to at the same time.
  radio.startWrite(buf, PLOAD_WIDTH, true);
//  Serial.println("Wrote liao");
  radio.txStandBy();
  radio.startListening();
}

void read_if_avail(uint8_t* buf) {
  if (radio.available()) {
    if (radio.getDynamicPayloadSize() < 1) {
      return;
    }
    radio.read(buf, PLOAD_WIDTH);
    print_buffer(buf, PLOAD_WIDTH);
  }
}

void print_buffer(uint8_t* buf, uint8_t len) {
  for (int i = 0; i < len; i++) Serial.print(buf[i]);
  Serial.println();
}

void wait_for_reply() {
  radio.startListening();
  long total = 0;
  long onemsgtime;
  for (int i = 0; i < 1024; i++) {
    start = millis();
    tx_buf[PLOAD_WIDTH - 1] = i;
    broadcast(tx_buf, ADDRESS1);
    while (!radio.available());
    read_if_avail(rx_buf);
    onemsgtime = millis() - start;
    total += onemsgtime;
  }

  Serial.print(F("Total time: "));
  Serial.println(total);
}
