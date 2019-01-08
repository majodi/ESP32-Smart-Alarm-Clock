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
bool timeValid = false;                                                   // is time valid (retrieved from NTP on last attempt)
bool dots = true;                                                         // show dots (seconds indicator)

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
const uint8_t TM1637_tijd[] = {120, 6, 30, 94};                           // display word "tijd"
const uint8_t TM1637_niet[] = {55, 6, 121, 120};                          // display word "niet"
const uint8_t TM1637_fout[] = {113, 126, 62, 120};                        // display word "fout"
const uint8_t TM1637_info[] = {6, 55, 113, 126};                          // display word "info"

// *** NeoPixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, NEO_PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// *** Debug ***
bool logging = true;                                                      // controls Serial console log info on/off

// *** SPI ***
SemaphoreHandle_t SPISemaphore = NULL;                                    // semaphore for exclusive SPI usage

// *** VS1053 ***
uint8_t volume = VOL_LOW;                                                 // volume setting (lower is louder, initial LOW)
uint8_t volumeDesired = VOL_LOW;                                          // desired volume setting (when needed will adjust volume in steps slowly every second)
uint8_t mp3IOBuffer[32];                                                  // IO buffer
int streamType = NO_ACTIVE_STREAM;                                        // no stream active
bool radioOnTTSEnd = false;                                               // schedule radio after finishing tts

// *** Touch ***
uint16_t touchThreshold = 45;                                             // touch sensitivity
volatile bool touchDetected = false;                                      // touch detection flag is false

// *** Alarm ***
int alarmState = ALARM_PENDING;                                           // assume waiting for alarm
char alarmSearchStartZuluCstr[25];                                        // start of alarm search zulu time format
char alarmSearchEndZuluCstr[25];                                          // end of alarm search zulu time format
int alarmHour;                                                            // alarm time
int alarmMinute;
bool alarmSet = false;                                                    // alarm status
time_t lastForcedPollAlarmTime = 0;                                       // reset time of last forced polling of alarmTime setting
time_t snoozeStarted = 0;                                                 // when snooze started

// *** Radar ***
bool movementDetected = false;                                            // movement detected
int movementDebounce = MOVEMENT_DEBOUNCE_TIME;                            // seconds before arming movement radar again after movement detected
