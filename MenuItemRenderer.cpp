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
	switch(menu->getCurrentIndex()){
	case 0:
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
		break;
	case 1: // Date
		display.setCursor(2, 2);
		display.print(F("Date"));
		display.setCursor(2, 4);
		display.set2X();
		display.print(t.formatDate(buffer));
		break;
	case 2: // time
		display.setCursor(2, 2);
		display.print(F("Time"));
		display.setCursor(2, 4);
		display.set2X();
		display.print(t.formatTime(buffer));
		break;
	case 3: //temp
		display.setCursor(2, 2);display.print(F("Temperature"));
		display.setCursor(2, 4);
		display.set2X();
		display.print(menu->getTemperature()._t);
		display.print(F(" C"));
		break;
	case 4: //hum
		display.setCursor(2, 2);display.print(F("Humidity"));
		display.setCursor(2, 4);
		display.set2X();
		display.print(menu->getTemperature()._h);
		display.print(F("%"));
		break;
	case 5: //memory
		display.setCursor(2, 2);
		display.print(F("Memory Left"));
		display.setCursor(2, 4);
		display.set2X();
		display.print(TEST::testMemory());
		display.print(F(" B"));
		break;
	case 6: //seconds spent
		display.setCursor(2, 2);
		display.print(F("Seconds Spent"));
		display.setCursor(2, 4);
		display.set2X();
		display.print((double)millis()/(double)1000);
		break;
	}
	display.set1X();
}

void HomeMenuItemRenderer::clear() {
	display.clear();
}
