/*
 * AbstractMenuEntity.h
 *
 *  Created on: 14-May-2023
 *      Author: debojitk
 */

#ifndef ABSTRACTMENUENTITY_H_
#define ABSTRACTMENUENTITY_H_

#include <Arduino.h>
#include "CustomStack.h"
#include "CommonItems.h"
#include "SensorTypes.h"

template <typename SensorModule, typename Value>
class Sensor;
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
	virtual int getValue(uint8_t index){return 0;}
	virtual const char *getLabel(uint8_t index){return nullptr;}
	virtual uint8_t getFieldCount(){return 0;}
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

class TimeMenuItem: public MenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	TimeMenuItem(IMenuRenderer *renderer, const char* name, TimeSensor &timeModule);
	void handleClick();
	void handleDoubleClick();
	struct TimeValue getTime();
	void save();
	void back();
	void activate();

	int getValue(uint8_t index);
	const char *getLabel(uint8_t index);
	uint8_t getFieldCount();

	static constexpr uint8_t HOUR_INDEX  = 0;
    static constexpr uint8_t MIN_INDEX  = 1;
    static constexpr uint8_t SEC_INDEX  = 2;
    static constexpr uint8_t SAVE_INDEX = 3;
    static constexpr uint8_t BACK_INDEX  = 4;
    static constexpr uint8_t STATES  = 5;
private:
	TimeSensor &_timeModule;
	bool changeData = false;
	TimeValue _timeValue;
    static const char *labels[];
};


#endif /* ABSTRACTMENUENTITY_H_ */
