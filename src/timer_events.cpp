#include "timer_events.h"

void eachSecond() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));                   // life sign
  dots = !dots;                                                           // on/off dots every second as seconds indicator
  display.showNumberDecEx(timeinfo.tm_hour*100+timeinfo.tm_min, dots ? 64 : 0, true); // update display
  movementDebounce = movementDebounce > 0 ? movementDebounce - 1 : MOVEMENT_DEBOUNCE_TIME; // set current movement debounce value (0 = armed)
  if (movementDebounce == 0) {                                            // reset movement detector (arm for next movement)
    movementDetected = false;
  }
  if (!movementDetected) {                                                // while not already in detected state
    movementDetected = digitalRead(RADAR_PIN);                            // see if there is movement
    if (movementDetected) {                                               // if so
      slog("detect count +1");                                            // increment counter
      // increase peakValue with debounceTime (clip at high value)
      // every sec no activity (after debounce cycle) decrease 1
      // every minute take current peakValue cumulative (write graph)
      // every hour cumulative / 60 save in last 6 hour array (shift) and reset cumulative
    }
  }
  if (abs(volume - volumeDesired) > 1) {                                  // if needed catch-up real volume to desired volume
    if (volume < volumeDesired) {
      volume += (volumeDesired - volume) / 2;
    }
    if (volume > volumeDesired) {
      volume -= (volume - volumeDesired) / 2;
    }
    vsSetVolume(volume);
  }
}

void eachMinute() {
  slog("minute");
  syncTime();                                                             // get up-to-date with time server
  handleAlarm();                                                          // handle possible alarm (fire it on time)
  handleSnooze();                                                         // handle possible snooze state (end it on time)
}

void eachHour() {
  slog("hour");
  pollAlarmTimeSetting(false);                                            // see if the alarm time can be found and set alarm for (new) time (silent - no feedback)
}

void eachDay() {

}
