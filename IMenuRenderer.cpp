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


void OLEDHorizontalMenuItemRenderer::renderContent(FormMenuItem *_menu) {
	display.setCursor(0, 3);
	display.setFont(Arial_bold_14);
	display.clearToEOL();
	if (_menu->getCurrentIndex() > -1 && _menu->getCurrentIndex() < (int)_menu->getFieldCount()) {
		display.print(_menu->getLabel(_menu->getCurrentIndex()));
		display.print(F(":"));
		display.setCol(OLED_COLUMNS/2);
		display.setInvertMode(true);
		display.print(_menu->getValue(_menu->getCurrentIndex()));
		display.setInvertMode(false);
	} else {
		uint8_t index = _menu->getFieldCount() - 1;
		if (_menu->getCurrentIndex() == -1) index = 0;
		display.print(_menu->getLabel(index));
		display.print(F(":"));
		display.setCol(OLED_COLUMNS/2);
		display.print(_menu->getValue(index));
	}

}


void OLEDBaseFormMenuItemRenderer::renderMenu(AbstractMenuEntity *menu) {
	FormMenuItem *_menu = (FormMenuItem*)menu;
	//display.setFont(Arial_bold_14);
	clear();
	renderHeader(_menu);
	renderContent(_menu);
	renderFooter(_menu);
}

void OLEDBaseFormMenuItemRenderer::renderHeader(FormMenuItem *menu) {
	display.setInvertMode(false);
	display.setCursor(0, 0);
	display.print(menu->getName());
}

void OLEDBaseFormMenuItemRenderer::renderFooter(FormMenuItem *menu) {
	// display ok
	display.setCursor(0, 6);
	if (menu->getCurrentIndex() == menu->getOkIndex()){
		display.setInvertMode(true);
	}
	display.print(menu->getLabel(menu->getOkIndex()));
	display.setInvertMode(false);

	// display back
	display.setCursor(OLED_COLUMNS/2, 6);
	if (menu->getCurrentIndex() == menu->getBackIndex()) {
		display.setInvertMode(true);
	}
	display.print(menu->getLabel(menu->getBackIndex()));
	display.setInvertMode(false);
}


void OLEDCompactMenuItemRenderer::renderContent(FormMenuItem *menu) {
	display.setRow(2);
	display.clearToEOL();
	display.setRow(4);
	display.clearToEOL();
	for (uint8_t i = 0, col = 0; i < menu->getFieldCount(); i++, col = col + COL_WIDTH) {
		display.setCursor(col, 2);
		uint8_t charCount = COL_WIDTH / 8;
		char trimmedString[charCount + 1];
		strlcpy_P(trimmedString, (const char *)menu->getLabel(i), charCount + 1);

		display.print(trimmedString);
		display.setCursor(col, 4);
		if (i == menu->getCurrentIndex()) {
			display.setInvertMode(true);
		}
		display.print(menu->getValue(i));
		display.setInvertMode(false);
	}
}
