#include "state.h"
#include "messaging.h"
#include "comms.h"

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
byte ID[3];

void main_loop() {
  if (!(indiv_var_set && globals_set)) {
    if (!indiv_var_set) {
      uint8_t num_try = 0;
      byte* req = make_indiv_req(ID);
      while (!indiv_var_set && (num_try < RETRY_TIMES)) {
        // TODO send req
        delay(100); // TODO not a good idea to use delays
        read_and_handle();
      }
      if (!indiv_var_set) power_down(); // Still haven't set up individual configurations after checking, go back to sleep
    } else if (!globals_set && millis() - time_since_last_glob_req > 2000) {
      byte* req = make_glob_req();  // TODO send this
      read_and_handle();
      time_since_last_glob_req = millis();
    }
  }
  else {
    // TODO try to listen for reset message from master - should be encrypted (low priority)
    // TODO try to receive drum hit/hello, add to drum hit when received, set last_hello to millis()
    // TODO update LEDs
    if (millis() - last_hello > 30000) {
      reset_vars();
      power_down();
    }
  }
}

void reset_vars() { // resets all variables that need to be reset
  indiv_var_set = false;
  globals_set = false;
  is_relay = false;
  last_hello = 0;
  expiry_time = 0;
  time_since_last_glob_req = 0;
  x = 0;
  y = 0;
  for (uint8_t i = 0; i < NUM_OF_DRUMS; i++) {
    Drum drum;
    drums[i] = drum;
  }
  for (uint8_t i = 0; i < 32; i++) msg_buf[i] = NULL;
  wavelength = 0;
  period = 0;
  while (hits.counter > 0) pop_hit(&hits);
  Timer1.detachInterrupt();
  // Do not reset ID and do not stop listening on radio
  // TODO reset other variables
}

void power_down() { // powers down for a few seconds
  // TODO power down radio
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
                SPI_OFF, USART0_OFF, TWI_OFF);  // TODO choose between this or the one below to save power
  //  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  // TODO power up radio
}

void read_and_handle() {
  if (radio.available()) {
    // TODO read message into variable msg_buf here
    if (is_relay && to_be_relayed(msg_buf)) // TODO forward msg_buf
      if (is_hello(msg_buf)) last_hello = millis();
    switch (get_msg_type(msg_buf)) {
      case SETUP_MSG :
        if (is_global(msg_buf)) global_setup();
        else if (addressed_to_id(msg_buf, ID)) indiv_setup();
        break;
      case DRUM_HIT_MSG :
        last_hello = millis();
        add_drum_hit(&hits, get_drum_id(msg_buf), get_hit_intensity(msg_buf), get_hit_counter(msg_buf));
        break;
    }
  }
}

void indiv_setup() {
  x = (msg_buf[LAMP_X1] << 8 | msg_buf[LAMP_X1 + 1]) / 10.0;
  y = (msg_buf[LAMP_Y1] << 8 | msg_buf[LAMP_Y1 + 1]) / 10.0;
  is_relay = to_set_as_relay(msg_buf);
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(remove_old_hits);
  indiv_var_set = true;
  byte* ack = make_ack(ID); // TODO send this
}

void global_setup() {
  for (uint8_t i = 0; i < NUM_OF_DRUMS; i++) {
    Drum newdrum;
    newdrum.x = get_drum_x(msg_buf, i);
    newdrum.y = get_drum_y(msg_buf, i);
    for (uint8_t j = 0; j < 3; j++) {
      newdrum.colour[j] = get_drum_colour(msg_buf, i, j);
    }
    drums[i] = newdrum;
  }

  wavelength = get_wavelength(msg_buf);
  period = get_period(msg_buf);
  expiry_time = get_expiry(msg_buf);
  globals_set = true;
}

void push_hit(HitQueue* queue, DrumHit hit) {
  while (queue->counter == MAX_HITS) { // TODO this may cause timing problems if queue is full and hits do not expire for a while
    remove_old_hits();
  }
  queue->hits[(queue->head + queue->counter) % MAX_HITS] = hit;
  queue->counter++;
}

void pop_hit(HitQueue* queue) {
  uint16_t ret = queue->head;
  queue->head = (ret + 1) % MAX_HITS;
  queue->counter--;
  return queue->hits[ret];
}

void add_drum_hit(HitQueue* queue, uint8_t drum_id, float intensity, uint16_t counter) {
  DrumHit hit;
  hit.drum_id = drum_id;
  hit.intensity = intensity;
  hit.incoming_time = millis();
  hit.counter = get_hit_counter(counter);
  push_hit(queue, hit);
}

void remove_old_hits() {
  while (hits.counter > 0) {
    if (millis() - hits.hits[hits.head].incoming_time > expiry_time) pop_hit(&hits);
    else break;
  }
}
