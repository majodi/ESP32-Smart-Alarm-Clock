#ifndef CONSTANTS_H
#define CONSTANTS_H 1

#include <Arduino.h>
#include <SPI.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include "private.h"

// *** MCU pin layout ***
#define VS1053_SCK 18
#define VS1053_MISO 19
#define VS1053_MOSI 23
#define VS1053_xDCS 32
#define VS1053_xCS 5
#define VS1053_DREQ 4

// *** WiFi ***
extern const char* ssid;
extern const char* password;
extern String ipaddress;
extern WiFiManager wifiManager;

// *** HTTP ***
#define GCLOUD_TTS_HOST "texttospeech.googleapis.com"
#define GCLOUD_TTS_PATH "/v1/text:synthesize?key="
extern WiFiClient http;
extern WiFiClientSecure https;
extern char *host;
extern char *path;
extern int port;
extern bool secureConnect;
extern bool connected;
extern String postData;

// *** Time ***
extern struct tm timeinfo;
extern char timeCstr[9];
extern bool timeValid;
extern int timeSyncRetries;

// *** Timer ***
extern volatile int secTickCounter;
extern int secondsCounter;
extern int minutesCounter;
extern int hoursCounter;
extern int daysCounter;
extern hw_timer_t* timer;
extern portMUX_TYPE timerMux;

// *** Debug ***
extern bool logging;

// *** SPI ***
extern SemaphoreHandle_t SPISemaphore;

// *** VS1053 ***
extern uint8_t mp3IOBuffer[32];

#define VS1053_REG_MODE  0x00
#define VS1053_REG_STATUS 0x01
#define VS1053_REG_BASS 0x02
#define VS1053_REG_CLOCKF 0x03
#define VS1053_REG_DECODETIME 0x04
#define VS1053_REG_AUDATA 0x05
#define VS1053_REG_WRAM 0x06
#define VS1053_REG_WRAMADDR 0x07
#define VS1053_REG_HDAT0 0x08
#define VS1053_REG_HDAT1 0x09
#define VS1053_REG_VOLUME 0x0B

#define VS1053_MODE_SM_DIFF 0x0001
#define VS1053_MODE_SM_LAYER12 0x0002
#define VS1053_MODE_SM_RESET 0x0004
#define VS1053_MODE_SM_CANCEL 0x0008
#define VS1053_MODE_SM_EARSPKLO 0x0010
#define VS1053_MODE_SM_TESTS 0x0020
#define VS1053_MODE_SM_STREAM 0x0040
#define VS1053_MODE_SM_SDINEW 0x0800
#define VS1053_MODE_SM_ADPCM 0x1000
#define VS1053_MODE_SM_LINE1 0x4000
#define VS1053_MODE_SM_CLKRANGE 0x8000

#endif
