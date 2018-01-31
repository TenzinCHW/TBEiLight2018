#define TESTING 1
#ifdef TESTING
  #include "test.h"
#else
#include "state.h"
#endif

void setup() {
  Serial.begin(115200);
  init_ID();

  // TESTS //
  all_tests();
  // END TESTS //
  byte meow[32];
  for (int i = 0; i < 32; i++) {
    meow[i] = i;
  }
  broadcast(1, meow);
  Serial.println("Done");

  delay(1000);  // For stability
}


void loop() {
  main_loop();
}
