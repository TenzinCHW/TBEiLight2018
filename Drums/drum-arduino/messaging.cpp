#include <Arduino.h>
#include "messaging.h"

/*
 * The byte* msg input into each function is the pointer to the 32-bit transmission buffer.
 * To use, call set_setup with SETUP_MSG as msg_type, then if it's global, call set_global and 
 * the other global setup functions.
 * If it is individual, call set_individual_setup with the lamp id and x and y-coords.
 * Once global or individual setup message prep is complete, send the msg and finally call
 * clear_header to remove header info from msg to get ready for next transmission.
 */

// BIT PACKING FUNCTIONS //
void clear_header(byte* msg) {
    msg[0] = 0;
}

void set_setup(byte* msg, uint8_t msg_type) {
    msg[0] = msg_type;
}

void set_global(byte* msg) {
    msg[0] |= 1 << GLOBAL_BIT;
}

void set_set_as_relay(byte* msg) {
    msg[0] |= 1 << SET_AS_RELAY;
}

void set_individual_setup(byte* msg, uint16_t id, uint16_t X, uint16_t Y) {
    set_address(msg, id);
    msg[LAMP_X1] = X >> 8;
    msg[LAMP_X1 + 1] = X;
    msg[LAMP_Y1] = Y >> 8;
    msg[LAMP_Y1 + 1] = Y;
}

void set_drum_loc(byte* msg, uint8_t drum_index, uint16_t X, uint16_t Y) {
    msg[4 * drum_index + DRUM_X1_START] = X >> 8;
    msg[4 * drum_index + DRUM_X2_START] = X;
    msg[4 * drum_index + DRUM_Y1_START] = Y >> 8;
    msg[4 * drum_index + DRUM_Y2_START] = Y;
}

void set_drum_colour(byte* msg, uint8_t drum_index, uint8_t R, uint8_t G, uint16_t B) {
    msg[3 * drum_index + COLOUR_START_BYTE] = R;
    msg[3 * drum_index + 1 + COLOUR_START_BYTE] = G;
    msg[3 * drum_index + 2 + COLOUR_START_BYTE] = B;
}

void set_wavelength(byte* msg, uint8_t wavelength) {
    msg[WAVELENGTH_BYTE] = wavelength;
}

void set_period(byte* msg, uint8_t period) {
    msg[PERIOD_BYTE] = period;
}

void set_expiry(byte* msg, uint8_t expiry) {
    msg[EXPIRY_BYTE] = expiry;
}

void set_address(byte* msg, uint16_t id) {
    msg[LAMP_ID] = id >> 8;
    msg[LAMP_ID + 1] = id;
}

void set_drum_id(byte* msg, uint8_t id) {
    msg[DRUM_ID_BYTE] = id;
}

void set_drum_hit_intensity(byte* msg, uint16_t intensity) {
    msg[DRUM_HIT_INTENSITY] = intensity >> 8;
    msg[DRUM_HIT_INTENSITY + 1] = intensity;
}

void set_drum_hit_counter(byte* msg, uint16_t count) {
    msg[DRUM_COUNTER_BYTE] = count >> 8;
    msg[DRUM_COUNTER_BYTE + 1] = count;
}
