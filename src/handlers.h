#ifndef HANDLERS_H
#define HANDLERS_H 1

#include "constants.h"
#include "general.h"

void handleAlarm();
void handleMovement();
void updateMovementState();
void handleNeoPixel();
void handleSnooze();
void handleRadioPlayTime();
void handleStream();
void handleTouch();
void nextAlarmSection(bool force);
void updateTouchState();
void handleTimer();

#endif
