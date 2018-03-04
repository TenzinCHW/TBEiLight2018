#include <EEPROM.h>
#include <TimerOne.h>
#include "comms.h"
#include "messaging.h"

#define FILTER_SIZE 5
#define THRESHOLD 60

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

void read_value() {
  input.push(analogRead(A5));
  cor_sum = 0;

  //    corrrelation part
  for (j = 0; j < FILTER_SIZE; j++) {
    // multiply filter[i] with the i-th element of input
    cor_sum += input.get_val(j) * filter[j];
  }
  Serial.println(cor_sum);
  if (cor_sum > THRESHOLD) {
    if (cor_sum > 400) cor_sum = 100;
    else cor_sum = cor_sum / 400 * 100;
    send_drum_hit(hit_counter, cor_sum);
  }
}

void setup() {
  startup_nRF();
  Serial.begin(115200);
  ID = EEPROM.read(0) << 8 | EEPROM.read(1);
  Timer1.initialize(20000);
  Timer1.attachInterrupt(read_value);
}

void loop() {
  delay(10000);
  send_hello();
}

void send_hello() {
  make_hello(msg_buf);
  broadcast(msg_buf, 0);
}

void send_drum_hit(uint16_t counter, uint8_t intensity) {
  make_drum_hit(msg_buf, ID, counter, intensity);
  broadcast(msg_buf, 0);
}

