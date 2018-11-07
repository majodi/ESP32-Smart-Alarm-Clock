#include "general.h"

void slog ( const char* format, ... ) {                                   // format options: http://www.cplusplus.com/reference/cstdio/printf/
  if (logging) {
    static char formattedMessage[128];                                    // formatted message
    va_list varArgs;                                                      // parameter list
    va_start (varArgs, format);                                           // parameters start
    vsnprintf (formattedMessage, sizeof(formattedMessage), format, varArgs); // Format message
    va_end (varArgs);                                                     // parameters end
    Serial.print ("! ");                                                  // log formatted message
    Serial.println (formattedMessage);
  }
}

void blink ( int count ) {                                                // count short blinks of built-in LED to signal something
  for (int i=0; i<count; ++i) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);    
  }    
}

void syncTime() {                                                         // sync internal time with NTP server
  if (!getLocalTime(&timeinfo)) {
    timeValid = false;                                                    // internal time no longer accurate, not synced
    slog("time sync failed!");
  } else {
    timeValid = true;                                                     // internal time up-to-date with real time
    timeSyncRetries = 0;                                                  // reset retry count for next regular sync
    sprintf (timeCstr, "%02d:%02d:%02d",                                  // format internal time to Cstr
             timeinfo.tm_hour,
             timeinfo.tm_min,
             timeinfo.tm_sec);
    sprintf (dateCstr, "%04d-%02d-%02d",                                  // format internal date to Cstr
           timeinfo.tm_year + 1900,
           timeinfo.tm_mon + 1,
           timeinfo.tm_mday);
    slog("internal time synced: %s", timeCstr);
  }
}

void handleTimer() {
  if (secTickCounter > 0) {                                               // at least one second passed?
    secondsCounter += secTickCounter;                                     // inc seconds
    portENTER_CRITICAL(&timerMux);
    secTickCounter = 0;                                                   // reset ISR secTick counter
    portEXIT_CRITICAL(&timerMux);
    eachSecond();                                                         // fire event
    if (secondsCounter > 60) {                                            // minute passed?
      minutesCounter++;                                                   // inc minutes
      secondsCounter = 0;
      eachMinute();                                                       // fire event
      if (minutesCounter > 60) {                                          // hour passed?
        hoursCounter++;                                                   // inc hours
        minutesCounter = 0;
        eachHour();                                                       // fire event
        if (hoursCounter % 24 == 0) {                                     // day passed?
          daysCounter++;                                                  // inc days
          eachDay();                                                      // fire event
        }
      }
    }
  }  
}

void cleanStr(char *target, char *unwanted) {                             // replace unwanted chars with a space
  char * match = strpbrk(target, unwanted);
  while (match != NULL) {
    *match = ' ';
    match = strpbrk(match+1, unwanted);
  }
}