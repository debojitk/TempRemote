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
	display.setCursor(0, 0);
	display.print(menu->getName());
	display.setCursor(0, 2);
	display.print(F("Temp: "));
	display.print(menu->getTemperature());
	display.print(F(" C"));
	display.setCursor(0, 4);
	display.print(t.formatDate(buffer));
	display.setCursor(0, 6);
	display.print(t.formatTime(buffer));

}

void HomeMenuItemRenderer::clear() {
	display.clear();
}
