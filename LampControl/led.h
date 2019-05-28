#ifndef LED
#define LED

#include <FastLED.h>
#include "state.h"

#define CHIPSET UCS1903B
#define NUM_LED 4
#define LED_PIN 3
#define MAX_WAVE_DIST 3
#define WAVE_SPREAD 20

void setup_lights();
void set_rgb();
void reset_lights();
float absolute(float num);

#endif
