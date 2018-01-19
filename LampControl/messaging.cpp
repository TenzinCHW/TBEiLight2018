#include "messaging.h"

// ==== PACKING MSG ==== //

void set_relay_bit(byte* msg) {
  msg[0] |= (1 << 7);
}

byte* make_ack(byte* id) {
  byte ack[4];
  ack[0] = 0b10;
  copy_id(&ack[1], id);
  return ack;
}

byte* make_indiv_req(byte* id) {
  byte req[4];
  req[0] = 1;
  copy_id(&req[1], id);
  return req;
}

byte make_glob_req() {
  return 1 << GLOBAL_BIT;
}

void copy_id(byte* buf, byte* id) {
  for (int i = 0; i < 3; i++) {
    buf[i] = id[i];
  }
}

// ==== PARSING MSG ==== //

size_t get_msg_type(byte* msg) {
  return msg[0] & 0b11;
}

bool is_global(byte* msg) {
  return msg[0] & (1 << GLOBAL_BIT);
}

bool to_set_as_relay(byte* msg) {
  return msg[0] & (1 << SET_AS_RELAY);
}

bool is_hello(byte* msg) {
  return msg[0] & (1 << HELLO_BIT);
}

bool to_be_relayed(byte* msg) {
  return msg[0] & (1 << RELAY_BIT);
}

float get_drum_x(byte* msg, size_t drum_index) {
  return (msg[4 * drum_index + DRUM_X1_START] << 8 | msg[4 * drum_index + DRUM_X2_START]) / 10.0;
}

float get_drum_y(byte* msg, size_t drum_index) {
  return (msg[4 * drum_index + DRUM_Y1_START] << 8 | msg[4 * drum_index + DRUM_Y2_START]) / 10.0;
}

size_t get_drum_colour(byte* msg, size_t drum_index, size_t colour) {  // colour maps as 0 (R), 1 (G) and 2 (B)
  return msg[3 * drum_index + colour + COLOUR_START_BYTE];
}

float get_wavelength(byte* msg) {
  return msg[WAVELENGTH_BYTE] / 10.0;
}

int get_period(byte* msg) {
  return msg[PERIOD_BYTE] * 100;
}

int get_expiry(byte* msg) {
  return msg[EXPIRY_BYTE] * 100;
}

bool addressed_to_id(byte* msg, byte* id) {
  for (int i = 0; i < 3; i++) {
    if (msg[i] != id[i]) {
      return false;
    }
  }
  return true;
}

// For drum hits
size_t get_drum_id(byte* msg) {
  return msg[1];
}

float get_hit_intensity(byte* msg) {
  return msg[2] << 8 | msg[3];
}

