#include "messaging.h"

#define SETUP_REQ_MSG 0
#define SETUP_MSG 1
#define SETUP_ACK 2
#define DRUM_HIT_MSG 3

#define RELAY_BIT 7
#define HELLO_BIT 6
#define SET_AS_RELAY 3
#define GLOBAL_BIT 2

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
  return msg[0] & (1 << 2);
}

bool to_set_as_relay(byte* msg) {
  return msg[0] & (1 << 3);
}

bool is_hello(byte* msg) {
  return msg[0] & (1 << 6);
}

bool to_be_relayed(byte* msg) {
  return msg[0] & (1 << 7);
}

int get_drum_x(byte* msg, size_t drum_index) {
  return msg[4 * drum_index + 1] << 8 | msg[4 * drum_index + 2];
}

int get_drum_y(byte* msg, size_t drum_index) {
  return msg[4 * drum_index + 3] << 8 | msg[4 * drum_index + 4];
}

size_t get_drum_colour(byte* msg, size_t drum_index, size_t colour) {  // colour maps as 0 (R), 1 (G) and 2 (B)
  return msg[3 * drum_index + colour + 17];  // drum colours start on 18th byte, may want to put this as #define at the top
}

size_t get_wavelength(byte* msg) {
  return msg[29];
}

size_t get_period(byte* msg) {
  return msg[30];
}

size_t get_expiry(byte* msg) {
  return msg[31];
}

bool addressed_to_id(byte* msg, byte* id) {
  for (int i = 0; i < 3; i++) {
    if (msg[i] != id[i]) {
      return false;
    }
  }
  return true;
}

size_t get_drum_id(byte* msg) {
  return msg[1];
}

int get_hit_intensity(byte* msg) {
  return msg[2] << 8 | msg[3];
}

