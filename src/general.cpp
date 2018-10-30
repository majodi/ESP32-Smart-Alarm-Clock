#include "general.h"

// @@@ code herzien met goede time functionaliteit en comments
void setTimeVars() 
{
  time_t now = time(nullptr);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  thisMinute = timeinfo.tm_min;
  thisHour = timeinfo.tm_hour;
}

char* dbgPrint ( const char* format, ... )
// format options: http://www.cplusplus.com/reference/cstdio/printf/
{
  static char formattedMessage[128];                            // formatted message
  va_list varArgs;                                              // parameter list
  va_start (varArgs, format);                                   // parameters start
  vsnprintf (formattedMessage, sizeof(sbuf), format, varArgs);  // Format message
  va_end (varArgs);                                             // parameters end
  if (debug)                                                    // debug mode
  {
    Serial.print ("! ");                                        // log formatted message
    Serial.println (formattedMessage);
  }
}

void blink ( int count )
{
  for (int i=0; i<count; ++i) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);    
  }    
}