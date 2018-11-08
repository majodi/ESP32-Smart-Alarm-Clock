#ifndef GENERAL_H
#define GENERAL_H 1

#include "constants.h"
#include "timer_events.h"
#include "vs1053.h"
#include <time.h>

void slog(const char* format, ...);
void blink(int count);
void syncTime();
void setDateTimeStrn(char* dest, time_t adjustment);
void handleTimer();
void cleanStr(char *target, char *unwanted);

#endif