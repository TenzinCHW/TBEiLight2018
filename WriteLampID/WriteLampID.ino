#include <EEPROM.h>

void setup() {
  // Use this to write the IDs to EEPROM
  uint16_t ID = 1;
  EEPROM.write(0, ID >> 8);
  EEPROM.write(1, ID);
  
  uint16_t check = EEPROM.read(0) << 8 | EEPROM.read(1);
  if (check != ID) Serial.println(F("ID was not written to EEPROM correctly"));
}

void loop() {
  // put your main code here, to run repeatedly:

}
