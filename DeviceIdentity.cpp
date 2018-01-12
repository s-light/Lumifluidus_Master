/** Include yourselfs header file **/
#include "./DeviceIdentity.h"

// #include <EEPROMEx.h>

/************************************************/
/**  EEPROMEx functions                        **/
/************************************************/

// print Helper
// void print_HEX(Print &pOut, uint8_t bValue) {
//   if( bValue < 0x10) {
//   pOut.print("0");
//   }
//   pOut.print(bValue,HEX);
// }
//
// void print_MAC(Print &pOut, uint8_t *array) {
//   //pOut.print(F(" "));
//   uint8_t bIndex = 0;
//   print_HEX(pOut, array[bIndex]);
//   for(bIndex = 1; bIndex < 6; bIndex++){
//   pOut.print(F(", "));
//   print_HEX(pOut, array[bIndex]);
//   }
// }
//
//
// uint8_t print_AlignRight_uint16_t(Print &pOut, uint16_t wValue) {
//   uint8_t bLeadingZeros = 0;
//   if (wValue < 10000) {
//   bLeadingZeros = bLeadingZeros + 1;
//   pOut.print(F(" "));
//   if (wValue < 1000) {
//   bLeadingZeros = bLeadingZeros + 1;
//   Serial.print(F(" "));
//   if (wValue < 100) {
//   bLeadingZeros = bLeadingZeros + 1;
//   Serial.print(F(" "));
//   if (wValue < 10) {
//   bLeadingZeros = bLeadingZeros + 1;
//   Serial.print(F(" "));
//   }
//   }
//   }
//   }
//   pOut.print(wValue);
//   return bLeadingZeros;
// }

// const uint16_t eeprom_Address_DeviceHardware = 0;
// const uint16_t eeprom_Address_DeviceConfig =
//   eeprom_Address_DeviceHardware + sizeof(tDeviceHardware);


void eeprom_DeviceHW_print(Print &pOut, tDeviceHardware *dhwTemp) {
  /* *
  // Individual Hardware Addresses
  struct tDeviceHardware {
  uint16_t ID;
  };
  * */
  pOut.println(F("\t deviceHW values:"));
  pOut.print(F("\t   ID: "));
  pOut.print((*dhwTemp).ID);
  pOut.println();
  pOut.print(F("\t   radio_Frequency: "));
  printRFM69Frequence(pOut, (*dhwTemp).radio_Frequency);
  pOut.println();
}

// return:  >0 = success
uint8_t eeprom_DeviceHW_read(tDeviceHardware *dhwNew) {
  // read EEPROM and update dpThisDevice.
  uint8_t bResultFlag = 0;
  Serial.println(F("reading DeviceHW values from EEPROM:"));
  Serial.print(F("\t eeprom_Address_DeviceHardware:"));
  Serial.println(eeprom_Address_DeviceHardware);
  Serial.print(F("\t sizeof(tDeviceHardware):"));
  Serial.print(sizeof(tDeviceHardware));
  Serial.println(F(" uint8_ts"));
  tDeviceHardware dhwTemp;
  uint8_t bReaduint8_t = 0;
  bReaduint8_t = EEPROM.readBlock(eeprom_Address_DeviceHardware, dhwTemp);
  if ( bReaduint8_t == sizeof(tDeviceHardware) ) {
  bResultFlag = 1;
  Serial.print(F("\t   read "));
  Serial.print(bReaduint8_t);
  Serial.println(F(" uint8_ts"));
  // copy data to local struct.
  // (*dhwNew).ID = dhwTemp.ID;
  memcpy(dhwNew, &dhwTemp, sizeof(tDeviceHardware));
  } else {
  // something went wrong.
  bResultFlag = 0;
  Serial.println(F("\t there occurred an error while reading EEPROM."));
  Serial.print(F("\t   read "));
  Serial.print(bReaduint8_t);
  Serial.println(F(" uint8_ts"));
  }
  return bResultFlag;
}

// return:  >0 = success
uint8_t eeprom_DeviceHW_update(Print &pOut, tDeviceHardware *dhwNew) {
  uint8_t bResultFlag = 0;
  // update EEPROM with dhwNew.
  pOut.println(F("update Device Info in EEPROM:"));
  // pOut.print(F("  free RAM = "));
  // pOut.println(freeRam());
  pOut.print(F("\t eeprom_Address_DeviceHardware: "));
  pOut.println(eeprom_Address_DeviceHardware);
  pOut.print(F("\t sizeof(tDeviceHardware): "));
  pOut.print(sizeof(tDeviceHardware));
  pOut.println(F(" uint8_ts"));
  uint8_t bWrittenuint8_t = 0;
  bWrittenuint8_t = EEPROM.updateBlock(
    eeprom_Address_DeviceHardware,
    (*dhwNew));
  // pOut.print(F("  free RAM = "));
  // pOut.println(freeRam());
  if ( bWrittenuint8_t > 0 ) {
  pOut.print(F("\t   updated "));
  pOut.print(bWrittenuint8_t);
  pOut.println(F(" uint8_ts"));
  // pOut.println(F(" is it a timing thing?"));
  pOut.println(F("\t succesfull updated values."));
  } else {
  pOut.println(F("\t nothing to update. values are equal."));
  }
  return bResultFlag;
}



void eeprom_DeviceConfig_print(Print &pOut, tDeviceConfig *dconfTemp) {
  /*
  // Device Configuration
  struct tDeviceConfig {
  uint8_t bBallID;
  uint8_t bNetworkID;
  uint8_t bMasterID;
  uint8_t bBallStartAddress;
  uint16_t uiFadeTime;
  };
  */

  pOut.print(F("\t tDeviceConfig:"));
  pOut.println();
  pOut.print(F("\t   bBallID : "));
  pOut.print((*dconfTemp).bBallID);
  pOut.println();
  pOut.print(F("\t   bNetworkID : "));
  pOut.print((*dconfTemp).bNetworkID);
  pOut.println();
  pOut.print(F("\t   bMasterID : "));
  pOut.print((*dconfTemp).bMasterID);
  pOut.println();
  pOut.print(F("\t   bBallStartAddress : "));
  pOut.print((*dconfTemp).bBallStartAddress);
  pOut.println();
  pOut.print(F("\t   uiFadeTime : "));
  pOut.print((*dconfTemp).uiFadeTime);
  pOut.println();
}

// return:  >0 = success
uint8_t eeprom_DeviceConfig_read(tDeviceConfig *dconfNew) {
  // read EEPROM and update dpThisDevice.
  uint8_t bResultFlag = 0;
  Serial.println(F("reading DeviceConf values from EEPROM:"));
  Serial.print(F("\t eeprom_Address_DeviceConfig:"));
  Serial.println(eeprom_Address_DeviceConfig);
  Serial.print(F("\t sizeof(tDeviceConfig):"));
  Serial.print(sizeof(tDeviceConfig));
  Serial.println(F(" uint8_ts"));
  tDeviceConfig dconfTemp;
  uint8_t bReaduint8_t = 0;
  bReaduint8_t = EEPROM.readBlock(eeprom_Address_DeviceConfig, dconfTemp);
  if ( bReaduint8_t == sizeof(tDeviceConfig) ) {
  Serial.print(F("\t   read "));
  Serial.print(bReaduint8_t);
  Serial.println(F(" uint8_ts"));
  // copy data to local struct.
  // --> is it possible to use memcpy ??
  // (*dconfNew).uiFadeTime = dconfTemp.uiFadeTime;
  memcpy(dconfNew, &dconfTemp, sizeof(tDeviceConfig));
  bResultFlag = 1;
  } else {
  // something went wrong.
  Serial.println(F("\t there occurred an error while reading EEPROM."));
  Serial.print(F("\t   read "));
  Serial.print(bReaduint8_t);
  Serial.println(F(" uint8_ts"));
  bResultFlag = 0;
  }
  return bResultFlag;
}

// return:  >0 = success
uint8_t eeprom_DeviceConfig_update(Print &pOut, tDeviceConfig *dconfNew) {
  // update EEPROM with dconfNew.
  uint8_t bResultFlag = 0;
  pOut.println(F("update Device Info in EEPROM:"));
  pOut.print(F("\t eeprom_Address_DeviceConfig: "));
  pOut.println(eeprom_Address_DeviceConfig);
  pOut.print(F("\t sizeof(tDeviceConfig): "));
  pOut.print(sizeof(tDeviceConfig));
  pOut.println(F(" uint8_ts"));
  uint8_t bWritenuint8_t = 0;
  bWritenuint8_t = EEPROM.updateBlock(eeprom_Address_DeviceConfig, (*dconfNew));
  if ( bWritenuint8_t > 0 ) {
  pOut.print(F("\t   updated "));
  pOut.print(bWritenuint8_t);
  pOut.println(F(" uint8_ts"));
  pOut.println(F("\t successful updated values."));
  bResultFlag = 2;
  } else {
  pOut.println(F("\t nothing to update. values are equal."));
  bResultFlag = 1;
  }
  return bResultFlag;
}
