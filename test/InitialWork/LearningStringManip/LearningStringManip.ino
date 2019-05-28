void setup() {
  Serial.begin(115200);
  char* meow = "1.5,3.5:0.4:23";

  char* woof = strtok(meow, ":");
  Serial.println(woof);
  woof = strtok(0,":");
  Serial.println(woof);
  woof = strtok(0,":");
  Serial.println(woof);
//  char* sep = strchr(woof, ',');
//  float grr = atof(meow);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (true) {
    Serial.println("meow");
    delay(1000);
  }
}
