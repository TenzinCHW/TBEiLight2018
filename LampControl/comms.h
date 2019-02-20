#ifndef COMMS
#define COMMS

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DigitalIO.h>

#define PACKET_SZ 32
#define POWER_PIN 13

void startup_nRF();
void radio_on();
void radio_off();
bool read_if_avail(uint8_t* buf);
void read_and_flush(uint8_t* buf);
void broadcast(uint8_t addr, byte* msg, uint8_t sz);
void print_buffer(uint8_t* buf, uint8_t len);

#endif
