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

void httpConnect(char *host, int port) {
  if (!http.connect(host, port)) {                                        // conect to a test station
    connected = false;
    slog("Connection failed");
  } else {
    connected = true;
  }
}

void httpGetStream(char *host, char *path) {
  http.print(String("GET ") + path + " HTTP/1.1\r\n" +
                                      "Host: " + host + "\r\n" + 
                                      "Connection: close\r\n\r\n");
}

void transferAvailableMP3Data() {
  if (digitalRead(VS1053_DREQ)) {                                         // if the VS1053 is hungry
    if (http.available() > 0) {                                           // and we have food
      uint8_t bytesread = http.read(mp3IOBuffer, 32);                     // get some food
      vsWriteBuffer(mp3IOBuffer, bytesread);                              // and feed it to the VS1053
    }
  }
}
