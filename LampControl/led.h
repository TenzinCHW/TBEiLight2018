#ifndef LED
#define LED

#include <FastLED.h>
#include "state.h"

#define CHIPSET UCS1903B
#define NUM_LED 4
#define LED_PIN 2
#define CLOCK_PIN 5

void setup_lights();
void set_rgb(); // TODO test

#endif
