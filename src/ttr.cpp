#include "ttr.h"

String TTSPostData(String text) {                                         // prepare tts post request
  return String("{\r\n") +
"     'input':{\r\n" +
"       'ssml':'<speak>" + text + "</speak>'\r\n" +
"     },\r\n" +
"     'voice':{\r\n" +
"       'languageCode':'nl-NL',\r\n" +
"       'name':'nl-NL-Wavenet-A',\r\n" +
"       'ssmlGender':'FEMALE'\r\n" +
"     },\r\n" +
"     'audioConfig':{\r\n" +
"       'audioEncoding':'MP3',\r\n" +
"       'speakingRate':'0.9'\r\n" +
"     }\r\n" +
"   }";
}

void ttrAddNews(int nrItems) {                                            // add news items to tts buffer
  int itemCnt = nrItems;
  strncat(ttrbuf, SSML_NEWS_INTRO, TTR_LEFT);                             // add intro text
  host = NEWS_API_HOST;                                                   // connect to news provider
  path = NEWS_API_PATH NEWS_API_KEY;
  httpConnect(host, port, true);
  httpGetRequest(host, path);                                             // send get request
  int ttrLen = strlen(ttrbuf);                                            // length of existing ttr (text to read) buffer
  bool validResponseData = httpWaitAvailable(2000) && https.find(",\"title\":\""); // see if we received a (first title)
  while (validResponseData && ttrLen < (TTR_MAXLEN - TTR_MAXNEWSITEMLEN) && itemCnt && httpWaitAvailable(2000)) { // get title texts
    ttrbuf[ttrLen + https.readBytesUntil('"', ttrbuf + ttrLen, TTR_MAXNEWSITEMLEN)] = 0; // only the title text (long enough for our purpose)
    strncat(ttrbuf, SSML_PAUSE, TTR_LEFT);                                // add pause
    ttrLen = strlen(ttrbuf);
    itemCnt--;
    validResponseData = https.find(",\"title\":\"");                      // prepare pointer to next item
  }
  httpEnd();                                                              // don't need this server anymore
}

void ttrAddWeather() {                                                    // add news items to tts buffer
  strncat(ttrbuf, SSML_WEATHER_INTRO, TTR_LEFT);                          // add intro text
  strncat(ttrbuf, "Op dit moment is het ", TTR_LEFT);                     // temperature anouncement
  host = WEATHER_API_HOST;                                                // connect to weather provider
  path = WEATHER_API_PATH WEATHER_API_KEY;
  httpConnect(host, port, false);
  httpGetRequest(host, path);                                             // send get request
  httpFetchAndAdd(", \"temp\": \"", ttrbuf, TTR_LEFT);                    // temperature
  strncat(ttrbuf, " graden. " SSML_PAUSE " De verdere verwachtingen zijn. " SSML_PAUSE, TTR_LEFT); // add unit, pause, anouncement for summary
  httpFetchAndAdd(", \"verw\": \"", ttrbuf, TTR_LEFT);                    // weather summary
  strncat(ttrbuf, ". " SSML_PAUSE, TTR_LEFT);                             // add pause
  httpEnd();                                                              // don't need this server anymore
}

void ttrAddCalendar() {                                                   // add calendar items to tts buffer
  strncat(ttrbuf, SSML_CALENDAR_INTRO1, TTR_LEFT);                        // add intro text
  host = GOOGLE_APIS_HOST;                                                // connect to google apis
  path = GOOGLE_OAUTH_PATH;                                               // OAuth for refreshing token
  googleToken[0] = 0;                                                     // clear token
  httpConnect(host, port, true);                                          // connect
  httpPostRequest(host, path, String("client_id=" OAUTH_CLIENT_ID1 "&client_secret=" OAUTH_CLIENT_SECRET1 "&refresh_token=" OAUTH_REFRESH_TOKEN1 "&grant_type=refresh_token"), "application/x-www-form-urlencoded"); // request new token
  httpFetchAndAdd("\"access_token\": \"", googleToken, sizeof(googleToken) - 1); // fetch if found
  sprintf (dayStartZuluCstr, "%04d-%02d-%02dT00:00:00.000Z",              // day start Zulu
           timeinfo.tm_year + 1900,
           timeinfo.tm_mon + 1,
           timeinfo.tm_mday);
  sprintf (dayEndZuluCstr, "%04d-%02d-%02dT23:59:59.000Z",                // day end Zulu
           timeinfo.tm_year + 1900,
           timeinfo.tm_mon + 1,
           timeinfo.tm_mday);
  char enhancedPath[351];                                                 // long path buffer (for path with parameters)
  strncpy(enhancedPath, GOOGLE_CALENDAR_PATH "?timeMin=", 349);           // build path
  strncat(enhancedPath, dayStartZuluCstr, 25);
  strncat(enhancedPath, "&timeMax=", 10);
  strncat(enhancedPath, dayEndZuluCstr, 25);
  strncat(enhancedPath, "&access_token=", 15);
  strncat(enhancedPath, googleToken, 350 - strlen(enhancedPath));
  httpEnd();                                                              // end former connection
  delay(500);                                                             // give it some time
  httpConnect(host, port, true);                                          // connect
  httpGetRequest(host, enhancedPath);                                     // send get request
  bool validResponseData = httpWaitAvailable(2000) && https.find("\"summary\": \""); // first occurrance is calendar object
  int itemCnt = 0;
  while (validResponseData && httpWaitAvailable(2000)) {
    char itemText[51] = {0};
    validResponseData = httpFetchAndAdd("\"summary\": \"", itemText, 50); // fetch items for today (including passed items as reminder) and save in temp var
    if (validResponseData) {
      validResponseData = https.find("\"dateTime\": \"");                 // advance to dateTime
      if (validResponseData) {                                  
        validResponseData = httpFetchAndAdd("T", ttrbuf, 5);              // advance to time part (time exactly 5 so only hh:mm) and add
        if (validResponseData) {
          strncat(ttrbuf, " <emphasis level=\"strong\">", TTR_LEFT);      // empasize
          strncat(ttrbuf, itemText, TTR_LEFT);                            // now add item text
          strncat(ttrbuf, "</emphasis>" SSML_PAUSE, TTR_LEFT);            // add pause
          itemCnt++;
        }
      }
    }
  }
  if (!itemCnt) {
    strncat(ttrbuf, "Geen agenda items vandaag.", TTR_LEFT);
  }
  httpEnd();
}

void prepareTTSInfo(char *intro, int nrItems) {                           // paste intro to buffer, add news and clean invalid char(s)
  display.setSegments(TM1637_info);                                       // show "info" on display
  strncpy(ttrbuf, (unsigned)(timeinfo.tm_hour-0) < (12) ? SSML_MORNING : (unsigned)(timeinfo.tm_hour-12) < (6) ? SSML_AFTERNOON : SSML_EVENING, TTR_MAXLEN - 1); // greeting
  strncat(ttrbuf, "Het is vandaag ", TTR_LEFT);
  strncat(ttrbuf, "<say-as interpret-as=\"date\" format=\"yyyymmdd\" detail=\"1\">", TTR_LEFT); strncat(ttrbuf, dateCstr, TTR_LEFT); strncat(ttrbuf, "</say-as>, ", TTR_LEFT); // say date
  strncat(ttrbuf, intro, TTR_LEFT);                                       // add intro text
  ttrAddNews(nrItems);                                                    // add news items from news provider
  ttrAddWeather();                                                        // add weather from weather provider
  ttrAddCalendar();                                                       // add calendar from google
  cleanStr(ttrbuf, "\'");                                                 // clean unwanted characters
}

void requestTTSMP3Data() {                                                // request tts
  stopRadio();                                                            // if still playing...
  host = GCLOUD_TTS_HOST;
  path = GCLOUD_TTS_PATH GCLOUD_KEY;
  httpConnect(host, port, true);                                          // connect to host
  httpPostRequest(host, path, TTSPostData(String(ttrbuf)), "application/json;charset=UTF-8"); // request stream
  streamType = TTS_STREAM;                                                // stream should be picked up by the right handler
  digitalWrite (MUTE_PIN, HIGH);                                          // un-mute
  volumeDesired = VOL_HIGH;                                               // for speach set volume high immediately (no regulation)
  volume = VOL_HIGH;
  vsSetVolume(volume);
}
