/*
 * AbstractMenuEntity.h
 *
 *  Created on: 14-May-2023
 *      Author: debojitk
 */

#ifndef ABSTRACTMENUENTITY_H_
#define ABSTRACTMENUENTITY_H_

#include "CustomStack.h"
#include "CommonItems.h"
#include <Arduino.h>
enum EventType: unsigned int;
class IMenuRenderer;
class EventManager;
class HomeMenuItemRenderer;

class AbstractMenuEntity: public IEventReceiver, public IRenderable { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	AbstractMenuEntity(const char *name, IMenuRenderer *renderer);
	const char* getName();
	void setBackIndex(int index);
	int getBackIndex();
	void setCurrentIndex(int index);
	int getCurrentIndex();

	void handleEvent(EventType event);
	void render();


	virtual bool isActive();
	virtual void setActive(bool active);
	virtual void setEventManager(EventManager *manager);
	virtual void activate();
	virtual void handleClick() = 0;
	virtual void handleDoubleClick() = 0;
	virtual void back() = 0;
	// declare the stack
	static CustomStack<AbstractMenuEntity *, 10> menuStack;
protected:
	const char *name;
	int currentIndex = -1;
	int backIndex = 0;
	bool active = false;
	EventManager *eventManager = nullptr;
	IMenuRenderer *renderer;

};

class MenuEntity: public AbstractMenuEntity { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	MenuEntity(IMenuRenderer *renderer, const char* name, AbstractMenuEntity* items[], int numItems);
	void activate();
	int getNumItems();
	AbstractMenuEntity* getItem(int index);
	void handleClick();
	void goToNextItem();
	void handleDoubleClick();
	void back();
private:
	AbstractMenuEntity** items;
	int numItems;
};

class MenuItem: public AbstractMenuEntity { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	MenuItem(const char *name, IMenuRenderer *renderer);
	virtual void activate();
	virtual void handleClick() = 0;
	virtual void handleDoubleClick() = 0;
	virtual void back() = 0;
};

class HomeMenu: public MenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	HomeMenu(HomeMenuItemRenderer *renderer, const char *name, AbstractMenuEntity *child);
	void setTime(uint8_t hour, uint8_t min, uint8_t sec);
	void handleClick();
	void handleDoubleClick();
	void activate();
	void back();
	double getTemperature();
	struct CurrentTime getTime();
private:
	double temperature = 30;
	int humidity = 70;
	struct CurrentTime cTime;
	AbstractMenuEntity *child = nullptr;
};



#endif /* ABSTRACTMENUENTITY_H_ */
