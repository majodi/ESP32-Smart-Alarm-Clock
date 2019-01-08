#include "handlers.h"

void handleAlarm() {
  if (alarmState == ALARM_PENDING && alarmSet && timeinfo.tm_hour == alarmHour && timeinfo.tm_min == alarmMinute) { // waiting for alarm? have alarm time? alarm time reached?
    alarmState = ALARM_ACTIVE;                                            // set alarm state to active
    alarmSet = false;                                                     // clear alarm time set (wait for next alarm time fetch)
    radioOnTTSEnd = true;                                                 // radio on end of tts stream
    prepareTTSInfo("ik hoop dat u lekker heeft geslapen. <break strength=\"strong\"/>", 3);
    requestTTSMP3Data();
  }
}

void handleTouch() {
  if (touchDetected) {                                                    // touch detected
    slog("touch detected");
    portENTER_CRITICAL(&timerMux);                                        // critical section (for shared variable change)
    touchDetected = false;                                                // reset detected event
    portEXIT_CRITICAL(&timerMux);
    if (alarmState == ALARM_PENDING) {                                    // are we waiting for an alarm? then touch means we want to refresh/fetch the alarm time setting from google calendar
      syncTime();                                                         // for last forced timer value
      slog("last forced = %d", lastForcedPollAlarmTime);
      slog("now = %d", mktime(&timeinfo));
      if (mktime(&timeinfo) - lastForcedPollAlarmTime > 25) {             // if we did not handle such a request just a short time ago
        lastForcedPollAlarmTime = mktime(&timeinfo);                      // set a timer for checking if we do not refresh too often
        pollAlarmTimeSetting(true);                                       // try to fetch new alarm time with feedback
      }
      return;
    }
    if (alarmState == ALARM_ACTIVE) {
      alarmState = ALARM_SNOOZED;
      snoozeStarted = mktime(&timeinfo);
      return;
    }
    if (alarmState == ALARM_SNOOZED) {
      alarmState = ALARM_PENDING;
      return;
    }
  }
}

void handleSnooze() {
  if (alarmState == ALARM_SNOOZED) {                                      // if in snooze state
    if ((mktime(&timeinfo) - snoozeStarted) > 9*60) {                     // snooze time passed
      alarmState = ALARM_ACTIVE;                                          // go back to alarm state
      startRadio();                                                       // and start radio, no (repeated) talking after a snooze
    }
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
