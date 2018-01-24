#include <SPI.h>
#include <nRF24L01.h>
#include "printf.h"
#include <RF24.h>

RF24 radio (9, 10);

#define PLOAD_WIDTH  32  // 32 unsigned chars TX payload

unsigned char rx_buf[PLOAD_WIDTH] = {0};
unsigned char ADDRESS1[5]  = {0xb1, 0x43, 0x88, 0x99, 0x45}; // Define a static TX address
unsigned char ADDRESS0[5]  = {0xb0, 0x43, 0x88, 0x99, 0x45}; // Define a static TX address

void setup() {
  Serial.begin(115200);
  radio.begin();
  printf_begin();
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  //  turn off acknowledgements
  radio.setAddressWidth(5); //  5 byte addresses
  radio.setRetries(1, 15);
  radio.setChannel(50);
  radio.setPALevel(RF24_PA_MIN);  // TODO change to RF24_PA_MAX for actual one
  //  void setRetries(uint8_t delay, uint8_t count);  // for TX code
  //   * @param delay How long to wait between each retry, in multiples of 250us,
  //   * max is 15.  0 means 250us, 15 means 4000us.
  //   * @param count How many retries before giving up, max 15
  //setChannel(uint8_t channel);  // can choose channel from 0-125
  //    void setPALevel ( uint8_t level );  //   * RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
  //   * The power levels correspond to the following output levels respectively:
  //   * NRF24L01: -18dBm, -12dBm,-6dBM, and 0dBm

  //  radio.openWritingPipe(ADDRESS0);  // for relay code
  radio.openReadingPipe(0, ADDRESS0);
  radio.openReadingPipe(1, ADDRESS1);

  radio.startListening();
  radio.printDetails();
  delay(1000);
}

void loop() {
  read_if_avail(0, rx_buf);
  read_if_avail(1, rx_buf);
  delay(10);
}

void read_if_avail(uint8_t* pipe_num, uint8_t* buf) {
  if (radio.available(pipe_num)) {
    if (radio.getDynamicPayloadSize() < 1) {
      return;
    }
    radio.read(buf, sizeof(buf));
    print_buffer(buf, PLOAD_WIDTH);
  }
}

void print_buffer(uint8_t* buf, uint8_t len) {
  for (int i = 0; i < len; i++) Serial.print(buf[i]);
  Serial.println();
}

