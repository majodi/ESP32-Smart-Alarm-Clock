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
  host = NEWS_API_HOST;                                                   // connect to news provider
  path = NEWS_API_PATH NEWS_API_KEY;
  secureConnect = true;
  httpConnect(host, port);
  httpGetRequest(host, path);                                             // send get request
  httpWaitAvailable(2000);                                                // wait for response with timeout
  int ttrLen = strlen(ttrbuf);                                            // length of existing ttr (text to read) buffer
  bool validResponseData = https.find(",\"title\":\"");                   // see if we received a (first title)
  httpWaitAvailable(2000);                                                // be sure to have more to read
  while (https.available() && validResponseData && ttrLen < (TTR_MAXLEN - TTR_MAXNEWSITEMLEN) && itemCnt) { // get title texts
    ttrbuf[ttrLen + https.readBytesUntil('"', ttrbuf + ttrLen, TTR_MAXNEWSITEMLEN)] = 0; // only the title text (long enough for our purpose)
    ttrLen = strlen(ttrbuf);
    itemCnt--;
    validResponseData = https.find(",\"title\":\"");                      // prepare pointer to next item
  }
  https.stop();                                                           // don't need this server anymore
}

void prepareTTSNews(char *intro, int nrItems) {                           // paste intro to buffer, add news and clean invalid char(s)
  strncpy(ttrbuf, intro, TTR_MAXLEN - 1);
  ttrAddNews(nrItems);
  cleanStr(ttrbuf, "\'");
}

void requestTTSMP3Data() {                                                // request tts
  host = GCLOUD_TTS_HOST;
  path = GCLOUD_TTS_PATH GCLOUD_KEY;
  secureConnect = true;
  httpConnect(host, port);                                                // connect to host
  httpPostRequest(host, path, TTSPostData(String(ttrbuf)), "application/json;charset=UTF-8"); // request stream
}
