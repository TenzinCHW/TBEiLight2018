#ifndef MESSAGING
#define MESSAGING

#include <Arduino.h>

/*
 * Messages are to be parsed with big endianness
 * Locations in messages are 2-byte long, representing distance in tens of cm (dm)
 * Time is represented by one byte as hundreds of milliseconds
 * Colours take up 3 bytes, representing values for R, G and B respectively
 * Intensity value for drum hit messages are represented by 2 bytes, which will be scaled down by 100
 */

// HEADER BYTE - MSG TYPE //
#define SETUP_REQ_MSG 0
#define SETUP_MSG 1
#define SETUP_ACK 2
#define DRUM_HIT_MSG 3
#define HELLO_MSG 4

// HEADER BYTE - OTHER INFO //
#define GLOBAL_BIT 3
#define SET_AS_RELAY 4
#define RELAY_BIT 7

// INDIV SETUP //
#define LAMP_ID 1
#define LAMP_X1 3
#define LAMP_Y1 5

// GLOBAL SETUP //
#define DRUM_X1_START 1
#define DRUM_X2_START 2
#define DRUM_Y1_START 3
#define DRUM_Y2_START 4
#define COLOUR_START_BYTE 17
#define WAVELENGTH_BYTE 29
#define PERIOD_BYTE 30
#define EXPIRY_BYTE 31

// DRUM HIT //
#define DRUM_ID_BYTE 1
#define DRUM_HIT_INTENSITY 2
#define DRUM_COUNTER_BYTE 4

uint8_t get_drum_id(byte* msg);
float get_hit_intensity(byte* msg);
uint16_t get_hit_counter(byte* msg);
#endif
