#ifndef DEVICEIDENTITY_H_
#define DEVICEIDENTITY_H_

// Includes Core Arduino functionality
#include <Arduino.h>

// device identity structure definitions
#include "DeviceIdentityStruct.h"

#include "printHelper.h"

#include <EEPROMEx.h>


// const uint16_t eeprom_Address_DeviceHardware;
// const uint16_t eeprom_Address_DeviceConfig;
const uint16_t eeprom_Address_DeviceHardware = 0;
const uint16_t eeprom_Address_DeviceConfig =
  eeprom_Address_DeviceHardware + sizeof(tDeviceHardware);

void eeprom_DeviceHW_print(Print &pOut, tDeviceHardware *dhwTemp);
uint8_t eeprom_DeviceHW_read(tDeviceHardware *dhwNew);
uint8_t eeprom_DeviceHW_update(Print &pOut, tDeviceHardware *dhwNew);

void eeprom_DeviceConfig_print(Print &pOut, tDeviceConfig *dconfTemp);
uint8_t eeprom_DeviceConfig_read(tDeviceConfig *dconfNew);
uint8_t eeprom_DeviceConfig_update(Print &pOut, tDeviceConfig *dconfNew);

#endif  // DEVICEIDENTITY_H_
