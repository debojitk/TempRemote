/*
 * SerialMenuRenderer.h
 *
 *  Created on: 14-May-2023
 *      Author: debojitk
 */

#ifndef SERIALMENURENDERER_H_
#define SERIALMENURENDERER_H_
#include "IMenuRenderer.h"

class AbstractMenuEntity;
class SerialMenuRenderer: public IMenuRenderer { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	void renderMenu(AbstractMenuEntity* menu);
};

#endif /* SERIALMENURENDERER_H_ */
