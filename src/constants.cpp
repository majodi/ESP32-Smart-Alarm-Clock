#include "constants.h"

char ttrbuf[1500] = {0};                                                        // text to read

// *** WiFi ***
WiFiManager wifiManager;
const char* ssid = "not set";                                             // not used with WiFi Manager
const char* password = "not set";
String ipaddress = "";

// *** HTTP ***
WiFiClient http;                                                          // http client
WiFiClientSecure https;
char *host = "ice1.somafm.com";
char *path = "/u80s-128-mp3";
int port = 80;
bool secureConnect = false;                                               // secure client (https) 
bool connected = false;                                                   // is connected or not
String postData;
char googleToken[200] = {0};

// *** Time ***
struct tm timeinfo;                                                       // timeinfo structure for NTP server
char dateCstr[11] = "2000-01-01";                                         // textual representation of internal date
char timeCstr[9] = "00:00:00";                                            // textual representation of internal time
char dayStartZuluCstr[25] = "2000-01-01T00:00:00.000Z";                   // start of day zulu time format
char dayEndZuluCstr[25] = "2000-01-01T00:00:00.000Z";                     // end of day zulu time format
char alarmSearchStartZuluCstr[25];                                        // start of alarm search zulu time format
char alarmSearchEndZuluCstr[25];                                          // end of alarm search zulu time format
bool timeValid = false;                                                   // is time valid (retrieved from NTP on last attempt)
bool dots = true;                                                         // show dots (seconds indicator)
int alarmHour;                                                            // alarm time
int alarmMinute;
bool alarmSet = false;                                                    // alarm status

// *** Timer ***
volatile int secTickCounter = 0;                                          // seconds tick counter (counts seconds since last handled)
int secondsCounter = 0;                                                   // counters since system boot
int minutesCounter = 0;
int hoursCounter = 0;
int daysCounter = 0;
hw_timer_t * timer = NULL;                                                // hardware timer
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;                    // for critical sections ISR variables

// *** TM1637 ***
TM1637Display display(TM1637_CLK, TM1637_DIO);                            // display

// *** Debug ***
bool logging = true;                                                      // controls Serial console log info on/off

// *** SPI ***
SemaphoreHandle_t SPISemaphore = NULL;                                    // semaphore for exclusive SPI usage

// *** VS1053 ***
uint8_t mp3IOBuffer[32];                                                  // IO buffer
int streamType = NO_ACTIVE_STREAM;                                        // no stream active
bool radioOnTTSEnd = false;                                               // schedule radio after finishing tts
