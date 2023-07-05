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
#include "KeyValueStore.h"
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

// creating renderer
IMenuRenderer *oledMenuRenderer = new OLEDMenuRenderer(display);
IMenuRenderer *oledFieldMenuRenderer = new OLEDCompactMenuItemRenderer(display);

//Create remote menu

RemoteProgramMenuItem *remoteProgramMenu = new RemoteProgramMenuItem(oledFieldMenuRenderer, "Program", RX);
AbstractMenuEntity *remoteMenus[] = {remoteProgramMenu};
// creating main menu
AbstractMenuEntity *menu1 = new TimeMenuItem(oledFieldMenuRenderer, "Set Time", timeSensorModule);
AbstractMenuEntity *menu4 = new DateMenuItem(oledFieldMenuRenderer, "Set Date", timeSensorModule);
AbstractMenuEntity *menu2 = new MenuEntity(oledMenuRenderer, "Set Schedule", nullptr, 0);
AbstractMenuEntity *menu3 = new MenuEntity(oledMenuRenderer, "Train Remote", remoteMenus, 1);
AbstractMenuEntity *mainMenus[] = {menu1, menu4, menu2, menu3};
AbstractMenuEntity *mainMenu = new MenuEntity(oledMenuRenderer, "Main Menu", mainMenus, 4);

HomeMenuItemRenderer *renderer = new HomeMenuItemRenderer(display);
HomeMenu *homeMenu = new HomeMenu(renderer, "TempRemote V1.0", mainMenu, timeSensorModule);

IEventSourceObserver *buttonObserver = ButtonInputObserver::getInstance(BUTTON_PIN, 300);
// creating eventManager
SleepWakeupInterruptHandler *interruptHandler = SleepWakeupInterruptHandler::getInstance(BUTTON_PIN, 10000, 20);
EventManager *eventManager = new EventManager(buttonObserver);
RemoteData remote;

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

// TODO: 1. EEPROM reset
// TODO: 2. Remote Test menu
// TODO: 3. Schedule Menu


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
	// end of freeRam
}


void testMemoryV2 ()
{
	SerialPrint(F("MemoryLeft->")); SerialPrintln(RAMEND - size_t (__malloc_heap_start));
}

void sizeTest() {
	SerialPrint(F("Serial -> "));				SerialPrintln(sizeof(Serial));
	SerialPrint(F("SSD1306AsciiAvrI2c -> "));	SerialPrintln(sizeof(display));
	SerialPrint(F("TimeSensor -> "));			SerialPrintln(sizeof(timeSensorModule));
	SerialPrint(F("TXSensor -> "));				SerialPrintln(sizeof(TX));
	SerialPrint(F("RXSensor -> "));				SerialPrintln(sizeof(RX));
	SerialPrint(F("AbstractMenuEntity -> "));	SerialPrintln(sizeof(AbstractMenuEntity));
	SerialPrint(F("TimeMenuItem -> "));			SerialPrintln(sizeof(TimeMenuItem));
	SerialPrint(F("DateMenuItem -> "));			SerialPrintln(sizeof(DateMenuItem));
	SerialPrint(F("MenuEntity -> "));			SerialPrintln(sizeof(MenuEntity));
	SerialPrint(F("HomeMenu -> "));				SerialPrintln(sizeof(HomeMenu));
	SerialPrint(F("ButtonInputObserver -> "));	SerialPrintln(sizeof(ButtonInputObserver));
	SerialPrint(F("SleepWakeupInterruptHandler -> "));	SerialPrintln(sizeof(SleepWakeupInterruptHandler));
	SerialPrint(F("EventManager -> "));			SerialPrintln(sizeof(EventManager));
	testMemoryV2();
	testMemory();

}


};  // namespace TEST



//------------------------------------------------------------------------------
void setup() {
	Serial.begin(CONFIG::BAUD_RATE);
	SerialPrintln(F("Hello from SmartRemote!"));
	TEST::sizeTest();
	TEST::testTxRxSetup();

	setupOled();
	setupSleepWakeupHandler();

	buttonObserver->enable();
	homeMenu->setEventManager(eventManager);
	mainMenu->setEventManager(eventManager);
	menu1->setEventManager(eventManager);
	eventManager->registereventReceiver(mainMenu);
	eventManager->setEventCallback(receiveEvent);
	homeMenu->activate();
}
//------------------------------------------------------------------------------
void loop() {
	eventManager->processEvents();
	interruptHandler->observeEvents();
	homeMenu->update();
	remoteProgramMenu->read();
}


