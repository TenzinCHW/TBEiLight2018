#include "test.h"

State copy_state(State st) {
  State t;
  Serial.println(F("Got new state"));
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
  for (int i = 0; i < NUM_LED; i++) t.lights[i] = st.lights[i];
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
  bool lights = true;
  for (int i = 0; i < NUM_LED; i++) {
    if (orig_state.lights[i] != test_state.lights[i]) {
      lights = false;
      break;
    }
  }
  bool ID = orig_state.ID == test_state.ID;
  bool msg_buf = orig_state.msg_buf == test_state.msg_buf;
  return indiv_var_set && globals_set && is_relay && last_hello && last_hello && expiry_time && time_since_last_glob_req && x && y && drums && hits && wavelength
  && period && lights && ID && msg_buf;
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
