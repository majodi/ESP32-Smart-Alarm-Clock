#include "alarm.h"

void pollAlarmTimeSetting() {
  alarmSet = false;                                                       // set alarm not set
  host = GOOGLE_APIS_HOST;                                                // connect to google apis
  path = GOOGLE_OAUTH_PATH;                                               // OAuth for refreshing token
  googleToken[0] = 0;                                                     // clear token
  httpConnect(host, port, true);                                          // connect
  httpPostRequest(host, path, String("client_id=" OAUTH_CLIENT_ID1 "&client_secret=" OAUTH_CLIENT_SECRET1 "&refresh_token=" OAUTH_REFRESH_TOKEN1 "&grant_type=refresh_token"), "application/x-www-form-urlencoded"); // request new token
  httpFetchAndAdd("\"access_token\": \"", googleToken, sizeof(googleToken) - 1); // fetch if found
  httpEnd();                                                              // end current connection
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
    if (httpFetchAndAdd("T", alarmTime, 5)) {                             // advance to time part (time exactly 5 so only hh:mm) and add
      alarmTime[2] = 0;
      alarmHour = atoi(alarmTime);                                        // fill hour, minute and set alarm
      alarmMinute = atoi(alarmTime + 3);
      alarmSet = true;
    }
  }
  httpEnd();
}

void handleAlarm() {
  if (alarmSet && timeinfo.tm_hour == alarmHour && timeinfo.tm_min == alarmMinute) { // alarm?
    alarmSet = false;                                                     // turn off alarm (wait for next alarm time)
    radioOnTTSEnd = true;                                                 // radio on end of tts stream
    prepareTTSInfo("ik hoop dat u lekker heeft geslapen. <break strength=\"strong\"/>", 3);
    requestTTSMP3Data();
  }
}