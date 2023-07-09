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

enum EventType: unsigned int;
class IMenuRenderer;
class EventManager;
class HomeMenuItemRenderer;
class RemoteData;

class AbstractMenuEntity: public IEventReceiver, public IRenderable { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	AbstractMenuEntity(const char *name, IMenuRenderer *renderer);
	virtual const char* getName();
	uint8_t getBackIndex();
	void setCurrentIndex(int8_t index);
	int8_t getCurrentIndex();

	void handleEvent(EventType event);
	void render();


	bool isActive();
	void setActive(bool active);
	void setEventManager(EventManager *manager);
	virtual void activate();
	void deactivate();
	virtual void handleClick() = 0;
	virtual void handleDoubleClick() = 0;
	void back();
	// declare the stack
	static AbstractMenuEntity *CurrentMenu;
	const char *name;
protected:
	static CustomStack<AbstractMenuEntity *, 6> menuStack;
	int8_t currentIndex = -1;
	uint8_t backIndex = 0;
	bool active = false;
	EventManager *eventManager = nullptr;
	IMenuRenderer *renderer;
	static char stringBuffer[];

};

class MenuEntity: public AbstractMenuEntity { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	MenuEntity(IMenuRenderer *renderer, const char* name, AbstractMenuEntity* items[], uint8_t numitems);
	void activate();
	uint8_t getNumItems();
	virtual AbstractMenuEntity* getItem(uint8_t index);
	void handleClick();
	void goToNextItem();
	void handleDoubleClick();
protected:
	AbstractMenuEntity** items;
	uint8_t numItems = 0;
};

class MenuItem: public AbstractMenuEntity { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	MenuItem(const char *name, IMenuRenderer *renderer);
	virtual uint16_t getValue(uint8_t index) = 0;
	virtual const __FlashStringHelper* getLabel(uint8_t index) = 0;
};

class HomeMenu: public MenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	HomeMenu(
			HomeMenuItemRenderer *renderer,
			const char *name,
			AbstractMenuEntity *child,
			TimeSensor &timeSensor,
			TemperatureModule &tm,
			HumidityModule &hm);
	void handleClick();
	void handleDoubleClick();
	TemperatureValue getTemperature();
	HumidityValue getHumidity();
	struct TimeValue getTime();
	void activate();
	void update();
	uint16_t getValue(uint8_t index);
	const __FlashStringHelper* getLabel(uint8_t index);
private:
	AbstractMenuEntity *_child;
	TimeSensor &_timeSensorModule;
	uint32_t lastUpdateTime = 0;
	TemperatureModule &_tm;
	HumidityModule &_hm;
};


class FormMenuItem: public MenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	FormMenuItem(const char *name, IMenuRenderer *renderer):MenuItem(name, renderer){
		states = 2;
		backIndex = 1;
	}
	virtual uint16_t getValue(uint8_t index) = 0;
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
	void activate(){
		changeData = false;
		AbstractMenuEntity::activate();
	}
protected:
	virtual void updateData(int8_t currentIndex) = 0;
	uint8_t states;
	bool changeData = false;
};


class TimeMenuItem: public FormMenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	TimeMenuItem(IMenuRenderer *renderer, const char* name, TimeSensor &timeModule);
	void ok();
	void activate();

	virtual uint16_t getValue(uint8_t index);
	virtual const __FlashStringHelper* getLabel(uint8_t index);
	virtual void updateData(int8_t currentIndex);

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
	uint16_t getValue(uint8_t index);
	const __FlashStringHelper* getLabel(uint8_t index);
	void updateData(int8_t currentIndex);

	static constexpr uint8_t DAY_INDEX  = 0;
    static constexpr uint8_t MON_INDEX  = 1;
    static constexpr uint8_t YEAR_INDEX  = 2;

};

class RemoteTestMenuItem: public FormMenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	RemoteTestMenuItem(IMenuRenderer *renderer, TXSensor &tx):
		FormMenuItem(nullptr, renderer), _tx(tx)
	{
		states = 5;
		backIndex = states - 1;
	}
	virtual void ok();
	virtual const char * getName();
	virtual void updateData(int8_t currentIndex);
	virtual uint16_t getValue(uint8_t index);
	virtual const __FlashStringHelper* getLabel(uint8_t index);
	virtual boolean isReadOnly(uint8_t index){return true;}

	TemperatureRange _tr;
protected:
	static constexpr uint8_t START_RANGE_INDEX  = 0;
    static constexpr uint8_t END_RANGE_INDEX  = 1;
    static constexpr uint8_t CODE_INDEX  = 2;
private:
    TXSensor &_tx;

};

class RemoteProgramMenuItem: public RemoteTestMenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	RemoteProgramMenuItem(IMenuRenderer *renderer, const char* name, RXSensor &rx, TXSensor &tx, RemoteData &rd, TemperatureRange tr):
		RemoteTestMenuItem(renderer, tx), _rx(rx), _rd(rd)
	{
		this->name = name;
		states = 5;
		backIndex = states - 1;
		_tr = DefaultTemperatureRange;
	}
	void ok();
	const char * getName() {return AbstractMenuEntity::getName();}
	void updateData(int8_t currentIndex);
	boolean isReadOnly(uint8_t index){return false;}
	const __FlashStringHelper* getLabel(uint8_t index);
	void read();
private:
	RXSensor &_rx;
	RemoteData &_rd;
};

class DynamicMenuEntity: public MenuEntity { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	DynamicMenuEntity(
			IMenuRenderer *renderer,
			IMenuRenderer *subMenurenderer,
			const char *name,
			RemoteData &rd,
			TXSensor &tx);
	void activate();
	AbstractMenuEntity* getItem(uint8_t index);
private:
	RemoteData &_rd;
	RemoteTestMenuItem subMenu;
};

class ScheduleMenuItem: public FormMenuItem { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	ScheduleMenuItem(
			IMenuRenderer *renderer,
			RemoteData &rd, uint8_t index):
				FormMenuItem(nullptr, renderer),
				_rd(rd),
				_index(index)
	{
		states = 6;
		backIndex = states - 1;
		_schedule._begin._hr = 0;
		_schedule._begin._min = 0;
		_schedule._end._hr = 0;
		_schedule._end._min = 0;
	}
	void ok();
	const char * getName();
	void updateData(int8_t currentIndex);
	const __FlashStringHelper* getLabel(uint8_t index);
	uint16_t getValue(uint8_t index);
private:
	Schedule _schedule;
	RemoteData &_rd;
	uint8_t _index;

	static constexpr uint8_t SCHEDULE_START_HOUR  = 0;
	static constexpr uint8_t SCHEDULE_START_MIN  = 1;
	static constexpr uint8_t SCHEDULE_END_HOUR  = 2;
	static constexpr uint8_t SCHEDULE_END_MIN  = 3;

};

#endif /* ABSTRACTMENUENTITY_H_ */
