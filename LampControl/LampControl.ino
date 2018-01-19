#include <TimerOne.h>
#include <QueueArray.h>
#include <SPI.h>
#include "RF24.h"
#include "messaging.h"

#define MAX_HITS 120

struct DrumHit {
  size_t drum_id;
  long incoming_time;
  float intensity; // should be between 0 and 1
};

bool started = false;
bool is_relay = false;
long last_hello = 0;
int expiry_time;

byte ID[3];
byte msg_buf[32];
QueueArray<DrumHit> hits;

RF24 radio(7,8);

void setup() {
  Serial.begin(115200);
  // TODO read the ID from EEPROM
}

void loop() {
  if (!started) {
    byte* req = make_indiv_req(ID);
    // TODO request for individual configuration and wait for individual and global reply
    // TODO if individual reply sent, set the individual configuration and send ack
    // TODO if global reply sent, set global configuration
    // TODO if global config not set, send global config request. Set global config when received
  } else {
    // TODO try to listen for reset message from master - should be encrypted (low priority)
    // TODO try to receive drum hit/hello, add to drum hit when received, set last_hello to millis()
    // TODO update LEDs
    // TODO if 30s have passed since last drum hit/hello, power down arduino through power_down()
  }
}

void power_down() {
  // resets all variables that need to be reset and powers down for a few seconds
  started = false;
  is_relay = false;
  expiry_time = 0;
  last_hello = 0;
  Timer1.detachInterrupt();
  // TODO reset other variables
  // TODO power down
}

void read_and_handle() {
  if (radio.available()) {
    // TODO read message into variable msg_buf here
    if (is_relay && to_be_relayed(msg_buf)) // TODO forward msg_buf
    if (is_hello(msg_buf)) last_hello = millis();
    switch (get_msg_type(msg_buf)) {
      case SETUP_MSG :
        // TODO set up local/global state
        Timer1.initialize(1000000);
        Timer1.attachInterrupt(remove_old_hits);
        break;
      case DRUM_HIT_MSG :
        // TODO add drum hit to buffer
        break;
    }
  }
}

void add_drum_hit(size_t drum_id, float intensity) {  // TODO must test the performance of this, otherwise we are going back to statically allocating and managing buffer
  DrumHit hit;
  hit.drum_id = drum_id;
  hit.intensity = intensity;
  hit.incoming_time = millis();
  hits.push(hit);
}

void remove_old_hits() {
  while (!hits.isEmpty()) {
    if(millis() - hits.peek().incoming_time > expiry_time) hits.pop();
    else break;
  }
}
