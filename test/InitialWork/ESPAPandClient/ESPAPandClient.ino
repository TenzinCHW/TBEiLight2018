#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiUDP.h>

//const char* ssid     = "ESP8266 Access Point";         // The SSID (name) of the Wi-Fi network you want to connect to
//const char* password = "thereisnospoon";     // The password of the Wi-Fi network

#define INCOMING_PACKET_SIZE 255

const char* ssid = "NetwerkzLit";
const char* password = "iLoveNetworks";

WiFiUDP UDP;
unsigned int local_UDP_port = 4210;
unsigned int root_UDP_port = 5010;
char incomingPacket[INCOMING_PACKET_SIZE];

void setup() {
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  WiFi.mode(WIFI_AP_STA);

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer


  Serial.println("Now trying to set up another AP...");
  WiFi.softAP("Meow", "woof.meow", 7, false);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print("Meow");
  Serial.println("\" started");

  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());         // Send the IP address of the ESP8266 to the computer

  UDP.begin(local_UDP_port);
}

void loop() {
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    callback(packetSize);
  }
}

void callback(int packetSize) {
  Serial.printf("Received %d bytes from %s, port %d\n", packetSize, UDP.remoteIP().toString().c_str(), UDP.remotePort());
  int len = UDP.read(incomingPacket, INCOMING_PACKET_SIZE);
  if (len > 0) {
    incomingPacket[len] = 0;
  }
  Serial.printf("UDP packet contents: %s\n", incomingPacket);
  if (isdigit(incomingPacket[0])) {
    Serial.println("Drum hit!");
    //    parse_drum_hit();
  } else {
    Serial.println(F("Not a drum hit"));
  }
}
