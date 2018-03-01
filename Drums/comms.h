#ifndef COMMS
#define COMMS 1

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define PACKET_SZ 32

void startup_nRF();
bool read_if_avail(uint8_t* buf);
void read_and_flush(uint8_t* buf);
void broadcast(uint8_t addr, byte* msg);
void print_buffer(uint8_t* buf, uint8_t len);

#endif