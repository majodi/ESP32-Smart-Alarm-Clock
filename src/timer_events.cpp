#include "timer_events.h"

void eachSecond() {

}

void eachMinute() {
  slog("minute event");
//   if (!timeValid && timeSyncRetries < 10) {
//     timeSyncRetries++;                                                    // retry every minute for the first 10 minutes of the hour, then wait 50 minutes for regular sync
//     syncTime();
//   }
}

void eachHour() {
//   syncTime();                                                             // regular time sync, on fail will retry for 10 minutes on minute event
}

void eachDay() {

}
