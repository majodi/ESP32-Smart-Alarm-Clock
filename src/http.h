#ifndef HTTP_H
#define HTTP_H 1

#include "constants.h"
#include "general.h"

void httpConnect(char *host, int port, bool secure);
void httpEnd();
void httpGetRequest(char *host, char *path);
void httpPostRequest(char *host, char *path, String postData, String contentType);
bool httpWaitAvailable(int timeOutTime);
bool httpFetchAndAdd(char *token, char *target, int maxlen);
void logResponse(int nrBytes);

#endif