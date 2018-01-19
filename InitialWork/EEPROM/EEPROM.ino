byte val;
byte value;

void setup() {
  
}

void loop() {
  // write to EEPROM
  for (int address = 0; address <= 3; address++){
    EEPROM.write(address, val);
    //each byte of the EEPROM can only hold a value from 0 to 255.
  }

// read from EEPROM
  value = EEPROM.read(address);



}
