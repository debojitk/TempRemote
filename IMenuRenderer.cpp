/*
 * MenuRenderer.cpp
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */


#include "IMenuRenderer.h"

#include "AbstractMenuEntity.h"
#include "SerialMenuRenderer.h"
#include "OLEDMenuRenderer.h"
#include <Arduino.h>


#define CHAR_HEIGHT 2
#define VIEWPORT_MENU_COUNT 3
#define OLED_COLUMNS 128
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

void IMenuRenderer::renderMenu(AbstractMenuEntity* menu) {
	Serial.println(F("MenuRenderer::rendermenu called"));
}

void SerialMenuRenderer::renderMenu(AbstractMenuEntity* _menu) {
	MenuEntity *menu = reinterpret_cast<MenuEntity *>(_menu);
	Serial.println(F("SerialMenuRenderer::rendermenu called"));
	Serial.println(menu->getName());
	for (int i = 0; i < menu->getNumItems(); i++) {
		Serial.print(i + 1);
		Serial.print(F(". "));
		Serial.println(menu->getItem(i)->getName());
	}
	Serial.print(F("Currently selected = "));
	Serial.println(menu->getCurrentIndex());
}


OLEDMenuRenderer:: OLEDMenuRenderer(SSD1306AsciiAvrI2c& displayObject):display(displayObject){
	//this->display = display;
	//setupOled();
}

void OLEDMenuRenderer::renderMenu(AbstractMenuEntity* _menu) {
	this->menu = reinterpret_cast<MenuEntity *>(_menu);
	startRange = 0;
	endRange = min(this->menu->getNumItems() + 1, VIEWPORT_MENU_COUNT); // +1 for back menu
	renderMenuHeader(this->menu->getName());
	selectMenu(this->menu->getCurrentIndex());
}

void OLEDMenuRenderer::setupOled() {
#if RST_PIN >= 0
	display.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
	display.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
	// Call oled.setI2cClock(frequency) to change from the default frequency.

	display.setFont(Verdana12_bold);
}


void OLEDMenuRenderer::renderMenuHeader(const char *menuHeader) {
	display.setCursor(0, 0);
	display.clearToEOL();
	display.print(menuHeader);
}

void OLEDMenuRenderer::selectMenu(int index) {
	if (index >= 0) {
		if (index + 1 > endRange) {
			// shift range right
			startRange = (index + 1) - VIEWPORT_MENU_COUNT;
			endRange = index + 1;
		} else if (index < startRange){
			startRange = index;
			endRange = startRange + VIEWPORT_MENU_COUNT;
		}
	}
	for (int i = startRange; i< endRange; i++){
		int cursorPos = CHAR_HEIGHT*(i + 1 - startRange);
		display.setCursor(0, cursorPos);
		if (i == index){
			display.setInvertMode(true);
		}
		display.clearToEOL();
		if (i == menu->getBackIndex()){
			display.print(F("Back"));
		} else{
			display.print(menu->getItem(i)->getName());
		}
		display.setInvertMode(false);
	}
}

//OLEDSingleFieldMenuItemRenderer definition

OLEDSingleFieldMenuItemRenderer::OLEDSingleFieldMenuItemRenderer(SSD1306AsciiAvrI2c &displayObject):display(displayObject) {
}

void OLEDSingleFieldMenuItemRenderer::renderMenu(AbstractMenuEntity *menu) {
	this->menu = reinterpret_cast<SingleFieldMenuItem *>(menu);
	display.clear();
	display.setInvertMode(false);
	display.setCursor(0, 0);
	display.print(this->menu->getName());

	display.setCursor(52, 3);
	display.setFont(lcdnums12x16);
	if(this->menu->getValue() < 10){
		display.print(F("0"));
		display.setCol(64);
	}
	display.print(this->menu->getValue());
	display.setFont(Verdana12_bold);
	display.setCursor(0, 6);
	display.print(F("Save"));
	display.setCursor(OLED_COLUMNS/2, 6);
	display.print(F("Back"));
	display.setInvertMode(true);

	switch(this->menu->getCurrentIndex()){
	case SingleFieldMenuItem::DATA_INDEX://value
		display.setCursor(52, 3);
		display.setFont(lcdnums12x16);
		if(this->menu->getValue() < 10){
			display.print(F("0"));
			display.setCol(64);
		}
		display.print(this->menu->getValue());
		display.setFont(Verdana12_bold);
		break;
	case SingleFieldMenuItem::SAVE_INDEX://save
		display.setCursor(0, 6);
		display.print(F("Save"));
		break;
	case SingleFieldMenuItem::BACK_INDEX:
		display.setCursor(OLED_COLUMNS/2, 6);
		display.print(F("Back"));
		break;
	default:
		break;
	}
	display.setInvertMode(false);
}
