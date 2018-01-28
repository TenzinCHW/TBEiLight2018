#include "messaging.h"

// ==== PACKING MSG ==== //

void set_relay_bit(byte* msg) {
  msg[0] |= (1 << 7);
}

byte* make_ack(uint16_t id) {
  byte ack[3];
  ack[0] = 0b10;
  ack[2] = id;  // id fills in from the back of 2 bits
  return ack;
}

byte* make_indiv_req(uint16_t id) {
  byte req[3];
  req[0] = 1;
  req[2] = id;
  return req;
}

byte make_glob_req() {
  return 1 << GLOBAL_BIT;
}

// ==== PARSING MSG ==== //

uint8_t get_msg_type(byte* msg) {
  return msg[0] & 0b111;
}

bool is_global(byte* msg) {
  return (msg[0] >> GLOBAL_BIT) & 1;
}

bool to_set_as_relay(byte* msg) {
  return (msg[0] >> SET_AS_RELAY) & 1;
}

bool to_be_relayed(byte* msg) {
  return ~((msg[0] >> RELAY_BIT) & 1);
}

float get_drum_x(byte* msg, uint8_t drum_index) {
  return (msg[4 * drum_index + DRUM_X1_START] << 8 | msg[4 * drum_index + DRUM_X2_START]) / 10.0;
}

float get_drum_y(byte* msg, uint8_t drum_index) {
  return (msg[4 * drum_index + DRUM_Y1_START] << 8 | msg[4 * drum_index + DRUM_Y2_START]) / 10.0;
}

uint8_t get_drum_colour(byte* msg, uint8_t drum_index, uint8_t colour) {  // colour maps as 0 (R), 1 (G) and 2 (B)
  return msg[3 * drum_index + colour + COLOUR_START_BYTE];
}

float get_wavelength(byte* msg) {
  return msg[WAVELENGTH_BYTE] / 10.0;
}

uint16_t get_period(byte* msg) {
  return msg[PERIOD_BYTE] * 100;
}

uint16_t get_expiry(byte* msg) {
  return msg[EXPIRY_BYTE] * 100;
}

bool addressed_to_id(byte* msg, uint16_t id) {
  return ((msg[1] << 8) | msg[2]) == id;
}

// For drum hits
uint8_t get_drum_id(byte* msg) {
  return msg[DRUM_ID_BYTE];
}

float get_hit_intensity(byte* msg) {
  return msg[DRUM_HIT_INTENSITY1] << 8 | msg[DRUM_HIT_INTENSITY2];
}

uint16_t get_hit_counter(byte* msg) {
  return msg[DRUM_COUNTER_BYTE] << 8 | msg[DRUM_COUNTER_BYTE + 1];  //msg[DRUM_COUNTER_BYTE] << 24 | msg[DRUM_COUNTER_BYTE + 1] << 16 | 
}

