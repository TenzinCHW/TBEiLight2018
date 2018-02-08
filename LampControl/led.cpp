#include "led.h"

CRGB lights[NUM_LED];
extern State state;

void setup_lights() {
  FastLED.addLeds<CHIPSET, LED_PIN>(lights, NUM_LED);
}

void set_rgb(CRGB* lights, State state) {
  // TODO wave equation goes here. Add up values for each drum.
  // use lights[i].setRGB(r_val, g_val, b_val);
  // where r_val, g_val and b_val are the values of red, green and blue respectively.
}
