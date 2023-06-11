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

class HomeMenuItemRenderer: public IMenuRenderer {
public:
	HomeMenuItemRenderer(SSD1306AsciiAvrI2c& display);
	void renderMenu(AbstractMenuEntity* menu);
private:
	SSD1306AsciiAvrI2c& display;
	HomeMenu *menu;
};


#endif /* MENUITEMRENDERER_H_ */
