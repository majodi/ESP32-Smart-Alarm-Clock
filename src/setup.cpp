#include "setup.h"

void setupPins() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void setupWiFi() {
  Serial.begin(115200);
  wifiManager.autoConnect("NickStickIoT");
  ipaddress = WiFi.localIP().toString();
  dbgPrint ("IP = %s", ipaddress.c_str());
}

void setupTime() {
  configTime(2 * 3600, 0 * 3600, "0.nl.pool.ntp.org", "1.nl.pool.ntp.org", "2.nl.pool.ntp.org");
  // dst parameter doesn't work... - make time adjust setting and reset time as menu option
}

