// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Lumifluidus_Master
// 	uses Hope RFM69 Module to transmit data.
// 	Master for Lumifluidus Project.
// 	debugout on usbserial interface: 115200baud
//
// Functions:
// 	can send 'How Is There' requests and collects answering IDs
// 	than the master can send color information to every listed ID
//
// hardware:
// 	Board:
// 		Arduino compatible
// 		with RFM69 module
// 		for example: Mega1284RFM69_PCB
//
// libraries used:
// 	~ Wire.h  arduino TWI-lib
// 	~ SPI.h arduino SPI-lib
// 	~ EEPROMEx.h
// 		Extended EEPROM library
// 		Copyright (c) 2012 Thijs Elenbaas.  All right reserved.
// 		GNU Lesser General Public  License, version 2.1
// 		http://thijs.elenbaas.net/2012/07/extended-eeprom-library-for-arduino/
// 	~ RFM69.h
// 		RFM69 Library
// 		for Hope RF RFM69W, RFM69HW, RFM69CW, RFM69HCW (semtech SX1231, SX1231H)
// 		Copyright Felix Rusu (2014), felix@lowpowerlab.com
// 		http://lowpowerlab.com/
// 		GNU General Public License 3 (http://www.gnu.org/licenses/gpl-3.0.txt)
// 		https://github.com/LowPowerLab/
// 	~ slight_ButtonInput
// 		written by stefan krueger (s-light),
// 			code@s-light.eu, http://s-light.eu, https://github.com/s-light/
// 		cc by sa, Apache License Version 2.0
// 	~
//
//
// written by stefan krueger (s-light),
// 	code@s-light.eu, http://s-light.eu, https://github.com/s-light/
//
//
// changelog / history
// 	08.02.2015 14:51 created based on 'LightBall_V1.ino'
// 	08.02.2015 14:52 change id.
// 	14.03.2015 13:13 added 'send color' function
// 	02.06.2015 12:35 added 'send ir' function
// 	06.06.2015 23:00 added sequencer for automatic color sending
// 	07.06.2015 07:50 added button for
// 		click:			manual 'sequencer_NextStep()'
// 		doubleclick:	'sequencer_ActiveToggle'
//
// TO DO:
// 	~ implement 'How is there' requests
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// license
//
// GNU GENERAL PUBLIC LICENSE (Version 3, 29 June 2007 or later)
// 	details see LICENSE file
//
// Apache License Version 2.0
// 	Copyright 2015-2018 Stefan Krueger
//
// 	Licensed under the Apache License, Version 2.0 (the "License");
// 	you may not use this file except in compliance with the License.
// 	You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// 	Unless required by applicable law or agreed to in writing, software
// 	distributed under the License is distributed on an "AS IS" BASIS,
// 	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// 	See the License for the specific language governing permissions and
// 	limitations under the License.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Includes:  (must be at the beginning of the file.)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// use "" for files in same directory as .ino
//#include "file.h"

// device identity structure definitions
#include "DeviceIdentityStruct.h"

#include "TypeLightBallInfo.h"
//#include "sequencerData.cpp"
#include "sequenceData.h"

// arduino SPI
#include <SPI.h>

// arduino TWI / I2C lib
// #include <Wire.h>

#include <RFM69.h>

#include <slight_ButtonInput.h>


#include "DeviceIdentity.h"
#include "printHelper.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// info
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void print_info(Print &pOut) {
	pOut.println();
	//             "|~~~~~~~~~|~~~~~~~~~|~~~..~~~|~~~~~~~~~|~~~~~~~~~|"
	pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	pOut.println(F("|                       ^ ^                      |"));
	pOut.println(F("|                      (0,0)                     |"));
	pOut.println(F("|                      ( _ )                     |"));
	pOut.println(F("|                       \" \"                      |"));
	pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	pOut.println(F("| Lumifluidus_Master.ino"));
	pOut.println(F("|  Light Ball MASTER"));
	pOut.println(F("|"));
	pOut.println(F("| This Sketch has a debug-menu:"));
	pOut.println(F("| send '?'+Return for help"));
	pOut.println(F("|"));
	pOut.println(F("| dream on & have fun :-)"));
	pOut.println(F("|"));
	pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	pOut.println(F("|"));
	//pOut.println(F("| Version: Nov 11 2013  20:35:04"));
	pOut.print(F("| version: "));
	pOut.print(F(__DATE__));
	pOut.print(F("  "));
	pOut.print(F(__TIME__));
	pOut.println();
	pOut.println(F("|"));
	pOut.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	pOut.println();

	//pOut.println(__DATE__); Nov 11 2013
	//pOut.println(__TIME__); 20:35:04
}


/** Serial.print to Flash: Notepad++ Replace RegEx
	Find what:		Serial.print(.*)\("(.*)"\);
	Replace with:	Serial.print\1\(F\("\2"\)\);
**/


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// definitions (gloabl)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DebugOut
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

boolean bLEDState = 0;
// const uint8_t cbID_LED_Info = 9;
#if defined(__AVR_ATmega1284P__)
	// use LED_Info on 'Mega1284RFM69_PCB'
	const uint8_t cbID_LED_Info = 1;
#else
	// default
	const uint8_t cbID_LED_Info = 9;
#endif

unsigned long ulDebugOut_LiveSign_TimeStamp_LastAction	= 0;
const uint16_t cwDebugOut_LiveSign_UpdateInterval			= 1000; //ms

boolean bDebugOut_LiveSign_Serial_Enabled	= 0;
boolean bDebugOut_LiveSign_LED_Enabled		= 1;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Device Information
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#include "DeviceInfo.h"

// store this in EEPROM
// This Device --> must be updated in setup to use EEPROM values
tDeviceHardware dhwThisBall = {
	  0,			// uint16_t ID
	RF69_433MHZ	// uint8_t radio_Frequency;
};

tDeviceConfig dconfDefault = {
	     0,		// uint8_t bBallID;
	    42,		// uint8_t bNetworkID;
	     0,		// uint8_t bMasterID;
	     1,		// uint8_t bBallStartAddress;
	  1000,		// uint16_t uiFadeTime; (ms)
};

tDeviceConfig dconfThisBall = {
	     0,		// uint8_t bBallID;
	    42,		// uint8_t bNetworkID;
	     0,		// uint8_t bMasterID;
	     1,		// uint8_t bBallStartAddress;
	  1000,		// uint16_t uiFadeTime; (ms)
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// RFM69
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

RFM69 radio;
// other config values are stored in eeprom.

const uint8_t radio_ACKTime =			  30; // ms

//radio_PromiscuosMode set to 'true' to sniff all packets on the same network
const bool radio_PromiscuosMode =	false;

//radio_KEY has to be same 16 characters/uint8_ts on all nodes, not more not less!
//									'1234567890123456'
// #define radio_KEY 				"thisIsEncryptKey"
#define radio_KEY 					"!Light_Ball_Art!"


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// slight ButtonInput
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const uint8_t myButtonSequencer_ID = 42;
slight_ButtonInput myButtonSequencer(
	myButtonSequencer_ID, // uint8_t cbID_New
	13, // uint8_t cbPin_New,
	myInput_callback_GetInput, // tCbfuncGetInput cbfuncGetInput_New,
	myCallback_onEvent, // tcbfOnEvent cbfCallbackOnEvent_New,
	  30, // const uint16_t cwDuration_Debounce_New = 30,
	1500, // const uint16_t cwDuration_HoldingDown_New = 1000,
	  50, // const uint16_t cwDuration_ClickSingle_New =   50,
	2000, // const uint16_t cwDuration_ClickLong_New =   3000,
	 100  // const uint16_t cwDuration_ClickDouble_New = 1000
);

const uint8_t myButtonInfrared_ID = 14;
slight_ButtonInput myButtonInfrared(
	myButtonInfrared_ID, // uint8_t cbID_New
	14, // uint8_t cbPin_New,
	myInput_callback_GetInput, // tCbfuncGetInput cbfuncGetInput_New,
	myCallback_onEvent, // tcbfOnEvent cbfCallbackOnEvent_New,
	  30, // const uint16_t cwDuration_Debounce_New = 30,
	1500, // const uint16_t cwDuration_HoldingDown_New = 1000,
	  50, // const uint16_t cwDuration_ClickSingle_New =   50,
	2000, // const uint16_t cwDuration_ClickLong_New =   3000,
	 100  // const uint16_t cwDuration_ClickDouble_New = 1000
);


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu Input
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// a string to hold new data
char  sMenu_Input_New[]				= "t1;F:65535;255,255,255,255,255,255,255,255,255,255,255,255;XYZ";
// flag if string is complete
bool bMenu_Input_Flag_BF		= false; // BufferFull
bool bMenu_Input_Flag_EOL		= false;
bool bMenu_Input_Flag_CR		= false;
bool bMenu_Input_Flag_LF		= false;
bool bMenu_Input_Flag_LongLine	= false;
bool bMenu_Input_Flag_SkipRest	= false;

// string for Currently to process Command
char  sMenu_Command_Current[]		= "t1;F:65535;255,255,255,255,255,255,255,255,255,255,255,255;XYZ ";


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Output system
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DualWrite from pYro_65 read more at: http://forum.arduino.cc/index.php?topic=200975.0
class DualWriter : public Print{
	public:
		DualWriter( Print &p_Out1, Print &p_Out2 ) : OutputA( p_Out1 ), OutputB( p_Out2 ){}

		size_t write( uint8_t u_Data ) {
			OutputA.write( u_Data );
			OutputB.write( u_Data );
			return 0x01;
		}
	protected:
		Print &OutputA;
		Print &OutputB;
};

//DualWriter dwOUT( Serial, Serial1);



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// other things...
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// sequencer
bool sequencer_Active = false;
const uint8_t cbPIN_LED_Sequencer = 12;
bool infraed_Active = false;
const uint8_t cbPIN_LED_Infrared = 15;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// functions
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Debug things
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// http://forum.arduino.cc/index.php?topic=183790.msg1362282#msg1362282
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


void printBinary8(uint8_t bIn)  {

	for (unsigned int mask = 0b10000000; mask; mask >>= 1) {
		// check if this bit is set
		if (mask & bIn) {
			Serial.print('1');
		}
		else {
			Serial.print('0');
		}
	}
}

void printBinary12(uint16_t bIn)  {
	//                       B12345678   B12345678
	//for (unsigned int mask = 0x8000; mask; mask >>= 1) {
	for (unsigned int mask = 0b100000000000; mask; mask >>= 1) {
		// check if this bit is set
		if (mask & bIn) {
			Serial.print('1');
		}
		else {
			Serial.print('0');
		}
	}
}

void printBinary16(uint16_t wIn)  {
	for (unsigned int mask = 0b1000000000000000; mask; mask >>= 1) {
	// check if this bit is set
		if (mask & wIn) {
			Serial.print('1');
		}
		else {
			Serial.print('0');
		}
	}
}


void printArray(uint16_t *array, uint8_t bCount) {
	Serial.print(F(" "));
	uint8_t bIndex = 0;
	printuint8_tAlignRight(array[bIndex]);
	for(bIndex = 1; bIndex < bCount; bIndex++){
		Serial.print(F(", "));
		printuint8_tAlignRight(array[bIndex]);
	}
}

void printuint8_tAlignRight(uint8_t bValue) {
	//uint8_t bOffset = 0;
	if (bValue < 100) {
		if (bValue < 10) {
			//bOffset = 2;
			Serial.print(F("  "));
		} else {
			//bOffset = 1;
			Serial.print(F(" "));
		}
	}
	Serial.print(bValue);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu System
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Modes for Menu Switcher
const uint8_t cbMenuMode_MainMenu	= 1;
const uint8_t cbMenuMode_SubMenu1	= 2;
uint8_t bMenuMode = cbMenuMode_MainMenu;


// SubMenu SetValues
void handleMenu_Sub1(Print &pOut, char *caCommand) {
	pOut.println(F("SubMenu1:"));
	pOut.println(F("\t nothing here."));
	pOut.println(F("\t finished."));
	// exit submenu
	// reset state manschine of submenu
	// jump to main
	bMenuMode = cbMenuMode_MainMenu;
	bMenu_Input_Flag_EOL = true;
}


// Main Menu
void handleMenu_Main(Print &pOut, char *caCommand) {
	/* pOut.print("sCommand: '");
	pOut.print(sCommand);
	pOut.println("'"); */
	switch (caCommand[0]) {
		case 'h':
		case 'H':
		case '?': {
			// help
			pOut.println(F("____________________________________________________________"));
			pOut.println();
			pOut.println(F("Help for Commands:"));
			pOut.println();
			pOut.println(F("\t '?': this help"));
			pOut.println(F("\t 'i': sketch info"));
			pOut.println(F("\t 'y': toggle DebugOut livesign print"));
			pOut.println(F("\t 'Y': toggle DebugOut livesign LED"));
			pOut.println(F("\t 'x': tests"));
			pOut.println();
			pOut.println(F("\t 'q': toggle sequencer"));
			pOut.println(F("\t 'T': send runtime 'T255'"));
			pOut.println(F("\t 'c': send color 'c255:255,255,255'"));
			pOut.println(F("\t 'I': send ir 'I255:255'"));
			pOut.println(F("\t 'W': toggle IR all 'q'"));
			pOut.println(F("\t 't': send command to target 't255$COMMAND TO SEND'"));
			// pOut.println(F("\t 'f': DemoFadeTo(ID, value) 'f1:65535'"));
			pOut.println();
			pOut.println(F("\t 'set:' enter SubMenu1"));
			pOut.println();
			pOut.println(F("____________________________________________________________"));
		} break;
		case 'i': {
			print_info(pOut);
		} break;
		case 'y': {
			pOut.println(F("\t toggle DebugOut livesign Serial:"));
			bDebugOut_LiveSign_Serial_Enabled = !bDebugOut_LiveSign_Serial_Enabled;
			pOut.print(F("\t bDebugOut_LiveSign_Serial_Enabled:"));
			pOut.println(bDebugOut_LiveSign_Serial_Enabled);
		} break;
		case 'Y': {
			pOut.println(F("\t toggle DebugOut livesign LED:"));
			bDebugOut_LiveSign_LED_Enabled = !bDebugOut_LiveSign_LED_Enabled;
			pOut.print(F("\t bDebugOut_LiveSign_LED_Enabled:"));
			pOut.println(bDebugOut_LiveSign_LED_Enabled);
		} break;
		case 'x': {
			// get state
			pOut.println(F("__________"));
			pOut.println(F("Tests:"));

			pOut.println(F("nothing to do."));

			// uint16_t wTest = 65535;
			uint16_t wTest = atoi(&caCommand[1]);
			pOut.print(F("wTest: "));
			pOut.print(wTest);
			pOut.println();

			pOut.print(F("1: "));
			pOut.print((uint8_t)wTest);
			pOut.println();

			pOut.print(F("2: "));
			pOut.print((uint8_t)(wTest>>8));
			pOut.println();

			pOut.println();

			pOut.println(F("__________"));
		} break;
		//--------------------------------------------------------------------------------
		case 'q': {
			sequencer_ActiveToggle();
		} break;

		case 'T': {
			pOut.println(F("\t sendRunTime"));
			char *caTempPos = &caCommand[1];

			uint8_t temp_target = atoi(caTempPos);
			// pOut.print(F("\t temp_target: '"));
			// pOut.print(temp_target);
			// pOut.println(F("'"));

			sendRunTime(temp_target);
		} break;

		case 'c': {
			pOut.println(F("\t send Color"));

			pOut.print(F("\t   caCommand: '"));
			pOut.print(caCommand);
			pOut.println(F("'"));

			// c8:50,100,200


			//  0123456789
			// 'c255:255,255,255'
			// 'c1:11,50,100'
			char *caTempPos = &caCommand[1];

			uint8_t temp_target = atoi(caTempPos);
			// pOut.print(F("\t caTempPos: '"));
			// pOut.print(caTempPos);
			// pOut.println(F("'"));

			// find ':'
			// init
			caTempPos = strtok(caTempPos, ": ");
			// get first position
			caTempPos = strtok(NULL, ": ");
			// pOut.print(F("\t caTempPos: '"));
			// pOut.print(caTempPos);
			// pOut.println(F("' (first ':')"));


			uint8_t temp_red = atoi(caTempPos);
			// pOut.print(F("\t caTempPos: '"));
			// pOut.print(caTempPos);
			// pOut.println(F("' (temp_red)"));

			// find ','
			// init
			caTempPos = strtok(caTempPos, ", ");
			// get first position
			caTempPos = strtok(NULL, ", ");
			// pOut.print(F("\t caTempPos: '"));
			// pOut.print(caTempPos);
			// pOut.println(F("'"));

			uint8_t temp_green = atoi(caTempPos);

			// get second position
			caTempPos = strtok(NULL, ", ");
			// pOut.print(F("\t caTempPos: '"));
			// pOut.print(caTempPos);
			// pOut.println(F("'"));

			uint8_t temp_blue = atoi(caTempPos);


			// pOut.print(F("\t temp_red: "));
			// pOut.print(temp_red);
			// pOut.println();
			// pOut.print(F("\t temp_green: "));
			// pOut.print(temp_green);
			// pOut.println();
			// pOut.print(F("\t temp_blue: "));
			// pOut.print(temp_blue);
			// pOut.println();

			sendColor(temp_target, temp_red, temp_green, temp_blue);


		} break;

		case 't': {
			pOut.println(F("\t sendCommandToTarget"));
			sendCommandToTarget(pOut, caCommand);
		} break;

		case 'I': {
			pOut.println(F("\t send IR"));
			pOut.print(F("\t   caCommand: '"));
			pOut.print(caCommand);
			pOut.println(F("'"));
			//  0123456789
			// 'c255:255'
			// 'c1:11'
			char *caTempPos = &caCommand[1];
			uint8_t temp_target = atoi(caTempPos);
			// find ':'
			// init
			caTempPos = strtok(caTempPos, ": ");
			// get first position
			caTempPos = strtok(NULL, ": ");
			uint8_t temp_ir = atoi(caTempPos);
			// send new IR state
			sendIR(temp_target, temp_ir);
		} break;

		case 'W': {
			infraed_ActiveToggle();
		} break;

		//--------------------------------------------------------------------------------
		case 's': {
			// SubMenu1
			if ( (caCommand[1] == 'e') && (caCommand[2] == 't') && (caCommand[3] == ':') ) {
				//if full command is 'set:' enter submenu
				bMenuMode = cbMenuMode_SubMenu1;
				if(1){	//if ( caCommand[4] != '\0' ) {
					//full length command
					//handle_SetValues(pOut, &caCommand[4]);
				} else {
					bMenu_Input_Flag_EOL = true;
				}
			}
		} break;
		//--------------------------------------------------------------------------------
		default: {
			pOut.print(F("command '"));
			pOut.print(caCommand);
			pOut.println(F("' not recognized. try again."));
			sMenu_Input_New[0] = '?';
			bMenu_Input_Flag_EOL = true;
		}
	} //end switch

	//end Command Parser
}


// Menu Switcher
void menuSwitcher(Print &pOut, char *caCommand) {
	switch (bMenuMode) {
			case cbMenuMode_MainMenu: {
				handleMenu_Main(pOut, caCommand);
			} break;
			case cbMenuMode_SubMenu1: {
				handleMenu_Sub1(pOut, caCommand);
			} break;
			default: {
				// something went wronge - so reset and show MainMenu
				bMenuMode = cbMenuMode_MainMenu;
			}
		} // end switch bMenuMode
}

// Check for NewLineComplete and enter menuSwitcher
// sets Menu Output channel (pOut)
void check_NewLineComplete() {
	// complete line found:
		if (bMenu_Input_Flag_EOL) {
			// Serial.println(F("check_NewLineComplete"));
			// Serial.println(F("  bMenu_Input_Flag_EOL is set. "));
			// Serial.print  (F("    sMenu_Input_New: '"));
			// Serial.print(sMenu_Input_New);
			// Serial.println(F("'"));

			// Serial.println(F("  Flags:"));
			// Serial.print  (F("    bMenu_Input_Flag_BF: '"));
			// Serial.println(bMenu_Input_Flag_BF);
			// Serial.print  (F("    bMenu_Input_Flag_CR: '"));
			// Serial.println(bMenu_Input_Flag_CR);
			// Serial.print  (F("    bMenu_Input_Flag_LF: '"));
			// Serial.println(bMenu_Input_Flag_LF);
			// Serial.print  (F("    bMenu_Input_Flag_EOL: '"));
			// Serial.println(bMenu_Input_Flag_EOL);


			// Serial.println(F("  copy sMenu_Input_New to sMenu_Command_Current."));
			// copy to current buffer
			strcpy(sMenu_Command_Current, sMenu_Input_New);

			// Serial.println(F("  clear sMenu_Input_New"));
			// reset memory
			memset(sMenu_Input_New,'\0',sizeof(sMenu_Input_New)-1);

			// Serial.println(F("  clear bMenu_Input_Flag_EOL"));
			// reset flag
			bMenu_Input_Flag_EOL = false;
			bMenu_Input_Flag_LF = false;

			// print info if things were skipped.
			if (bMenu_Input_Flag_BF) {
				Serial.println(F("input was to long. used first part - skipped rest."));
				bMenu_Input_Flag_BF = false;
			}

			// parse line / run command
			menuSwitcher(Serial, sMenu_Command_Current);


			// Serial.print  (F("    sMenu_Input_New: '"));
			// Serial.print(sMenu_Input_New);
			// Serial.println(F("'"));
			// Serial.print  (F("    sMenu_Command_Current: '"));
			// Serial.print(sMenu_Command_Current);
			// Serial.println(F("'"));


			// Serial.println(F("  check bMenu_Input_Flag_SkipRest"));
			// if ( !bMenu_Input_Flag_SkipRest) {
				// Serial.println(F("   parse Line:"));

				// if (bMenu_Input_Flag_BF) {
					// Serial.println(F("input was to long. used first part - skipped rest."));
					// bMenu_Input_Flag_BF = false;
				// }

				// parse line / run command
				// menuSwitcher(Serial, sMenu_Command_Current);

				// if(bMenu_Input_Flag_LongLine) {
					// bMenu_Input_Flag_SkipRest = true;
					// bMenu_Input_Flag_LongLine = false;
				// }
			// } else {
				// Serial.println(F("   skip rest of Line"));
				// bMenu_Input_Flag_SkipRest = false;
			// }

		}// if Flag complete
}

// serial example
// t1$F:100;255,0,0,0,20,0,0,0,1,255,255,255



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Serial Receive Handling
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void handle_SerialReceive() {
	// collect next input text
	while ((!bMenu_Input_Flag_EOL) && (Serial.available())) {
	// while (Serial.available()) {
		// get the new uint8_t:
		char charNew = (char)Serial.read();
		/*Serial.print(F("charNew '"));
		Serial.print(charNew);
		Serial.print(F("' : "));
		Serial.println(charNew, DEC);*/

		// collect next full line
		/* http://forums.codeguru.com/showthread.php?253826-C-String-What-is-the-difference-between-n-and-r-n
			'\n' == 10 == LineFeed == LF
			'\r' == 13 == Carriage Return == CR
			Windows: '\r\n'
			Linux: '\n'
			Apple: '\r'
		*/
		// check for line end
		switch (charNew) {
			case '\r': {
				// Serial.println(F("incoming char is \\r: set EOL"));
				bMenu_Input_Flag_EOL = true;
				bMenu_Input_Flag_CR = true;
				// bMenu_Input_Flag_LF = false;
			} break;
			case '\n': {
				// Serial.println(F("incoming char is \\n: set EOL"));
				// Serial.println(F("  Flags:"));
					// Serial.print  (F("    bMenu_Input_Flag_BF: '"));
					// Serial.println(bMenu_Input_Flag_BF);
					// Serial.print  (F("    bMenu_Input_Flag_CR: '"));
					// Serial.println(bMenu_Input_Flag_CR);
					// Serial.print  (F("    bMenu_Input_Flag_LF: '"));
					// Serial.println(bMenu_Input_Flag_LF);
					// Serial.print  (F("    bMenu_Input_Flag_EOL: '"));
					// Serial.println(bMenu_Input_Flag_EOL);



				bMenu_Input_Flag_LF = true;

				// Serial.println(F("  check for CR"));
				// check if last char was not CR - if this is true set EOL - else ignore.
				if(!bMenu_Input_Flag_CR) {
					bMenu_Input_Flag_EOL = true;
				} else {
					bMenu_Input_Flag_CR = false;
				}

				// Serial.println(F("  Flags:"));
					// Serial.print  (F("    bMenu_Input_Flag_BF: '"));
					// Serial.println(bMenu_Input_Flag_BF);
					// Serial.print  (F("    bMenu_Input_Flag_CR: '"));
					// Serial.println(bMenu_Input_Flag_CR);
					// Serial.print  (F("    bMenu_Input_Flag_LF: '"));
					// Serial.println(bMenu_Input_Flag_LF);
					// Serial.print  (F("    bMenu_Input_Flag_EOL: '"));
					// Serial.println(bMenu_Input_Flag_EOL);


				// this check also works for windows double line ending
				//if (strlen(sMenu_Input_New) > 0) {
					// bMenu_Input_Flag_EOL = true;
				// }
			} break;
			default: {
				// normal char -
				// add it to the sMenu_Input_New:
				//check for length
				if (strlen(sMenu_Input_New) < (sizeof(sMenu_Input_New)-1) ) {
					sMenu_Input_New[strlen(sMenu_Input_New)] = charNew;
				} else {
					//Serial.println(F(" line to long! ignoring rest of line"));
					// set complete flag so line will be parsed
					// Serial.println(F("Buffer is full: set EOL; set LongLine"));
					//bMenu_Input_Flag_EOL = true;
					bMenu_Input_Flag_BF = true;
					// skip rest of line
					bMenu_Input_Flag_LongLine = true;
				}
			}// default
		}// switch charNew

		//check_NewLineComplete();
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// RFM69
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void handle_RFM69Receive() {
	if ( radio.receiveDone() ) {
		// new packet received

		// oh boy! We've got data!!! Let's save it to my own buffer!
		// https://lowpowerlab.com/forum/index.php/topic,776.msg4990.html#msg4990
		uint8_t tempData[RF69_MAX_DATA_LEN];
		uint8_t datalen = radio.DATALEN;
		uint8_t sender = radio.SENDERID;
		uint8_t target = radio.TARGETID;
		int16_t rssi = radio.RSSI;
		memcpy(tempData, (const void *)radio.DATA, datalen);

		bool bACKreq = radio.ACKRequested();

		// first send ACK.
		if (bACKreq) {
			radio.sendACK();
		}

		// now output data:
		Serial.print("received from '");
		Serial.print(sender, DEC);
		Serial.print("' [RX_RSSI:");
		Serial.print(rssi);
		Serial.print("] : '");
		for (uint8_t i = 0; i < datalen; i++) {
			Serial.print( (char)tempData[i] );
		}
		Serial.print("' ");

		if (bACKreq) {
			Serial.print("--> ACK sent");
		}

		Serial.println();
	}
}

void sendRunTime(uint8_t uiTarget) {
	Serial.println("sendRunTime:");
	char tempData[] = "Hi i am running 000000000000000000ms";
	sprintf(tempData, "Hi i am running %0u ms", millis());

	Serial.print("\t uiTarget: ");
	Serial.println(uiTarget);

	Serial.print("\t tempData: ");
	Serial.println(tempData);

	Serial.print("\t sendWithRetry: ");
	if (  radio.sendWithRetry(uiTarget, tempData, strlen(tempData) )   ) {
		Serial.print(" ok!");
	} else {
		Serial.print(" nothing...");
	}

	Serial.println();
}

void sendColor(uint8_t uiTarget, uint8_t uiRed, uint8_t uiGreen, uint8_t uiBlue) {
	Serial.println("sendColor:");
	char tempData[] = "color:255,255,255";
	sprintf(tempData, "color:%0u,%0u,%0u,", uiRed, uiGreen, uiBlue);

	Serial.print("\t uiTarget: ");
	Serial.println(uiTarget);

	Serial.print("\t tempData: ");
	Serial.println(tempData);

	Serial.print("\t sendWithRetry: ");
	if (  radio.sendWithRetry(uiTarget, tempData, strlen(tempData) )   ) {
		Serial.print(" ok!");
	} else {
		Serial.print(" nothing...");
	}

	Serial.println();
}


void sendFadeColorIndividualASCII(uint8_t uiTarget, uint16_t fadetime, uint8_t colordata[]) {
	// command:
	// F:65535;255,255,255,255,255,255,255,255,255,255,255,255
	Serial.println("sendFadeColorIndividualASCII:");
	char tempData[] = "F:65535;255,255,255,255,255,255,255,255,255,255,255,255";
	sprintf(
		tempData,
		"F:%0u;%0u,%0u,%0u,%0u,%0u,%0u,%0u,%0u,%0u,%0u,%0u,%0u",
		fadetime,
		colordata[0],
		colordata[1],
		colordata[2],
		colordata[3],
		colordata[4],
		colordata[5],
		colordata[6],
		colordata[7],
		colordata[8],
		colordata[9],
		colordata[10],
		colordata[11]
	);

	Serial.print("\t uiTarget: ");
	Serial.println(uiTarget);

	Serial.print("\t tempData: ");
	Serial.println(tempData);

	Serial.print("\t sendWithRetry: ");
	if (  radio.sendWithRetry(uiTarget, tempData, strlen(tempData) )   ) {
		Serial.print(" ok!");
	} else {
		Serial.print(" nothing...");
	}

	Serial.println();
}

void sendIR(uint8_t uiTarget, uint8_t uiIR) {
	Serial.println("sendIR:");
	char tempData[] = "ir:255";
	sprintf(tempData, "ir:%0u", uiIR);

	Serial.print("\t uiTarget: ");
	Serial.println(uiTarget);

	Serial.print("\t tempData: ");
	Serial.println(tempData);

	Serial.print("\t sendWithRetry: ");
	if (  radio.sendWithRetry(uiTarget, tempData, strlen(tempData) )   ) {
		Serial.print(" ok!");
	} else {
		Serial.print(" nothing...");
	}

	Serial.println();
}

void sendIR_all(uint8_t ir_value){
	for (size_t i_target = 1; i_target < 10; i_target++) {
		sendIR(i_target, ir_value);
	}
}




void sendCommandToTarget(Print &out, char *command) {
	out.println(F("send Command To Target:"));

	out.print(F("\t   command: '"));
	out.print(command);
	out.println(F("'"));

	char *tempPos = &command[0];

	// command:
	// t255$COMMANDTOSEND

  // strip target ID
	// // init for ':'
	// tempPos = strtok(tempPos, ": ");
	// out.println(F("\t   init ': '"));
	// out.print(F("\t   tempPos: '"));
	// out.print(tempPos);
	// out.println(F("'"));
  //
	// // get first position
	// tempPos = strtok(NULL, ": ");
	// out.println(F("\t   first ': '"));
	// out.print(F("\t   tempPos: '"));
	// out.print(tempPos);
	// out.println(F("'"));

  // strip 't'
	tempPos += 1;
	out.print(F("\t   tempPos: '"));
	out.print(tempPos);
	out.println(F("'"));

	uint8_t target = atoi(tempPos);
	out.print(F("\t   target: '"));
	out.print(target);
	out.println(F("'"));


	// find first ';'
	tempPos = strtok(tempPos, "$ ");
	out.println(F("\t   find '$ '"));
	out.print(F("\t   tempPos: '"));
	out.print(tempPos);
	out.println(F("'"));

	tempPos = strtok(NULL, "$ ");
	out.println(F("\t   second '$ '"));
	out.print(F("\t   tempPos: '"));
	out.print(tempPos);
	out.print(F("' (length:"));
	out.print(strlen(tempPos));
	out.println(F(")"));


	// // strip 't'
	// tempPos += 1;
	// out.print(F("\t   tempPos: '"));
	// out.print(tempPos);
	// out.println(F("'"));


  // send
	Serial.print("\t sendWithRetry: ");
	if (
		radio.sendWithRetry(target, tempPos, strlen(tempPos))
	) {
		Serial.print(" ok!");
	} else {
		Serial.print(" nothing...");
	}
	Serial.println();
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// input handler
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// slight_ButtonInput things
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

boolean myInput_callback_GetInput(uint8_t bID, uint8_t bPin) {
	// read input invert reading - button closes to GND.
	// check HWB
	// return ! (PINE & B00000100);
	return ! digitalRead(bPin);
}


void myCallback_onEvent(slight_ButtonInput *pInstance, uint8_t bEvent) {

	// Serial.print(F("Instance ID:"));
	// Serial.println((*pInstance).getID());
	uint8_t buttonID = (*pInstance).getID();

	// Serial.print(F("Event: "));
	// (*pInstance).printEvent(Serial, bEvent);
	// Serial.println();

	// show event additional infos:
	switch (bEvent) {
		/*case slight_ButtonInput::event_StateChanged : {
			Serial.print(F("\t state: "));
			(*pInstance).printState(Serial);
			Serial.println();
		} break;*/
		// click
		/*case slight_ButtonInput::event_Down : {
			Serial.println(F("the button is pressed down! do something.."));
		} break;*/
		/*case slight_ButtonInput::event_HoldingDown : {
			Serial.print(F("duration active: "));
			Serial.println((*pInstance).getDurationActive());
		} break;*/
		/*case slight_ButtonInput::event_Up : {
			Serial.println(F("up"));
		} break;*/
		case slight_ButtonInput::event_Click : {
			Serial.println(F("click"));
			switch(buttonID) {
				case myButtonInfrared_ID : {
					Serial.println(F("infraed button"));
					infraed_ActiveToggle();
				} break;
				case myButtonSequencer_ID: {
					Serial.println(F("sequencer button"));
					sequencer_NextStep();
				} break;
			}
		} break;
		case slight_ButtonInput::event_ClickLong : {
			Serial.println(F("click long"));
			switch(buttonID) {
				case myButtonInfrared_ID : {
					Serial.println(F("infraed button"));
					// infraed_ActiveToggle();
				} break;
				case myButtonSequencer_ID: {
					Serial.println(F("sequencer button"));
					sequencer_ActiveToggle();
				} break;
			}
		} break;
		/*case slight_ButtonInput::event_ClickDouble : {
			Serial.println(F("click double"));
		} break;*/
		/*case slight_ButtonInput::event_ClickTriple : {
			Serial.println(F("click triple"));
		} break;*/
		/*case slight_ButtonInput::event_ClickMulti : {
			Serial.print(F("click count: "));
			Serial.println((*pInstance).getClickCount());
		} break;*/
	} //end switch
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// IrToggle
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


void infraed_ActiveToggle() {
	Serial.print(F("infraed_ActiveToggle: "));
	infraed_Active = !infraed_Active;
	Serial.println(infraed_Active);
	if (infraed_Active) {
		digitalWrite(cbPIN_LED_Infrared, LOW);
		sendIR_all(255);
	} else {
		digitalWrite(cbPIN_LED_Infrared, HIGH);
		sendIR_all(0);
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sequencer
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// bool sequencer_Active = false; // now at top of file
uint16_t sequencer_CurrentStep = 0;

unsigned long ulSequence_StartTimeStamp = 0;
uint16_t sequencer_CurrentWaitDuration = 0;

/*tLightBallInfo sequencer_StepData[] = {
	{
		    1,	// uint8_t ballID;
		  100,	// uint8_t red;
		  200,	// uint8_t green;
		    0,	// uint8_t blue;
			0,  // uint8_t infraed;
		 1000, 	// uint16_t waitDuration;
	},
};*/


// imported from sequenceData.h
// tLightBallInfo sequencer_StepData[] = {
// 	// ------------------------------------
// 	// Loop 1
// 	{
// 		    1,	// uint8_t ballID;
// 		  100,	// uint8_t red;
// 		  200,	// uint8_t green;
// 		  200,	// uint8_t blue;
// 			0,  // uint8_t infraed;
// 		 1000, 	// uint16_t waitDuration;
// 	},
// 	{
// 		    2,	// ballID;
// 		  100,	// red;
// 		    0,	// green;
// 		    0,	// blue;
// 			0,  // infraed;
// 		 1000, 	// waitDuration;
// 	},
// };




const uint8_t sequencer_StepCount =
	(uint16_t)sizeof(sequencer_StepData) / sizeof(tLightBallInfo);


void sequencer_NextStep() {
	sequencer_CurrentStep = sequencer_CurrentStep +1;
	if ( sequencer_CurrentStep >= sequencer_StepCount) {
		sequencer_CurrentStep = 0;
	}
	sendColor(
		sequencer_StepData[sequencer_CurrentStep].ballID,
		sequencer_StepData[sequencer_CurrentStep].red,
		sequencer_StepData[sequencer_CurrentStep].green,
		sequencer_StepData[sequencer_CurrentStep].blue
	);
	sendIR(
		sequencer_StepData[sequencer_CurrentStep].ballID,
		sequencer_StepData[sequencer_CurrentStep].infrared
	);
	// set new wait duration
	sequencer_CurrentWaitDuration = sequencer_StepData[sequencer_CurrentStep].waitDuration;
	// reset start time
	ulSequence_StartTimeStamp = millis();
}

void sequencer_ActiveToggle() {
	Serial.print(F("sequencer_ActiveToggle: "));
	sequencer_Active = !sequencer_Active;
	Serial.println(sequencer_Active);
	if (sequencer_Active) {
		digitalWrite(cbPIN_LED_Sequencer, LOW);
	} else {
		digitalWrite(cbPIN_LED_Sequencer, HIGH);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// some things
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
// Setup
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void setup() {

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialise PINs
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//LiveSign
		pinMode(cbID_LED_Info, OUTPUT);
		digitalWrite(cbID_LED_Info, HIGH);

		//Sequencer
		pinMode(cbPIN_LED_Sequencer, OUTPUT);
		digitalWrite(cbPIN_LED_Sequencer, LOW);

		// as of arduino 1.0.1 you can use INPUT_PULLUP

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// init serial
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// for ATmega32U4 devices:
		#if defined (__AVR_ATmega32U4__)
			//wait for arduino IDE to release all serial ports after upload.
			delay(2000);
		#endif

		Serial.begin(115200);

		// for ATmega32U4 devices:
		#if defined (__AVR_ATmega32U4__)
			// Wait for Serial Connection to be Opend from Host or 6second timeout
			unsigned long ulTimeStamp_Start = millis();
			while( (! Serial) && ( (millis() - ulTimeStamp_Start) < 6000 ) ) {
				1;
			}
		#endif

		Serial.println();

		Serial.print(F("# Free RAM = "));
		Serial.println(freeRam());

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Welcom
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		print_info(Serial);

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// read device identity
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Serial.println(F("Device Identity:")); {
			bool bEEPROM_Fine =  eeprom_DeviceHW_read(&dhwThisBall);
			if ( !bEEPROM_Fine ) {
				Serial.println(F("\t EEPROM-HW information are not present. pleas set."));
				//bMenuMode = cbMenuMode_HWID;
			} else {
				eeprom_DeviceHW_print(Serial, &dhwThisBall);
			}

			eeprom_DeviceConfig_read(&dconfThisBall);
			eeprom_DeviceConfig_print(Serial, &dconfThisBall);
		}
		Serial.println(F("\t finished."));

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// setup RFM69
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Serial.print(F("# Free RAM = "));
		Serial.println(freeRam());

		// print all global parameters
		printRFM69Info(Serial, &dconfThisBall, &dhwThisBall);
		Serial.println(F("setup RFM69:")); {

			Serial.println(F("\t --> initialize"));
			radio.initialize(dhwThisBall.radio_Frequency, dconfThisBall.bBallID, dconfThisBall.bNetworkID);

			Serial.println(F("\t --> set HighPower"));
			//uncomment only for RFM69HW!
			radio.setHighPower();

			Serial.println(F("\t --> encrypt"));
			radio.encrypt(radio_KEY);

			// Serial.println(F("\t --> set Frequency:"));
			// set frequency to some custom frequency
			// radio.setFrequency(919000000);

		}
		Serial.println(F("\t --> finished."));

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// start slight_ButtonInput
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		Serial.print(F("# Free RAM = "));
		Serial.println(freeRam());

		Serial.println(F("slight_ButtonInput:"));
		{
			Serial.println(F("\t pinMode INPUT_PULLUP"));
			// pinMode(myButtonSequencer.getPin(), INPUT_PULLUP);
			pinMode(myButtonSequencer.getPin(), INPUT);
			digitalWrite(myButtonSequencer.getPin(), HIGH);
			pinMode(myButtonInfrared.getPin(), INPUT);
			digitalWrite(myButtonInfrared.getPin(), HIGH);

			Serial.println(F("\t begin()"));
			myButtonSequencer.begin();
			myButtonInfrared.begin();

		}
		Serial.println(F("\t finished."));



	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// show Serial Commands
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		// reset Serial Debug Input
		memset(sMenu_Input_New, '\0', sizeof(sMenu_Input_New)-1);
		//print Serial Options
		sMenu_Input_New[0] = '?';
		bMenu_Input_Flag_EOL = true;


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// GO
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		Serial.println(F("Loop:"));



} // setup


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
// Main Loop
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
void loop() {

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Menu Input
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Serial
		handle_SerialReceive();
		check_NewLineComplete();

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// RFM69
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		handle_RFM69Receive();

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// my Button
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		myButtonSequencer.update();
		myButtonInfrared.update();

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Timed things
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if(sequencer_Active) {
			// sequencer timing
			if ( ( millis() - ulSequence_StartTimeStamp ) > sequencer_CurrentWaitDuration) {
				sequencer_NextStep();
			}
		}



	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Debug Out
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if ( (millis() - ulDebugOut_LiveSign_TimeStamp_LastAction) > cwDebugOut_LiveSign_UpdateInterval) {
			ulDebugOut_LiveSign_TimeStamp_LastAction = millis();

			if ( bDebugOut_LiveSign_Serial_Enabled ) {
				Serial.print(millis());
				Serial.print(F("ms;"));
				Serial.print(F("  free RAM = "));
				Serial.println(freeRam());
			}

			if ( bDebugOut_LiveSign_LED_Enabled ) {
				bLEDState = ! bLEDState;
				if (bLEDState) {
					//set LED to HIGH
					digitalWrite(cbID_LED_Info, HIGH);
				} else {
					//set LED to LOW
					digitalWrite(cbID_LED_Info, LOW);
				}
			}

		}

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

} // loop


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
// THE END
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~**/
