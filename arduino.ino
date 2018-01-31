String inputString = "";
boolean stringComplete = false;

void setup() {
  Serial.begin(115200);
}

void loop() {
    Serial.flush();
    if (stringComplete) {
      // parse the request from bytes 
      // and redirect to either other lamps or to mDrum
    }
    // mock sending msgs to mDrum
    String msgs [] = {
        "g$SR$2",
        "G$SR$3",
        "g$SA$4"
    };
    int msg_count = 3;
    for(int i=0; i<msg_count; i++) {
        Serial.println(msgs[i]);
        delay(5000);  
    }  

}
/*
  SerialEvent occurs asynchronously whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}
