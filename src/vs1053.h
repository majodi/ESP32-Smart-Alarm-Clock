#ifndef VS1053_H
#define VS1053_H 1

#include "constants.h"
#include "general.h"

void vsReset();
uint16_t vsReadRegister(uint8_t register);
void vsWriteRegister(uint8_t register, uint16_t value);
void vsWriteBuffer(uint8_t *buffer, uint8_t size);
void vsWriteWram(uint16_t address, uint16_t data);

void vsSineTest();
void spiwrite(uint8_t c);
void spiwrite(uint8_t *c, uint16_t num);


#endif
