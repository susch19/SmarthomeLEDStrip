#include <vector>
#include <algorithm>
#include <sstream>
#include <LedStrip.hpp>
using namespace std;

LedStrip ledStrip;

void setup()
{
  Serial.begin(115200);
  ledStrip.setup(true);
  Serial.println("Firmware V3 " __DATE__);
  // mesh.setContainsRoot(true);
  // mesh.setDebugMsgTypes(ERROR | COMMUNICATION);
  // mesh.initOTA(typeOfNode);
  // mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_STA, 6); //,
  // mesh.onReceive(&receivedCallback);
  // mesh.onNewConnection(&newConnectionCallback);

  // // userScheduler.addTask(clockTask);
  // // clockTask.enable();
  // userScheduler.addTask(taskUpdateTemp);
  // taskUpdateTemp.enable();
}

void loop()
{
  ledStrip.loop();
}
