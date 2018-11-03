#include "http.h"

void httpConnect(char *host, int port) {
  bool success = secureConnect ? https.connect(host, port = 80 ? 443 : port) : http.connect(host, port);
  if (!success) {                                                         // conected?
    connected = false;
    slog("Connection failed");
  } else {
    connected = true;
  }
}

String TTSPostData(String text) {
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
"       'audioEncoding':'MP3'\r\n" +
"     }\r\n" +
"   }";
}

void httpGetRequest(char *host, char *path) {
  String getRequest = String("GET ") + path + " HTTP/1.1\r\n" +
                                      "Host: " + host + "\r\n" + 
                                      "Connection: close\r\n\r\n";
  if (secureConnect) {
    https.print(getRequest);
  } else {
    http.print(getRequest);
  }
}

void httpPostRequest(char *host, char *path, String postData, String contentType) { // application/json;charset=UTF-8
  String postRequest = String("POST ") + path + " HTTP/1.1\r\n" +
                                                 "Host: " + host + "\r\n" + 
                                                 "Connection: close\r\n" +
                                                 "Content-Length: " + postData.length() + "\r\n" +
                                                 "Content-Type: " + contentType + "\r\n\r\n" +
                                                 postData + "\r\n";
  if (secureConnect) {
    https.print(postRequest);
  } else {
    http.print(postRequest);
  }
}
