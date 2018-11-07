#include "http.h"

void httpConnect(char *host, int port, bool secure) {
  secureConnect = secure;
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

bool httpWaitAvailable(int timeOutTime) {
  if (!(secureConnect ? https.connected() : http.connected())) {return false;}
  int countDown = timeOutTime < 200 ? 200 : timeOutTime;                  // wait at least 200ms
  while (!(secureConnect ? https.available() : http.available()) && (countDown)) {
    delay(1);
    countDown = countDown ? countDown - 1 : 0;
  }
  return countDown == 0 ? false : true;
}

bool httpFetchAndAdd(char *token, char *target, int maxlen) {
  bool validResponseData = httpWaitAvailable(2000) && secureConnect ? https.find(token) : http.find(token); // see if we received token
  slog("fetch found %s: %d", token, validResponseData);
  if (validResponseData && httpWaitAvailable(2000)) {
    if (secureConnect) {
      target[strlen(target) + https.readBytesUntil('"', target + strlen(target), maxlen)] = 0; // only this token
    } else {
      target[strlen(target) + http.readBytesUntil('"', target + strlen(target), maxlen)] = 0; // only this token
    }
    return true;
  } else {
    slog("fetch failed: %s", token);
    return false;
  }
}
