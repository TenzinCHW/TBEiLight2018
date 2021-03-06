#include <EEPROM.h>
#include <TimerOne.h>
#include "FastLED.h"
#include "comms.h"
#include "messaging.h"

#define FILTER_SIZE 5
#define THRESHOLD 60
#define HIT_MIN_TIME 100

struct InputQueue {
  int input[FILTER_SIZE];
  uint16_t head = 0;
  uint16_t counter = 0;

  void push(uint16_t reading) {
    if (counter == FILTER_SIZE) {
      input[head] = reading;
      if (head == FILTER_SIZE - 1) {
        head = 0;
      } else {
        head++;
      }
    } else {
      input[counter] = reading;
      counter++;
    }
  }

  uint16_t get_val(uint16_t i) {
    return input[(head + i) % FILTER_SIZE];
  }
};

uint16_t ID;
uint16_t filter[FILTER_SIZE] = {1, 2, 6, 8, 10};
InputQueue input;
uint16_t j;
uint16_t cor_sum;
uint16_t hit_counter;
byte msg_buf[PACKET_SZ];
long time_since_last_hit;
bool lock = false;

#define NUM_LED 5
CRGB leds[NUM_LED];

void send_drum_hit(uint16_t counter, uint8_t intensity) {
  make_drum_hit(msg_buf, ID, counter, intensity);
  broadcast(0, msg_buf, DRUM_HIT_SZ);
}

void read_value() {
  input.push(analogRead(A5)*3);
  cor_sum = 0;

  //    corrrelation part
  for (j = 0; j < FILTER_SIZE; j++) {
    // multiply filter[i] with the i-th element of input
    cor_sum += input.get_val(j) * filter[j];
  }
//  Serial.println(cor_sum);
  if (cor_sum > THRESHOLD && millis() - time_since_last_hit > HIT_MIN_TIME) {
        Serial.println(F("Sending"));
    cor_sum /= 10;
    if (cor_sum > 100) cor_sum = 100;
    Serial.println(cor_sum);
    send_drum_hit(hit_counter, 100);
    hit_counter++;
    time_since_last_hit = millis();
  }
}

void setup() {
  Serial.begin(115200);
  startup_nRF();
  ID = EEPROM.read(0) << 8 | EEPROM.read(1);
  Timer1.initialize(20000);
  Timer1.attachInterrupt(read_value);

  // SET LED BRIGHTNESS AND COLOUR
  FastLED.addLeds<UCS1903, 2>(leds, NUM_LED);
  for (int i = 0; i < NUM_LED; i++) {
    leds[i].setRGB(20, 20, 20);
  }
  FastLED.show();
}

void loop() {
  if (millis() - time_since_last_hit > 5000) {
    send_hello();
    time_since_last_hit = millis();
  }
}

void send_hello() {
  Serial.println(F("Hello there"));
  make_hello(msg_buf);
  broadcast(0, msg_buf, HELLO_SZ);
}
