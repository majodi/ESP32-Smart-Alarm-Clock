#include "blynkio.h"
#include <BlynkSimpleEsp32.h>                                             // due to deviating structure of Blynk only local

void setupBlynk() {                                                       // setup Blynk
  Blynk.config(blynkAuth);                                                // config with token only
  Blynk.connect();                                                        // connect to Blynk server
  blynkSyncState();                                                       // sync initial state
}

void handleBlynk() {                                                      // stub for Blynk.run() for global usage
  Blynk.run();
}

void blynkSyncState() {                                                   // sync to app
  Blynk.virtualWrite(V1, radioPlaying);                                   // sync current state: radio playing
  int appVolume = -100 * (((volumeDesired - 25) / 4) - 10);               // translate device scale versus app scale
  Blynk.virtualWrite(V2, appVolume);                                      // sync current state: volume setting scale 60-25 to 1-10
}

BLYNK_WRITE(V1) {                                                         // Virtual pin 1 app to device call-back
  int value = param.asInt();                                              // Get value as integer
  if (!radioPlaying && value == 1) {                                      // not playing but app turns radio ON
    startRadio();                                                         // start radio
    return;                                                               // short stop
  }
  if (radioPlaying && value == 0) {                                       // playing but app turns radio OFF
    stopRadio();                                                          // stop radio
  }
}

BLYNK_WRITE(V2) {                                                         // Virtual pin 1 app to device call-back
  int value = param.asInt();                                              // get value as integer
  volumeDesired = (4 * (10 - (value / 100))) + 25;                        // set desired volume from 1-10 scale to 60-25 scale
}
