#include "neopixel.h"

// //---
//   // ====> adafruit

//   strip.begin(); // This initializes the NeoPixel library.
//   strip.show();
//   colorWipe(strip.Color(255, 0, 0), 50);
//   delay(500);
//   colorWipe(strip.Color(0, 255, 0), 50);  // Green
//   delay(500);
//   colorWipe(strip.Color(0, 0, 255), 50);  // Blue
//   delay(250);
//   colorWipe(strip.Color(0, 0, 255), 50);
//   delay(500);
//   colorWipe(strip.Color(0, 255, 0), 50);  // Green
//   delay(500);
//   colorWipe(strip.Color(255, 0, 0), 50);  // Blue
//   delay(500);
//   theaterChase(strip.Color(127, 127, 127), 50); // White
//   theaterChase(strip.Color(127,   0,   0), 50); // Red
//   theaterChase(strip.Color(  0,   0, 127), 50); // Blue
//   rainbow(20);
//   rainbowCycle(20);
//   theaterChaseRainbow(50);
//   colorWipe(strip.Color(0, 0, 0), 50);

//   // ====> adafruit
// //---

void neoShow() {
  strip.show();                                                           // NeoPixel is on 5v level so switching from ESP 3.3v needs repeat (otherwise a levelshifter would be needed)
  delay(10);
  strip.show();                                                           // works fairly well, for 100% use levelshifter
  // delay(10);
  // strip.show();                                                           // works fairly well, for 100% use levelshifter
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    neoShow();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    neoShow();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    neoShow();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      neoShow();
      delay(wait);
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      neoShow();
      delay(wait);
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
