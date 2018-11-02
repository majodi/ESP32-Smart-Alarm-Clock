#include "constants.h"

// *** WiFi ***
WiFiManager wifiManager;
const char* ssid = "not set";                                             // not used with WiFi Manager
const char* password = "not set";
String ipaddress = "";

// *** HTTP ***
WiFiClient http;                                                          // http client
char *host = "ice1.somafm.com";
char *path = "/u80s-128-mp3";
int port = 80;
bool connected = false;                                                   // is connected or not

// *** Time ***
struct tm timeinfo;                                                       // timeinfo structure for NTP server
char timeCstr[9] = "00:00:00";                                            // textual representation of internal time
bool timeValid = false;                                                   // is time valid (retrieved from NTP on last attempt)
int timeSyncRetries = 0;

// *** Timer ***
volatile int secTickCounter = 0;                                          // seconds tick counter (counts seconds since last handled)
int secondsCounter = 0;                                                   // counters since system boot
int minutesCounter = 0;
int hoursCounter = 0;
int daysCounter = 0;
hw_timer_t * timer = NULL;                                                // hardware timer
portMUX_TYPE timerMux  = portMUX_INITIALIZER_UNLOCKED;                    // for critical sections ISR variables

// *** Debug ***
bool logging = true;                                                      // controls Serial console log info on/off

// *** SPI ***
SemaphoreHandle_t SPISemaphore = NULL;                                    // semaphore for exclusive SPI usage

// *** VS1053 ***
uint8_t mp3IOBuffer[32];                                                  // IO buffer
