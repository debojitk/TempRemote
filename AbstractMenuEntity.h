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

template <typename SensorModule, typename Value>
class Sensor;
class TimeModuleDS3231;
class TemperatureModule;
struct TimeValue;
using TemperatureValue = float;
enum EventType: unsigned int;
class IMenuRenderer;
class EventManager;
class HomeMenuItemRenderer;

using TimeSensor = Sensor<TimeModuleDS3231, TimeValue>;
using TempSensor = Sensor<TemperatureModule, TemperatureValue>;

class AbstractMenuEntity: public IEventReceiver, public IRenderable { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	AbstractMenuEntity(const char *name, IMenuRenderer *renderer);
	const char* getName();
	int getBackIndex();
	void setCurrentIndex(int index);
	int getCurrentIndex();

	void handleEvent(EventType event);
	void render();


	virtual bool isActive();
	virtual void setActive(bool active);
	virtual void setEventManager(EventManager *manager);
	virtual void activate();
	virtual void deactivate();
	virtual void handleClick() = 0;
	virtual void handleDoubleClick() = 0;
	virtual void back();
	// declare the stack
	static AbstractMenuEntity *CurrentMenu;
protected:
	static CustomStack<AbstractMenuEntity *, 10> menuStack;
	const char *name;
	int currentIndex = -1;
	int backIndex = 0;
	bool active = false;
	EventManager *eventManager = nullptr;
	IMenuRenderer *renderer;

};

class MenuEntity: public AbstractMenuEntity { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	MenuEntity(IMenuRenderer *renderer, const char* name, AbstractMenuEntity* items[], uint8_t numitems);
	void activate();
	int getNumItems();
	AbstractMenuEntity* getItem(int index);
	void handleClick();
	void goToNextItem();
	void handleDoubleClick();
private:
	AbstractMenuEntity** items;
	uint8_t numItems = 0;
};

class MenuItem: public AbstractMenuEntity { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	MenuItem(const char *name, IMenuRenderer *renderer);
	virtual void handleClick() = 0;
	virtual void handleDoubleClick() = 0;
	virtual void back() = 0;
};

class HomeMenu: public MenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	HomeMenu(HomeMenuItemRenderer *renderer, const char *name, AbstractMenuEntity *child, TimeSensor &timeSensor);
	void setTime(uint8_t hour, uint8_t min, uint8_t sec);
	void handleClick();
	void handleDoubleClick();
	void back();
	double getTemperature();
	struct TimeValue getTime();
	void activate();
	void update();
private:
	double temperature = 30;
	int humidity = 70;
	AbstractMenuEntity *child = nullptr;
	TimeSensor &_timeSensorModule;
	uint32_t lastUpdateTime = 0;

};

class SingleFieldMenuItem: public MenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	SingleFieldMenuItem(IMenuRenderer *renderer, const char* name, const char *label, uint8_t maxValue);
	int getValue();
	void setValue(int value);
	const char *getLabel();
	void handleClick();
	void handleDoubleClick();
	void save();
	void back();
    static constexpr uint8_t DATA_INDEX  = 0;
    static constexpr uint8_t SAVE_INDEX = 1;
    static constexpr uint8_t BACK_INDEX  = 2;
private:
	const char *label;
	uint8_t value;
	uint8_t maxValue;
	bool changeData = false;
};


#endif /* ABSTRACTMENUENTITY_H_ */
