String inputString, header, binary= "";
boolean stringComplete = false;
int no_of_drums = 4;
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

      // if input is binary - from lamp arduinos
      if (header == "00000100") {
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

      // else if string is from rpi in string format
      else { 
          char *elements[20]; 
          char *ptr = NULL;
          byte index, bit_index = 0;
          char input[50];
          inputString.toCharArray(input, 50);
          
          /* get the first token */
          ptr = strtok(input, "$:,");
          while(ptr != NULL) {
            elements[index] = ptr;
            index++;
            ptr = strtok(NULL, "$:,"); 
          }
          Serial.write("received: ");
          Serial.write(elements[0]);
          Serial.write(elements[1]);
          
          index = 2;
          if ((elements[0] == "G") && (elements[1] == "SI")) {
            Serial.write("global SI received: ");
            byte bit_msg[32]; 
            bit_msg[0] = byte(9); //set header byte to 00000101
            bit_index = 1;
            
            boolean doneLoc, doneRgb = false;
            while ((!doneLoc) | (!doneRgb)) {
              for (int i=0; i<no_of_drums; i++) {
                while (!doneLoc) {
                  if (i == 3) { doneLoc = true; } 
                  write_loc(elements, bit_msg, index, bit_index);
                }
                while (!doneRgb) {
                  if (i == 3) { doneRgb = true; }
                  write_rgb(elements, bit_msg, index, bit_index);
                }
              }
            }
            // write last two elements (period & wavelength) to output
            for (int i=0; i<2; i++) { 
              bit_msg[bit_index] = elements[30+i]; 
            }
            send_lamp(bit_msg);
          }
          

          else if ((elements[0] == "g") && (elements[1] == "SI")) {
            Serial.write("non-global SI received: ");
            byte bit_msg[4]; 
            bit_msg[0] = byte(1); //set header byte to 00000001
            bit_index = 1;
            int lamp_id = atoi(elements[index]);
            // lamp_id to take 3 bytes 
            for (int i=0; i<3; i++) {
              bit_msg[bit_index + i] = (byte) lamp_id >> (i*8);
            }
            send_lamp(bit_msg);
          }

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
        delay(1000);  
    }  

}

void write_loc(char *elements[], byte buf[], int index, int bit_index) {
  for (int a=0; a<2; a++) {
      int loc = atoi(elements[index]);
      buf[bit_index] = (byte) loc;
      buf[bit_index + 1] = (byte) loc >> 8;
      index, bit_index++;
  }
}

void write_rgb(char *elements[], byte buf[], int index, int bit_index) {
    for (int a=0; a<3; a++) {
      int loc = atoi(elements[index]);
      buf[bit_index] = (byte) loc;
      index, bit_index++;
  }
}


void send_lamp(byte bit_msg[]) {
  for (int i=0; i<sizeof(bit_msg); i++) {
    Serial.write(bit_msg[i]);
  }
  Serial.write(byte('\n'));
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
