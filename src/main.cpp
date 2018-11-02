#include "setup.h"

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("***************************");
  Serial.println("* ESP32 Smart Alarm Clock *");
  Serial.println("***************************");
  Serial.println();
  slog("running on ESP Core %d (%d MHz). Free Memory: %d.",
    xPortGetCoreID(),
    ESP.getCpuFreqMHz(),
    ESP.getFreeHeap());
  setupWiFi();                                                            // connect to WiFi
  configTime(1 * 3600, 0 * 3600, "pool.ntp.org");                         // set time server config for Dutch time
  setupPins();                                                            // initialise pins
  setupTimer();                                                           // setup timer for timer_events
  setupSPI();                                                             // setup SPI with VS1053
  blink(50);                                                              // signal end of setup

  // No Patches loaded...
  // No real buffering (todo: FreeRTOS queues)
  // Single processor (todo: use second core)

  // Debug info - check if vs1053 is alive
  slog("VS1053_REG_MODE   = %x (HEX)", vsReadRegister(VS1053_REG_MODE));
  slog("VS1053_REG_STATUS = %x (HEX)", vsReadRegister(VS1053_REG_STATUS));
  slog("VS1053_REG_CLOCKF = %x (HEX)", vsReadRegister(VS1053_REG_CLOCKF));
  slog("VS1053_REG_AUDATA = %x (HEX)", vsReadRegister(VS1053_REG_AUDATA));
  slog("VS1053_REG_VOLUME = %x (HEX)", vsReadRegister(VS1053_REG_VOLUME));

  vsSineTest();                                                           // quick test for sound (debug)
  host = "ice1.somafm.com";
  path = "/u80s-128-mp3";
  httpConnect(host, port);                                                // connect to host
  httpGetStream(host, path);                                              // request stream
}

void loop() {
  transferAvailableMP3Data();
  handleTimer();
}
