#include <painlessMesh.h>

#define   MESH_SSID       "Meow"
#define   MESH_PASSWORD   "woof"
#define   MESH_PORT       5555
#define   UPDATE_PERIOD   10

painlessMesh  mesh;

String message = "";
uint32_t addr;

SimpleList<uint32_t> nodes;
Task serialService;

void setup() {
  Serial.begin(115200);

  //  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  serialService.set(UPDATE_PERIOD, 5, &serviceSerial);
  mesh.scheduler.addTask(serialService);
  serialService.enable();

  randomSeed(analogRead(A0));
}

void loop() {
  mesh.update();
}

void receivedCallback(uint32_t from, String & msg) {
  Serial.println(msg);
}

void serviceSerial() {
  while (Serial.available()) {
    message += char(Serial.read());
  }

  if (message != "") {
    if (message.charAt(0) == 'L') {
      int colon = message.indexOf(':');
      addr = message.substring(1, colon).toInt();
      message = message.substring(colon + 1);
      mesh.sendSingle(addr, message);
    } else if (isdigit(message.charAt(0))) {
      bool error = mesh.sendBroadcast(message);
      if (error) Serial.println("ErrorBroadcasting");
    }
    message = "";
  }
  serialService.setIterations(5);
}

void newConnectionCallback(uint32_t nodeId) {
}

void changedConnectionCallback() {
}

void nodeTimeAdjustedCallback(int32_t offset) {
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
}
