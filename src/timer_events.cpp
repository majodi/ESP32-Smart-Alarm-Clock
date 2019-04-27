#include "timer_events.h"

void eachSecond() {
  updateBootState();
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));                   // life sign
  updateTouchState();                                                     // update touch state
  handleMovement();                                                       // handle possible movement state
  handleRadioPlayTime();                                                  // handle possible radio play time
  regulateVolume();                                                       // slowly regulate volume to desired level when needed
  dots = !dots;                                                           // on/off dots every second as seconds indicator
  showTime();                                                             // show time on display
}

void eachMinute() {
  // slog("-----> pollAlarmTest <-----");
  // pollAlarmTest();
  // slog("minute %d - %d", minutesCounter, timeinfo.tm_hour*100+timeinfo.tm_min);
  syncTime();                                                             // get time struct up-to-date
  handleAlarm();                                                          // handle possible alarm (fire it on time) or change alarm section
  handleSnooze();                                                         // handle possible snooze state (end it on time)
}

void eachHour() {
  slog("hour %d - %d", hoursCounter, timeinfo.tm_hour*100+timeinfo.tm_min);
  pollAlarmTimeSetting(false);                                            // see if the alarm time can be found and set alarm for (new) time (silent - no feedback)
  updateMovementQ();                                                      // update total movements this hour in queue of last 7 hourly totals plus sync app
}

void eachDay() {
  slog("day %d - %d", timeinfo.tm_hour*100+timeinfo.tm_min);
}

void updateBootState() {
  if (boot && (secondsCounter > 5 || remoteDebug.isActive())) {           // if still in boot state, is telnet ready? (after 5 seconds no longer wait for a possible debug client)
    boot = false;                                                         // boot sequence ended (ends when able to communicate with outside world or else after 5 seconds anyway)
    slog("%s", ttrbuf);                                                   // dump buffered messages
    ttrbuf[0] = 0;                                                        // empty ttrbuf (was used as temporary buffer space to save memory)
  }
}