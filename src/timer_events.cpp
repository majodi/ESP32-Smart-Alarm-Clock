#include "timer_events.h"

void eachSecond() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));                   // life sign
  dots = !dots;                                                           // on/off dots every second as seconds indicator
  display.showNumberDecEx(timeinfo.tm_hour*100+timeinfo.tm_min, dots ? 64 : 0, true);
}

void eachMinute() {
  slog("minute event");
  syncTime();                                                             // get up-to-date with time server
  handleAlarm();                                                          // handle possible alarm
  handleSnooze();                                                         // handle possible snooze state
}

void eachHour() {
  pollAlarmTimeSetting(false);                                            // see if the alarm time can be found and set alarm for (new) time (silent - no feedback)
}

void eachDay() {

}
