#include <SPI.h>
#include "state.h"

void setup() {
  Serial.begin(115200);
  // TODO read the ID from EEPROM
  delay(1000);  // For stability
}

void loop() {
  main_loop();
}
