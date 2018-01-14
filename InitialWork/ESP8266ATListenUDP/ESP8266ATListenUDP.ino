#include <SoftwareSerial.h>

SoftwareSerial lamp1 (10, 11);  //RX,TX

//  Arduino pin 10 (RX) to ESP8266 TX
//  Arduino pin 11 to voltage divider then to ESP8266 RX
//  Connect GND from the Arduiono to GND on the ESP8266
//  Pull ESP8266 CH_PD HIGH

// When a command is entered in to the serial monitor on the computer
// the Arduino will relay it to the ESP8266

void setup() {
  Serial.begin(9600);
  Serial.println("Meow");
  lamp1.begin(115200);
  lamp1.println("AT");
  while (lamp1.available()) {
    Serial.write(lamp1.read());
  }
}

void loop() {
  // listen for communication from the ESP8266 and then write it to the serial monitor
  if (lamp1.available()) {
    Serial.write(lamp1.read());
  }

  // listen for user input and send it to the ESP8266
  if (Serial.available()) {
    lamp1.write(Serial.read());
  }
}
