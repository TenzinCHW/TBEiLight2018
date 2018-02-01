#include "test.h"

// Apply functions and check state //
extern State state;

void all_tests() {
  Serial.println(F("Begin tests"));
  assert_true(test_indiv_setup());
  assert_true(test_global_setup());
  assert_true(test_add_rm_old_drum_hits());
}

bool test_indiv_setup() {
  Serial.println(F("indiv_setup()"));
  state.msg_buf[0] = 0b0010001; // == 9 set as relay, setup msg
  uint16_t x = 400;
  uint16_t y = 230;
  state.msg_buf[LAMP_X1] = x >> 8;
  state.msg_buf[LAMP_X1 + 1] = x;
  state.msg_buf[LAMP_Y1] = y >> 8;
  state.msg_buf[LAMP_Y1 + 1] = y;

  indiv_setup();
  if (state.x == x / 10.0 && state.y == y / 10.0 && state.is_relay) return true;
  return false;
}

bool test_global_setup() {
  Serial.println(F("global_setup()"));
  state.msg_buf[0] = 0b00001001;  // == 7 global message, setup msg
  uint16_t dx1, dx2, dx3, dx4, dy1, dy2, dy3, dy4;
  dx1 = 249; dx2 = 384; dx3 = 234; dx4 = 947; dy1 = 135; dy2 = 30; dy3 = 647; dy4 = 5;
  state.msg_buf[DRUM_X1_START] = dx1 >> 8; state.msg_buf[DRUM_X2_START] = dx1; state.msg_buf[DRUM_Y1_START] = dy1 >> 8; state.msg_buf[DRUM_Y2_START] = dy1;
  state.msg_buf[DRUM_X1_START + 4] = dx2 >> 8; state.msg_buf[DRUM_X2_START + 4] = dx2; state.msg_buf[DRUM_Y1_START + 4] = dy2 >> 8; state.msg_buf[DRUM_Y2_START + 4] = dy2;
  state.msg_buf[DRUM_X1_START + 8] = dx3 >> 8; state.msg_buf[DRUM_X2_START + 8] = dx3; state.msg_buf[DRUM_Y1_START + 8] = dy3 >> 8; state.msg_buf[DRUM_Y2_START + 8] = dy3;
  state.msg_buf[DRUM_X1_START + 12] = dx4 >> 8; state.msg_buf[DRUM_X2_START + 12] = dx4; state.msg_buf[DRUM_Y1_START + 12] = dy4 >> 8; state.msg_buf[DRUM_Y2_START + 12] = dy4;
  uint8_t rgb1[] = {153, 45, 235}; uint8_t rgb2[] = {23, 95, 169}; uint8_t rgb3[] = {42, 128, 185}; uint8_t rgb4[] = {73, 104, 217};
  for (int i = 0; i < 3; i++) state.msg_buf[COLOUR_START_BYTE + i] = rgb1[i];
  for (int i = 0; i < 3; i++) state.msg_buf[COLOUR_START_BYTE + 3 + i] = rgb2[i];
  for (int i = 0; i < 3; i++) state.msg_buf[COLOUR_START_BYTE + 6 + i] = rgb3[i];
  for (int i = 0; i < 3; i++) state.msg_buf[COLOUR_START_BYTE + 9 + i] = rgb4[i];
  uint8_t wl = 184;
  state.msg_buf[WAVELENGTH_BYTE] = wl;
  uint8_t period = 201;
  state.msg_buf[PERIOD_BYTE] = period;
  uint8_t expiry = 140;
  state.msg_buf[EXPIRY_BYTE] = expiry;

  global_setup();
  bool passed;
  passed = (state.drums[0].x == dx1 / 10.0) && (state.drums[0].y == dy1 / 10.0) && (state.drums[0].colour[0] == rgb1[0])
           && (state.drums[0].colour[1] == rgb1[1]) && (state.drums[0].colour[2] == rgb1[2]);
  passed = passed && (state.drums[1].x == dx2 / 10.0) && (state.drums[1].y == dy2 / 10.0) && (state.drums[1].colour[0] == rgb2[0])
           && (state.drums[1].colour[1] == rgb2[1]) && (state.drums[1].colour[2] == rgb2[2]);
  passed = passed && (state.drums[2].x == dx3 / 10.0) && (state.drums[2].y == dy3 / 10.0) && (state.drums[2].colour[0] == rgb3[0])
           && (state.drums[2].colour[1] == rgb3[1]) && (state.drums[2].colour[2] == rgb3[2]);
  passed = passed && (state.drums[3].x == dx4 / 10.0) && (state.drums[3].y == dy4 / 10.0) && (state.drums[3].colour[0] == rgb4[0])
           && (state.drums[3].colour[1] == rgb4[1]) && (state.drums[3].colour[2] == rgb4[2]);
  if (!passed) {
    Serial.println(F("drums not initialized properly"));
    return false;
  }

  passed = passed && (state.wavelength == wl / 10.0);
  if (!passed) {
    Serial.println(F("wavelength not initialized properly"));
    return false;
  }

  passed = passed && (state.period == period * 100);
  if (!passed) {
    Serial.println(F("period not initialized properly"));
    return false;
  }

  passed = passed && (state.expiry_time == expiry * 100);
  if (!passed) {
    Serial.println(F("expiry_time not initialized properly"));
    return false;
  }

  return true;
}

bool test_add_rm_old_drum_hits() {
  Serial.println(F("add_drum_hit(HitQueue* queue, uint8_t drum_id, float intensity, uint16_t counter) and rm_old_hits()"));
  for (int i = 0; i < MAX_HITS / 2; i++) {
    DrumHit hit, hit2;
    uint8_t dID;
    if (i % 2 == 0) dID = 0;
    else if (i % 3 == 0) dID = 1;
    else if (i % 5 == 0) dID = 2;
    else dID = 3;
    hit.drum_id = dID;
    hit.incoming_time = millis(); // Won't be the same as the one being added later
    hit.counter = i;
    hit.intensity = 6.3;

    add_drum_hit(&state.hits, dID, 6.3, i);

    if (state.hits.counter != i + 1) {
      Serial.println(F("Added wrong drum hit"));
      return false;
    }
    DrumHit test = state.hits.hits[state.hits.head + state.hits.counter - 1];
    if (test.drum_id != hit.drum_id || test.intensity != hit.intensity || test.counter != hit.counter) {
      Serial.println(F("Drum hit added is not the same as the one that's supposed to be added"));
      return false;
    }
  }
  add_drum_hit(&state.hits, 0, 2.5, 0);
  if (state.hits.counter != MAX_HITS / 2) {
    Serial.println(F("Added a hit that's already in buffer"));
    return false;
  }

  for (int i = MAX_HITS / 2; i < MAX_HITS; i++) {
    uint8_t dID;
    if (i % 2 == 0) dID = 0;
    else if (i % 3 == 0) dID = 1;
    else if (i % 5 == 0) dID = 2;
    else dID = 3;
    add_drum_hit(&state.hits, dID, 4.2, i);
  }

  add_drum_hit(&state.hits, 0, 3.5, 0);
  if (state.hits.counter > MAX_HITS) {
    Serial.println(F("Added too many hits"));
    Serial.print(F("Number of hits: ")); Serial.println(state.hits.counter);
    return false;
  }
  state.hits.pop_hit();
  add_drum_hit(&state.hits, 0, 3.5, 0);
  if (state.hits.head != 1 || state.hits.counter != MAX_HITS) {
    Serial.println(F("Wraparound for queue is not working"));
    return false;
  }

  state.expiry_time = 1000;
  while (state.hits.counter > 0) {
    remove_old_hits();
  }
  if (state.hits.counter > 0) {
    Serial.println(F("Not removing old hits"));
    return false;
  }
  return true;
}

void assert_true(bool test) {
  Serial.print(F(" "));
  if (test) Serial.println(F("PASS"));
  else Serial.println(F("FAIL"));
}


// Make a copy and compare functions - WARNING not enough memory to do this //

State copy_state(State st) {
  State t;
  t.indiv_var_set = st.indiv_var_set;
  t.globals_set = st.globals_set;
  t.is_relay = st.is_relay;
  t.last_hello = st.last_hello;
  t.expiry_time = st.expiry_time;
  t.time_since_last_glob_req = st.time_since_last_glob_req;
  t.x = st.x;
  t.y = st.y;
  for (int i = 0; i < NUM_OF_DRUMS; i++) t.drums[i] = st.drums[i];
  t.hits = copy_hit_queue(st.hits);
  t.wavelength = st.wavelength;
  t.period = st.period;
//  for (int i = 0; i < NUM_LED; i++) t.lights[i] = st.lights[i];
  t.ID = st.ID;
  for (int i = 0; i < PACKET_SZ; i++) t.msg_buf[i] = st.msg_buf[i];
  return t;
}

DrumHit copy_drum_hits(DrumHit hit) {
  DrumHit hitcpy;
  hitcpy.drum_id = hit.drum_id;
  hitcpy.incoming_time = hit.incoming_time;
  hitcpy.counter = hit.counter;
  hitcpy.intensity = hit.intensity;
  return hitcpy;
}

Drum copy_drum(Drum drum) {
  Drum drumcpy;
  drumcpy.x = drum.x;
  drumcpy.y = drum.y;
  for (int i = 0; i < 3; i++) drumcpy.colour[i] = drum.colour[i];
  return drumcpy;
}

HitQueue copy_hit_queue(HitQueue queue) {
  HitQueue queuecpy;
  for (int i = 0; i < MAX_HITS; i++) queuecpy.hits[i] = queue.hits[i];
  queuecpy.head = queue.head;
  queuecpy.counter = queue.counter;
  return queuecpy;
}

bool compare_states(State orig_state, State test_state) {
  bool indiv_var_set = orig_state.indiv_var_set == test_state.indiv_var_set;
  bool globals_set = orig_state.globals_set == test_state.globals_set;
  bool is_relay = orig_state.is_relay == test_state.is_relay;
  bool last_hello = orig_state.last_hello == test_state.last_hello;
  bool expiry_time = orig_state.expiry_time == test_state.expiry_time;
  bool time_since_last_glob_req = orig_state.time_since_last_glob_req == test_state.time_since_last_glob_req;
  bool x = orig_state.x == test_state.x;
  bool y = orig_state.y == test_state.y;
  bool drums = true;
  for (int i = 0; i < NUM_OF_DRUMS; i++) {
    if (!compare_drums(orig_state.drums[i], test_state.drums[i])) {
      drums = false;
      break;
    }
  }
  bool hits = compare_hit_queues(orig_state.hits, test_state.hits);
  bool wavelength = orig_state.wavelength == test_state.wavelength;
  bool period = orig_state.period == test_state.period;
  bool lighted = true;
//  for (int i = 0; i < NUM_LED; i++) {
//    if (orig_state.lights[i] != test_state.lights[i]) {
//      lighted = false;
//      break;
//    }
//  }
  bool ID = orig_state.ID == test_state.ID;
  bool msg_buf = orig_state.msg_buf == test_state.msg_buf;
  return indiv_var_set && globals_set && is_relay && last_hello && last_hello && expiry_time && time_since_last_glob_req && x && y && drums && hits && wavelength
         && period && lighted && ID && msg_buf;
}

bool compare_drum_hits(DrumHit orig_hit, DrumHit test_hit) {
  bool drum_id = orig_hit.drum_id == test_hit.drum_id;
  bool incoming_time = orig_hit.incoming_time == test_hit.incoming_time;
  bool counter = orig_hit.counter == test_hit.counter;
  bool intensity = orig_hit.intensity == test_hit.intensity;
  return drum_id && incoming_time && counter && intensity;
}

bool compare_drums(Drum orig_drum, Drum test_drum) {
  bool x = orig_drum.x == test_drum.x;
  bool y = orig_drum.y == test_drum.y;
  bool colour = true;
  for (int i = 0; i < 3; i++) {
    if (orig_drum.colour[i] != test_drum.colour[i]) {
      colour = false;
      break;
    }
  }
  return x && y && colour;
}

bool compare_hit_queues(HitQueue orig_hits, HitQueue test_hits) {
  bool hits = true;
  for (int i = 0; i < MAX_HITS; i++) {
    if (!compare_drum_hits(orig_hits.hits[i], test_hits.hits[i])) {
      hits = false;
      break;
    }
  }
  bool head = orig_hits.head == test_hits.head;
  bool counter = orig_hits.counter == test_hits.counter;
  return hits && head && counter;
}
