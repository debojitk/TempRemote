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

class OLEDBaseFormMenuItemRenderer: public IMenuRenderer {
public:
	OLEDBaseFormMenuItemRenderer(SSD1306AsciiAvrI2c& displayObject):display(displayObject){}
	virtual void renderMenu(AbstractMenuEntity* menu);
	virtual void renderContent(FormMenuItem* menu) = 0;
	virtual void renderHeader(FormMenuItem* menu);
	virtual void renderFooter(FormMenuItem* menu);
	void clear() {
		display.clear();
	}
protected:
	SSD1306AsciiAvrI2c& display;
};



class OLEDHorizontalMenuItemRenderer: public OLEDBaseFormMenuItemRenderer {
public:
	OLEDHorizontalMenuItemRenderer(SSD1306AsciiAvrI2c& display):OLEDBaseFormMenuItemRenderer(display){}
	void renderContent(FormMenuItem* menu);
};

class OLEDCompactMenuItemRenderer: public OLEDBaseFormMenuItemRenderer {
public:
	OLEDCompactMenuItemRenderer(SSD1306AsciiAvrI2c& display):OLEDBaseFormMenuItemRenderer(display){}
	void renderContent(FormMenuItem* menu);
private:
	static constexpr uint8_t COL_WIDTH = 40;
};


#endif /* OLEDMENURENDERER_H_ */
