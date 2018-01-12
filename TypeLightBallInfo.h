// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LightBallInfoType.h
// for more information see *.ino
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Include Guard
#ifndef TYPELIGHTBALLINFO_H_
#define TYPELIGHTBALLINFO_H_

// core arduino
#include <Arduino.h>

// Device Configuration
struct tLightBallInfo {
  uint8_t ballID;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t infrared;
  uint16_t waitDuration;
};

#endif  // TYPELIGHTBALLINFO_H_
