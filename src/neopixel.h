#ifndef NEOPIXEL_H
#define NEOPIXEL_H 1

#include "constants.h"
#include "general.h"

void startAnim_Cylon();
void stopAnim_All();
void endAnim_All();
void updateAnim_PixelExponentialIn(AnimationParam param);
void updateAnim_StripExponentialIn(AnimationParam param);
void updateAnim_PixelExponentialOut(AnimationParam param);
void startAnim_CylonMoveOne(int direction, int lastPosition, unsigned long runUntil);
void startAnim_wideColors();

#endif