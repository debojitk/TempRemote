/*
 * OLEDMenuRenderer.h
 *
 *  Created on: 04-Jun-2023
 *      Author: debojitk
 */

#ifndef OLEDMENURENDERER_H_
#define OLEDMENURENDERER_H_

#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

class MenuRenderer;
class MenuEntity;

class OLEDMenuRenderer: public MenuRenderer {
public:
	OLEDMenuRenderer();
	OLEDMenuRenderer(SSD1306AsciiAvrI2c& display);
	void renderMenu(MenuEntity* menu);
private:
	SSD1306AsciiAvrI2c& display;
	MenuEntity* menu = nullptr;
	void renderMenuHeader(const char *menuHeader);
	void setupOled();
	void selectMenu(int index);
	int startRange = 0;
	int endRange = 0;

};

#endif /* OLEDMENURENDERER_H_ */
