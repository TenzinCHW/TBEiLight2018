#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

#include <Wire.h>

#define PACKET_SZ 32

int address = 0x28; // 28 is the address
byte byte1, byte2, byte3, byte4;

RF24 radio (9, 10);

struct dataStruct{
byte stuff[PACKET_SZ];
}transmitter1_data;

byte meow[PACKET_SZ];

unsigned char ADDRESS0[5]  = 
{
  0xb1,0x43,0x88,0x99,0x45
}; // Define a static TX address
//just change b1 to b2 or b3 to send to other pip on reciever

void setup()
{
 // Wire.begin();
  radio.begin();
  Serial.begin(115200);
  printf_begin();
  radio.setRetries(15,15);
  radio.enableDynamicPayloads();
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(ADDRESS0);
  radio.openReadingPipe(0,ADDRESS0);
  radio.stopListening();
  radio.printDetails();
  
  for (int i = 0; i < PACKET_SZ-1; i++) {
    transmitter1_data.stuff[i] = i;
    meow[i] = i;
  }
  for (int i = 0; i < 40; i++) {
//    transmitter1_data.stuff[PACKET_SZ-1] = i;
    meow[PACKET_SZ-1] = i;
//    radio.write(&transmitter1_data, sizeof(transmitter1_data));
    radio.write(&meow, sizeof(meow));
    delay(20);
  }
}

void loop() {
}
