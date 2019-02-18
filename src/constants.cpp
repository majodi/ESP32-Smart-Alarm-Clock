#include "constants.h"

bool boot = true;                                                         // initially boot is true (we are booting)
bool initialized = false;                                                 // initialized true when synced with server

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
struct tm timeinfo;                                                       // timeinfo structure
char dateCstr[11] = "2000-01-01";                                         // textual representation of internal date
char timeCstr[9] = "00:00:00";                                            // textual representation of internal time
char dayStartZuluCstr[25] = "2000-01-01T00:00:00.000Z";                   // start of day zulu time format
char dayEndZuluCstr[25] = "2000-01-01T00:00:00.000Z";                     // end of day zulu time format
bool timeValid = false;                                                   // is time valid
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
const uint8_t TM1637_fout[] = {113, 63, 62, 120};                         // display word "fout"
const uint8_t TM1637_info[] = {6, 55, 113, 63};                           // display word "info"

// *** NeoPixel
bool animationsEnded = true;                                              // all animations ended with clean-up
const uint8_t PixelPin = NEO_PIXEL_PIN;                                   // set pin variable
const uint16_t PixelCount = 8;                                            // set count variable
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin); // create instance
const uint16_t animCount = 12;                                            // max nr of animations
animationStateStruct animationState[animCount];                           // animation state array
NeoPixelAnimator animations(animCount);                                   // animation manager
NeoGamma<NeoGammaTableMethod> colorGamma;                                 // gamma correction object

// *** Debug ***
bool logging = false;                                                     // controls Serial console log info on/off

// *** SPI ***
SemaphoreHandle_t SPISemaphore = NULL;                                    // semaphore for exclusive SPI usage

// *** VS1053 ***
bool radioPlaying = false;                                                // is radio playing?
int radioPlayTime = 0;                                                    // duration to play (in seconds, 0 is continues)
int radioTimePlayed = 0;                                                  // seconds radio played
bool TTSPlaying = false;                                                  // is TTS stream playing
bool TTSEndRequest = false;                                               // request to end TTS stream
uint8_t volume = VOL_LOW;                                                 // volume setting (lower is louder, initial LOW)
uint8_t volumeDesired = VOL_LOW;                                          // desired volume setting (when needed will adjust volume in steps slowly every second)
uint8_t mp3IOBuffer[32];                                                  // IO buffer
int streamType = NO_ACTIVE_STREAM;                                        // no stream active

// *** TTR (Text To Read) ***
char ttrbuf[1500] = {0};                                                  // text to read
char alternativeOpening[128] = {0};                                       // alternative opening

// *** Touch ***
uint16_t touchThreshold = 35;                                             // touch sensitivity
volatile bool touchDetected = false;                                      // touch detection flag is false
bool touchOn = false;                                                     // touchState (on after detected, off when on next handle cycle no longer detected)
int touchHoldCount = 0;                                                   // nr seconds touch was continues
bool touchHold = false;                                                   // touch helt for more then 3 consecutive seconds
int lastTouched = 0;                                                      // last time touch was handled
bool reArmTouch = false;                                                  // get rid of former detections and start fresh detection

// *** Alarm ***
int currentAlarmSection = AS_NONE;                                        // current alarm section (which part of alarm cycle is active)
int lastSectionSwitch = 0;                                                // time at which there was an alarm section switch
bool alarmAnimation = true;                                               // alarm (light)animation
bool alarmWakeRadio = true;                                               // alarm wake-up radio
char wakeRadioHost[50] = "nickstick.nl";                                  // wake-up station host
int wakeRadioPort = 8000;                                                 // wake-up station port
char wakeRadioPath[50] = "/awake";                                        // wake-up station path (mount point)
int wakeRadioMinutes = 1;                                                 // wake-up station playtime
bool alarmInfoTTS = true;                                                 // alarm info TTS
bool alarmContRadio = true;                                               // alarm continuous radio
char contRadioHost[50] = "ice1.somafm.com";                               // continuous station host
int contRadioPort = 80;                                                   // continuous station port
char contRadioPath[50] = "/u80s-128-mp3";                                 // continuous station path (mount point)
int alarmState = ALARM_PENDING;                                           // assume waiting for alarm ALARM_PENDING (other states: ACTIVE - SNOOZE)
char alarmSearchStartZuluCstr[25];                                        // start of alarm search zulu time format
char alarmSearchEndZuluCstr[25];                                          // end of alarm search zulu time format
int alarmHour;                                                            // alarm time
int alarmMinute;
bool alarmSet = false;                                                    // alarm status
time_t lastForcedPollAlarmTime = 0;                                       // reset time of last forced polling of alarmTime setting
time_t snoozeStarted = 0;                                                 // when snooze started
int snoozeMinutes = 6;                                                    // nr of minutes to snooze

// *** Radar ***
std::queue<int> movementQ;                                                // total movements per hour last 7 hours
int movementsThisHour = 0;                                                // number of movements this hour
bool movementDetected = false;                                            // movement detected
int movementDebounceTime = STD_MOVEMENT_DEBOUNCE_TIME;                    // seconds before arming movement radar again after movement detected
int movementDebounce = movementDebounceTime;                              // deboumce countdown

// *** Blynk ***
char blynkAuth[] = BLYNK_AUTH;                                            // set Blynk token

// *** remote debug ***
RemoteDebug remoteDebug;                                                  // remote debug object