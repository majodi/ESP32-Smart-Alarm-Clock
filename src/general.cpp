#include "general.h"

void slog ( const char* format, ... ) {                                   // format options: http://www.cplusplus.com/reference/cstdio/printf/
  static char formattedMessage[128];                                      // formatted message
  va_list varArgs;                                                        // parameter list
  va_start (varArgs, format);                                             // parameters start
  vsnprintf (formattedMessage, sizeof(formattedMessage), format, varArgs);  // Format message
  va_end (varArgs);                                                       // parameters end
  if (logging)                                                            // logging on?
  {
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
    slog("internal time synced: %s", timeCstr);
  }
}

void handleTimer() {
  if (secTickCounter > 0) {
    secondsCounter += secTickCounter;
    portENTER_CRITICAL(&timerMux);
    secTickCounter = 0;
    portEXIT_CRITICAL(&timerMux);
    eachSecond();
    if (secondsCounter > 60) {
      minutesCounter++;
      secondsCounter = 0;
      eachMinute();
      if (minutesCounter > 60) {
        hoursCounter++;
        minutesCounter = 0;
        eachHour();
        if (hoursCounter % 24 == 0) {
          daysCounter++;
          eachDay();
        }
      }
    }
  }  
}
