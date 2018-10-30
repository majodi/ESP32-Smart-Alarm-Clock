#include "setup.h"

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("***************************");
  Serial.println("* ESP32 Smart Alarm Clock *");
  Serial.println("***************************");
  Serial.println();
  slog("running on Core %d (%d MHz). Free Memory: %d.",
    xPortGetCoreID(),
    ESP.getCpuFreqMHz(),
    ESP.getFreeHeap());
  configTime(3600, 3600, "pool.ntp.org");                                 // Dutch time
  setupPins();
  setupTimer();                                                           // setup timer for timer_events
  blink(25);                                                              // signal end of setup
}

void loop() {
  // slog("loop");
  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(200);
  // digitalWrite(LED_BUILTIN, LOW);
  // delay(200);    
}
