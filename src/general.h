#ifndef GENERAL_H
#define GENERAL_H 1

#include "constants.h"
#include <time.h>

void setTimeVars();
char* dbgPrint( const char* format, ... );
void blink( int count );

#endif