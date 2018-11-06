#include "ttr.h"

String TTSPostData(String text) {                                         // prepare tts post request
  return String("{\r\n") +
"     'input':{\r\n" +
"       'text':'" + text + "'\r\n" +
"     },\r\n" +
"     'voice':{\r\n" +
"       'languageCode':'nl-NL',\r\n" +
"       'name':'nl-NL-Wavenet-A',\r\n" +
"       'ssmlGender':'FEMALE'\r\n" +
"     },\r\n" +
"     'audioConfig':{\r\n" +
"       'audioEncoding':'MP3',\r\n" +
"       'speakingRate':'0.8'\r\n" +
"     }\r\n" +
"   }";
}

void ttrAddNews(int nrItems) {                                            // add news items to tts buffer
  int itemCnt = nrItems;
  strncat(ttrbuf, NEWS_INTRO, TTR_MAXLEN - 1);                            // add intro text
  host = NEWS_API_HOST;                                                   // connect to news provider
  path = NEWS_API_PATH NEWS_API_KEY;
  httpConnect(host, port, true);
  httpGetRequest(host, path);                                             // send get request
  int ttrLen = strlen(ttrbuf);                                            // length of existing ttr (text to read) buffer
  bool validResponseData = httpWaitAvailable(2000) && https.find(",\"title\":\"");                   // see if we received a (first title)
  while (validResponseData && ttrLen < (TTR_MAXLEN - TTR_MAXNEWSITEMLEN) && itemCnt && httpWaitAvailable(2000)) { // get title texts
    ttrbuf[ttrLen + https.readBytesUntil('"', ttrbuf + ttrLen, TTR_MAXNEWSITEMLEN)] = 0; // only the title text (long enough for our purpose)
    ttrLen = strlen(ttrbuf);
    itemCnt--;
    validResponseData = https.find(",\"title\":\"");                      // prepare pointer to next item
  }
  https.stop();                                                           // don't need this server anymore
}

void ttrAddWeather() {                                                    // add news items to tts buffer
  strncat(ttrbuf, WEATHER_INTRO, TTR_MAXLEN - 1);                         // add intro text
  host = WEATHER_API_HOST;                                                // connect to weather provider
  path = WEATHER_API_PATH WEATHER_API_KEY;
  httpConnect(host, port, false);
  httpGetRequest(host, path);                                             // send get request
  httpFetchAndAdd(", \"temp\": \"", ttrbuf, TTR_MAXNEWSITEMLEN);
  httpFetchAndAdd(", \"verw\": \"", ttrbuf, TTR_MAXNEWSITEMLEN);
  http.stop();                                                            // don't need this server anymore
}

void ttrAddCalendar() {                                                   // add calendar items to tts buffer
  strncat(ttrbuf, CALENDAR_INTRO1, TTR_MAXLEN - 1);                       // add intro text
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
  char enhancedPath[350];                                                 // long path buffer (for path with parameters)
  strncpy(enhancedPath, GOOGLE_CALENDAR_PATH "?timeMin=", 349);           // build path
  strncat(enhancedPath, dayStartZuluCstr, 25);
  strncat(enhancedPath, "&timeMax=", 10);
  strncat(enhancedPath, dayEndZuluCstr, 25);
  strncat(enhancedPath, "&access_token=", 15);
  strncat(enhancedPath, googleToken, 349 - strlen(enhancedPath));
  https.stop();                                                           // end former connection
  delay(200);                                                             // give it some time
  httpConnect(host, port, true);                                          // connect
  httpGetRequest(host, enhancedPath);                                     // send get request
  bool validResponseData = httpWaitAvailable(2000) && https.find("\"summary\": \""); // first occurrance is calendar object
  while (validResponseData && httpWaitAvailable(2000)) {
    validResponseData = httpFetchAndAdd("\"summary\": \"", ttrbuf, sizeof(ttrbuf) - strlen(ttrbuf) - 1); // fetch items for today (including passed items as reminder)
  }
  https.stop();
}

void prepareTTSInfo(char *intro, int nrItems) {                           // paste intro to buffer, add news and clean invalid char(s)
  strncpy(ttrbuf, intro, TTR_MAXLEN - 1);                                 // add intro text
  ttrAddNews(nrItems);                                                    // add news items from news provider
  ttrAddWeather();                                                        // add weather from weather provider
  ttrAddCalendar();                                                       // add calendar from google
  cleanStr(ttrbuf, "\'");                                                 // clean unwanted characters
}

void requestTTSMP3Data() {                                                // request tts
  host = GCLOUD_TTS_HOST;
  path = GCLOUD_TTS_PATH GCLOUD_KEY;
  httpConnect(host, port, true);                                          // connect to host
  httpPostRequest(host, path, TTSPostData(String(ttrbuf)), "application/json;charset=UTF-8"); // request stream
}
