#include "blynkio.h"
#include <BlynkSimpleEsp32.h>                                             // due to deviating structure of Blynk only local

void setupBlynk() {                                                       // setup Blynk
  Blynk.config(blynkAuth);                                                // config with token only
  Blynk.connect();                                                        // connect to Blynk server
}

void handleBlynk() {                                                      // stub for Blynk.run() for global usage
  Blynk.run();
}

void blynkSyncState() {                                                   // sync exposed variables (for which changes can be made from esp32) to app
  Blynk.virtualWrite(V1, radioPlaying);                                   // sync current state: radio playing
  int appVolume = -100 * (((volumeDesired - 25) / 4) - 10);               // translate device scale versus app scale
  Blynk.virtualWrite(V2, appVolume);                                      // sync current volume state: volume setting scale 60-25 to 1-10
  Blynk.virtualWrite(V3, alarmState);                                     // sync current alarm state
}

void blynkSyncMovements() {
  Blynk.virtualWrite(V8, movementsThisHour);                              // sync current movementsThisHour
}

BLYNK_CONNECTED() {                                                       // when (re)connected
  if (boot && !initialized) {                                             // if esp32 is in boot sequeunce (just (re)booted)
    Blynk.syncVirtual(V127);                                              // first get value of last saved (on server/app) virtual127 (init state), BLYNK_WRITE(127) will initialize server if needed
  }
}

BLYNK_WRITE(V1) {                                                         // Virtual pin 1 app to device call-back
  int value = param.asInt();                                              // Get value as integer
  if (boot && value) {                                                    // when during boot an old radio-is-on state setting detected, reset state
    Blynk.virtualWrite(V1, radioPlaying);                                 // sync current state: radio playing
    return;                                                               // short stop
  }
  if (!radioPlaying && value) {                                           // not playing but app turns radio ON
    startRadio(contRadioHost, contRadioPort, contRadioPath);              // start radio
    return;                                                               // short stop
  }
  if (radioPlaying && !value) {                                           // playing but app turns radio OFF
    stopRadio();                                                          // stop radio
  }
}

BLYNK_WRITE(V2) {                                                         // Virtual pin 2 app to device call-back
  int value = param.asInt();                                              // get value as integer
  volumeDesired = (4 * (10 - (value / 100))) + 25;                        // set desired volume from 1-10 scale to 60-25 scale
}

BLYNK_WRITE(V3) {                                                         // Virtual pin 3 app to device call-back (trigger alarm)
  int value = param.asInt();                                              // get value as integer
  if (boot && value) {                                                    // when during boot an old alarm-active state setting detected, reset state
    Blynk.virtualWrite(V3, alarmState);                                   // sync current state: 
    return;                                                               // short stop
  }
  if (value && (alarmState == ALARM_PENDING)) {                           // on request during pending state? (not during boot, we don't want last state from before boot)
    alarmState = ALARM_ACTIVE;                                            // set alarm state to active
    alarmSet = false;                                                     // clear alarm time set (wait for next alarm time fetch)
    blynkSyncState();                                                     // update state to app
    nextAlarmSection(false);                                              // activate next alarm section in sequence (after currentAlarmSection) when this section is done (force = false)
  }
}

BLYNK_WRITE(V4) {                                                         // Virtual pin 4 app to device call-back
  snoozeMinutes = param.asInt();                                          // get value as integer
}

BLYNK_WRITE(V5) {                                                         // Virtual pin 5 app to device call-back
  wakeRadioMinutes = param.asInt();                                       // get value as integer
}

BLYNK_WRITE(V6) {                                                         // Virtual pin 6 app to device call-back
  strcpy(alternativeOpening, param.asStr());                              // get value as string
}

BLYNK_WRITE(V7) {                                                         // Virtual pin 7 app to device call-back
  movementDebounceTime = param.asInt();                                   // get value as integer
}

BLYNK_WRITE(V9) {                                                         // Virtual pin 9 app to device call-back
  alarmContRadio = param.asInt();                                         // get value as integer
}

BLYNK_WRITE(V10) {                                                         // Virtual pin 10 app to device call-back
  strcpy(wakeRadioHost, param.asStr());                                    // get value as string
}

BLYNK_WRITE(V11) {                                                         // Virtual pin 11 app to device call-back
  wakeRadioPort = param.asInt();                                           // get value as integer
}

BLYNK_WRITE(V12) {                                                         // Virtual pin 12 app to device call-back
  strcpy(wakeRadioPath, param.asStr());                                    // get value as string
}

BLYNK_WRITE(V13) {                                                         // Virtual pin 13 app to device call-back
  strcpy(contRadioHost, param.asStr());                                    // get value as string
}

BLYNK_WRITE(V14) {                                                         // Virtual pin 14 app to device call-back
  contRadioPort = param.asInt();                                           // get value as integer
}

BLYNK_WRITE(V15) {                                                         // Virtual pin 15 app to device call-back
  strcpy(contRadioPath, param.asStr());                                    // get value as string
}

BLYNK_WRITE(V16) {                                                         // Virtual pin 15 app to device call-back
  timeAdjust = param.asInt();                                              // get value as integer
  syncTime();                                                              // sync display time
}

BLYNK_WRITE(V127) {                                                       // Virtual pin 127 app to device call-back (no widget, only called/requested at boot time after blynk connected)
  if (boot && !initialized) {                                             // if in boot state and not yet initialized
    initialized = true;                                                   // only once
    int value = param.asInt();                                            // get value as integer
    if (value == BLYNK_SERVER_INITIALIZED) {                              // if server values initialized already with defaults (at the first connect but perhaps changed afterwards)
      slog("Get settings");
      Blynk.syncAll();                                                    // retrieve all values from server (every BLYNK_WRITE is called)
    } else {                                                              // server not yet (never) initialzed with default values, write defaults to server
      slog("Write defaults");
      Blynk.virtualWrite(V1,  radioPlaying);                              // set default radio state
      Blynk.virtualWrite(V2,  volumeDesired);                             // set default volume (desired)
      Blynk.virtualWrite(V3,  alarmState);                                // set default alarm state
      Blynk.virtualWrite(V4,  snoozeMinutes);                             // set default snooze minutes
      Blynk.virtualWrite(V5,  wakeRadioMinutes);                          // set default wakeRadioMinutes     
      Blynk.virtualWrite(V6,  alternativeOpening);                        // set default alternativeOpening   
      Blynk.virtualWrite(V7,  movementDebounceTime);                      // set default movementDebounceTime 
      Blynk.virtualWrite(V9,  alarmContRadio);                            // set default alarmContRadio       
      Blynk.virtualWrite(V10, wakeRadioHost);                             // set default wakeRadioHost        
      Blynk.virtualWrite(V11, wakeRadioPort);                             // set default wakeRadioPort        
      Blynk.virtualWrite(V12, wakeRadioPath);                             // set default wakeRadioPath        
      Blynk.virtualWrite(V13, contRadioHost);                             // set default contRadioHost        
      Blynk.virtualWrite(V14, contRadioPort);                             // set default contRadioPort        
      Blynk.virtualWrite(V15, contRadioPath);                             // set default contRadioPath
      Blynk.virtualWrite(V16, timeAdjust);                                // set default timeAdjust
      Blynk.virtualWrite(V127, BLYNK_SERVER_INITIALIZED);                 // set server initialized (keep settings)
    }
  }
}

// V1 = radioPlaying 1/0
// V2 = volumeDesired 0-10
// V3 = alarmState 1/0

// V4  = snoozeMinutes 1-15
// V5  = wakeRadioMinutes 1-15        
// V6  = alternativeOpening[127] $    
// V7  = movementDebounceTime 1-15    
// V8  = movementsThisHour to app only
// V9  = alarmContRadio 1/0           
// V10 = wakeRadioHost[50] $         
// V11 = wakeRadioPort 0-32767       
// V12 = wakeRadioPath[50] $         
// V13 = contRadioHost[50] $         
// V14 = contRadioPort 0-32767       
// V15 = contRadioPath[50] $
// V16 = timeAdjust -24 - +24

