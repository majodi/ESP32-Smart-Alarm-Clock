#ifndef TTR_H
#define TTR_H 1

#include "constants.h"
#include "http.h"

String TTSPostData(String text);
void ttrAddNews(int nrItems);
void prepareTTSNews(char *intro, int nrItems);
void requestTTSMP3Data();

#endif
