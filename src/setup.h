#ifndef SETUP_H
#define SETUP_H 1

#include "constants.h"
#include "general.h"
#include "http.h"
#include "timer_events.h"
#include "vs1053.h"
#include "ttr.h"
#include "alarm.h"
#include <WiFiManager.h>

void setupPins();
void setupWiFi();
void setupTimer();
void setupSPI();

#endif
