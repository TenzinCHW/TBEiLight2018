

String inputString, header, binary, msg, empty = "";
boolean stringComplete = false;
int no_of_drums = 4;
int bit_size = 128;
char buf[5];
long lamp_id;

void setup() {
  Serial.begin(115200);
}

void loop() {
    Serial.flush();
    if (stringComplete) {
      header = inputString.substring(0,7);
      // parse the request from bytes 
      // and redirect to either other lamps or to mDrum
      if (header == "00000100") { // if input is binary - from lamp arduinos
          // global SR 
          send_rpi("G$SR");
      }
      else if (header == "00000000") { // non-global SR
          binary = inputString.substring(8,23);
          lamp_id = strtol(binary.c_str(), NULL, 2);
          ltoa(lamp_id, buf, 10);
          send_rpi(strcat("g$SR$", buf));
      }
      else if (header == "00000010") { // SA
          binary = inputString.substring(8,23);
          lamp_id = strtol(binary.c_str(), NULL, 2);
          ltoa(lamp_id, buf, 10);
          send_rpi(strcat("g$SA$", buf));
      }

      else { // read from rpi
//          int len;
//          byte bit_msg[bit_size];
//          msg = inputString;
//          msg.trim();
//          msg.replace(String::String('$'), empty);
//          msg.replace(String::String(':'), empty);
//          msg.replace(String::String(','), empty);
//          msg.getBytes(bit_msg, bit_size);
//          Serial.write(bit_msg, bit_size);

            byte bit_msg[bit_size];
            char *elements[20]; 
            char *ptr = NULL;
            byte index = 0;
            char input[30];
            inputString.toCharArray(input, 30);
            
            /* get the first token */
            ptr = strtok(input, "$:,");
            while(ptr != NULL) {
              elements[index] = ptr;
              index++;
              ptr = strtok(NULL, "$:,"); 
            }
            for (int i=0; i<sizeof(elements); i++) {
              bit_msg[i] = byte(elements[i]);
              Serial.write(bit_msg[i]);
            }
            Serial.write(byte('\n'));
      };
    }
//     mock sending msgs to mDrum
    String msgs [] = {
        "g$SR$2",
        "G$SR",
        "g$SA$4"
    };
    int msg_count = 3;
    for(int i=0; i<msg_count; i++) {
        Serial.println(msgs[i]);
        delay(5000);  
    }  

}

/*
The following function convert any int from 0-255 to binary.
You need to pass the int as agrument.
You also need to pass the 8bit array of boolean
*/
void convertDecToBin(int Dec, boolean Bin[]) {
  for(int i = 7 ; i >= 0 ; i--) {
    if(pow(2, i)<=Dec) {
      Dec = Dec - pow(2, i);
      Bin[8-(i+1)] = 1;
    } else {
    }
  }
}
void send_lamp(byte msg) {

}
void send_rpi(String msg) {
  Serial.println(msg);
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
