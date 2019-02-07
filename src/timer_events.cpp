#include "timer_events.h"

void eachSecond() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));                   // life sign
  updateTouchState();                                                     // update touch state
  updateMovementState();                                                  // update movement state
  handleRadioPlayTime();                                                  // handle possible radio play time
  regulateVolume();                                                       // slowly regulate volume to desired level when needed
  dots = !dots;                                                           // on/off dots every second as seconds indicator
  showTime();                                                             // show time on display
}

void eachMinute() {
  slog("minute");
  syncTime();                                                             // get time struct up-to-date
  handleAlarm();                                                          // handle possible alarm (fire it on time) or change alarm section
  handleSnooze();                                                         // handle possible snooze state (end it on time)
}

void eachHour() {
  slog("hour");
  pollAlarmTimeSetting(false);                                            // see if the alarm time can be found and set alarm for (new) time (silent - no feedback)
}

void eachDay() {

}
