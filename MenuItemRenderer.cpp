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
void HomeMenuItemRenderer::renderMenu(MenuItem *menu) {
	this->menu = menu;
	HomeMenu *homeMenu = reinterpret_cast<HomeMenu *>(this->menu);
	display.clear();
	display.setCursor(0, 0);
	display.print(menu->getName());
	display.setCursor(0, 2);
	display.print(F("Temp: "));
	display.print(homeMenu->getTemperature());
	display.print(F(" C"));
	display.setCursor(0, 4);
	display.print(homeMenu->getTime().hours);
	display.print(F(":"));
	display.print(homeMenu->getTime().minutes);
	display.print(F(":"));
	display.print(homeMenu->getTime().seconds);

}

