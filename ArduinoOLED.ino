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
// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#define CHAR_HEIGHT 12
#define VIEWPORT_MENU_COUNT 3
#define RST_PIN -1


SSD1306AsciiAvrI2c display;
// creating renderer
IMenuRenderer *oledMenuRenderer = new OLEDMenuRenderer(display);
//// creating main menu
AbstractMenuEntity *menu11 = new MenuEntity(oledMenuRenderer, "Hello 11", nullptr, 0);
AbstractMenuEntity *menu12 = new MenuEntity(oledMenuRenderer, "Hello 12", nullptr, 0);
AbstractMenuEntity *menu13 = new MenuEntity(oledMenuRenderer, "Hello 13", nullptr, 0);
AbstractMenuEntity *menu14 = new MenuEntity(oledMenuRenderer, "Back", nullptr, 0);
AbstractMenuEntity *menus1[] = {menu11, menu12, menu13, menu14};

AbstractMenuEntity *menu1 = new MenuEntity(oledMenuRenderer, "Hello 1", menus1, 4);
AbstractMenuEntity *menu2 = new MenuEntity(oledMenuRenderer, "Hello 2", nullptr, 0);
AbstractMenuEntity *menu3 = new MenuEntity(oledMenuRenderer, "Hello 3", nullptr, 0);
AbstractMenuEntity *menu4 = new MenuEntity(oledMenuRenderer, "Back", nullptr, 0);
AbstractMenuEntity *mainMenus[] = {menu1, menu2, menu3, menu4};
AbstractMenuEntity *mainMenu = new MenuEntity(oledMenuRenderer, "Main Menu", mainMenus, 4);

HomeMenuItemRenderer *renderer = new HomeMenuItemRenderer(display);
AbstractMenuEntity *homeMenu = new HomeMenu(renderer, "TempRemote V1.0", mainMenu);
// creating eventSourceObserver
//IEventSourceObserver *serialObserver = new SerialObserver();

IEventSourceObserver *buttonObserver = ButtonInputObserver::getInstance(2, 500);
// creating eventManager
EventManager *eventManager = new EventManager(buttonObserver);

void setupOled() {
#if RST_PIN >= 0
	display.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
	display.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
	// Call oled.setI2cClock(frequency) to change from the default frequency.

	display.setFont(Verdana12_bold);
}


//------------------------------------------------------------------------------
void setup() {
	Serial.begin(115200);
	setupOled();
	Serial.println(F("Hello World!"));
	Serial.println(freeMemory());
	buttonObserver->enable();

	homeMenu->setEventManager(eventManager);

	mainMenu->setBackIndex(3);
	mainMenu->setEventManager(eventManager);

	menu1->setBackIndex(3);
	menu1->setEventManager(eventManager);

	eventManager->registereventReceiver(mainMenu);
	homeMenu->activate();
}
//------------------------------------------------------------------------------
void loop() {
	eventManager->processEvents();
	//delay(10);
}


