/*
 * MenuRenderer.h
 *
 *  Created on: 14-May-2023
 *      Author: debojitk
 */

#ifndef MENURENDERER_H_
#define MENURENDERER_H_
class MenuEntity;

class MenuRenderer {
public:
	virtual void renderMenu(MenuEntity* menu);
};

#endif /* MENURENDERER_H_ */
