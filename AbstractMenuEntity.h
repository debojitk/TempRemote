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
class RemoteData;

using TimeSensor = Sensor<TimeModuleDS3231, TimeValue>;
using TempSensor = Sensor<TemperatureModule, TemperatureValue>;

class AbstractMenuEntity: public IEventReceiver, public IRenderable { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	AbstractMenuEntity(const char *name, IMenuRenderer *renderer);
	virtual const char* getName();
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
	static char stringBuffer[];

};

class IDynamicMenuItemProvider { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	virtual AbstractMenuEntity** getValues() = 0;
	virtual uint8_t getSize() = 0;
	virtual ~IDynamicMenuItemProvider() {}
};

class RemoteMenuItemProvider : public IDynamicMenuItemProvider {
	RemoteMenuItemProvider(RemoteData& rd) : _rd(rd) {}
	virtual ~RemoteMenuItemProvider() {
		free();
	}
	void free() {
		for(unsigned i = 0; i < _size; ++i){
			delete _values[i];
		}
		memset(_values, 0, CONFIG::NUM_INDEX);
	}
	AbstractMenuEntity** getValues();
	uint8_t getSize() { return _size; }

private:
	const RemoteData& _rd;
	size_t            _size = 0;
	AbstractMenuEntity* _values[CONFIG::NUM_INDEX];
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
protected:
	AbstractMenuEntity** items;
	uint8_t numItems = 0;
};

class DynamicMenuEntity: public MenuEntity { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	DynamicMenuEntity(IMenuRenderer *renderer, const char *name, IDynamicMenuItemProvider &valueProvider);
	void activate();
private:
	void setItems(AbstractMenuEntity* items[],uint8_t numitems);
	IDynamicMenuItemProvider &_valueProvider;
};

class MenuItem: public AbstractMenuEntity { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	MenuItem(const char *name, IMenuRenderer *renderer);
	virtual uint32_t getValue(uint8_t index) = 0;
	virtual const __FlashStringHelper* getLabel(uint8_t index) = 0;
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
	uint32_t getValue(uint8_t index);
	const __FlashStringHelper* getLabel(uint8_t index);
private:
	double temperature = 30;
	int humidity = 70;
	AbstractMenuEntity *child = nullptr;
	TimeSensor &_timeSensorModule;
	uint32_t lastUpdateTime = 0;
};


class FormMenuItem: public MenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	FormMenuItem(const char *name, IMenuRenderer *renderer):MenuItem(name, renderer){
		states = 2;
		backIndex = 1;
	}
	virtual uint32_t getValue(uint8_t index) = 0;
	virtual const __FlashStringHelper* getLabel(uint8_t index) = 0;
	virtual void handleClick();
	virtual void handleDoubleClick();
	virtual void ok() = 0;
	virtual uint8_t getFieldCount() {
		return states - 2;
	}
	uint8_t getStates() {
		return states;
	}
	virtual uint8_t getOkIndex() {
		return getFieldCount();
	}
	virtual boolean isReadOnly(uint8_t index);
protected:
	virtual void updateData(int currentIndex) = 0;
	uint8_t states;
	bool changeData = false;
};


class TimeMenuItem: public FormMenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	TimeMenuItem(IMenuRenderer *renderer, const char* name, TimeSensor &timeModule);
	void ok();
	void activate();

	virtual uint32_t getValue(uint8_t index);
	virtual const __FlashStringHelper* getLabel(uint8_t index);
	virtual void updateData(int currentIndex);

	static constexpr uint8_t HOUR_INDEX  = 0;
    static constexpr uint8_t MIN_INDEX  = 1;
    static constexpr uint8_t SEC_INDEX  = 2;
protected:
	TimeSensor &_timeModule;
	TimeValue _timeValue;
};

class DateMenuItem: public TimeMenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	DateMenuItem(IMenuRenderer *renderer, const char* name, TimeSensor &timeModule): TimeMenuItem(renderer, name, timeModule){}
	uint32_t getValue(uint8_t index);
	const __FlashStringHelper* getLabel(uint8_t index);
	void updateData(int currentIndex);

	static constexpr uint8_t DAY_INDEX  = 0;
    static constexpr uint8_t MON_INDEX  = 1;
    static constexpr uint8_t YEAR_INDEX  = 2;

};

/*class RemoteTestMenuItem: public MenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	RemoteTestMenuItem(IMenuRenderer *renderer);
	virtual void handleClick();
	virtual void handleDoubleClick();
	virtual void ok();
	void back();
	uint8_t getRangeEnd() const;
	void setRangeEnd(uint8_t rangeEnd = 0);
	uint8_t getRangeStart() const;
	void setRangeStart(uint8_t rangeStart = 0);
	const char * getName();
	uint8_t getFieldCount();

    static constexpr uint8_t STATES  = 2;
protected:
	uint8_t rangeStart = 0;
	uint8_t rangeEnd = 0;
private:
	uint32_t code = 0;
};

class RemoteProgramMenuItem: public RemoteTestMenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	RemoteProgramMenuItem(IMenuRenderer *renderer);
	void handleClick();
	void handleDoubleClick();
	void ok();
	void activate();
	const char * getName();
	uint8_t getFieldCount();

	static constexpr uint8_t START_RANGE_INDEX  = 0;
    static constexpr uint8_t END_RANGE_INDEX  = 1;
    static constexpr uint8_t CODE_INDEX  = 2;
    static constexpr uint8_t STATES  = 5;
private:
	bool changeData = false;
};*/



#endif /* ABSTRACTMENUENTITY_H_ */
