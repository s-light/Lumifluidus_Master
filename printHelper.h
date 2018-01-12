// Include Guard
#ifndef PRINTHELPER_H_
#define PRINTHELPER_H_

// Includes Core Arduino functionality
#include <Arduino.h>

// device identity structure definitions
#include "DeviceIdentityStruct.h"

#include <RFM69.h>

void printRFM69Frequence(Print &pOut, const uint8_t cbFrequence);

void printRFM69Info(
  Print &pOut,
  tDeviceConfig *dconfThis,
  tDeviceHardware *dhwThis
);


#endif  // PRINTHELPER_H_
