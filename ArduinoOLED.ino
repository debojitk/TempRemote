/**
 * Smart Remote V1.0
 * Developers - Arnab Chatterjee, Debojit Kundu
 * Date - 10-JUL-2023
 * Devices and Corresponding Libraries
 * OLED Module - SSD1306 Oled .96"/1.44" - SSD1306Ascii - https://github.com/greiman/SSD1306Ascii (1.3.5)
 * Temperature/Humidity Module - DHT22 - LightWeight DHT Lib - http://github.com/jweigelt/dht22-library (1.0)
 * RTC Module - DS3231 - Inbuilt - Wire I2C lib https://github.com/vishnuajith/i2c-minimal
 * RTC Module - DS1302 - RTC By Makuna - https://github.com/Makuna/Rtc/wiki (2.3.7)
 * Infrared Receiver TSOPV1738 - IRemote Lib - https://github.com/Arduino-IRremote/Arduino-IRremote (4.1.2)
 * Infrared Transmitter - IR LED Module by Seed studio - IRemote Lib - https://github.com/Arduino-IRremote/Arduino-IRremote (4.1.2)
 * Timer1 - TimerOne Library - http://code.google.com/p/arduino-timerone(1.1.0)
 * EEPROM - Inbuilt - http://www.arduino.cc/en/Reference/EEPROM (2.0.0)
 *
 * Switches found in CommonItems.h
 * #define DS3231 - If DS3231 RTC is used, if disabled then DS1302 is used - Default - Enabled
 * #define DISABLE_SERIAL_PRINT - If enabled, turns off all debug prints - Default - Disabled, enable to save another 1.5k progmem
 * #define ENABLE_TEST - Enable to run RemoteData and EEPROM test cases - Default -  Disabled
 *
 * Switches in SensorTypes.cpp - 	#define DECODE_NEC
									#define DECODE_SAMSUNG
									#define DECODE_PANASONIC
 * Enables the above Remote controls in IRemote library
 * Constants in HexProgrammer.h
 *
 	 NUM_INDEX         = 15;            // starting from START_TEMPERATURE
     NUM_SCHEDULE      = 2;				// Number of Schedules
     MAX_HEX_CODES     = 7;				// Max number of remote codes
     BAUD_RATE         = 115200;		// Serial baud rate
     START_TEMPERATURE = 20;			// Start temperature
     MAX_TEMPERATURE   = 100;			// max temperature, treated as invalid temp
     NULL_HOUR         = 24; 			// Null Hour definition
     NULL_MIN          = 60; 			// Null Minute definition
 *
 *	 Credits-
 *	 1. https://www.thecoderscorner.com/electronics/microcontrollers/efficiency/arduino-avr-sketch-compilation-cost-of-vtables-wire-analysis/
 *	 2. Offloading Vtable to Progmem -
 *	 	https://forum.arduino.cc/t/put-avr-vtables-into-the-flash/639611
 *	 	https://github.com/jcmvbkbc/avr-flash-vtbl
 *	 	https://c.1und1.de/@520232801136547587/bdXynAy6S5ym-0B2PZPquA
 *	 	**NOTE** - This code uses this plugin to gain > 300 bytes. This is enabled in C++ compile options as below:
 *	 	-fplugin="full\path\to\avr-flash-vtbl.dll"
 *	 	The avr compiler is - avr-gcc-9.2.0-x64-mingw
 *     3. Progmem Details -
 *         http://www.gammon.com.au/progmem
 *         https://andybrown.me.uk/wk/2011/01/01/debugging-avr-dynamic-memory-allocation/
 *     4. Low Power Arduino - https://www.youtube.com/watch?v=urLSDi7SD8M
 *     5. Chat GPT 3.0 for all other stuffs that I did not know :)
 *
 *
 */
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <avr/pgmspace.h>
#include "AbstractMenuEntity.h"
#include "CommonItems.h"
#include "EventManager.h"
#include "OLEDMenuRenderer.h"
#include "MenuItemRenderer.h"
#include <EEPROM.h>
#include "IMenuRenderer.h"
#include "HexProgrammer.h"
#include "Sensor.h"
#include "SensorTypes.h"
#include "SmartRemoteTests.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#define CHAR_HEIGHT 12
#define VIEWPORT_MENU_COUNT 3
#define RST_PIN -1
#define BUTTON_PIN 2

// Creating input and output devices
SSD1306AsciiAvrI2c display;
TimeSensor timeSensorModule;
TXSensor TX;
RXSensor RX;
RemoteData RD;
TemperatureModule TM;

// creating renderer
IMenuRenderer *oledMenuRenderer = new OLEDMenuRenderer(display);
IMenuRenderer *oledFieldMenuRenderer = new OLEDCompactMenuItemRenderer(display);

//Create Remote Program menu
RemoteProgramMenuItem remoteProgramMenu(oledFieldMenuRenderer, "Program", RX, TX, RD, DefaultTemperatureRange);

//Remote Test menu items
DynamicMenuEntity remoteTestMenu(oledMenuRenderer, oledFieldMenuRenderer, "Test", RD, TX);

AbstractMenuEntity *remoteMenus[] = {&remoteTestMenu, &remoteProgramMenu};

// create the scheduleMenus in setup
AbstractMenuEntity *scheduleMenus[CONFIG::NUM_SCHEDULE];

// creating main menu
TimeMenuItem timeMenu(oledFieldMenuRenderer, "Set Time", timeSensorModule);
DateMenuItem dateMenu(oledFieldMenuRenderer, "Set Date", timeSensorModule);
MenuEntity scheduleMenu(oledMenuRenderer, "Set Schedule", scheduleMenus, CONFIG::NUM_SCHEDULE);
MenuEntity remoteMenu(oledMenuRenderer, "Train Remote", remoteMenus, 2);
ResetMenu resetMenu("Reset EEPROM", RD);
AbstractMenuEntity *mainMenus[] = {&timeMenu, &dateMenu, &scheduleMenu, &remoteMenu, &resetMenu};
MenuEntity mainMenu(oledMenuRenderer, "Main Menu", mainMenus, 5);

// creating home menu
HomeMenuItemRenderer *renderer = new HomeMenuItemRenderer(display);
HomeMenu homeMenu(renderer, "Smart Remote", &mainMenu, timeSensorModule, TM);

IEventSourceObserver *buttonObserver = ButtonInputObserver::getInstance(BUTTON_PIN, 100);
// creating eventManager
SleepWakeupInterruptHandler *interruptHandler = SleepWakeupInterruptHandler::getInstance(BUTTON_PIN, 10000, 20);
EventManager eventManager(buttonObserver);


void createSchedules() {
	for (uint8_t i = 0; i< CONFIG::NUM_SCHEDULE; i++) {
		scheduleMenus[i] = new ScheduleMenuItem(oledFieldMenuRenderer, RD, i);
	}
}

void autoWakeupCallback() {
	SerialPrintln(F("AutoWakeupCallback"));
	static RemoteRXValue prev = NullRemoteRXValue;
	TimeValue v = timeSensorModule.get();
	if(!RD.isScheduleOn(v)) {
		return;
	}
	TemperatureValue tv = TM.get();
	RemoteRXValue rxv = RD.atTemperature((uint8_t)tv._t);
	SerialPrint(F("Temp: ")); SerialPrintln(tv._t);
	if(!(rxv == prev)) {
		TX.set(rxv);
		//SerialPrint(F("Executing Code: ")); rxv.p();
		prev = rxv;
	}
}

void sleepCallback() {
	display.clear();
	buttonObserver->disable();
	if (AbstractMenuEntity::CurrentMenu)AbstractMenuEntity::CurrentMenu->deactivate();
}
void wakeupCallback() {
//	SerialPrint(F("Waked up from button interrupt event-"));
//	SerialPrintlnWithDelay(millis());
	if (AbstractMenuEntity::CurrentMenu)AbstractMenuEntity::CurrentMenu->activate();
	delay(300);
	buttonObserver->enable();
}

void setupSleepWakeupHandler() {
	interruptHandler->setSleepCallback(sleepCallback);
	interruptHandler->setWakeupCallback(wakeupCallback);
	interruptHandler->setAutoWakeupCallback(autoWakeupCallback);
}

void receiveEvent(EventType event) {
	interruptHandler->clearLastEvent();
}


void setupOled() {
#if RST_PIN >= 0
	display.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
	display.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
	// Call oled.setI2cClock(frequency) to change from the default frequency.
	//display.setFont(Arial_bold_14);
	//display.setFont(X11fixed7x14B);
	display.setFont(Verdana12);
	display.setContrast(255);

}

void setupRemote() {
	TX.setup();
	RX.setup();
}


namespace TEST {

uint16_t testMemory() {
	uint16_t bytesLeft;
	for(size_t i = 1; i < 2048; ++i) {
		void* ptr = malloc(sizeof(uint8_t) * i);
		if(!ptr) {
			bytesLeft = i;
			break;
		}
		free(ptr);
	}
	SerialPrint(F("Allocable chunk size -> "));
	SerialPrintln(bytesLeft);
	return bytesLeft;
}


void testMemoryV2 ()
{
	SerialPrint(F("MemoryLeft->")); SerialPrintln(RAMEND - size_t (__malloc_heap_start));
}

void sizeTest() {
	testMemoryV2();
	testMemory();
}


};  // namespace TEST

static void setupTest() {
#ifdef ENABLE_TEST
	TEST::sizeTest();
//	TEST::TestRemoteData trd(RD);
//	trd.setup();
//	TEST::TestSaveRestore sr;
//	sr.setup();
#endif
}


//------------------------------------------------------------------------------
void setup() {
#ifndef DISABLE_SERIAL_PRINT
	Serial.begin(CONFIG::BAUD_RATE);
#endif
//	SerialPrintln(F("Hello from SmartRemote!"));
//	RD.save();
	RD.restore();
	setupOled();
	setupSleepWakeupHandler();
	createSchedules();

	buttonObserver->enable();
	homeMenu.setEventManager(&eventManager);
	mainMenu.setEventManager(&eventManager);
	timeMenu.setEventManager(&eventManager);
	eventManager.registereventReceiver(&mainMenu);
	eventManager.setEventCallback(receiveEvent);
	homeMenu.activate();
	setupRemote();
	setupTest();

}
//------------------------------------------------------------------------------
void loop() {
	eventManager.processEvents();
	interruptHandler->observeEvents();
	homeMenu.update();
	remoteProgramMenu.read();
}


