#include "messaging.h"

// ==== PACKING MSG ==== //

/*
 * input msg: pointer to state message buffer
 * sets the relay bit of message in state message buffer
 */
void set_relay_bit(byte* msg) {
  msg[0] |= (1 << RELAY_BIT);
}

/*
 * input id: lamp ID from state
 * input msg: pointer to state message buffer
 * sets message content to acknowledge that this lamp has set up local configuration
 */
void make_ack(byte* msg, uint16_t id) {
  msg[0] = SETUP_ACK;
  msg[1] = id >> 8;
  msg[2] = id;  // id fills in from the back of 2 bits
}

/*
 * input id: lamp ID from state
 * input msg: pointer to state message buffer
 * sets message content to request for this specific lamp's settings
 */
void make_indiv_req(byte* msg, uint16_t id) {
  msg[0] = SETUP_REQ_MSG;
  msg[1] = id >> 8;
  msg[2] = id;
}

/*
 * input msg: pointer to state message buffer
 * sets global bit of first byte of msg, which is the message for a global configuration request
 */
void make_glob_req(byte* msg) {
  msg[0] = (1 << GLOBAL_BIT);
}

// ==== PARSING MSG ==== //

/*
 * input msg: pointer to state message buffer
 * return: type of message in state message buffer
 * SETUP_REQ_MSG 0
 * SETUP_MSG 1
 * SETUP_ACK 2
 * DRUM_HIT_MSG 3
 * HELLO_MSG 4
 */
uint8_t get_msg_type(byte* msg) {
  return msg[0] & 0b111;
}

/*
 * input msg: pointer to state message buffer
 * return: true if message is a global request/setup configuration info message
 */
bool is_global(byte* msg) {
  return (msg[0] >> GLOBAL_BIT) & 1;
}

/*
 * input msg: pointer to state message buffer
 * return: true if lamp is to be set as a relay (rebroadcasts messages)
 */
bool to_set_as_relay(byte* msg) {
  return (msg[0] >> SET_AS_RELAY) & 1;
}

/*
 * input msg: pointer to state message buffer
 * return: true if message in state message buffer is to be relayed (rebroadcasted) with relayed flag set
 */
bool to_be_relayed(byte* msg) {
  return ~((msg[0] >> RELAY_BIT) & 1);
}

/*
 * input msg: pointer to state message buffer
 * input drum_index: ID of drum (0-3)
 * return: x-coordinate of specified drum in metres
 */
float get_drum_x(byte* msg, uint8_t drum_index) {
  return (msg[4 * drum_index + DRUM_X1_START] << 8 | msg[4 * drum_index + DRUM_X2_START]) / 10.0;
}

/*
 * input msg: pointer to state message buffer
 * input drum_index: ID of drum (0-3)
 * return: y-coordinate of specified drum in metres
 */
float get_drum_y(byte* msg, uint8_t drum_index) {
  return (msg[4 * drum_index + DRUM_Y1_START] << 8 | msg[4 * drum_index + DRUM_Y2_START]) / 10.0;
}

/*
 * input msg: pointer to state message buffer
 * input drum_index: ID of drum (0-3)
 * input colour: red (0), green (1), blue (2)
 * return: respective base value of colour specified for given drum
 */
uint8_t get_drum_colour(byte* msg, uint8_t drum_index, uint8_t colour) {  // colour maps as 0 (R), 1 (G) and 2 (B)
  return msg[3 * drum_index + colour + COLOUR_START_BYTE];
}

/*
 * input msg: pointer to state message buffer
 * return: wavelength of drum hits in metres
 */
float get_wavelength(byte* msg) {
  return msg[WAVELENGTH_BYTE] / 10.0;
}

/*
 * input msg: pointer to state message buffer
 * return: period of drum hits in milliseconds
 */
uint16_t get_period(byte* msg) {
  return msg[PERIOD_BYTE] * 100;
}

/*
 * input msg: pointer to state message buffer
 * return: how long to wait number of milliseconds before removing a drum hit
 */
uint16_t get_expiry(byte* msg) {
  return msg[EXPIRY_BYTE] * 100;
}

/*
 * input msg: pointer to state message buffer
 * input id: lamp ID of this lamp, or ID in state
 * return: true if the message is addressing the ID
 */
bool addressed_to_id(byte* msg, uint16_t id) {
  return ((msg[LAMP_ID] << 8) | msg[LAMP_ID+1]) == id;
}

/*
 * input msg: pointer to state message buffer
 * return: x-coordinate of lamp in metres
 */
float get_lamp_x(char* msg) {
    return (msg[LAMP_X1] << 8 | msg[LAMP_X1+1]) / 10.0;
}

/*
 * input msg: pointer to state message buffer
 * return: y-coordinate of lamp in metres
 */
float get_lamp_y(char* msg) {
    return (msg[LAMP_Y1] << 8 | msg[LAMP_Y1+1]) / 10.0;
}

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
  return (msg[DRUM_HIT_INTENSITY] << 8 | msg[DRUM_HIT_INTENSITY+1]) / 100.0;
}

/*
 * input msg: pointer to state message buffer
 * return: drum hit counter of this drum
 */
uint16_t get_hit_counter(byte* msg) {
  return msg[DRUM_COUNTER_BYTE] << 8 | msg[DRUM_COUNTER_BYTE + 1];  //msg[DRUM_COUNTER_BYTE] << 24 | msg[DRUM_COUNTER_BYTE + 1] << 16 | 
}

