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
  setDateTimeStrn(alarmSearchStartZuluCstr, 0);                           // time window to search for alarm set (now)
  setDateTimeStrn(alarmSearchEndZuluCstr, 86400);                         // now + 24h
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
