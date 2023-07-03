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
	SerialPrintln(F("MenuRenderer::rendermenu called"));
}

void SerialMenuRenderer::renderMenu(AbstractMenuEntity* _menu) {
	MenuEntity *menu = reinterpret_cast<MenuEntity *>(_menu);
	SerialPrintln(F("SerialMenuRenderer::rendermenu called"));
	SerialPrintln(menu->getName());
	for (int i = 0; i < menu->getNumItems(); i++) {
		SerialPrint(i + 1);
		SerialPrint(F(". "));
		SerialPrintln(menu->getItem(i)->getName());
	}
	SerialPrint(F("Currently selected = "));
	SerialPrintln(menu->getCurrentIndex());
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
void OLEDMenuRenderer::clear() {
	display.clear();
}


void SerialMenuRenderer::clear() {
}

OLEDHorizontalMenuItemRenderer::OLEDHorizontalMenuItemRenderer(
		SSD1306AsciiAvrI2c &displayObject): display(displayObject) {
}

void OLEDHorizontalMenuItemRenderer::renderMenu(AbstractMenuEntity *menu) {
	TimeMenuItem * _menu = (TimeMenuItem *)menu;

	display.clear();
	display.setInvertMode(false);
	display.setCursor(0, 0);
	display.print(_menu->getName());

	// on form field line
	display.setCursor(0, 3);
	display.setFont(Arial_bold_14);
	// print label
	display.clearToEOL();
	if (_menu->getCurrentIndex() > -1 && _menu->getCurrentIndex() < (int)_menu->getFieldCount()) {
		display.print((const __FlashStringHelper *)_menu->getLabel(_menu->getCurrentIndex()));
		display.print(F(":"));
		display.setCol(OLED_COLUMNS/2);
		display.setInvertMode(true);
		display.print(_menu->getValue(_menu->getCurrentIndex()));
		display.setInvertMode(false);
	} else {
		uint8_t index = _menu->getFieldCount() - 1;
		if (menu->getCurrentIndex() == -1) index = 0;
		display.print(_menu->getLabel(index));
		display.print(F(":"));
		display.setCol(OLED_COLUMNS/2);
		display.print(_menu->getValue(index));
	}
	// reverting to main font
	display.setFont(Verdana12_bold);
	display.setCursor(0, 6);
	if (_menu->getCurrentIndex() == _menu->getOkIndex()){
		display.setInvertMode(true);
		display.print(_menu->getLabel(_menu->getOkIndex()));
		display.setInvertMode(false);
	} else {
		display.print(_menu->getLabel(_menu->getOkIndex()));
	}
	display.setCursor(OLED_COLUMNS/2, 6);
	if (_menu->getCurrentIndex() == _menu->getBackIndex()) {
		display.setInvertMode(true);
		display.print(_menu->getLabel(_menu->getBackIndex()));
		display.setInvertMode(false);
	} else {
		display.print(_menu->getLabel(_menu->getBackIndex()));
	}
	display.setInvertMode(false);
}

void OLEDHorizontalMenuItemRenderer::clear() {
	display.clear();
}

