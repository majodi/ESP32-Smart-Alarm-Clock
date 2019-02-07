#ifndef SETUP_H
#define SETUP_H 1

#include "constants.h"
#include "general.h"
#include "http.h"
#include "timer_events.h"
#include "vs1053.h"
#include "ttr.h"
#include "alarm.h"
#include "handlers.h"
#include <WiFiManager.h>
#include <NeoPixelBus.h>
#include <blynkio.h>
#include <RemoteDebug.h>
#include <ArduinoOTA.h>

void setupPins();
void setupWiFi();
void setupTimer();
void setupTouch();
void setupSPI();
void setupNeoPixel();
void setupRemoteDebug();
void setupOTA();

#endif
