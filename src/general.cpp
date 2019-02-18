#include "general.h"

void slog ( const char* format, ... ) {                                   // format options: http://www.cplusplus.com/reference/cstdio/printf/
  if (logging) {
    static char formattedMessage[128];                                    // formatted message
    va_list varArgs;                                                      // parameter list
    va_start (varArgs, format);                                           // parameters start
    vsnprintf (formattedMessage, sizeof(formattedMessage), format, varArgs); // Format message
    va_end (varArgs);                                                     // parameters end
    Serial.print ("! ");                                                  // log formatted message
    Serial.println (formattedMessage);                                    // output to serial
    if (boot && formattedMessage[0] != '*') {                             // buffer debug messages during boot, skip info messages (*) to save buffer room
      strncat(ttrbuf, "|", TTR_LEFT);                                     // separator
      strncat(ttrbuf, formattedMessage, TTR_LEFT);                        // formatted message
    } else {                                                              // when not in boot mode just send debug message (telnet now activated)
      remoteDebug.println(formattedMessage);                              // output to telnet
    }
  }
}

void blink ( int count ) {                                                // count short blinks of built-in LED to signal something
  for (int i=0; i<count; ++i) {                                           // for nr of count
    digitalWrite(LED_BUILTIN, HIGH);                                      // built-in led on
    strip.ClearTo(RgbColor((i%3)*127, (i%2)*255, (2-(i%3))*127));         // set alternating color for all pixels
    strip.Show();                                                         // and show
    delay(50);                                                            // short wait
    digitalWrite(LED_BUILTIN, LOW);                                       // built-in led off
    strip.ClearTo(RgbColor(0, 0, 0));                                     // set color to black for all pixels
    strip.Show();                                                         // and show
    delay(50);                                                            // short wait
  }    
}

void syncTime() {                                                         // sync internal time struct
  if (!getLocalTime(&timeinfo)) {
    timeValid = false;                                                    // internal time no longer accurate, not synced
  } else {
    timeValid = true;                                                     // internal time up-to-date with real time
    sprintf (timeCstr, "%02d:%02d:%02d",                                  // format internal time to Cstr
             timeinfo.tm_hour,
             timeinfo.tm_min,
             timeinfo.tm_sec);
    sprintf (dateCstr, "%04d-%02d-%02d",                                  // format internal date to Cstr
           timeinfo.tm_year + 1900,
           timeinfo.tm_mon + 1,
           timeinfo.tm_mday);
  }
}

void setDateTimeStrn(char* dest, time_t adjustment) {                     // set date/time with adjustment in seconds
  time_t newSeconds = mktime(&timeinfo) + adjustment;                     // get current time seconds and adjust
  struct tm plus24h = *localtime(&newSeconds);                            // create date/time structure
  sprintf (dest, "%04d-%02d-%02dT%02d:%02d:00.000Z",                      // format date/time and write to dest cstr
           plus24h.tm_year + 1900,
           plus24h.tm_mon + 1,
           plus24h.tm_mday,
           plus24h.tm_hour,
           plus24h.tm_min);
}

void cleanStr(char *target, char *unwanted) {                             // replace unwanted chars with a space
  char * match = strpbrk(target, unwanted);
  while (match != NULL) {
    *match = ' ';
    match = strpbrk(match+1, unwanted);
  }
}

void showTime() {
  if (dots && (alarmState == ALARM_SNOOZED)) {                            // if snoozed show snooze time remaining every 2nd second
    int minutesRemaining = ((snoozeMinutes * 60) - (mktime(&timeinfo) - snoozeStarted)) / 60; // calculate minutes remaining
    display.clear();                                                      // clear display
    display.showNumberDec(minutesRemaining, false, 1, 1);                 // show minutes remaining
  } else {
    display.showNumberDecEx(timeinfo.tm_hour*100+timeinfo.tm_min, dots ? 64 : 0, true); // update display
  }
}