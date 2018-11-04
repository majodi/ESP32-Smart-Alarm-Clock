#include "http.h"

void httpConnect(char *host, int port) {
  bool success = secureConnect ? https.connect(host, port = 80 ? 443 : port) : http.connect(host, port);
  if (!success) {                                                         // conected?
    connected = false;
    slog("Connection to %s FAILED", host);
  } else {
    connected = true;
    slog("Connected to %s", host);
  }
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

void httpPostRequest(char *host, char *path, String postData, String contentType) {
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

void httpWaitAvailable(int timeOutTime) {
  int countDown = timeOutTime;
  while (!(secureConnect ? https.available() : http.available()) && !(timeOutTime && !countDown)) {
    delay(1);
    countDown = countDown ? countDown-- : 0;
  }
}
