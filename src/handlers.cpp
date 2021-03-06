#include "handlers.h"

void handleAlarm() {
  if (alarmState == ALARM_PENDING && alarmSet && timeinfo.tm_hour == alarmHour && timeinfo.tm_min == alarmMinute) { // waiting for alarm? have alarm time? alarm time reached?
    alarmState = ALARM_ACTIVE;                                            // set alarm state to active
    alarmSet = false;                                                     // clear alarm time set (wait for next alarm time fetch)
    blynkSyncState();                                                     // update state to app
  }
  if (alarmState == ALARM_ACTIVE) {                                       // alarm active?
    nextAlarmSection(false);                                              // activate next alarm section in sequence (after currentAlarmSection) when this section is done (force = false)
  }
}

void handleNeoPixel() {
  if (animations.IsAnimating()) {                                         // if any animation is ongoing
    animations.UpdateAnimations();                                        // update animations
    strip.Show();                                                         // and show
  } else {
    if (!animationsEnded) {                                               // if no animations and not yet cleaned-up, call clean-up
      endAnim_All();                                                      // cleanup if needed after all animations stopped
    }
  }
}

void handleRadioPlayTime() {
  if (radioPlaying && radioPlayTime) {                                    // if radio is playing and a play time was set
    radioTimePlayed += 1;                                                 // update time played ** called every second **
    if (radioTimePlayed > radioPlayTime) {                                // if play time has passed
      if (alarmState == ALARM_ACTIVE) {                                   // and we are in alarm state (awake radio)
        nextAlarmSection(true);                                           // activate next alarm section (force = true, stop playing)
      } else {
        stopRadio();                                                      // else just stop radio (probably not used)
      }
    }
  }
}

void handleStream() {
  if (streamType != NO_ACTIVE_STREAM) {                                     // should stream data be coming in?
    if (streamType == MP3_STREAM) {                                         // is it MP3?
      transferAvailableMP3Data();                                           // process MP3 data
    }
    if (streamType == TTS_STREAM) {                                         // is it TTS?
      transferAvailableTTSMP3Data();                                        // process TTS data
    }
  } else {
    if (alarmState == ALARM_ACTIVE && currentAlarmSection == AS_WAKE_RADIO) {
        nextAlarmSection(false);                                            // start next alarm section (force = false, not needed, no active stream)
    }
  }
}

void handleTouch() {
  if (touchDetected) {                                                    // touch detected (set by ISR)
    slog("touch detected; touchHoldCount = %d", touchHoldCount);
    touchOn = true;                                                       // set touch state for this cycle/second (for touchHold counting)
    portENTER_CRITICAL(&timerMux);                                        // critical section (for shared variable change)
    touchDetected = false;                                                // reset detected event
    portEXIT_CRITICAL(&timerMux);
    if (reArmTouch) {                                                     // if request to re-arm
      reArmTouch = false;                                                 // reset re-arm switch
      return;                                                             // leave without acting on touch (detection already set false)
    }
    if (millis() - lastTouched < 500) {                                   // time passed less than 500ms?
      return;                                                             // ignore, don't act (debounce)
    }
    lastTouched = millis();                                               // if acting then remember time
    if (touchHold) {                                                      // if touch is hold (long touch)
      touchHoldCount = 0;                                                 // reset hold count to act as debounce
      if (alarmState == ALARM_PENDING) {                                  // alarm pending?
        syncTime();                                                       // refresh time info struct for mktime()
        if (mktime(&timeinfo) - lastForcedPollAlarmTime > 25) {           // if we did not handle such a request just a short time ago
          lastForcedPollAlarmTime = mktime(&timeinfo);                    // set a timer for checking if we do not refresh too often
          pollAlarmTimeSetting(true);                                     // try to fetch new alarm time with feedback (true)
          // pollAlarmTest();
        }
        return;                                                           // short stop if/then
      }
      if (alarmState == ALARM_SNOOZED) {                                  // if (long)touch when snoozed: end snooze en arm for next alarm (end current alarm cycle)
        blink(10);                                                        // feedback signal to let loose hold
        currentAlarmSection = AS_NONE;                                    // reset alarm section for next cycle
        alarmState = ALARM_PENDING;                                       // set state to alarm pending
        blynkSyncState();                                                 // update state to app
        return;                                                           // short stop if/then (for possible future enhancements)
      }
    }
    if (alarmState == ALARM_ACTIVE) {                                     // alarm state active?
      if (currentAlarmSection == AS_CONT_RADIO) {                         // in continuous radio (alarm)mode? --> touch = snooze
        if (alarmState != ALARM_SNOOZED) {                                // not snoozed?
          alarmState = ALARM_SNOOZED;                                     // set state to snoozed
          snoozeStarted = mktime(&timeinfo);                              // remember the time snooze started
        }
      } else {                                                            // not in ncontinuous radio (alarm)mode --> touch = skip
        if ((millis() - lastSectionSwitch) > 1000) {                      // avoid double skip (time between skips)
          nextAlarmSection(true);                                         // initiate skip (true ends current section so next section will be activated)
        }
      }
      return;                                                             // short stop if/then
    }
  }
}

void nextAlarmSection(bool force) {                                       // switch action based upon current section (or force = is force current section to end premature)
  lastSectionSwitch = millis();                                           // remember switch time (for debouncing)
  switch(currentAlarmSection) {
    case AS_NONE:                                                         // --> NONE (no forced ending)
      startAnim_Cylon();                                                  // start wake-up light animation
      currentAlarmSection = AS_ANIMATION;                                 // set new current section
      return;                                                             // short stop
    case AS_ANIMATION:                                                    // --> ANIMATION
      if (force) {                                                        // forced end only?
        stopAnim_All();                                                   // stop all animations
        return;                                                           // short stop
      }
      if (animationsEnded) {                                              // switch when animations ended
        radioPlayTime = wakeRadioMinutes * 60;                            // set (wake) radio play time
        volumeOverrule = VOL_MED;                                         // overrule volume to medium until this radio session stops
        startRadio(wakeRadioHost, wakeRadioPort, wakeRadioPath);          // start wake radio
        currentAlarmSection = AS_WAKE_RADIO;                              // set current section
        reArmTouch = true;                                                // re arm touch to flush possible unhandled touch events
        return;                                                           // short stop
      }
      break;
    case AS_WAKE_RADIO:                                                   // --> WAKE_RADIO
      if (force) {                                                        // forced end only?
        stopRadio();                                                      // stop radio
        return;                                                           // short stop
      }
      if (!radioPlaying) {                                                // switch when radio not playing (stopped)
        prepareTTSInfo(SSML_OPENING, 3);                                  // prepare TTS stream
        requestTTSMP3Data();                                              // process TTS MP3 data
        currentAlarmSection = AS_INFO_TTS;                                // set current section
        reArmTouch = true;                                                // re arm touch to flush possible unhandled touch events
        return;                                                           // short stop
      }
      break;
    case AS_INFO_TTS:                                                     // --> INFO_TTS
      if (force) {                                                        // forced end only?
        TTSEndRequest = true;                                             // request ending of possible TTS stream
        return;                                                           // short stop
      }
      if (!TTSPlaying) {                                                  // switch when TTS not playing (stopped)
        startRadio(contRadioHost, contRadioPort, contRadioPath);          // start continuous radio
        currentAlarmSection = AS_CONT_RADIO;                              // set current section
        reArmTouch = true;                                                // re arm touch to flush possible unhandled touch events
        return;                                                           // short stop
      }
      break;
    case AS_CONT_RADIO:                                                   // --> CONT_RADIO (no forced ending, no next section)
      if (!radioPlaying && (alarmState != ALARM_SNOOZED)) {               // if not playing but not snoozed (anymore)
        startRadio(contRadioHost, contRadioPort, contRadioPath);          // start radio (should be playing now)
      }
      return;
    default:
      currentAlarmSection = AS_NONE;
      return;
  }  
}

void updateTouchState() {
  touchHoldCount = touchOn ? touchHoldCount + 1 : 0;
  touchHold = touchHoldCount > 2 ? true : false;
  touchOn = false;
}

void handleMovement() {
  movementDebounce = movementDebounce > 0 ? movementDebounce - 1 : movementDebounceTime; // count down debounce every second (0 = armed)
  if (movementDebounce == 0) {                                            // if movement detection armed
    movementDetected = false;                                             // reset movement detector to watch for movement (armed)
  }
  if (!movementDetected) {                                                // while not already in detected state (handled by debounce)
    movementDetected = digitalRead(RADAR_PIN);                            // see if there is a new movement
    if (movementDetected) {                                               // if so
      // slog("movement");
      movementsThisHour += 1;                                             // register new movement for hourly total
    }
  }
}

void updateMovementQ() {
  blynkSyncMovements();                                                   // sync total movements this hour to app before resetting
  movementQ.push(movementsThisHour);                                      // insert current hour total at end of Q
  movementsThisHour = 0;                                                  // reset movement counter for current hour
  while (movementQ.size() > 6) {                                          // do we have more than 6 hourly totals?
    movementQ.pop();                                                      // pop oldest item, just save the last 6 full hour items
  }
}

int movementsLastSleepCycle() {
  std::queue<int> tmpq = movementQ;                                       // temporary queue for processing
  int sum = 0;                                                            // to hold sum
  while (!tmpq.empty()) {                                                 // loop through hour items
      sum += tmpq.front();                                                // add
      tmpq.pop();                                                         // remove, next
  }
  return sum + movementsThisHour;                                         // add this hour (pardly depending on current time), total 6-7 hours
}

void handleSnooze() {
  if (alarmState == ALARM_SNOOZED) {                                      // if in snooze state
    if ((mktime(&timeinfo) - snoozeStarted) > snoozeMinutes*60) {         // snooze time passed
      alarmState = ALARM_ACTIVE;                                          // go back to alarm state
      blynkSyncState();                                                   // update state to app
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
