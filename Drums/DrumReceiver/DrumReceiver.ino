#include "comms.h"
#include "messaging.h"

byte msg_buf[32] = {0};

void setup() {
  startup_nRF();
  Serial.begin(115200);
  Serial.print(F("Buffer contents: "));
  print_buffer(msg_buf, 32);
}

void loop() {
  if (read_if_avail(msg_buf)) {
    Serial.print(F("Drum ID: ")); Serial.println(get_drum_id(msg_buf));
    Serial.print(F("Counter: ")); Serial.println(get_hit_counter(msg_buf));
    Serial.print(F("Intensity: ")); Serial.println(get_hit_intensity(msg_buf));
  }
}
