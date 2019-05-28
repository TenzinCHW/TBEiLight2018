#include "messaging.h"

// For drum hits //
/*
 * input msg: pointer to state message buffer
 * return: ID of drum that sent the message currently in state message buffer (0-3)
 */
uint8_t get_drum_id(byte* msg) {
  return msg[DRUM_ID_BYTE];
}

/* 
 *  input msg: pointer to state message buffer
 * return: intensity from 0 to 1
 */
float get_hit_intensity(byte* msg) {
  uint16_t temp = msg[DRUM_HIT_INTENSITY] << 8 | msg[DRUM_HIT_INTENSITY+1];
  return temp / 100.0;
}

/*
 * input msg: pointer to state message buffer
 * return: drum hit counter of this drum
 */
uint16_t get_hit_counter(byte* msg) {
  uint16_t temp = msg[DRUM_COUNTER_BYTE] << 8 | msg[DRUM_COUNTER_BYTE + 1];
  return temp;
}

