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
  setupWiFi();                                                            // connect to WiFi
  configTime(3600, 3600, "pool.ntp.org");                                 // set time server config for Dutch time
  setupPins();                                                            // initialise pins
  setupTimer();                                                           // setup timer for timer_events
  blink(25);                                                              // signal end of setup
}

void loop() {
  handleTimer();
}
