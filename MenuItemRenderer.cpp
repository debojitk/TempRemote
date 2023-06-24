/*
 * MenuItemRenderer.cpp
 *
 *  Created on: 05-Jun-2023
 *      Author: debojitk
 */

#include "AbstractMenuEntity.h"
#include "MenuItemRenderer.h"
#include <Arduino.h>

HomeMenuItemRenderer::HomeMenuItemRenderer(SSD1306AsciiAvrI2c& displayObject):display(displayObject){
}
void HomeMenuItemRenderer::renderMenu(AbstractMenuEntity *menu) {
	this->menu = reinterpret_cast<HomeMenu *>(menu);
	display.setCursor(0, 0);
	display.print(menu->getName());
	display.setCursor(0, 2);
	display.print(F("Temp: "));
	display.print(this->menu->getTemperature());
	display.print(F(" C"));
	display.setCursor(0, 4);
	display.print(this->menu->getTime().hours);
	display.print(F(":"));
	display.print(this->menu->getTime().minutes);
	display.print(F(":"));
	display.print(this->menu->getTime().seconds);

}

void HomeMenuItemRenderer::clear() {
	display.clear();
}
