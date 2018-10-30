#ifndef GENERAL_H
#define GENERAL_H 1

#include "constants.h"
#include "timer_events.h"
#include <time.h>

void slog( const char* format, ... );
void blink( int count );
void syncTime();
void handleTimer();

#endif