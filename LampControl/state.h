#include <Arduino.h>
#include <TimerOne.h>
#include "LowPower.h"
#include "messaging.h"

#define MAX_HITS 80
#define NUM_OF_DRUMS 4
#define RETRY_TIMES 2

struct DrumHit {
  uint8_t drum_id;
  uint32_t incoming_time;
  uint16_t counter;
  float intensity; // should be between 0 and 1
};

struct Drum {
  float x;
  float y;
  uint8_t colour[3];
};

struct HitQueue {
  DrumHit hits[MAX_HITS];
  uint16_t head;
  uint16_t counter;
};

void main_loop();
void reset_vars();
void power_down();
void read_and_handle();
void indiv_setup();
void global_setup();
void push_hit(HitQueue* queue, DrumHit hit);
void pop_hit(HitQueue* queue);
void add_drum_hit(HitQueue* queue, uint8_t drum_id, float intensity, uint16_t counter);
void remove_old_hits();
