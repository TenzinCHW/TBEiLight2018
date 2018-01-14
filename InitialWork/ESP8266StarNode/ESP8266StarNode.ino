#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#define INCOMING_PACKET_SIZE 255

const char* ssid = "NetwerkzLit";
const char* password = "iLoveNetworks";

WiFiUDP UDP;
unsigned int local_UDP_port = 4210;
unsigned int root_UDP_port = 5010;
char incoming_packet[INCOMING_PACKET_SIZE];

IPAddress server_IP;          // NTP server address: root node
const char* server_name = "192.168.0.1";  // IP of root
const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message
byte NTP_buffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing NTP packets
uint32_t current_time = 0;

float x;
float y;
float x_src;
float y_src;
float wl = 20.0;
float damp = 0.5;
int drum_loud;
uint32_t drum_time;
uint32_t chip_id;
uint8_t LED = 0;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  pinMode(LED, OUTPUT);

  UDP.begin(local_UDP_port);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(F("Waiting for WiFi to connect"));
  }

  Serial.println(F("Connection established!"));
  Serial.print(F("IP address:\t"));
  Serial.println(WiFi.localIP());

  set_NTP_time();
  set_location();
}

void loop() {
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    callback(packetSize);
  }
  update_lights();
}

void callback(int packetSize) {
  Serial.printf("Received %d bytes from %s, port %d\n", packetSize, UDP.remoteIP().toString().c_str(), UDP.remotePort());
  int len = UDP.read(incoming_packet, INCOMING_PACKET_SIZE);
  if (len > 0) {
    incoming_packet[len] = 0;
  }
  Serial.printf("UDP packet contents: %s\n", incoming_packet);
  if (isdigit(incoming_packet[0])) {
    parse_drum_hit();
  } else {
    Serial.println(F("Not a drum hit"));
  }
}

void parse_drum_hit() { // TODO: This still doesn't take care of multiple hits
  char* drum_loc = strtok(incoming_packet, ":");
  // Split the command in two values
  char* separator = strchr(drum_loc, ',');
  if (separator != 0) {
    // Split the string in 2: replace ',' with 0
    *separator = 0;
    x_src = atof(drum_loc);
    ++separator;
    y_src = atof(separator);
  }
  drum_loud = atoi(strtok(0, ":"));
  drum_time = millis();
//  drum_time = atoi(strtok(0, ":")); // For use in actual project
}

void update_lights() {  // Simplified form that only takes into account the latest drum hit
  float dist = sqrt(pow((x - x_src), 2.0) + pow((y - y_src), 2.0));
  float delta_t = (millis() - drum_time) / 1000;
  float dist_from_wave = abs(dist - wl * delta_t);
  uint16_t brightness = (int)(drum_loud * (wl * delta_t - dist_from_wave) * pow(2.71, (-damp * dist_from_wave * pow(delta_t, 3.0))) / 2.71 / dist);
  if (brightness < 0) {
    brightness = 0;
  }
  analogWrite(0, brightness);
}

void send_UDP(char* message, IPAddress IP) {
  UDP.beginPacket(IP, root_UDP_port);
  UDP.write(message);
  UDP.endPacket();
}

void set_location() {
  chip_id = ESP.getChipId();

  // Send the chip_id to root node, which will reply with the "x,y" value of this node's location
  char c_id_msg[16];  // This just nicely fits the C string for "c_id:<10digitnum>"
  strncpy(c_id_msg, "c_id:", sizeof("c_id:"));
  char c_id[11];  // Max size of uint32_t number is 4294967295, 10 digits so 11 char spaces needed
  itoa(chip_id, c_id, 10);
  strcat(c_id_msg, c_id);
  Serial.println(c_id_msg);
  while (UDP.parsePacket() == 0) {  // Wait for root node to reply
    send_UDP(c_id_msg, server_IP);
    delay(100);
  }
  int len = UDP.read(incoming_packet, INCOMING_PACKET_SIZE);
  incoming_packet[len] = 0;
  if (incoming_packet[0] == 'L') {
    memmove(incoming_packet, incoming_packet + 1, strlen(incoming_packet));
  }
  
  // Split the incoming message and set x and y value for this node
  char* separator = strchr(incoming_packet, ',');
  *separator = 0;
  x = atof(incoming_packet);
  ++separator;
  y = atof(separator);
  send_UDP("ok", server_IP);
}

// Sets current_time to be NTP time to sync
void set_NTP_time() {
  if (!WiFi.hostByName(server_name, server_IP)) { // Get the IP address of the NTP server
    Serial.println(F("DNS lookup failed. Rebooting."));
    Serial.flush();
    ESP.reset();
  }
  Serial.print(F("Time server IP:\t"));
  Serial.println(server_IP);

  Serial.println(F("\r\nSending NTP request ..."));
  sendNTPpacket(server_IP);
  while (current_time == 0) current_time = get_time();
  Serial.println(current_time);
}

uint32_t get_time() {
  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(NTP_buffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTP_time = (NTP_buffer[40] << 24) | (NTP_buffer[41] << 16) | (NTP_buffer[42] << 8) | NTP_buffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventy_years = 2208988800UL;
  // subtract seventy years:
  uint32_t UNIX_time = NTP_time - seventy_years;
  return UNIX_time;
}

void sendNTPpacket(IPAddress& address) {
  memset(NTP_buffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTP_buffer[0] = 0b11100011;   // LI, Version, Mode
  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTP_buffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}
