#include "setup.h"

void setup() {
  setupPins();                                                            // initialise pins
  logging = true;                                                         // turn on logging (serial and telnet)
  Serial.begin(115200);
  Serial.println();
  Serial.println("*********************************");
  Serial.println("* ESP32 Smart Alarm Clock v1.01 *");
  Serial.println("*********************************");
  slog("* running on ESP Core %d (%d MHz). Free Memory: %d.",
    xPortGetCoreID(),
    ESP.getCpuFreqMHz(),
    ESP.getFreeHeap());
  Serial.println();
  setupNeoPixel();                                                        // setup neoPixels (needed for life signs, so at start)
  setupWiFi();                                                            // connect to WiFi
  setupRemoteDebug();                                                     // setup remote debug
  setupOTA();                                                             // setup over the air updates
  configTime(1 * 3600, 0 * 3600, "pool.ntp.org");                         // set time server config for Dutch time
  syncTime();                                                             // first time sync
  setupTimer();                                                           // setup timer for timer_events
  setupSPI();                                                             // setup SPI with VS1053
  setupTouch();                                                           // setup touch(pad)
  setupBlynk();                                                           // setup Blynk
  display.setBrightness(0x0f);                                            // turn on display
  pollAlarmTimeSetting(false);                                            // see if the alarm time can be found and set alarm (repeat each hour for changes)
  blink(30);                                                              // signal end of setup

  // *** tests and comments ***

  // No vs1053 patches loaded...
  // No real buffering (todo: FreeRTOS queues)
  // Single processor (todo: use second core)
  // no certificate checking with https (security)
  // todo: use wifimanager extra parameters for radio preset

  // Debug info - check if vs1053 is alive
  // slog("VS1053_REG_MODE   = %x (HEX)", vsReadRegister(VS1053_REG_MODE));
  // slog("VS1053_REG_STATUS = %x (HEX)", vsReadRegister(VS1053_REG_STATUS));
  // slog("VS1053_REG_CLOCKF = %x (HEX)", vsReadRegister(VS1053_REG_CLOCKF));
  // slog("VS1053_REG_AUDATA = %x (HEX)", vsReadRegister(VS1053_REG_AUDATA));
  // slog("VS1053_REG_VOLUME = %x (HEX)", vsReadRegister(VS1053_REG_VOLUME));

  // internet radio test
  // vsSineTest();                                                           // quick test for sound (debug)
  // host = "ice1.somafm.com";
  // path = "/u80s-128-mp3";
  // httpConnect(host, port, false);                                         // connect to host
  // httpGetRequest(host, path);                                             // request stream
  // streamType = MP3_STREAM;                                                // set stream type to MP3 data

  // TTS test
  // host = GCLOUD_TTS_HOST;
  // path = GCLOUD_TTS_PATH GCLOUD_KEY;
  // httpConnect(host, port, true);                                          // connect to host
  // httpPostRequest(host, path, TTSPostData("dit is een test."), "application/json;charset=UTF-8"); // request stream
  // streamType = TTS_STREAM;                                                // set stream type to TTS data

  // read latest news, weather and calendar - test
  // prepareTTSInfo("ik hoop dat u lekker heeft geslapen. <break strength=\"strong\"/>", 3);
  // requestTTSMP3Data();

}

void loop() {
  ArduinoOTA.handle();
  handleBlynk();
  handleNeoPixel();
  handleStream();
  handleTouch();
  handleTimer();
  remoteDebug.handle();
}
