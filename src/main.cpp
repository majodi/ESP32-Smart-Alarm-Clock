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
  syncTime();                                                             // first time sync (later every hour)
  setupPins();                                                            // initialise pins
  setupTimer();                                                           // setup timer for timer_events
  setupSPI();                                                             // setup SPI with VS1053
  blink(50);                                                              // signal end of setup

  // No vs1053 patches loaded...
  // No real buffering (todo: FreeRTOS queues)
  // Single processor (todo: use second core)
  // no certificate checking with https (security)
  // add SSML support for better/more control over tts

  // Debug info - check if vs1053 is alive
  slog("VS1053_REG_MODE   = %x (HEX)", vsReadRegister(VS1053_REG_MODE));
  slog("VS1053_REG_STATUS = %x (HEX)", vsReadRegister(VS1053_REG_STATUS));
  slog("VS1053_REG_CLOCKF = %x (HEX)", vsReadRegister(VS1053_REG_CLOCKF));
  slog("VS1053_REG_AUDATA = %x (HEX)", vsReadRegister(VS1053_REG_AUDATA));
  slog("VS1053_REG_VOLUME = %x (HEX)", vsReadRegister(VS1053_REG_VOLUME));

  // internet radio test
  // vsSineTest();                                                           // quick test for sound (debug)
  // host = "ice1.somafm.com";
  // path = "/u80s-128-mp3";
  // httpConnect(host, port, false);                                         // connect to host
  // httpGetRequest(host, path);                                             // request stream

  // TTS test
  // host = GCLOUD_TTS_HOST;
  // path = GCLOUD_TTS_PATH GCLOUD_KEY;
  // httpConnect(host, port, true);                                          // connect to host
  // httpPostRequest(host, path, TTSPostData("dit is een test."), "application/json;charset=UTF-8"); // request stream

  // read latest news, weather and calendar test
  prepareTTSInfo("Goedemorgen, ik hoop dat u lekker heeft geslapen. ", 3);
  requestTTSMP3Data();

}

void loop() {
  // transferAvailableMP3Data(); // internet radio test
  transferAvailableTTSMP3Data(); // TTS test, news test
  handleTimer();
}
