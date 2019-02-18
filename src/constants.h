#ifndef CONSTANTS_H
#define CONSTANTS_H 1

#include <Arduino.h>
#include <queue>
#include <SPI.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <TM1637Display.h>
#include <RemoteDebug.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include "private.h"

extern bool boot;
extern bool initialized;

// *** MCU pin layout ***
#define VS1053_SCK 18       // geel
#define VS1053_MISO 19      // oranje
#define VS1053_MOSI 23      // rood
#define VS1053_xDCS 32      // bruin
#define VS1053_xCS 5        // zwart
#define VS1053_DREQ 4       // wit
// VS1053_xRST --> ESP32_EN - grijs
// VS1053_5V --> ESP32_VIN - rood
// VS1053_GND --> ESP32_GND - zwart
#define TM1637_CLK 16
#define TM1637_DIO 17
// TM1637_5V --> ESP32_VIN - rood (split of vanaf VS1053 dubbele 5V)
// TM1637_GND --> ESP32_GND - zwart (split)
#define TOUCH_PAD 15
#define NEO_PIXEL_PIN 27
#define MUTE_PIN 22
#define RADAR_PIN 25

// *** WiFi ***
extern const char* ssid;
extern const char* password;
extern String ipaddress;
extern WiFiManager wifiManager;

// *** HTTP ***
#define GCLOUD_TTS_HOST "texttospeech.googleapis.com"
#define GCLOUD_TTS_PATH "/v1/text:synthesize?key="
#define NEWS_API_HOST "newsapi.org"
#define NEWS_API_PATH "/v2/top-headlines?sources=rtl-nieuws&apiKey="
#define WEATHER_API_HOST "weerlive.nl"
#define WEATHER_API_PATH "/api/json-data-10min.php?locatie="
#define GOOGLE_APIS_HOST "www.googleapis.com"
#define GOOGLE_OAUTH_PATH "/oauth2/v4/token"
#define GOOGLE_CALENDAR_PATH "/calendar/v3/calendars/primary/events"
extern WiFiClient http;
extern WiFiClientSecure https;
extern char *host;
extern char *path;
extern int port;
extern bool secureConnect;
extern bool connected;
extern String postData;
extern char googleToken[200];

// *** Time ***
extern struct tm timeinfo;
extern char dateCstr[11];
extern char timeCstr[9];
extern char dayStartZuluCstr[25];
extern char dayEndZuluCstr[25];
extern bool timeValid;
extern bool dots;

// *** Timer ***
extern volatile int secTickCounter;
extern int secondsCounter;
extern int minutesCounter;
extern int hoursCounter;
extern int daysCounter;
extern hw_timer_t* timer;
extern portMUX_TYPE timerMux;

// *** TM1637 ***
extern TM1637Display display;
extern const uint8_t TM1637_tijd[];
extern const uint8_t TM1637_niet[];
extern const uint8_t TM1637_fout[];
extern const uint8_t TM1637_info[];

// *** NeoPixels ***
extern bool animationsEnded;
extern const uint8_t PixelPin;
extern const uint16_t PixelCount;
extern NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip;
struct animationStateStruct
{
    RgbColor StartingColor;
    RgbColor EndingColor;
    uint16_t IndexPixel;
    void (*postFunction)(int lastDirection, int lastPosition, unsigned long runUntil);
    int startPixel;
    int currentDirection;
    int currentHue;
    unsigned long runUntil;
};
extern const uint16_t animCount;
extern animationStateStruct animationState[];
extern NeoPixelAnimator animations;
extern NeoGamma<NeoGammaTableMethod> colorGamma;

// *** Debug ***
extern bool logging;

// *** SPI ***
extern SemaphoreHandle_t SPISemaphore;

// *** VS1053 ***
extern bool radioPlaying;
extern int radioPlayTime;
extern int radioTimePlayed;
extern bool TTSPlaying;
extern bool TTSEndRequest;
extern uint8_t volume;
extern uint8_t volumeDesired;
extern uint8_t mp3IOBuffer[32];
extern int streamType;

#define VOL_LOW 60
#define VOL_MED 35
#define VOL_HIGH 25
#define NO_ACTIVE_STREAM  0
#define MP3_STREAM  1
#define TTS_STREAM  2

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

// *** TTR (Text To Read) ***
#define TTR_MAXLEN 1500
#define TTR_LEFT sizeof(ttrbuf) - strlen(ttrbuf) - 1
#define TTR_MAXNEWSITEMLEN 320
#define SSML_OPENING "ik hoop dat u lekker heeft geslapen. <break strength=\"strong\"/>"
#define SSML_PAUSE ".<break time=\"500ms\"/>"
#define SSML_MORNING "Goedemorgen."
#define SSML_AFTERNOON "Goedemiddag."
#define SSML_EVENING "Goedenavond."
#define SSML_DATE_ANOUNCEMENT "Het is vandaag "
#define SSML_NEWS_INTRO "Het actuele nieuws van vandaag. " SSML_PAUSE
#define SSML_WEATHER_INTRO "Het weer voor vandaag. " SSML_PAUSE
#define SSML_CALENDAR_INTRO1 "Je agenda voor vandaag. " SSML_PAUSE
extern char ttrbuf[TTR_MAXLEN];
extern char alternativeOpening[128];

// *** Touch ***
extern uint16_t touchThreshold;
extern volatile bool touchDetected;
extern bool touchOn;
extern int touchHoldCount;
extern bool touchHold;
extern int lastTouched;
extern bool reArmTouch;

// *** Alarm ***
#define ALARM_PENDING 0
#define ALARM_ACTIVE 1
#define ALARM_SNOOZED 3
#define AS_NONE 0
#define AS_ANIMATION 1
#define AS_WAKE_RADIO 2
#define AS_INFO_TTS 3
#define AS_CONT_RADIO 4
extern int currentAlarmSection;
extern int lastSectionSwitch;
extern bool alarmAnimation;
extern bool alarmWakeRadio;
extern char wakeRadioHost[50];
extern int wakeRadioPort;
extern char wakeRadioPath[50];
extern int wakeRadioMinutes;
extern bool alarmInfoTTS;
extern bool alarmContRadio;
extern char contRadioHost[50];
extern int contRadioPort;
extern char contRadioPath[50];
extern char alarmSearchStartZuluCstr[25];
extern char alarmSearchEndZuluCstr[25];
extern int alarmHour;
extern int alarmMinute;
extern bool alarmSet;
extern int alarmState;
extern time_t lastForcedPollAlarmTime;
extern time_t snoozeStarted;
extern int snoozeMinutes;


// *** Radar ***
#define STD_MOVEMENT_DEBOUNCE_TIME 10
extern int movementDebounceTime;
extern std::queue<int> movementQ;
extern int movementsThisHour;
extern bool movementDetected;
extern int movementDebounce;

// *** Blynk ***
#define BLYNK_SERVER_INITIALIZED 2
extern char blynkAuth[];

// *** remote debug ***
extern RemoteDebug remoteDebug;

#endif
