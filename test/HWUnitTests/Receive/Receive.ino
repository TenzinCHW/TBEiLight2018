#include <TimerOne.h>
#include <SPI.h>
#include <nRF24L01.h>
#include "printf.h"
#include <RF24.h>
#include "FastLED.h"
#include "LowPower.h"

RF24 radio (7,8);

#define PLOAD_WIDTH  32  // 32 unsigned chars TX payload
#define DRUM_HIT_MSG 3
#define HELLO_MSG 4
#define NUM_OF_DRUM 4
#define DRUM_ID_BYTE 1
#define DRUM_HIT_INTENSITY 2
#define DRUM_COUNTER_BYTE 4
#define MAX_BLINK 15
#define MIN_BLINK 5
#define MAX_RAND_WAIT 3000
#define POWER_PIN 13
#define NUM_LED 4
#define LED_PIN 3

unsigned char rx_buf[PLOAD_WIDTH] = {0};
const unsigned char ADDRESS1[5]  = {0xb1, 0x41, 0x29, 0x75, 0x93};
const unsigned char ADDRESS0[5]  = {0xb0, 0x41, 0x29, 0x75, 0x93};

CRGB leds[4];
uint8_t blink_count = 0;
uint8_t colours[][3] = {{60, 60, 60}, {60, 0, 0}, {0, 60, 0}, {0, 0, 60}, {0, 60, 60}, {60, 60, 0}, {60, 0, 60}};
volatile bool hit_flag[NUM_OF_DRUM] = {0};
long incoming_time;
long random_wait;
uint8_t index;
uint8_t r;
uint8_t g;
uint8_t b;
uint8_t colour_total;
bool off_on = false;
long last_hit_hello;
long stay_awake;

void setup() {
  Serial.begin(115200);
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  radio.begin();
  printf_begin();
  radio.setDataRate(RF24_2MBPS);
  radio.enableDynamicPayloads();
  radio.setAutoAck(false);  //  turn off acknowledgements
  radio.setRetries(1, 15);
  radio.setChannel(50);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(0, ADDRESS0);
  radio.openReadingPipe(1, ADDRESS1);
  radio.startListening();
  radio.printDetails();
  Timer1.initialize(20000);
  Timer1.attachInterrupt(hit_toggle);
  Serial.println("Receiver");
  FastLED.addLeds<UCS1903, LED_PIN>(leds, NUM_LED);
  setRGB(0, 0, 0);
  randomSeed(analogRead(0));
  delay(1000);
  stay_awake = millis();
}

void loop() {
  while (millis() - stay_awake < 1000) {
    if (read_if_avail(rx_buf)) {
      Serial.println(F("Received something"));
      if (rx_buf[0] == DRUM_HIT_MSG) {
        hit_flag[get_drum_id(rx_buf)] = true;
        if (hit_flag[0] && hit_flag[1]) Serial.println(F("Both hit"));
        choose_colour();
        random_wait = random(MAX_RAND_WAIT);
        blink_count = random(MIN_BLINK, MAX_BLINK);
        incoming_time = millis();
        last_hit_hello = millis();
      } else if (rx_buf[0] == HELLO_MSG) {
        last_hit_hello = millis();
//        Serial.println(colour_total);
      }
      //    if (rx_buf[0] == 255) {
      //      Serial.println(F("Cycling"));
      //      cycle();
      //      setRGB(colours[counter][0], colours[counter][1], colours[2]);
      //    }
    }
  }
  if (millis() - last_hit_hello > 30000) {
    Serial.println(F("Gonna sleep"));
    setRGB(0,0,0);
    LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
                  SPI_OFF, USART0_OFF, TWI_OFF);
  }
  setRGB(0,0,0);
  stay_awake = millis();
}

bool read_if_avail(uint8_t* buf) {
  if (radio.available()) {
    if (radio.getDynamicPayloadSize() < 1) {
      return false;
    }
    read_and_flush(buf);
    return true;
  } else {
    return false;
  }
}

void read_and_reply(uint8_t pipe_num, uint8_t* buf) {
  if (radio.available()) {
    if (radio.getDynamicPayloadSize() < 1) return;
    read_and_flush(buf);
    print_buffer(buf, PLOAD_WIDTH); // Needed if you are using relay
    buf[0] |= 1 << 7;
    switch (pipe_num) {
      case 0: broadcast(buf, ADDRESS0); break;
      case 1: broadcast(buf, ADDRESS1); break;
    }
  }
}

void read_and_flush(uint8_t* buf) {
  radio.read(buf, PLOAD_WIDTH);
  radio.flush_rx();
}

void broadcast(uint8_t* buf, unsigned char* address) {
  radio.stopListening();
  radio.openWritingPipe(address); // No need to close, just change the address. Only 1 address can be written to at the same time.
  radio.startWrite(buf, PLOAD_WIDTH, true);
  //  Serial.println(F("Wrote liao"));
  radio.txStandBy();
  radio.startListening();
}

void print_buffer(uint8_t* buf, uint8_t len) {
  for (int i = 0; i < len; i++) Serial.print(buf[i]);
  Serial.println();
}

void setRGB(int r, int g, int b) {
  for (int i = 0; i < NUM_OF_DRUM; i++) {
    leds[i].setRGB(r, g, b);
  }
  FastLED.show();
}

void hit_toggle() {
  if (blink_count > 0 && millis() - incoming_time > random_wait) {
    if (off_on) {
      r = colours[colour_total][0];
      g = colours[colour_total][1];
      b = colours[colour_total][2];
    } else {
      r = 0;
      g = 0;
      b = 0;
      blink_count--;
    }
    off_on = !off_on;
    setRGB(r, g, b);
    if (blink_count == 0) {
      for (uint8_t i = 0; i < NUM_OF_DRUM; i++) {
        hit_flag[i] = false;
      }
    }
  }
}

// For drum hits //
/*
   input msg: pointer to state message buffer
   return: ID of drum that sent the message currently in state message buffer (0-3)
*/
uint8_t get_drum_id(byte * msg) {
  return msg[DRUM_ID_BYTE];
}

/*
    input msg: pointer to state message buffer
   return: intensity from 0 to 1
*/
float get_hit_intensity(byte * msg) {
  return (msg[DRUM_HIT_INTENSITY] << 8 | msg[DRUM_HIT_INTENSITY + 1]);
  //  return (msg[DRUM_HIT_INTENSITY] << 8 | msg[DRUM_HIT_INTENSITY+1]) / 100.0;
}

/*
   input msg: pointer to state message buffer
   return: drum hit counter of this drum
*/
uint16_t get_hit_counter(byte * msg) {
  return msg[DRUM_COUNTER_BYTE] << 8 | msg[DRUM_COUNTER_BYTE + 1];  //msg[DRUM_COUNTER_BYTE] << 24 | msg[DRUM_COUNTER_BYTE + 1] << 16 |
}

uint8_t choose_colour() {
  colour_total = 0;
  for (uint8_t i = 0; i < NUM_OF_DRUM; i++) {
    if (hit_flag[i]) colour_total += i;
  }
  Serial.println(colour_total);
}
