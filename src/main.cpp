#include <LedStrip.hpp>

LedStrip ledStrip;

void setup()
{
  Serial.begin(115200);
  ledStrip.setup("ledstri", 26, true);
  Serial.printf("Firmware V%d " __DATE__ "\n", ledStrip.firmwareVersion);
}

void loop()
{
  ledStrip.loop();
}