#include <LedStrip.hpp>

LedStrip ledStrip;

void setup()
{
  Serial.begin(115200);
  ledStrip.setup("ledstrip", 23, true);
  Serial.printf("Firmware V%d " __DATE__ "\n", ledStrip.firmwareVersion);
}

void loop()
{
  ledStrip.loop();
}