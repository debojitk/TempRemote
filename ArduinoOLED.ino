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
#include "MenuRenderer.h"
#include "SerialMenuRenderer.h"
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#define CHAR_HEIGHT 12
#define VIEWPORT_MENU_COUNT 3

// creating renderer
MenuRenderer *serialRenderer = new SerialMenuRenderer();
// creating main menu
AbstractMenuEntity *menu11 = new MenuEntity(serialRenderer, "Hello 11", nullptr, 0);
AbstractMenuEntity *menu12 = new MenuEntity(serialRenderer, "Hello 12", nullptr, 0);
AbstractMenuEntity *menu13 = new MenuEntity(serialRenderer, "Hello 13", nullptr, 0);
AbstractMenuEntity *menu14 = new MenuEntity(serialRenderer, "Back", nullptr, 0);
AbstractMenuEntity *menus1[] = {menu11, menu12, menu13, menu14};

AbstractMenuEntity *menu1 = new MenuEntity(serialRenderer, "Hello 1", menus1, 4);
AbstractMenuEntity *menu2 = new MenuEntity(serialRenderer, "Hello 2", nullptr, 0);
AbstractMenuEntity *menu3 = new MenuEntity(serialRenderer, "Hello 3", nullptr, 0);
AbstractMenuEntity *menu4 = new MenuEntity(serialRenderer, "Back", nullptr, 0);
AbstractMenuEntity *mainMenus[] = {menu1, menu2, menu3, menu4};
AbstractMenuEntity *mainMenu = new MenuEntity(serialRenderer, "Main Menu", mainMenus, 4);
// creating eventSourceObserver
IEventSourceObserver *serialObserver = new SerialObserver();

// creating eventManager
EventManager *eventManager = new EventManager(serialObserver);

//------------------------------------------------------------------------------
void setup() {
	Serial.begin(115200);
	Serial.println(F("Start"));
	Serial.println(freeMemory());
	serialObserver->enable();
	mainMenu->setBackIndex(3);
	mainMenu->setEventManager(eventManager);
	menu1->setBackIndex(3);
	menu1->setEventManager(eventManager);
	eventManager->registereventReceiver(mainMenu);
}
//------------------------------------------------------------------------------
void loop() {
	eventManager->processEvents();
	delay(10);
}

