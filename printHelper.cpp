#include "./printHelper.h"


void printRFM69Frequence(Print &pOut, const uint8_t cbFrequence) {
  switch (cbFrequence) {
    case RF69_433MHZ: {
      pOut.print(F("433MHz"));
    } break;
    case RF69_868MHZ: {
      pOut.print(F("868MHz"));
    } break;
    case RF69_915MHZ: {
      pOut.print(F("915MHz"));
    } break;
  }
}

void printRFM69Info(
  Print &pOut,
  tDeviceConfig *dconfThis,
  tDeviceHardware *dhwThis
) {
  // get local things
  // tDeviceConfig *dconfThis = &dconfThisBall;
  // tDeviceHardware *dhwThis = &dhwThisBall;

  pOut.println(F("RFM69 Info:"));
  pOut.print(F("\t radio_Frequency: "));
  printRFM69Frequence(pOut, (*dhwThis).radio_Frequency);
  pOut.println();
  pOut.print(F("\t NetworkID: "));
  pOut.println((*dconfThis).bNetworkID);
  pOut.print(F("\t BallID: "));
  pOut.println((*dconfThis).bBallID);
  pOut.print(F("\t MasterID: "));
  pOut.println((*dconfThis).bMasterID);

  // pOut.println(F("\t sw fixed values: "));
  // pOut.print(F("\t   radio_ACKTime: "));
  // pOut.println(radio_ACKTime);
  // pOut.print(F("\t   radio_PromiscuosMode: "));
  // pOut.println(radio_PromiscuosMode);
  // pOut.print(F("\t   radio_KEY: "));
  // pOut.println(radio_KEY);
}
