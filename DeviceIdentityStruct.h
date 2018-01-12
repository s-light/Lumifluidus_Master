// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// DeviceIdentityStruct.h
//  Identity structures
//
//  for more information see *.ino
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/** Serial.print to Flash: Notepad++ Replace RegEx
  Find what:		Serial.print(.*)\("(.*)"\);
  Replace with:	Serial.print\1(F("\2"));
**/


// Include Guard
#ifndef DEVICEIDENTITYSTRUCT_H_
#define DEVICEIDENTITYSTRUCT_H_


// **********************************************************************************************
// includes
// **********************************************************************************************
// core arduino
#include <Arduino.h>

// **********************************************************************************************
// type and struct definition
// **********************************************************************************************

/**/
// Individual Hardware Addresses
struct tDeviceHardware {
  uint16_t ID;
  uint8_t radio_Frequency;
};

// Device Configuration
struct tDeviceConfig {
  uint8_t bBallID;
  uint8_t bNetworkID;
  uint8_t bMasterID;
  uint8_t bBallStartAddress;
  uint16_t uiFadeTime;
};
/**/


#endif  // DEVICEIDENTITYSTRUCT_H_
