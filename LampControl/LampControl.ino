#include "messaging.h"

bool started = false;
long last_hello = 0;

byte ID[3] = {50,91,34};

void setup() {
  Serial.begin(115200);
  // TODO read the ID from EEPROM
  // TODO create individual request message
  byte meow[4];
  meow[0] = 0;
  copy_id(&meow[1], ID);
  for (int i = 0; i < 4; i++) Serial.println(meow[i]);
}

void loop() {
  if (!started) {
    
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
  // TODO reset other variables
  // TODO power down
}

    // TODO add timer interupt-driven routine to remove old drum hits every 1s
    // TODO create buffer managing functions for fast addition/removal of drum hits
