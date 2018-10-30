#include "setup.h"

void setupPins() {                                                        // initialise pins
  pinMode(LED_BUILTIN, OUTPUT);
}

void setupWiFi() {
  wifiManager.autoConnect("NickStickIoT");                                // connect or set AP mode with NickStickIoT ssid
  ipaddress = WiFi.localIP().toString();                                  // set ip address string for general purpose use
  slog("ip address = %s", ipaddress.c_str());                             // log ip address
}

void IRAM_ATTR timerISR() {
  portENTER_CRITICAL_ISR(&timerMux);                                      // critical section for writing shared var
  secTickCounter++;
  portEXIT_CRITICAL_ISR(&timerMux); 
}

void setupTimer() {
  // timer = timerBegin(0, (uint16_t) ESP.getCpuFreqMHz(), true);
  timer = timerBegin(0, 80, true);                                        // base signal used for ESP counters 80 MHz, so use prescaler of 80 for microseconds
  timerAttachInterrupt(timer, &timerISR, true);                           // attach interrupt
  timerAlarmWrite(timer, 1000000, true);                                  // set alarm
  timerAlarmEnable(timer);                                                // and enable it
}