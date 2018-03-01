#include "messaging.h"
#include "comms.h"

const int NO_OF_DRUMS = 4;
const int NO_OF_LOC = 2;
const int NO_OF_RGB = 3;

uint8_t radioInput[32];
char serialInput[100];
byte msg[32];
boolean serialIn, radioIn = false;
// global SI: "I$256,257:258,259:260,261:262,263$100,200,100:20,20,20:30,30,30:0,0,255$90$90$255"
// non-global SI: "i$20$256,257"

void setup() {
  startup_nRF();
  Serial.begin(115200);
  // ======= test =======
  //  char in[] = "i$20$256,257";
  //  for (int i = 0; i < (int)(sizeof(in) / sizeof(char)); i++) {
  //    serialInput[i] = in[i];
  //  }
  //  serialIn = true;
  // ====== end test ======

}

void loop() {
  clear_header(msg);
  parse_input();
}

void parse_input() {
  // handle input from radio 
  radioIn = read_if_avail(radioInput);
  
  // handle input from drum rpi via Serial
  if (serialIn) {
    handle_rpi_in();
  }
  else if (radioIn) {
    handle_lamp_in();
  }
}

void handle_rpi_in() {
  char *tokens[32];
  char *ptr = NULL;
  byte msgIndex, tokenIndex = 0;

//  Serial.write("received: ");
  /* get the first token */
  ptr = strtok(serialInput, "$:,");
  while (ptr != NULL) {
    tokens[tokenIndex] = ptr;
//    Serial.write(ptr);
    tokenIndex++;
    ptr = strtok(NULL, "$:,");
  }
//  Serial.write("\0");
//  Serial.println();


  // start dealing with the scenarios from rpi: global/non-global SI
  if ( tokens[0][0] == 'I' ) {

    //    Serial.println("global SI received: ");

    set_setup(msg, SETUP_MSG);
    set_global(msg);
    set_address(msg, atoi(tokens[1]));
    tokenIndex = 1;

    for (int i = 0; i < NO_OF_DRUMS; i++) {
      int x_loc = tokenIndex + (i * NO_OF_LOC);
      int y_loc = x_loc + 1;
      set_drum_loc(msg, i, atoi(tokens[x_loc]),
                   atoi(tokens[y_loc]));
      //      Serial.println(atoi(tokens[x_loc]));
      int r_loc = tokenIndex + (NO_OF_DRUMS * NO_OF_LOC) + (i * NO_OF_RGB);
      int g_loc = r_loc + 1;
      int b_loc = r_loc + 2;
      set_drum_colour(msg, i, atoi(tokens[r_loc]),
                      atoi(tokens[g_loc]),
                      atoi(tokens[b_loc]));
    }
    tokenIndex = tokenIndex + ((2 + 3) * NO_OF_DRUMS);
    set_period(msg, atoi(tokens[tokenIndex]));
    set_wavelength(msg, atoi(tokens[tokenIndex + 1]));
    set_expiry(msg, atoi(tokens[tokenIndex + 2]));
  }


  else if (tokens[0][0] == 'i') {

    //    Serial.println("non-global SI received: ");

    set_setup(msg, SETUP_MSG);
    tokenIndex = 1;
    set_individual_setup(msg, atoi(tokens[tokenIndex]),
                         atoi(tokens[tokenIndex + 1]),
                         atoi(tokens[tokenIndex + 2])
                        );
//    msg[tokenIndex + 3] = '\0';
  }
  else {
    //    Serial.println("error ");
  }
  // send to lamps
  broadcast(0, msg);
  serialIn = false;

  // ====== test ======
  //  Serial.write((uint8_t*)msg, sizeof(msg));
  //  for (int i = 0; i < sizeof(msg) / sizeof(msg[0]); i++) {
  //    if (i) {
  //      Serial.write(',');
  //    }
  //    Serial.print(msg[i],DEC);
  //  }
  //
  //  Serial.write('\n');
  //  Serial.write("finished!");
  //  serialIn = false;
  // ===== end test ======
}




void handle_lamp_in() {

  if (radioInput[0] == 8) {
    //handle global SR
    Serial.println("R");
  }
  else if (radioInput[0] << 5 == 0) {
    //handle non-global SR
    char* lamp_id;
    Serial.write(byte(radioInput));
    Serial.write('\n');
    itoa(radioInput[1] << 8 | radioInput[2], lamp_id, 10);
    Serial.print("r$");
    Serial.println(lamp_id);
  }
  else if (radioInput[0] << 5 == 0b01000000) {
    //handle SA
    char* lamp_id;
    Serial.println(radioInput[1] << 8 | radioInput[2]);
    itoa(radioInput[1] << 8 | radioInput[2], lamp_id, 10);
    Serial.print("a$");
    Serial.println(lamp_id);
  }
}

/*
  SerialEvent happens asynchronously whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    int count = 0;
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    serialInput[count] = inChar;
    count++;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == "\n") {

      serialIn = true;
    }
  }
}