#ifndef VS1053_H
#define VS1053_H 1

#include "constants.h"
#include "general.h"
#include "handlers.h"
#include "blynkio.h"
#include <Base64.h>

void vsReset();
uint16_t vsReadRegister(uint8_t register);
void vsWriteRegister(uint8_t register, uint16_t value);
void vsWriteBuffer(uint8_t *buffer, uint8_t size);
void vsWriteWram(uint16_t address, uint16_t data);
void vsSetVolume(uint8_t vol);
void regulateVolume();
void startRadio(char *host, int port, char *path);
void stopRadio();
void transferAvailableMP3Data();
void transferAvailableTTSMP3Data();
void vsSineTest(uint_least32_t);

#endif
