#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

#define PLOAD_WIDTH 32
#define POWER_PIN 13

RF24 radio (7, 8);

byte tx_buf[PLOAD_WIDTH];
byte rx_buf[PLOAD_WIDTH];

const unsigned char ADDRESS1[5]  = {0xb1, 0x41, 0x29, 0x75, 0x93};
const unsigned char ADDRESS0[5]  = {0xb0, 0x41, 0x29, 0x75, 0x93};

long start; // For timing
void setup() {
  Serial.begin(115200);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  radio.begin();
  printf_begin();
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  //  turn off acknowledgements
  radio.setRetries(1, 15);
  radio.setChannel(25);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(0, ADDRESS0);
  radio.openReadingPipe(1, ADDRESS1);
  radio.startListening();
  radio.printDetails();
  Serial.println("Transmitter");
  
  tx_buf[0] = 255;
}

void loop() {
  broadcast(tx_buf, ADDRESS0);
  Serial.println(F("Transmitting"));
  delay(1000);
}

void broadcast(uint8_t* buf, unsigned char* address) {
  radio.stopListening();
  radio.openWritingPipe(address); // No need to close, just change the address. Only 1 address can be written to at the same time.
  radio.startWrite(buf, PLOAD_WIDTH, true);
  radio.txStandBy();
  radio.startListening();
}

bool read_if_avail(uint8_t* buf) {
  if (radio.available()) {
    if (radio.getDynamicPayloadSize() < 1) {
      return false;
    }
    read_and_flush(buf);
    //    print_buffer(buf, PLOAD_WIDTH);
    return true;
  } else {
    return false;
  }
}

void read_and_flush(uint8_t* buf) {
  radio.read(buf, PLOAD_WIDTH);
  radio.flush_rx();
}

void print_buffer(uint8_t* buf, uint8_t len) {
  for (int i = 0; i < len; i++) Serial.print(buf[i]);
  Serial.println();
}

void wait_for_reply() {
  radio.startListening();
  long total = 0;
  long onemsgtime;
  for (int i = 0; i < 50; i++) {
    start = millis();
    tx_buf[PLOAD_WIDTH - 1] = i;
    while (!radio.available()) {
      broadcast(tx_buf, ADDRESS1);
      delay(2);
    }
    if (read_if_avail(rx_buf)) Serial.println("woohoo");
    onemsgtime = millis() - start;
    total += onemsgtime;
  }

  Serial.print(F("Total time: "));
  Serial.println(total);
}
