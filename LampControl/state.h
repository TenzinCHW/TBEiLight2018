#ifndef STATE
#define STATE

#include <Arduino.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include "LowPower.h"
#include "messaging.h"
#include "comms.h"
#include "led.h"

#define MAX_HITS 80
#define NUM_OF_DRUMS 4
#define RETRY_TIMES 1
#define WAIT_FOR_REPLY 100

struct DrumHit {
  uint8_t drum_id;
  uint32_t incoming_time;
  uint16_t counter;
  float intensity; // should be between 0 and 1
};

struct Drum {
  float x;
  float y;
  float dist_from_lamp;
  uint8_t colour[3];
};

void remove_old_hits();

struct HitQueue {
  DrumHit hits[MAX_HITS];
  uint16_t head;
  uint16_t counter;

  DrumHit pop_hit() {
    if (this->counter != 0) {
      uint16_t ret = this->head;
      this->head = (ret + 1) % MAX_HITS;
      this->counter--;
      return this->hits[ret];
    }
  }

  void push_hit(DrumHit hit) {
    while (this->counter == MAX_HITS) { // this may cause timing problems if queue is full and hits do not expire for a while
      remove_old_hits();
    }
    this->hits[(this->head + this->counter) % MAX_HITS] = hit;
    this->counter++;
  }
};

struct State {
  bool indiv_var_set = false;
  bool globals_set = false;
  bool is_relay = false;
  uint32_t last_hello = 0;
  uint16_t expiry_time;
  uint32_t time_since_last_glob_req = 0;
  float x;
  float y;
  Drum drums[NUM_OF_DRUMS];
  HitQueue hits;
  float wavelength;
  uint16_t period;
  uint16_t ID;
  byte msg_buf[PACKET_SZ];
};

void init_ID();
void req_indiv_setup();
void req_global_setup();
void main_loop(); // TODO test
void reset_vars();  // TODO test
void power_down();  // TODO test
void read_and_handle(); // TODO test
void read_drum_hit(); // TODO test
void forward(uint8_t msg_type); // TODO test
void indiv_setup();
void global_setup();
void add_drum_hit(HitQueue* queue, uint8_t drum_id, float intensity, uint16_t counter);

#endif
