/*
 * MenuRenderer.h
 *
 *  Created on: 14-May-2023
 *      Author: debojitk
 */

#ifndef IMENURENDERER_H_
#define IMENURENDERER_H_
class AbstractMenuEntity;

class IMenuRenderer { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	virtual void renderMenu(AbstractMenuEntity* menu) = 0;
	virtual void clear() = 0;
};

#endif /* IMENURENDERER_H_ */
