/*
 * MenuRenderer.cpp
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */


#include "AbstractMenuEntity.h"
#include "MenuRenderer.h"
#include "SerialMenuRenderer.h"
#include <Arduino.h>
void MenuRenderer::renderMenu(MenuEntity* menu) {
	Serial.println(F("MenuRenderer::rendermenu called"));
}

void SerialMenuRenderer::renderMenu(MenuEntity* menu) {
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


