// Simple I2C test for ebay 128x64 oled.
// Use smaller faster AvrI2c class in place of Wire.
// Edit AVRI2C_FASTMODE in SSD1306Ascii.h to change the default I2C frequency.
//
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <MemoryFree.h>
#include <avr/pgmspace.h>
#include "AbstractMenuEntity.h"
#include "CommonItems.h"
#include "EventManager.h"
#include "SerialMenuRenderer.h"
#include "OLEDMenuRenderer.h"
#include "MenuItemRenderer.h"
#include <EEPROM.h>
#include "IMenuRenderer.h"
#include "KeyValueStore.h"
#include "HexProgrammer.h"
#include "Sensor.h"
#include "SensorTypes.h"

using TempSensor = Sensor<TemperatureModule, TemperatureValue>;

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#define CHAR_HEIGHT 12
#define VIEWPORT_MENU_COUNT 3
#define RST_PIN -1
#define BUTTON_PIN 2


SSD1306AsciiAvrI2c display;
// creating renderer
IMenuRenderer *oledMenuRenderer = new OLEDMenuRenderer(display);
IMenuRenderer *oledSingleFieldMenuRenderer = new OLEDSingleFieldMenuItemRenderer(display);

//// creating main menu

MenuItem *hourMenu = new SingleFieldMenuItem(oledSingleFieldMenuRenderer, "Set Hour", "", 23);
MenuItem *minuteMenu = new SingleFieldMenuItem(oledSingleFieldMenuRenderer, "Set Minute", "", 59);
MenuItem *secondMenu = new SingleFieldMenuItem(oledSingleFieldMenuRenderer, "Set Second", "", 59);
AbstractMenuEntity *clockMenu[] = {hourMenu, minuteMenu, secondMenu};

AbstractMenuEntity *menu1 = new MenuEntity(oledMenuRenderer, "Set Time", clockMenu, 3);
AbstractMenuEntity *menu2 = new MenuEntity(oledMenuRenderer, "Set Schedule", nullptr, 0);
AbstractMenuEntity *menu3 = new MenuEntity(oledMenuRenderer, "Train Remote", nullptr, 0);
AbstractMenuEntity *mainMenus[] = {menu1, menu2, menu3};
AbstractMenuEntity *mainMenu = new MenuEntity(oledMenuRenderer, "Main Menu", mainMenus, 3);

HomeMenuItemRenderer *renderer = new HomeMenuItemRenderer(display);
AbstractMenuEntity *homeMenu = new HomeMenu(renderer, "TempRemote V1.0", mainMenu);
// creating eventSourceObserver
//IEventSourceObserver *serialObserver = new SerialObserver();

IEventSourceObserver *buttonObserver = ButtonInputObserver::getInstance(BUTTON_PIN, 500);
// creating eventManager
SleepWakeupInterruptHandler *interruptHandler = SleepWakeupInterruptHandler::getInstance(BUTTON_PIN, 10000, 20);
EventManager *eventManager = new EventManager(buttonObserver);

void autoWakeupCallback() {
	SerialPrint(F("Waked up from WDT interrupt event-"));
	SerialPrintlnWithDelay(millis());
}

void sleepCallback() {
	display.clear();
	buttonObserver->disable();
}
void wakeupCallback() {
	SerialPrint(F("Waked up from button interrupt event-"));
	SerialPrintlnWithDelay(millis());
	if (eventManager->geteventReceiver()) {
		AbstractMenuEntity *currentMenu = reinterpret_cast<AbstractMenuEntity *>(eventManager->geteventReceiver());
		currentMenu->render();
		buttonObserver->enable();
		Serial.println(freeMemory());
	}
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

	display.setFont(Verdana12_bold);
}

//TempSensor tempMod;
//TimeModule t;

//void keyvaluestoreTest(){
//	int x=50;
//	KeyValueStore.set(IntType, "testKey", x);
//	int y = 20;
//	KeyValueStore.get(IntType, "testKey", y);
//	Serial.println(y);
//}

using TXSensor = Sensor<RemoteRXModule, RemoteRXValue>;
using RXSensor = Sensor<RemoteTXModule, RemoteRXValue>;
namespace TEST {

void testTxRxSetup() {

}

};

//------------------------------------------------------------------------------
void setup() {
	Serial.begin(CONFIG::BAUD_RATE);
	setupOled();
	SerialPrint(F("Hello World!"));
	SerialPrintln(freeMemory());
	setupSleepWakeupHandler();
	buttonObserver->enable();

	homeMenu->setEventManager(eventManager);
	mainMenu->setEventManager(eventManager);
	menu1->setEventManager(eventManager);
	eventManager->registereventReceiver(mainMenu);
	eventManager->setEventCallback(receiveEvent);
	homeMenu->activate();
	//tempMod.get();
}
//------------------------------------------------------------------------------
void loop() {
	eventManager->processEvents();
	interruptHandler->observeEvents();
	//Serial.println(freeMemory());
	//delay(100);
}

