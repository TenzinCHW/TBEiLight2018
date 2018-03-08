#include <EEPROM.h>
#include <TimerOne.h>
#include "comms.h"
#include "messaging.h"
#include <avr/wdt.h>

#define FILTER_SIZE 5
#define THRESHOLD 60
#define HIT_MIN_TIME 100
#define NUM_RETRY 3

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
        //        head++;
        head = head + 1;
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
long time_to_reset;

void read_value() {
  input.push(analogRead(A5));
  cor_sum = 0;

  //    corrrelation part
  for (j = 0; j < FILTER_SIZE; j++) {
    // multiply filter[i] with the i-th element of input
    cor_sum += input.get_val(j) * filter[j];
  }
  if (cor_sum > THRESHOLD && millis() - time_since_last_hit > HIT_MIN_TIME) {
//    Serial.println(F("Sending"));
    if (cor_sum > 400) cor_sum = 100;
    else cor_sum = float(cor_sum) / 10;
//    Serial.println(cor_sum);
    send_drum_hit(hit_counter, cor_sum);
    hit_counter++;
    time_since_last_hit = millis();
  }
}

void setup() {
  startup_nRF();
  Serial.begin(115200);
  ID = EEPROM.read(0) << 8 | EEPROM.read(1);
  MCUSR = 0; // clear prev resets
  Timer1.initialize(20000);
  Timer1.attachInterrupt(read_value);
  long wake_everyone_up = millis();
  while (millis() - wake_everyone_up < 10000) {
    send_hello();
    delay(10);
  }
//  time_to_reset = millis();
}

void loop() {
  delay(5000);
  send_hello();
//  if ((millis() - time_to_reset) > 6000) {
//    Serial.println(F("Restarting"));
//    Timer1.detachInterrupt();
//    wdt_enable(WDTO_15MS);
//    for (;;) {}
//  }
}

void send_hello() {
  Serial.println(F("Hello there"));
  make_hello(msg_buf);
  broadcast(0, msg_buf);
}

void send_drum_hit(uint16_t counter, uint8_t intensity) {
  make_drum_hit(msg_buf, ID, counter, intensity);
  for (uint8_t i = 0; i < NUM_RETRY; i++) broadcast(0, msg_buf);
}
