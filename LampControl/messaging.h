#include <Arduino.h>

#define SETUP_REQ_MSG 0
#define SETUP_MSG 1
#define SETUP_ACK 2
#define DRUM_HIT_MSG 3

#define GLOBAL_BIT 2
#define SET_AS_RELAY 3
#define HELLO_BIT 6
#define RELAY_BIT 7

#define DRUM_X1_START 1
#define DRUM_X2_START 2
#define DRUM_Y1_START 3
#define DRUM_Y2_START 4
#define COLOUR_START_BYTE 17
#define WAVELENGTH_BYTE 29
#define PERIOD_BYTE 30
#define EXPIRY_BYTE 31

// ==== PACKING MSG ==== //
void set_relay_bit(byte* msg);
byte* make_ack(byte* id);
byte* make_indiv_req(byte* id);
byte make_glob_req();
void copy_id(byte* buf, byte* id);

// ==== PARSING MSG ==== //
size_t get_msg_type(byte* msg);
bool is_global(byte* msg);
bool to_set_as_relay(byte* msg);
bool is_hello(byte* msg);
bool to_be_relayed(byte* msg);
int get_drum_x(byte* msg, size_t drum_index);
int get_drum_y(byte* msg, size_t drum_index);
size_t get_drum_colour(byte* msg, size_t drum_index, size_t colour);
size_t get_wavelength(byte* msg);
size_t get_period(byte* msg);
size_t get_expiry(byte* msg);
bool addressed_to_id(byte* msg, byte* id);
size_t get_drum_id(byte* msg);
int get_hit_intensity(byte* msg);
