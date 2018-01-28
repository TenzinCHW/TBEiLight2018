//#define TESTING 1
//#ifdef TESTING
//  #include "test.h"
//#else
#include "state.h"
//#endif

void setup() {
  Serial.begin(115200);
  init_ID();
  extern State state;
  Serial.println(sizeof(state));
  //  State test = copy_state(state);
  //  if (compare_states(state, test)) Serial.println("MEOW");

  // TODO use this to write the IDs to EEPROM
  //  uint16_t moo = 4502;
  //  EEPROM.write(0, moo >> 8);
  //  EEPROM.write(1, moo);
  delay(1000);  // For stability
}


void loop() {
  main_loop();
}
