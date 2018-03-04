#include "messaging.h"
#include "comms.h"
#define SERIAL_SZ 100
#define PACKET_SZ 32

const int NO_OF_DRUMS = 4;
const int NO_OF_LOC = 2;
const int NO_OF_RGB = 3;

uint8_t radioInput[PACKET_SZ];
char serialInput[SERIAL_SZ];
byte radioOutput[PACKET_SZ];
bool serialIn = false;
bool radioIn = false;
// global SI: "I$256,257:258,259:260,261:262,263$100,200,100:20,20,20:30,30,30:0,0,255$90$90$255"
// non-global SI: "i$20$256,257"
int serialCounter = 0;

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
  clear_header(radioInput);
  parse_input();
}

void parse_input() {
  // handle input from radio
  radioIn = read_if_avail(radioInput);

  // handle input from drum rpi via Serial
  if (serialIn) {
    handle_rpi_in();
  }
  if (radioIn) {
    handle_lamp_in();
  }
}

void handle_rpi_in() {
  char *tokens[32];
  char *ptr = NULL;
  uint8_t tokenIndex = 0;

  Serial.print(F("received: "));
  /* get the first token */
  ptr = strtok(serialInput, "$:,");
  //  Serial.println(ptr);
  while (ptr != NULL) {
    tokens[tokenIndex] = ptr;
    Serial.print(tokens[tokenIndex]);
    tokenIndex++;
    ptr = strtok(NULL, "$:,");
  }

  // start dealing with the scenarios from rpi: global/non-global SI
  if ( tokens[0][0] == 'I' ) {
    Serial.println(F("Sending global SI"));

    set_setup(radioOutput, SETUP_MSG);
    set_global(radioOutput);
    set_address(radioOutput, atoi(tokens[1]));
    tokenIndex = 1;

    for (int i = 0; i < NO_OF_DRUMS; i++) {
      int x_loc = tokenIndex + (i * NO_OF_LOC);
      int y_loc = x_loc + 1;
      set_drum_loc(radioOutput, i, atoi(tokens[x_loc]),
                   atoi(tokens[y_loc]));
      //      Serial.println(atoi(tokens[x_loc]));
      int r_loc = tokenIndex + (NO_OF_DRUMS * NO_OF_LOC) + (i * NO_OF_RGB);
      int g_loc = r_loc + 1;
      int b_loc = r_loc + 2;
      set_drum_colour(radioOutput, i, atoi(tokens[r_loc]),
                      atoi(tokens[g_loc]),
                      atoi(tokens[b_loc]));
    }
    tokenIndex = tokenIndex + ((2 + 3) * NO_OF_DRUMS);
    set_period(radioOutput, atoi(tokens[tokenIndex]));
    set_wavelength(radioOutput, atoi(tokens[tokenIndex + 1]));
    set_expiry(radioOutput, atoi(tokens[tokenIndex + 2]));
  }


  else if (tokens[0][0] == 'i') {

    Serial.println(F("Sending non-global SI"));

    set_setup(radioOutput, SETUP_MSG);
    tokenIndex = 1;
    set_individual_setup(radioOutput, atoi(tokens[tokenIndex]),
                         atoi(tokens[tokenIndex + 1]),
                         atoi(tokens[tokenIndex + 2])
                        );
    //    radioOutput[tokenIndex + 3] = '\0';
  }
  else {
    Serial.println("error");
  }
  // send to lamps
  broadcast(0, radioOutput);
  serialIn = false;
  reset_serialInput();
  Serial.println(F("Done"));

  // ====== test ======
  //  Serial.write((uint8_t*)radioOutput, sizeof(radioOutput));
  //  for (int i = 0; i < sizeof(radioOutput) / sizeof(radioOutput[0]); i++) {
  //    if (i) {
  //      Serial.write(',');
  //    }
  //    Serial.print(radioOutput[i],DEC);
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
    Serial.println(F("R"));
  }
  else if (radioInput[0] << 5 == 0) {
    //handle non-global SR
    //    char* lamp_id;
    //    Serial.write(byte(radioInput));
    //    Serial.write('\n');
    uint16_t id = radioInput[1] << 8 | radioInput[2];
    Serial.print("r$");
    Serial.println(id);
  }
  else if (radioInput[0] << 5 == 0b01000000) {
    //handle SA
    //    char* lamp_id;
    uint16_t id = radioInput[1] << 8 | radioInput[2];
    //    itoa(radioInput[1] << 8 | radioInput[2], lamp_id, 10);
    Serial.print("a$");
    Serial.println(id);
  }
}

/*
  SerialEvent happens asynchronously whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void serialEvent() {
  Serial.println(F("serialevent"));
  char inChar;
  while (Serial.available() && !serialIn) {
    // get the new byte:
    inChar = (char)Serial.read();
    // add it to the serialInput buffer:
    serialInput[serialCounter] = inChar;
    serialCounter++;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      Serial.println(F("ohno"));
      serialCounter = 0;
      serialIn = true;
    }
  }
  //  for (int i = 0; i < SERIAL_SZ; i++) {
  //    Serial.print(serialInput[i]);
  //  }
  //  Serial.println();
}

void reset_serialInput() {
  for (int i = 0; i < SERIAL_SZ; i++) {
    serialInput[i] = 0;
  }
}
