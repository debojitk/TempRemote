/*
 * MenuItemRenderer.h
 *
 *  Created on: 05-Jun-2023
 *      Author: debojitk
 */

#ifndef MENUITEMRENDERER_H_
#define MENUITEMRENDERER_H_

#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include "CommonItems.h"
class MenuItem;

class MenuItemRenderer { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	virtual void renderMenu(MenuItem* menu);
protected:
	MenuItem* menu = nullptr;
};

class HomeMenuItemRenderer: public MenuItemRenderer {
public:
	HomeMenuItemRenderer(SSD1306AsciiAvrI2c& display);
	void renderMenu(MenuItem* menu);
private:
	SSD1306AsciiAvrI2c& display;
};


#endif /* MENUITEMRENDERER_H_ */
