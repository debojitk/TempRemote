/*
 * SerialMenuRenderer.h
 *
 *  Created on: 14-May-2023
 *      Author: debojitk
 */

#ifndef SERIALMENURENDERER_H_
#define SERIALMENURENDERER_H_

class MenuRenderer;
class MenuEntity;
class SerialMenuRenderer: public MenuRenderer {
public:
	void renderMenu(MenuEntity* menu);
};

#endif /* SERIALMENURENDERER_H_ */
