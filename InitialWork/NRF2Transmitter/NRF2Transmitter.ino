#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

#include <Wire.h>

int address = 0x28; // 28 is the address
byte byte1, byte2, byte3, byte4;

RF24 radio (9, 10);

struct dataStruct{
float p1;
float t1;
float s1; 
}transmitter1_data;

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
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(ADDRESS0);
  radio.openReadingPipe(0,ADDRESS0);
  radio.stopListening();
  radio.printDetails();
}

void loop()
{
   transmitter1_data.p1=1002.36;
   transmitter1_data.t1=27.33;
   transmitter1_data.s1=15.9;
   
   bool ok=radio.write(&transmitter1_data, sizeof(transmitter1_data));

   
     if(ok)
       {
          // content to be transmitted 
          Serial.println("Pipe 1");
          Serial.println(transmitter1_data.p1);
          Serial.println(transmitter1_data.t1);
          Serial.println(transmitter1_data.s1);
         
       }
      else
       {
          Serial.println("omg no it fail");
       }
   delay(1000);
  
}
