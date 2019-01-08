#include "setup.h"

void setupPins() {                                                        // initialise pins
  // general
  pinMode(LED_BUILTIN,       OUTPUT);                                     // signal/debug purpose
  // radar
  pinMode(RADAR_PIN,         INPUT);
  // PAM8403
  pinMode(MUTE_PIN,          OUTPUT);                                     // Mute sound
  digitalWrite (MUTE_PIN,    LOW);                                        // start muted --> muted = LOW, not muted = HIGH (can be left floating due to internal pull-up)   
  // VS1053
  pinMode(VS1053_xDCS,       OUTPUT);                                     // chip select SDI (Serial Data Interface)
  pinMode(VS1053_xCS,        OUTPUT);                                     // chip select SCI (Serial Control Interface)
  pinMode(VS1053_DREQ,       INPUT);                                      // data request
  digitalWrite (VS1053_xDCS, HIGH);                                       // chip select is Active LOW
  digitalWrite (VS1053_xCS,  HIGH);
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

void IRAM_ATTR touchISR() {
  portENTER_CRITICAL_ISR(&timerMux);                                      // critical section for writing shared var
  touchDetected = true;
  portEXIT_CRITICAL_ISR(&timerMux); 
}

void setupTimer() {
  timer = timerBegin(0, 80, true);                                        // base signal used for ESP counters 80 MHz, so use prescaler of 80 for microseconds
  timerAttachInterrupt(timer, &timerISR, true);                           // attach interrupt
  timerAlarmWrite(timer, 1000000, true);                                  // set alarm
  timerAlarmEnable(timer);                                                // and enable it
}

void setupTouch() {
  touchAttachInterrupt(TOUCH_PAD, touchISR, touchThreshold);
}

void setupSPI() {
  // *** for now only one SPI slave (VS1053)
  SPISemaphore = xSemaphoreCreateMutex();                                 // create SPI Semaphore for exclusive use
  SPI.begin (VS1053_SCK, VS1053_MISO, VS1053_MOSI);                       // init SPI
  vsReset();
}
