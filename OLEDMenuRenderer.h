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
#include "IMenuRenderer.h"
class AbstractMenuEntity;

class OLEDMenuRenderer: public IMenuRenderer {
public:
	OLEDMenuRenderer(SSD1306AsciiAvrI2c& display);
	void renderMenu(AbstractMenuEntity* menu);
	void clear();
private:
	SSD1306AsciiAvrI2c& display;
	MenuEntity* menu = nullptr;
	void renderMenuHeader(const char *menuHeader);
	void selectMenu(int index);
	int startRange = 0;
	int endRange = 0;

};

class OLEDHorizontalMenuItemRenderer: public IMenuRenderer {
public:
	OLEDHorizontalMenuItemRenderer(SSD1306AsciiAvrI2c& display);
	void renderMenu(AbstractMenuEntity* menu);
	void clear();
private:
	SSD1306AsciiAvrI2c& display;
	MenuItem *_menu = nullptr;
};


#endif /* OLEDMENURENDERER_H_ */
