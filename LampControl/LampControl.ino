#define TESTING
#ifdef TESTING
#include "test.h"
#else
#include "state.h"
#endif

extern State state;

void setup() {
  Serial.begin(115200);
  //  delay(1000);  // For stability

  // SETUP ID, COMMS AND LEDS //
  init_ID();
  startup_nRF();
  setup_lights();

  // TESTS //
  //  all_tests();
  // END TESTS //

  delay(1000);  // For stability
}


void loop() {
  if (!(state.globals_set && state.indiv_var_set)) {
    req_indiv_setup();
    req_global_setup();
  } else main_loop();
}
