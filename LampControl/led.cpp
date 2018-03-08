#include "led.h"

extern State state;
CRGB lights[NUM_LED];

/* temporary vars for use in set_total_intensity() */
float total_intensity[3];
float delta_t;
float dist;
float dist_from_wave;
float wave_travelled;
float src_intensity;
float base_intensity;
uint8_t temp_drum_id;

void setup_lights() {
  FastLED.addLeds<CHIPSET, LED_PIN>(lights, NUM_LED);
  for (uint8_t i = 0; i < NUM_LED; i++) lights[i].setRGB(0, 0, 0);
  FastLED.show();
}

void set_rgb() {
  // Wave equation goes here. Add up values for each drum.
  // use lights[i].setRGB(r_val, g_val, b_val);
  // where r_val, g_val and b_val are the values of red, green and blue respectively.
  total_intensity[0] = 0; total_intensity[1] = 0; total_intensity[2] = 0;
  for (int i = 0; i < state.hits.counter; i++) {
    src_intensity = state.hits.hits[state.hits.head + i].intensity;
    delta_t = millis() - state.hits.hits[state.hits.head + i].incoming_time;
    temp_drum_id = state.hits.hits[state.hits.head + i].drum_id;
    dist = state.drums[temp_drum_id].dist_from_lamp;
    wave_travelled = state.wavelength/state.period * delta_t/1000.0;
//    Serial.println(wave_travelled);
    dist_from_wave = absolute(dist - wave_travelled);
    base_intensity = src_intensity * pow(2, -dist_from_wave/10 * pow(delta_t, 2)) * (wave_travelled - dist_from_wave);
    //    if (dist_from_wave < MAX_WAVE_DIST) base_intensity = src_intensity * pow(100, -dist_from_wave * WAVE_SPREAD);
    //    else base_intensity = 0;
//    Serial.println(base_intensity);
    for (uint8_t j = 0; j < 3; j++) {
      total_intensity[j] += base_intensity * state.drums[temp_drum_id].colour[j];
    }
  }
  Serial.println(total_intensity[0]);

  //  Serial.print(F("Intensities: "));
  for (int i = 0; i < 3; i++) {
    //    Serial.print(total_intensity[i]); Serial.print(F(" "));
    if (total_intensity[i] > 255) total_intensity[i] = 255;
  }
  //  Serial.println(F(""));

  for (int i = 0; i < NUM_LED; i++) {
    lights[i].setRGB(total_intensity[0], total_intensity[1], total_intensity[2]);
  }

  FastLED.show();
}

void reset_lights() {
  for (uint8_t i = 0; i < NUM_LED; i++) lights[i].setRGB(0, 0, 0);
}

float absolute(float num) {
  if (num < 0) return -num;
  return num;
}

