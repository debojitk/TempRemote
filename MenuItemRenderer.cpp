/*
 * MenuItemRenderer.cpp
 *
 *  Created on: 05-Jun-2023
 *      Author: debojitk
 */

#include "AbstractMenuEntity.h"
#include "MenuItemRenderer.h"
#include "SensorTypes.h"
#include <Arduino.h>

HomeMenuItemRenderer::HomeMenuItemRenderer(SSD1306AsciiAvrI2c& displayObject):display(displayObject){
}
void HomeMenuItemRenderer::renderMenu(AbstractMenuEntity *_menu) {
	HomeMenu * menu = reinterpret_cast<HomeMenu *>(_menu);
	TimeValue t = menu->getTime();
	char buffer[12];
	display.setCursor(2, 0);
	display.print(menu->getName());
	display.setCursor(2, 2);
	display.print(F("T: "));
	display.print(menu->getTemperature()._t);
	display.print(F("C"));
	display.setCursor(60, 2);
	display.print(F("H: "));
	display.print(menu->getTemperature()._h);
	display.print(F("%"));
	display.setCursor(2, 4);
	display.print(F("Date: "));
	display.print(t.formatDate(buffer));
	display.setCursor(2, 6);
	display.print(F("Time: "));
	display.print(t.formatTime(buffer));

}

void HomeMenuItemRenderer::clear() {
	display.clear();
}
