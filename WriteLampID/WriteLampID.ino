#include <EEPROM.h>

void setup() {
  // Use this to write the IDs to EEPROM
  Serial.begin(115200);
  uint16_t ID = 0;
  EEPROM.write(0, ID >> 8);
  EEPROM.write(1, ID);
  
  uint16_t check = EEPROM.read(0) << 8 | EEPROM.read(1);
  Serial.println(check);
  if (check != ID) Serial.println(F("ID was not written to EEPROM correctly"));
}

void loop() {
  // put your main code here, to run repeatedly:

}
