#ifndef BLYNKIO_H
#define BLYNKIO_H 1

#include "constants.h"
#include "vs1053.h"
#include "neopixel.h"

void setupBlynk();
void handleBlynk();
void blynkSyncState();
void blynkSyncMovements();

#endif
