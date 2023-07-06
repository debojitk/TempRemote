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
#include "IMenuRenderer.h"
class AbstractMenuEntity;
class HomeMenu;

class HomeMenuItemRenderer: public IMenuRenderer { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	HomeMenuItemRenderer(SSD1306AsciiAvrI2c& display);
	void renderMenu(AbstractMenuEntity* menu);
	void clear();
private:
	SSD1306AsciiAvrI2c& display;
};

class RemoteMenuItemRenderer: public IMenuRenderer { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	RemoteMenuItemRenderer(SSD1306AsciiAvrI2c& display);
	void renderMenu(AbstractMenuEntity* menu);
	void clear();
private:
	SSD1306AsciiAvrI2c& display;
};




#endif /* MENUITEMRENDERER_H_ */
