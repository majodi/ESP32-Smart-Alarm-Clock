#include "alarm.h"

void pollAlarmTimeSetting(bool feedback) {
  alarmSet = false;                                                       // set alarm not set
  if (feedback) {                                                         // if feedback required
    display.clear();
    display.showNumberDec(3, false, 1, 1);                                // show coundtdown timer
  }
  host = GOOGLE_APIS_HOST;                                                // connect to google apis
  path = GOOGLE_OAUTH_PATH;                                               // OAuth for refreshing token
  googleToken[0] = 0;                                                     // clear token
  httpConnect(host, port, true);                                          // connect
  httpPostRequest(host, path, String("client_id=" OAUTH_CLIENT_ID1 "&client_secret=" OAUTH_CLIENT_SECRET1 "&refresh_token=" OAUTH_REFRESH_TOKEN1 "&grant_type=refresh_token"), "application/x-www-form-urlencoded"); // request new token
  httpFetchAndAdd("\"access_token\": \"", googleToken, sizeof(googleToken) - 1); // fetch if found
  httpEnd();                                                              // end current connection
  if (feedback) {                                                         // if feedback required
    display.clear();
    display.showNumberDec(2, false, 1, 1);                                // show coundtdown timer
  }
  delay(500);                                                             // give it some time
  setDateTimeStrn(alarmSearchStartZuluCstr, 0 - 3600, true);              // time window to search for alarm set (now - 1 hour away from Zulu in NL) take into account timeAdjust
  setDateTimeStrn(alarmSearchEndZuluCstr, 82800 - 3600, true);            // now + 23h, look ahead 23h (24 would catch next day as todays alarm)
  char enhancedPath[351];                                                 // long path buffer (for path with parameters)
  strncpy(enhancedPath, GOOGLE_CALENDAR_PATH "?q=Wekker&timeMin=", 350);  // build path
  strncat(enhancedPath, alarmSearchStartZuluCstr, 25);
  strncat(enhancedPath, "&timeMax=", 10);
  strncat(enhancedPath, alarmSearchEndZuluCstr, 25);
  strncat(enhancedPath, "&access_token=", 15);
  strncat(enhancedPath, googleToken, 350 - strlen(enhancedPath));
  httpConnect(host, port, true);                                          // connect
  httpGetRequest(host, enhancedPath);                                     // send get request
  char alarmTime[6] = {0};
  if (httpWaitAvailable(2000) && https.find("\"dateTime\": \"")) {        // see if we have at least one alarm item en position to dateTime
    if (feedback) {                                                       // if feedback required
      display.clear();
      display.showNumberDec(1, false, 1, 1);                              // show coundtdown timer
    }
    if (httpFetchAndAdd("T", alarmTime, 5)) {                             // advance to time part (time exactly 5 so only hh:mm) and add
      alarmTime[2] = 0;
      alarmHour = atoi(alarmTime);                                        // fill hour, minute and set alarm
      alarmMinute = atoi(alarmTime + 3);
      if (alarmMinute == 0) {                                             // calculate 1 minute subtract for collision window (collision alarm trigger and fetch new alarm time)
        alarmHour = alarmHour == 0 ? 23 : alarmHour -= 1;
        alarmMinute = 59;
      } else {
        alarmMinute -= 1;
      }
      alarmSet = true;
    }
  }
  httpEnd();
  if (feedback) {                                                         // if feedback required
    for(int i = 0; i < 4; ++i) {                                          // 4 times
      delay(500);
      display.setSegments(TM1637_tijd);                                   // "tijd"
      delay(500);
      alarmSet ? display.showNumberDecEx(alarmHour*100+alarmMinute, 64, true) : display.setSegments(TM1637_niet); // alarm time or "niet"
    }
    display.showNumberDecEx(timeinfo.tm_hour*100+timeinfo.tm_min, dots ? 64 : 0, true); // normal time
  }
}

void pollAlarmTest() {
  host = GOOGLE_APIS_HOST;                                                // connect to google apis
  path = GOOGLE_OAUTH_PATH;                                               // OAuth for refreshing token
  googleToken[0] = 0;                                                     // clear token
  httpConnect(host, port, true);                                          // connect
  httpPostRequest(host, path, String("client_id=" OAUTH_CLIENT_ID1 "&client_secret=" OAUTH_CLIENT_SECRET1 "&refresh_token=" OAUTH_REFRESH_TOKEN1 "&grant_type=refresh_token"), "application/x-www-form-urlencoded"); // request new token
  httpFetchAndAdd("\"access_token\": \"", googleToken, sizeof(googleToken) - 1); // fetch if found
  httpEnd();                                                              // end current connection
  delay(500);                                                             // give it some time
  setDateTimeStrn(alarmSearchStartZuluCstr, 0 - 3600, true);              // time window to search for alarm set (now - 1 hour away from Zulu) take into account timeAdjust
  slog("alarmSearchStartZuluCstr: %d", alarmSearchStartZuluCstr);
  setDateTimeStrn(alarmSearchEndZuluCstr, 82800 - 3600, true);            // now + 23h, look ahead 23h (24 would catch next day as todays alarm)
  slog("alarmSearchEndZuluCstr: %d", alarmSearchStartZuluCstr);
  char enhancedPath[351];                                                 // long path buffer (for path with parameters)
  strncpy(enhancedPath, GOOGLE_CALENDAR_PATH "?q=Wekker&timeMin=", 350);  // build path
  strncat(enhancedPath, alarmSearchStartZuluCstr, 25);
  strncat(enhancedPath, "&timeMax=", 10);
  strncat(enhancedPath, alarmSearchEndZuluCstr, 25);
  strncat(enhancedPath, "&access_token=", 15);
  strncat(enhancedPath, googleToken, 350 - strlen(enhancedPath));
  slog("Request: %s", enhancedPath);
  httpConnect(host, port, true);                                          // connect
  httpGetRequest(host, enhancedPath);                                     // send get request
  char alarmTime[6] = {0};
  if (httpWaitAvailable(2000)) {
    slog("connected and available");
    // test 1
    char respbuffer[300];
    // int resplen = https.readBytes(respbuffer, 299);
    // slog("read %d bytes: %s", resplen, respbuffer);
    int read = https.readBytesUntil('\n', respbuffer, 299);
    respbuffer[read] = 0;
    while (read) {
      slog("%s", respbuffer);
      read = https.readBytesUntil('\n', respbuffer, 299);
      respbuffer[read] = 0;
    }
    // // test 2
    // if (https.find("\"dateTime\": \"")) {        // see if we have at least one alarm item en position to dateTime
    //   slog("found \"dateTime\": \"");
    //   if (httpFetchAndAdd("T", alarmTime, 5)) {                             // advance to time part (time exactly 5 so only hh:mm) and add
    //     slog("found Txxxxx, time: %s",alarmTime);
    //     alarmTime[2] = 0;
    //     alarmHour = atoi(alarmTime);                                        // fill hour, minute and set alarm
    //     slog("hour: %d", alarmHour);
    //     alarmMinute = atoi(alarmTime + 3);
    //     slog("minute: %d", alarmMinute);
    //     alarmSet = true;
    //     slog("alarm set to true at %d - %d", minutesCounter, timeinfo.tm_hour*100+timeinfo.tm_min);
    //     slog("Alarm search window was %s - %s", alarmSearchStartZuluCstr, alarmSearchEndZuluCstr);
    //     slog("Alarm time: %d:%d", alarmHour, alarmMinute);
    //   } else {
    //     slog("no T found");
    //   }
    // } else {
    //   slog("no dateTime found");
    // }
  } else {
    slog("no reply after 2 seconds");
  }
  httpEnd();
  slog("alarmState: %d - alarmSet: %d - timeinfo.tm_hour: %d - timeinfo.tm_min: %d", alarmState, alarmSet, timeinfo.tm_hour, timeinfo.tm_min);
  if (alarmState == ALARM_PENDING && alarmSet && timeinfo.tm_hour == alarmHour && timeinfo.tm_min == alarmMinute) { // waiting for alarm? have alarm time? alarm time reached?
    slog("next handleAlarm will trigger alarm");
  } else {
    slog("next handleAlarm will NOT trigger alarm");
  }
}
