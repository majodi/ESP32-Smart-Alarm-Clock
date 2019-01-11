#ifndef NEOPIXEL_H
#define NEOPIXEL_H 1

#include "constants.h"

void neoShow();
uint32_t Wheel(byte WheelPos);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);

#endif