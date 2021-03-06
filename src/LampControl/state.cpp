#include "state.h"

State state;

void init_ID() {
  state.ID = EEPROM.read(0) << 8 | EEPROM.read(1);
  Serial.print(F("ID: ")); Serial.println(state.ID, DEC);
}

void req_indiv_setup() {
  if (!state.indiv_var_set) {
    for (uint8_t num_try = 0; num_try < RETRY_TIMES; num_try++) {
      make_indiv_req(state.msg_buf, state.ID);
      Serial.println(F("r"));
      broadcast(1, state.msg_buf, INDIV_REQ_SZ);
      long wait = millis();
      while (millis() - wait < WAIT_FOR_REPLY) read_and_handle();
      if (state.indiv_var_set) break;
    }
//    if (!state.indiv_var_set) power_down(); // Still haven't set up individual configurations after checking, go back to sleep
  }
}

void req_global_setup() {
  if (!state.globals_set && millis() - state.time_since_last_glob_req > 2000) {
    Serial.println(F("R"));
    make_glob_req(state.msg_buf);
    for (int num_try = 0; num_try < RETRY_TIMES; num_try++) {
      broadcast(1, state.msg_buf, GLOBAL_REQ_SZ);
      long wait = millis();
      while (millis() - wait < WAIT_FOR_REPLY) read_and_handle();
      if (state.globals_set) break;
    }
    state.time_since_last_glob_req = millis();
  }
}

void main_loop() {
  // TODO encrypted messages (low priority)
  read_drum_hit();  // try to receive drum hit/hello, add to drum hit when received, set last_hello to millis()
  set_rgb();
  if (millis() - state.last_hello > 10000) {
    reset_vars();
    power_down();
  }
}

void reset_vars() { // resets all variables that need to be reset
  Serial.println(F("V"));
  state.indiv_var_set = false;
  state.globals_set = false;
  state.is_relay = false;
  state.last_hello = 0;
  state.expiry_time = 0;
  state.time_since_last_glob_req = 0;
  state.x = 0;
  state.y = 0;
  for (uint8_t i = 0; i < NUM_OF_DRUMS; i++) {
    Drum drum;
    state.drums[i] = drum;
  }
  for (uint8_t i = 0; i < PACKET_SZ; i++) state.msg_buf[i] = NULL;
  state.wavelength = 0;
  state.period = 0;
  while (state.hits.counter > 0) state.hits.pop_hit();
  Timer1.detachInterrupt();
  reset_lights();
  // Do not reset ID and do not stop listening on radio
}

void power_down() {
  Serial.println(F("Sleep"));
  //  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
  //                SPI_OFF, USART0_OFF, TWI_OFF);
  radio_off();
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_ON); // Guaranteed to lower power consumption
  radio_on();
  delay(50);   // wait for radio to fully turn on;
}

void read_and_handle() {
  if (read_if_avail(state.msg_buf)) {
    uint8_t msg_type = get_msg_type(state.msg_buf);
    forward(msg_type);
//    (F("Header: ")); Serial.println(state.msg_buf[0], BIN);
    if (msg_type == SETUP_MSG) {
      if (is_global(state.msg_buf)) {
        global_setup();
        state.last_hello = millis();
      }
      else if (addressed_to_id(state.msg_buf, state.ID)) {
        indiv_setup();
        state.last_hello = millis();
      }
    }
  }
}

void read_drum_hit() {
  if (read_if_avail(state.msg_buf)) {
    uint8_t msg_type = get_msg_type(state.msg_buf);
    forward(msg_type);
    switch (msg_type) {
      case DRUM_HIT_MSG :
        Serial.println(F("H"));
        state.last_hello = millis();
        add_drum_hit(&state.hits, get_drum_id(state.msg_buf), get_hit_intensity(state.msg_buf), get_hit_counter(state.msg_buf));
        break;
      case HELLO_MSG :
        state.last_hello = millis();
        break;
    }
  }
}

void forward(uint8_t msg_type) {
  if (state.is_relay && to_be_relayed(state.msg_buf)) {
    set_relay_bit(state.msg_buf);
    switch (msg_type) {  // check what type of msg - info or hello or drum hit send on ADDR0, req or ack send on ADDR1
      case SETUP_REQ_MSG : broadcast(1, state.msg_buf, INDIV_REQ_SZ); break;  // Send larger of two request packet sizes (indiv)
      case SETUP_ACK : broadcast(1, state.msg_buf, ACK_SZ); break;
      case DRUM_HIT_MSG : broadcast(0, state.msg_buf, DRUM_HIT_SZ); break;
      case SETUP_MSG : broadcast(0, state.msg_buf, PACKET_SZ); break; // Global setup msg is larger
      case HELLO_MSG : broadcast(0, state.msg_buf, HELLO_SZ); break;
      default : broadcast(0, state.msg_buf, PACKET_SZ); break;  // Just send everything if all else fails
    }
  }
}

void indiv_setup() {
  state.x = get_lamp_x(state.msg_buf);
  state.y = get_lamp_y(state.msg_buf);
  state.is_relay = to_set_as_relay(state.msg_buf);
  state.indiv_var_set = true;
  if (state.globals_set) {
    for (uint8_t i = 0; i < NUM_OF_DRUMS; i++) {
      float drumx = state.drums[i].x;
      float drumy = state.drums[i].y;
      state.drums[i].dist_from_lamp = sqrt(pow(drumx - state.x, 2) + pow(drumy - state.y, 2));
    }
  }
  make_ack(state.msg_buf, state.ID);
  for (int i = 0; i < RETRY_TIMES; i++) {
    broadcast(1, state.msg_buf, ACK_SZ);
  }

  Serial.println(F("s"));
  //  Serial.print(F("X: ")); Serial.println(state.x);
  //  Serial.print(F("Y: ")); Serial.println(state.y);
  //  Serial.print(F("Is relay: ")); Serial.println(state.is_relay);
}

void global_setup() {
  for (uint8_t i = 0; i < NUM_OF_DRUMS; i++) {
    Drum newdrum;
    newdrum.x = get_drum_x(state.msg_buf, i);
    newdrum.y = get_drum_y(state.msg_buf, i);
    if (state.indiv_var_set) newdrum.dist_from_lamp = sqrt(pow(newdrum.x - state.x, 2) + pow(newdrum.y - state.y, 2));
    for (uint8_t j = 0; j < 3; j++) {
      newdrum.colour[j] = get_drum_colour(state.msg_buf, i, j);
    }
    state.drums[i] = newdrum;
  }

  state.wavelength = get_wavelength(state.msg_buf);
  state.period = get_period(state.msg_buf);
  state.expiry_time = get_expiry(state.msg_buf);
  state.globals_set = true;
  Timer1.initialize(state.expiry_time * 1000);
  Timer1.attachInterrupt(remove_old_hits);

  Serial.println(F("S"));
  //  for (uint8_t i = 0; i < NUM_OF_DRUMS; i++) {
  //    Serial.print(F("Drum ")); Serial.print(i); Serial.print(F(" Dist from lamp :")); Serial.println(state.drums[i].dist_from_lamp);
  //    for (uint8_t j = 0; j < 3; j++) {
  //      Serial.println(state.drums[i].colour[j]);
  //    }
  //  }
  //  Serial.print(F("Wavelength: ")); Serial.println(state.wavelength);
  //  Serial.print(F("Period: ")); Serial.println(state.period);
  //  Serial.print(F("Expiry: ")); Serial.println(state.expiry_time);
}

void add_drum_hit(HitQueue* queue, uint8_t drum_id, float intensity, uint16_t counter) {
  int j;
  for (int i = state.hits.head; i < state.hits.head + state.hits.counter; i++) {
    j = i % MAX_HITS;
    if (state.hits.hits[j].counter == counter && state.hits.hits[j].drum_id == drum_id) {  // check if hit is already in buffer before pushing
      return;
    }
  }
  DrumHit hit;
  hit.drum_id = drum_id;
  hit.intensity = intensity;
  hit.incoming_time = millis();
  hit.counter = counter;
  state.hits.push_hit(hit);
}

void remove_old_hits() {
  while (state.hits.counter > 0) {
    if (millis() - state.hits.hits[state.hits.head].incoming_time > state.expiry_time) state.hits.pop_hit();
    else return;
  }
}
