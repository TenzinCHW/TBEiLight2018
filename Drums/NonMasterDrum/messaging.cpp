#include "messaging.h"

void make_hello(byte* msg) {
  msg[0] = HELLO_MSG;
}

void make_drum_hit(byte* msg, uint8_t id, uint16_t counter, uint8_t intensity) {  // intensity is a value from 1 to 100
  msg[0] = DRUM_HIT_MSG;
  msg[DRUM_ID_BYTE] = id;
  msg[DRUM_HIT_INTENSITY] = intensity;
  msg[DRUM_COUNTER_BYTE] = counter;
}

