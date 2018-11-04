#ifndef HTTP_H
#define HTTP_H 1

#include "constants.h"
#include "general.h"

void httpConnect(char *host, int port);
void httpGetRequest(char *host, char *path);
void httpPostRequest(char *host, char *path, String postData, String contentType);
void httpWaitAvailable(int timeOutTime);

#endif