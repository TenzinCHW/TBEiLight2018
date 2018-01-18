#include <Arduino.h>

// ==== PACKING MSG ==== //
void set_relay_bit(byte* msg);
byte* make_ack(byte* id);
byte* make_indiv_req(byte* id);
byte make_glob_req();
void copy_id(byte* buf, byte* id);

// ==== PARSING MSG ==== //
size_t msg_type(byte* msg);
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
