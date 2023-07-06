// Simple I2C test for ebay 128x64 oled.
// Use smaller faster AvrI2c class in place of Wire.
// Edit AVRI2C_FASTMODE in SSD1306Ascii.h to change the default I2C frequency.
//
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <avr/pgmspace.h>
#include "AbstractMenuEntity.h"
#include "CommonItems.h"
#include "EventManager.h"
#include "SerialMenuRenderer.h"
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

// creating renderer
IMenuRenderer *oledMenuRenderer = new OLEDMenuRenderer(display);
IMenuRenderer *oledFieldMenuRenderer = new OLEDCompactMenuItemRenderer(display);

//Create Remote Program menu
RemoteProgramMenuItem *remoteProgramMenu = new RemoteProgramMenuItem(oledFieldMenuRenderer, "Program", RX, RD, DefaultTemperatureRange);

//Remote Test menu items
AbstractMenuEntity *remoteTestMenu = new DynamicMenuEntity(oledMenuRenderer, oledFieldMenuRenderer, "Test", RD);

//AbstractMenuEntity *remoteMenus[] = {remoteTestMenu};
// if these two are enabled then due to lack of memory system is non-functioning.
AbstractMenuEntity *remoteMenus[] = {remoteTestMenu, remoteProgramMenu};



// creating main menu
AbstractMenuEntity *menu1 = new TimeMenuItem(oledFieldMenuRenderer, "Set Time", timeSensorModule);
AbstractMenuEntity *menu4 = new DateMenuItem(oledFieldMenuRenderer, "Set Date", timeSensorModule);
AbstractMenuEntity *menu2 = new MenuEntity(oledMenuRenderer, "Set Schedule", nullptr, 0);
AbstractMenuEntity *menu3 = new MenuEntity(oledMenuRenderer, "Train Remote", remoteMenus, 2);
AbstractMenuEntity *mainMenus[] = {menu1, menu4, menu2, menu3};
AbstractMenuEntity *mainMenu = new MenuEntity(oledMenuRenderer, "Main Menu", mainMenus, 4);

// creating home menu
HomeMenuItemRenderer *renderer = new HomeMenuItemRenderer(display);
HomeMenu homeMenu(renderer, "TempRemote V1.0", mainMenu, timeSensorModule);

IEventSourceObserver *buttonObserver = ButtonInputObserver::getInstance(BUTTON_PIN, 300);
// creating eventManager
SleepWakeupInterruptHandler *interruptHandler = SleepWakeupInterruptHandler::getInstance(BUTTON_PIN, 100000, 20);
EventManager *eventManager = new EventManager(buttonObserver);

void autoWakeupCallback() {
	SerialPrint(F("Waked up from WDT interrupt event-"));
	SerialPrintlnWithDelay(millis());
}

void sleepCallback() {
	display.clear();
	buttonObserver->disable();
	if (AbstractMenuEntity::CurrentMenu)AbstractMenuEntity::CurrentMenu->deactivate();
}
void wakeupCallback() {
	SerialPrint(F("Waked up from button interrupt event-"));
	SerialPrintlnWithDelay(millis());
	buttonObserver->enable();
	if (AbstractMenuEntity::CurrentMenu)AbstractMenuEntity::CurrentMenu->activate();
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
	display.setFont(Arial_bold_14);
}

namespace TEST {

void testTxRxSetup() {
	TX.setup();
	SerialPrintln(F("TX Setup complete"));
	RX.setup();
	SerialPrintln(F("RX Setup complete"));
}

void testRegister() {
	IRNode node = RX.get();

}

void testMemory() {
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
#ifdef DISABLE_SERIAL_PRINT
	display.setCursor(0, 0);
	display.print(F("Mem : "));
	display.print(bytesLeft);
#endif
	// end of freeRam
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



//------------------------------------------------------------------------------
void setup() {
#ifndef DISABLE_SERIAL_PRINT
	Serial.begin(CONFIG::BAUD_RATE);
#endif
	SerialPrintln(F("Hello from SmartRemote!"));

	setupOled();
	setupSleepWakeupHandler();

	buttonObserver->enable();
	homeMenu.setEventManager(eventManager);
	mainMenu->setEventManager(eventManager);
	menu1->setEventManager(eventManager);
	eventManager->registereventReceiver(mainMenu);
	eventManager->setEventCallback(receiveEvent);
	homeMenu.activate();
	TEST::sizeTest();
//	TEST::testTxRxSetup();
//	TEST::TestRemoteData trd(RD);
//	trd.setup();
}
//------------------------------------------------------------------------------
void loop() {
	eventManager->processEvents();
	interruptHandler->observeEvents();
	homeMenu.update();
	remoteProgramMenu->read();
}


