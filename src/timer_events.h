#ifndef TIMER_EVENTS_H
#define TIMER_EVENTS_H 1

#include "constants.h"
#include "general.h"
#include "alarm.h"
#include "handlers.h"

void eachSecond();
void eachMinute();
void eachHour();
void eachDay();
void updateBootState();

#endif
