#include <TimerOne.h>

// the setup routine runs once when you press reset:
#define FILTER_SIZE 5
#define THRESHOLD 800

struct InputQueue {
  int input[FILTER_SIZE];
  uint16_t head = 0;
  uint16_t counter = 0;

  void push(uint16_t reading) {
    if (counter == FILTER_SIZE) {
      input[head] = reading;
      if (head == FILTER_SIZE - 1) {
        head = 0;
      } else {
        //        head++;
        head = head + 1;
      }
    } else {
      input[counter] = reading;
      counter++;
    }
  }

  uint16_t get_val(uint16_t i) {
    return input[(head + i) % FILTER_SIZE];
  }
};

uint16_t filter[FILTER_SIZE] = {1, 5, 10, 10, 10};
InputQueue input;
uint16_t j;
uint16_t cor_sum;

void read_value() {
  input.push(analogRead(A5));
  cor_sum = 0;

  //    corrrelation part
  for (j = 0; j < FILTER_SIZE; j++) {
    // multiply filter[i] with the i-th element of input
    cor_sum += input.get_val(j) * filter[j];
  }
  Serial.println(cor_sum);
  if (cor_sum > THRESHOLD) {
    Serial.println(F("yey"));
  }
}

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Timer1.initialize(10000);
  Timer1.attachInterrupt(read_value);
}

// the loop routine runs over and over again forever:
void loop() {
  //    int peak = input.counter;
  //    bool keeplooking = true;
  //    while(keeplooking) {
  //      int nextReading = sensorValue;
  //       if( nextReading > input.counter){
  //        peak = nextReading;
  //        input.counter = nextReading;
  //       } else {
  //          keeplooking = false;
  //       }

  //  Serial.println(input.get_val(FILTER_SIZE-1));
  //  Serial.println(analogRead(A5));
  delay(10);
}

