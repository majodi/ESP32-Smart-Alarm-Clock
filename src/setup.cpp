#include "setup.h"

void setupPins() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void setupWiFi() {
  wifiManager.autoConnect("NickStickIoT");
  ipaddress = WiFi.localIP().toString();
  slog("IP = %s", ipaddress.c_str());
}

void IRAM_ATTR timerISR() {
  portENTER_CRITICAL_ISR(&timerMux);
  secTickCounter++;
  portEXIT_CRITICAL_ISR(&timerMux); 
}

void setupTimer() {
  timer = timerBegin(0, ESP.getCpuFreqMHz(), true);
  timerAttachInterrupt(timer, &timerISR, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}