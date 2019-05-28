#include <EEPROM.h>
#include <TimerOne.h>
#include "messaging.h"
#include "comms.h"
#include "config.h"

#define SERIAL_SZ 100
#define PACKET_SZ 32
#define FILTER_SIZE 5
#define THRESHOLD 60
#define NO_OF_DRUMS 4;

uint8_t radioInput[PACKET_SZ];
byte radioOutput[PACKET_SZ];
bool radioIn = false;

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
  //Serial.println(cor_sum);
  if (cor_sum > THRESHOLD) {
    Serial.println(F("Sending"));
    if (cor_sum > 400) cor_sum = 100;
    else cor_sum = cor_sum / 400 * 100;
    send_drum_hit(hit_counter, cor_sum);
    hit_counter++;
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
  parse_input();
}

void parse_input() {
  // handle input from radio
  radioIn = read_if_avail(radioInput);

  // handle input from lamps
  if (radioIn) {
    handle_lamp_in();
  }
}

void handle_lamp_in() {
  uint8_t msg_type = get_msg_type(radioInput);
  if (msg_type == SETUP_REQ_MSG && is_global(radioInput)) {
    //handle global SR
    Serial.println(F("Sending global config"));
    set_setup(radioOutput, SETUP_MSG);
    set_global(radioOutput);
    for (int i = 0; i < NO_OF_DRUMS; i++) {
      int x_loc = drum_loc[i][0];
      int y_loc = drum_loc[i][1];
      set_drum_loc(radioOutput, i, x_loc, y_loc);
      uint8_t r = drum_colours[i][0];
      uint8_t g = drum_colours[i][1];
      uint8_t b = drum_colours[i][2];
      set_drum_colour(radioOutput, i, r, g, b);
    }
    set_period(radioOutput, period);
    set_wavelength(radioOutput, wl);
    set_expiry(radioOutput, expiry);
  }
  else if (msg_type == SETUP_REQ_MSG && !is_global(radioInput)) {
    //handle non-global SR
    Serial.println(F("Sending local config"));
    uint16_t id = radioInput[LAMP_ID] << 8 | radioInput[LAMP_ID+1];
    Serial.println(id);
    if (id > 119) {
      id = 0;
    }
    set_individual_setup(radioOutput, id, lamp_loc[id][0], lamp_loc[id][1]);
  }
  else if (radioInput[0] << 5 == 0b01000000) {
    //handle SA
    uint16_t id = radioInput[1] << 8 | radioInput[2];
    Serial.print("a$");
    Serial.println(id);
  }
  bool sent = false
  while (!sent) sent = protected_broadcast(0, radioOutput, PACKET_SZ);
  clear_header(radioInput);
}

void send_drum_hit(uint16_t counter, uint8_t intensity) {
  make_drum_hit(msg_buf, ID, counter, intensity);
  protected_broadcast(msg_buf, 0, DRUM_HIT_SZ);
}
