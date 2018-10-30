#ifndef CONSTANTS_H
#define CONSTANTS_H 1

#include <Arduino.h>
#include <WiFiManager.h>

// **********************
// *** MCU pin layout ***
// **********************
#define vs_SCK D18
#define vs_MISO D19
#define vs_MOSI D23
#define vs_xDCS D32
#define vs_xCS D5
#define vs_DREQ D4

// ************
// *** WiFi ***
// ************
extern const char* ssid;
extern const char* password;
extern String ipaddress;
extern WiFiManager wifiManager;

// ************
// *** Time ***
// ************
extern struct tm timeinfo;
extern char timeCstr[9];
extern bool timeValid;
extern int timeSyncRetries;

// *************
// *** Timer ***
// *************
extern volatile int secTickCounter;
extern int secondsCounter;
extern int minutesCounter;
extern int hoursCounter;
extern int daysCounter;
extern hw_timer_t* timer;
extern portMUX_TYPE timerMux;

// *************
// *** Debug ***
// *************
extern bool logging;

#endif
