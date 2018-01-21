#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

RF24 radio (9, 10);

#define PLOAD_WIDTH  32  // 32 unsigned chars TX payload
byte pip;
byte pload_width_now;
byte newdata;
unsigned char rx_buf[PLOAD_WIDTH] = {0};
struct dataStruct1 {
  float p1;
  float t1;
  float s1;
} transmitter1_data;

struct dataStruct2 {
  float p1;
  float t1;
  float s1;
} transmitter2_data;

struct dataStruct3 {
  float p1;
  float t1;
  float s1;
} transmitter3_data;

unsigned char ADDRESS2[1] = {0xb2};
unsigned char ADDRESS3[1] = {0xb3};
unsigned char ADDRESS4[1] = {0xb4};
unsigned char ADDRESS5[1] = {0xb5};


unsigned char ADDRESS1[5]  =
{
  0xb1, 0x43, 0x88, 0x99, 0x45
}; // Define a static TX address

unsigned char ADDRESS0[5]  =
{
  0xb0, 0x43, 0x88, 0x99, 0x45
}; // Define a static TX address

void setup()
{

  radio.begin();
  printf_begin();
  Serial.begin(115200);
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(ADDRESS0);
  radio.openReadingPipe(0, ADDRESS0);
  radio.openReadingPipe(1, ADDRESS1);
  radio.openReadingPipe(2, ADDRESS2);
  radio.openReadingPipe(3, ADDRESS3);
  radio.openReadingPipe(4, ADDRESS4);
  radio.openReadingPipe(5, ADDRESS5);


  radio.startListening();
  radio.printDetails();
  delay(1000);

}

void loop()
{
  if ( radio.available(&pip) )
  {

    // Fetch the payload, and see if this was the last one.
    pload_width_now = radio.getDynamicPayloadSize();

    // If a corrupt dynamic payload is received, it will be flushed
    if (!pload_width_now) {

    }
    else
    {

      radio.read( rx_buf, pload_width_now );

      newdata = 1;

      // Spew it
      Serial.print(F("Data on pip= "));
      Serial.print(pip);
      Serial.print(F(" Got data size="));
      Serial.print(pload_width_now);
      Serial.print(F(" data="));
      for (byte i = 0; i < pload_width_now; i++)
      {
        Serial.print(" ");
        Serial.print(rx_buf[i]);                              // print rx_buf
      }
      Serial.print(" ");
    }
  }
  if (newdata == 1)
  {
    newdata = 0;

    if (pip == 1 && pload_width_now == sizeof(transmitter1_data))
    {
      memcpy(&transmitter1_data, rx_buf, sizeof(transmitter1_data));


      //print content recieved
      Serial.print(" Pressure//temp//status ");
      Serial.print(transmitter1_data.p1);
      Serial.print(" // ");
      Serial.print(transmitter1_data.t1);
      Serial.print(" // ");
      Serial.print(transmitter1_data.s1);

    }

    if (pip == 2 && pload_width_now == sizeof(transmitter2_data))
    {
      memcpy(&transmitter2_data, rx_buf, sizeof(transmitter2_data));




      Serial.print(" Pressure//temp//status ");
      Serial.print(transmitter2_data.p1);
      Serial.print(" // ");
      Serial.print(transmitter2_data.t1);
      Serial.print(" // ");
      Serial.print(transmitter2_data.s1);

    }

    if (pip == 3 && pload_width_now == sizeof(transmitter3_data))
    {
      memcpy(&transmitter3_data, rx_buf, sizeof(transmitter3_data));




      Serial.print(" Pressure//temp//status ");
      Serial.print(transmitter3_data.p1);
      Serial.print(" // ");
      Serial.print(transmitter3_data.t1);
      Serial.print(" // ");
      Serial.print(transmitter3_data.s1);

    }



    Serial.println("");
  }

  delay(100);

}
