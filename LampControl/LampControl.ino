#define TESTING
#ifdef TESTING
#include "test.h"
#else
#include "state.h"
#endif

extern State state;

void setup() {
  Serial.begin(9600);

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
    if (!(state.globals_set && state.indiv_var_set)) power_down();
  } else main_loop();
  delay(50);  // At most update 20 times per second. No one will notice.
}
