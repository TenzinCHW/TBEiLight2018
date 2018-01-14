#include <painlessMesh.h>

#define   LED             0

#define   MESH_SSID       "Meow"
#define   MESH_PASSWORD   "woof"
#define   MESH_PORT       5555
#define   UPDATE_PERIOD   100

painlessMesh  mesh;
SimpleList<uint32_t> nodes;
uint32_t root = 4843090;

float x;
float y;
float x_src;
float y_src;
float wl = 20.0;
float damp = 0.5;
int drum_loud;
uint32_t drum_time;
uint32_t chip_id;

String message;
String reply;

// Task to update light state
Task update_lights;


void setup() {
  Serial.begin(115200);
  Serial.println(ESP.getChipId());

  pinMode(LED, OUTPUT);

  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  update_lights.set(UPDATE_PERIOD, 5, []() {
    float dist = sqrt(pow((x - x_src), 2.0) + pow((y - y_src), 2.0));
    float delta_t = (millis() - drum_time) / 1000;
    float dist_from_wave = abs(dist - wl * delta_t);
    uint16_t brightness = (int)(drum_loud * (wl * delta_t - dist_from_wave) * pow(2.71, (-damp * dist_from_wave * pow(delta_t, 3.0))) / 2.71 / dist);
    if (brightness < 0) {
      brightness = 0;
    }
    analogWrite(LED, brightness);

    if (update_lights.isLastIteration()) {
      // Never stop.
      update_lights.setIterations(5);
    }
  });
  mesh.scheduler.addTask(update_lights);
  update_lights.enable();

  message[0] = 'N'; // Indicate that there is nothing to read

  set_location();
  randomSeed(analogRead(A0));
}

void loop() {
  mesh.update();
}

void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  message = msg;
  parse_drum_hit();
}

void parse_drum_hit() { // TODO: This still doesn't take care of multiple hits
  if (!isdigit(message.charAt(0))) {
    return;
  }
  int comma = message.indexOf(',');
  int colon = message.indexOf(':');
  x_src = message.substring(0, comma).toFloat();
  y_src = message.substring(comma + 1, colon).toFloat();
  comma = message.indexOf(':', colon + 1);
  drum_loud = message.substring(colon + 1, comma).toInt();
  drum_time = mesh.getNodeTime();
  message[0] = 'N';
}

void set_location() {
  chip_id = ESP.getChipId();

  // Send the chip_id to root node, which will reply with the "x,y" value of this node's location
  reply = "c_id:" + chip_id;
  Serial.println(reply);
  while (message.charAt(0) == 'N') {
    mesh.sendSingle(root, reply);
  }

  if (message.charAt(0) == 'L') {
    message = message.substring(1);
  }

  // Split the incoming message and set x and y value for this node
  int comma = message.indexOf(',');
  x = message.substring(0, comma).toFloat();
  y = message.substring(comma + 1).toFloat();
  reply = "ok" + chip_id;
  mesh.sendSingle(root, reply);
  message[0] = 'N';
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
