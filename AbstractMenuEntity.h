/*
 * AbstractMenuEntity.h
 *
 *  Created on: 14-May-2023
 *      Author: debojitk
 */

#ifndef ABSTRACTMENUENTITY_H_
#define ABSTRACTMENUENTITY_H_

#include "CustomStack.h"
enum EventType: unsigned int;
class MenuRenderer;
class EventManager;

class IEventReceiver {
public:
	virtual void handleEvent(EventType event) = 0;
};


class AbstractMenuEntity: public IEventReceiver {
public:
	AbstractMenuEntity(const char *name);
	const char* getName();

	void setBackIndex(int index);
	int getBackIndex();

	void setCurrentIndex(int index);
	int getCurrentIndex();

	void handleEvent(EventType event);

	virtual void handleClick() = 0;
	virtual void handleDoubleClick() = 0;

	/**
	 * Activates this menu
	 */
	virtual void activate() = 0;

	virtual bool isActive();
	virtual void setActive(bool active);

	/**
	 * Go to Previous menu
	 */
	virtual void back() = 0;

	virtual void setEventManager(EventManager *manager);
	// declare the stack
	static CustomStack<AbstractMenuEntity *, 10> menuStack;
protected:
	const char *name;
	int currentIndex = -1;
	int backIndex = 0;
	bool active = false;
	EventManager *eventManager = nullptr;

};

class MenuEntity: public AbstractMenuEntity {
public:
	MenuEntity(MenuRenderer *renderer, const char* name, AbstractMenuEntity* items[], int numItems);
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
	MenuRenderer *renderer;
};

class MenuItem: public AbstractMenuEntity {
public:
	MenuItem(const char *name);
};

class FunctionMenuItem: public MenuItem {
public:
	FunctionMenuItem(const char* name, void (*func)());
	void select();
private:
	void (*func)();
};


#endif /* ABSTRACTMENUENTITY_H_ */
