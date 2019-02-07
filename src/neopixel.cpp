#include "neopixel.h"

void startAnim_Cylon() {
  uint16_t indexAnim;
  if (animations.NextAvailableAnimation(&indexAnim, 0)) {
    animationState[indexAnim].StartingColor = HslColor(240 / 360.0f, 0.0f, 0.0f);
    animationState[indexAnim].EndingColor = HslColor(240 / 360.0f, 1.0f, 0.5f);
    animationState[indexAnim].IndexPixel = 3;
    animationState[indexAnim].postFunction = nullptr;
    animationState[indexAnim].startPixel = 0;
    animationState[indexAnim].currentDirection = 0;
    animationState[indexAnim].currentHue = 0;
    animationState[indexAnim].runUntil = 0;
    animations.StartAnimation(indexAnim, 6000, updateAnim_PixelExponentialIn);
    animationsEnded = false;
  }
  if (animations.NextAvailableAnimation(&indexAnim, 0)) {
    animationState[indexAnim].StartingColor = HslColor(240 / 360.0f, 0.0f, 0.0f);
    animationState[indexAnim].EndingColor = HslColor(240 / 360.0f, 1.0f, 0.5f);
    animationState[indexAnim].IndexPixel = 4;
    animationState[indexAnim].postFunction = startAnim_CylonMoveOne;
    animationState[indexAnim].startPixel = 2;
    animationState[indexAnim].currentDirection = 1;
    animationState[indexAnim].currentHue = 0;
    animationState[indexAnim].runUntil = millis() + 30000;
    animations.StartAnimation(indexAnim, 6000, updateAnim_PixelExponentialIn);
    animationsEnded = false;
  }
}

void stopAnim_All() {                                                     // stop all running animations (handleNeoPixel() will call endAnim_All())
  for (int i=0; i < animCount; i=i+1) {                                   // for all animations
    if (animations.IsAnimationActive(i)) {                                // if animation is running
      animations.StopAnimation(i);                                        // stop animation
    }
  }
}

void endAnim_All() {                                                      // called when all animations are done
  strip.ClearTo(RgbColor(0, 0, 0));                                       // set color to black for all pixels
  strip.Show();                                                           // and show
  animationsEnded = true;                                                 // signal animation ended
  if (alarmState == ALARM_ACTIVE && currentAlarmSection == AS_ANIMATION ) { // if in alarm state and still in animation (not yet switched by alarm handler)
    nextAlarmSection(false);                                              // start next alarm section (force = false, not needed, animations ended)
  }
}

void updateAnim_PixelExponentialIn(AnimationParam param) {
  float progress = NeoEase::ExponentialIn(param.progress);
  RgbColor updatedColor = RgbColor::LinearBlend(animationState[param.index].StartingColor, animationState[param.index].EndingColor, progress);
  strip.SetPixelColor(animationState[param.index].IndexPixel, updatedColor);
  if (param.progress == 1.0f && animationState[param.index].postFunction != nullptr) {
    animationState[param.index].postFunction(animationState[param.index].currentDirection, animationState[param.index].startPixel, animationState[param.index].runUntil);
  }
}

void updateAnim_StripExponentialIn(AnimationParam param) {
  float progress = NeoEase::ExponentialIn(param.progress);
  RgbColor updatedColor = RgbColor::LinearBlend(animationState[param.index].StartingColor, animationState[param.index].EndingColor, progress);
  strip.ClearTo(updatedColor);
  if (param.progress == 1.0f && millis() < animationState[param.index].runUntil) {
    uint16_t indexAnim;
    if (animations.NextAvailableAnimation(&indexAnim, 0)) {
      int nextHue = animationState[param.index].currentHue + 30;
      nextHue = nextHue > 360 ? 30 : nextHue;
      animationState[indexAnim].StartingColor = animationState[param.index].EndingColor;
      animationState[indexAnim].EndingColor = HslColor(nextHue / 360.0f, 1.0f, 0.5f);
      animationState[indexAnim].IndexPixel = 0;
      animationState[indexAnim].postFunction = nullptr;
      animationState[indexAnim].startPixel = 0;
      animationState[indexAnim].currentDirection = 0;
      animationState[indexAnim].currentHue = nextHue;
      animationState[indexAnim].runUntil = animationState[param.index].runUntil;
      animations.StartAnimation(indexAnim, 6000, updateAnim_StripExponentialIn);
      animationsEnded = false;
    }
  }
}

void updateAnim_PixelExponentialOut(AnimationParam param) {
  float progress = NeoEase::ExponentialOut(param.progress);
  RgbColor updatedColor = RgbColor::LinearBlend(animationState[param.index].StartingColor, animationState[param.index].EndingColor, progress);
  strip.SetPixelColor(animationState[param.index].IndexPixel, updatedColor);
  if (param.progress == 1.0f && animationState[param.index].postFunction != nullptr) {
    animationState[param.index].postFunction(animationState[param.index].currentDirection, animationState[param.index].startPixel, animationState[param.index].runUntil);
  }
}

void startAnim_CylonMoveOne(int lastDirection, int lastPosition, unsigned long runUntil) {
  if ((lastPosition + (lastDirection * 3)) >= strip.PixelCount() || (lastPosition + (lastDirection * 3)) < 0) {
    lastPosition = lastPosition + (lastDirection * 3);
    lastDirection *= -1;
  }
  strip.SetPixelColor(lastPosition + (lastDirection * 3), HslColor(240 / 360.0f, 1.0f, 0.5f));
  if (millis() < runUntil) {
    uint16_t indexAnim;
    if (animations.NextAvailableAnimation(&indexAnim, 0)) {
      animationState[indexAnim].StartingColor = HslColor(240 / 360.0f, 1.0f, 0.5f);
      animationState[indexAnim].EndingColor = HslColor(240 / 360.0f, 0.0f, 0.0f);
      animationState[indexAnim].IndexPixel = lastPosition + lastDirection;
      animationState[indexAnim].postFunction = startAnim_CylonMoveOne;
      animationState[indexAnim].startPixel = lastPosition + lastDirection;
      animationState[indexAnim].currentDirection = lastDirection;
      animationState[indexAnim].currentHue = 0;
      animationState[indexAnim].runUntil = runUntil;
      animations.StartAnimation(indexAnim, 300, updateAnim_PixelExponentialOut);
      animationsEnded = false;
    }
  } else {
    startAnim_wideColors();
  }
}

void startAnim_wideColors() {
  strip.ClearTo(RgbColor(0, 0, 0), 0, 7);
  strip.Show();
  uint16_t indexAnim;
  if (animations.NextAvailableAnimation(&indexAnim, 0)) {
    animationState[indexAnim].StartingColor = HslColor(240 / 360.0f, 0.0f, 0.0f);
    animationState[indexAnim].EndingColor = HslColor(240 / 360.0f, 1.0f, 0.5f);
    animationState[indexAnim].IndexPixel = 0;
    animationState[indexAnim].postFunction = nullptr;
    animationState[indexAnim].startPixel = 0;
    animationState[indexAnim].currentDirection = 0;
    animationState[indexAnim].currentHue = 240;
    animationState[indexAnim].runUntil = millis() + 60000;
    animations.StartAnimation(indexAnim, 6000, updateAnim_StripExponentialIn);
    animationsEnded = false;
  }
}
