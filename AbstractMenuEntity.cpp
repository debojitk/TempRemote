/*
 * AbstractMenuEntity.cpp
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */
#include <Arduino.h>
#include <TimerOne.h>
#include "CommonItems.h"
#include "CustomStack.h"
#include "AbstractMenuEntity.h"
#include "EventManager.h"
#include "MenuItemRenderer.h"
#include "IMenuRenderer.h"
#include "Sensor.h"
#include "SensorTypes.h"
#include "HexProgrammer.h"

// define the stack
CustomStack<AbstractMenuEntity *, 6> AbstractMenuEntity::menuStack;
AbstractMenuEntity *AbstractMenuEntity::CurrentMenu = nullptr;

char AbstractMenuEntity::stringBuffer[10];
// Definition for AbstractMenuEntity
AbstractMenuEntity::AbstractMenuEntity(const char *name, IMenuRenderer *renderer) {
	this->name = name;
	this->renderer = renderer;
}
const char* AbstractMenuEntity::getName() {
	return this->name;
}

uint8_t AbstractMenuEntity::getBackIndex(){
	return this->backIndex;
}

void AbstractMenuEntity::setCurrentIndex(int8_t index){
	currentIndex = index;
}
int8_t AbstractMenuEntity::getCurrentIndex(){
	return currentIndex;
}

bool AbstractMenuEntity::isActive(){
	return active;
}

void AbstractMenuEntity::setActive(bool active){
	this->active = active;
	setCurrentIndex(-1);
}

void AbstractMenuEntity::handleEvent(EventType event){
	switch (event){
	case EventType::SingleClickEvent:
		handleClick();
		break;
	case EventType::DoubleClickEvent:
		handleDoubleClick();
		break;
	default:
		// do nothing
		break;
	}
}

void AbstractMenuEntity::setEventManager(EventManager *manager){
	eventManager = manager;
}


void AbstractMenuEntity::activate() {
	if (isActive()) return;
	SerialPrint(F("Activating ->"));
	SerialPrintln(getName());
	if(eventManager != nullptr){
		eventManager->registereventReceiver(this);
	}
	this->setActive(true);
	CurrentMenu = this;
	this->renderer->clear();
	render();
}

void AbstractMenuEntity::deactivate() {
	if (!isActive()) return;
	SerialPrint(F("Deactivating ->"));
	SerialPrintln(getName());
	setActive(false);
	if(eventManager != nullptr){
		eventManager->unregisterEventReceiver();
	}
	this->renderer->clear();
}

void AbstractMenuEntity::render(){
	this->renderer->renderMenu(this);
}

void AbstractMenuEntity::back(){
	AbstractMenuEntity *prevMenu = AbstractMenuEntity::menuStack.pop();
	if (prevMenu != nullptr) {
		deactivate();
		SerialPrint(F("Popped menu->"));
		SerialPrintln(prevMenu->getName());
		prevMenu->activate();
	}
}



// Definition for MenuEntity
MenuEntity::MenuEntity(IMenuRenderer *renderer, const char* name, AbstractMenuEntity* items[], uint8_t numitems): AbstractMenuEntity(name, renderer) {
	this->items = items;
	this->numItems = numitems;
	this->renderer = renderer;
	this->backIndex = numItems;
}

uint8_t MenuEntity::getNumItems() {
	return numItems;
}

void MenuEntity::activate(){
	AbstractMenuEntity::activate();
}

AbstractMenuEntity* MenuEntity::getItem(uint8_t index) {
	if (index >= 0 && index < numItems) {
		return items[index];
	} else {
		return nullptr;
	}
}
void MenuEntity::handleClick(){
	if (!this->isActive())
		return;
	goToNextItem();
	render();
}
void MenuEntity::goToNextItem() {
	currentIndex = (currentIndex + 1)%(numItems + 1);
//	SerialPrint(F("currentIndex = "));
//	SerialPrintln(currentIndex);
}

void MenuEntity::handleDoubleClick(){
	if (!this->isActive()) {
		this->activate();
	} else{
		if (currentIndex == backIndex){
			back();
		} else {
			AbstractMenuEntity *item = getItem(currentIndex);
			if (item != nullptr){
				deactivate();
				AbstractMenuEntity::menuStack.push(this);
				SerialPrint(F("Pushing menu->"));
				SerialPrintln(this->getName());
				item->setEventManager(eventManager);
				item->activate();
			}
		}
	}
}



// Definition for MenuItem

MenuItem::MenuItem(const char *name, IMenuRenderer *renderer): AbstractMenuEntity(name, renderer){

}

// Definition for HomeMenu

void HomeMenu::activate(){
	AbstractMenuEntity::activate();
}



HomeMenu::HomeMenu(HomeMenuItemRenderer *renderer,
		const char *name, AbstractMenuEntity *child,
		TimeSensor &timeSensor):MenuItem(name, renderer), _timeSensorModule(timeSensor) {
	this->child = child;
	this->temperature = 30;
}

void HomeMenu::handleClick(){
	// does nothing
}
void HomeMenu::handleDoubleClick(){
	if (this->child && !this->child->isActive()){
		deactivate();
		AbstractMenuEntity::menuStack.push(this);
		this->child->setEventManager(eventManager);
		this->child->activate();
	}
}


double HomeMenu::getTemperature(){
	return temperature;
}
struct TimeValue HomeMenu::getTime(){
	return _timeSensorModule.get();
}

void HomeMenu::update(){
	if (isActive()) {
		uint32_t currentTime = millis();
		if (lastUpdateTime > currentTime) {
			// handling millis overflow
			lastUpdateTime = currentTime;
		} else if ((currentTime - lastUpdateTime) > 1000) {
			lastUpdateTime = currentTime;
			render();
		}
	}
}

uint16_t HomeMenu::getValue(uint8_t index) {
	return 0;
}

const __FlashStringHelper* HomeMenu::getLabel(uint8_t index) {
	return nullptr;
}

// Definition for SingleFieldMenuItem


TimeMenuItem::TimeMenuItem(IMenuRenderer *renderer, const char *name,
		TimeSensor &timeModule): FormMenuItem(name, renderer),_timeModule(timeModule) {
	this->name = name;
	states = 5;
	backIndex = states - 1;
}

void TimeMenuItem::updateData(int8_t currentIndex) {
	switch(currentIndex) {
	case TimeMenuItem::HOUR_INDEX:
		_timeValue._hour = (_timeValue._hour + 1)%24;
		break;
	case TimeMenuItem::MIN_INDEX:
		_timeValue._min = (_timeValue._min + 1)%60;
		break;
	case TimeMenuItem::SEC_INDEX:
		_timeValue._sec = (_timeValue._sec + 1)%60;
		break;
	}
}

void TimeMenuItem::ok() {
	_timeModule.set(_timeValue);
}

void TimeMenuItem::activate() {
	_timeValue = _timeModule.get();
	AbstractMenuEntity::activate();
}

uint16_t TimeMenuItem::getValue(uint8_t index) {
	uint8_t retval = 0;
	if (index > getFieldCount() - 1) return retval;
	switch(index){
	case TimeMenuItem::HOUR_INDEX:
		retval = _timeValue._hour;
		break;
	case TimeMenuItem::MIN_INDEX:
		retval = _timeValue._min;
		break;
	case TimeMenuItem::SEC_INDEX:
		retval = _timeValue._sec;
		break;
	}
	return retval;
}

const __FlashStringHelper* TimeMenuItem::getLabel(uint8_t index) {
	if (index > states - 1) return nullptr;
	return (const __FlashStringHelper *)TimeMenuLabels[index];
}

/**
 *  DynamicMenuEntity definition
 */

DynamicMenuEntity::DynamicMenuEntity(
		IMenuRenderer *renderer,
		IMenuRenderer *subMenuRenderer,
		const char *name,
		RemoteData &rd,
		TXSensor &tx
): MenuEntity(renderer, name, nullptr, 0),
		_rd(rd), subMenu(subMenuRenderer, tx)
		 {

}

void DynamicMenuEntity::activate() {
	numItems = 0;
    for(auto it = _rd.beginRange(), itEnd = _rd.endRange(); it != itEnd; ++it) {
		numItems ++;
    }
	backIndex = numItems;
	AbstractMenuEntity::activate();
}

AbstractMenuEntity * DynamicMenuEntity::getItem(uint8_t index) {
	TEST::testMemory();
	if (index < 0 || index >= numItems) return nullptr;

	uint8_t counter = 0;
    for(auto it = _rd.beginRange(), itEnd = _rd.endRange(); it != itEnd; ++it) {
        TemperatureRange tr = *it;
        if (index == counter) {
        	subMenu._tr = tr;
        	return  &subMenu;
        }
        counter++;
    }
    return nullptr;
}
void FormMenuItem::handleClick() {
	if (!this->isActive())
		return;
	if(changeData){
		updateData(currentIndex);
	} else {
		currentIndex = (currentIndex + 1) % states;
	}
	render();

}

void FormMenuItem::handleDoubleClick() {
	if (currentIndex < 0 || currentIndex >= states || (currentIndex < getFieldCount() && isReadOnly(currentIndex))) return;
	if (currentIndex ==  getBackIndex()){
		back();
	} else if (currentIndex ==  getOkIndex()) {
		ok();
	} else {
		changeData = !changeData;
	}
}

boolean FormMenuItem::isReadOnly(uint8_t index) {
	if (index > -1 && index < getFieldCount()) {
		return false;
	}
	return true;
}

uint16_t DateMenuItem::getValue(uint8_t index) {
	uint16_t retval = 0;
	if (index > getFieldCount() - 1) return retval;
	switch(index){
	case DAY_INDEX:
		retval = _timeValue._day;
		break;
	case MON_INDEX:
		retval = _timeValue._month;
		break;
	case YEAR_INDEX:
		retval = _timeValue._year;
		break;
	}
	return retval;
}

const __FlashStringHelper* DateMenuItem::getLabel(uint8_t index) {
	if (index > states - 1) return nullptr;
	return (const __FlashStringHelper *)DateMenuLabels[index];
}

void DateMenuItem::updateData(int8_t currentIndex) {
	switch(currentIndex) {
	case DAY_INDEX:
		_timeValue._day = (_timeValue._day)%31 + 1;
		break;
	case MON_INDEX:
		_timeValue._month = (_timeValue._month)%12 + 1;
		break;
	case YEAR_INDEX:
		_timeValue._year = (_timeValue._year + 1);
		break;
	}
}

/**
 * RemoteTestMenuItem definition
 */
void RemoteTestMenuItem::ok() {
	if(_tx.set(_tr._hex)){
		SerialPrint(F("Emitting code"));
		_tr._hex.p();
	}
}

const char* RemoteTestMenuItem::getName() {
	sprintf(stringBuffer, "%2d-%2d", _tr._start, _tr._end);
	return stringBuffer;
}

void RemoteTestMenuItem::updateData(int8_t currentIndex) {
	//nothing to update
}

uint16_t RemoteTestMenuItem::getValue(uint8_t index) {
	uint16_t retval;
	if (index > 0 || index > getFieldCount())
		retval = 0;
	switch(index){
	case START_RANGE_INDEX:
		retval = _tr._start;
		break;
	case END_RANGE_INDEX:
		retval = _tr._end;
		break;
	case CODE_INDEX:
		retval = _tr._hex._command;
		break;
	}
	return retval;
}

const __FlashStringHelper* RemoteTestMenuItem::getLabel(uint8_t index) {
	if (index > states - 1) return nullptr;
	return (const __FlashStringHelper *)RemoteTestMenuLabels[index];
}

/**
 * RemoteProgramMenuItem definition
 */
void RemoteProgramMenuItem::ok() {
	if (_tr == DefaultTemperatureRange) return;
	if(_rd.addRange(_tr)){
		SerialPrintln(F("Saved range"));
		_tr.p();
	}
	//_rd.save();
}

void RemoteProgramMenuItem::updateData(int8_t currentIndex) {
	TEST::testMemory();
	switch(currentIndex) {
	case START_RANGE_INDEX:
		if (_tr._start > CONFIG::MAX_TEMPERATURE){
			_tr._start = CONFIG::START_TEMPERATURE;
		} else {
			_tr._start ++;
		}
		if (_tr._start >= _tr._end) _tr._end = _tr._start;
		break;
	case END_RANGE_INDEX:
		if (_tr._end > CONFIG::MAX_TEMPERATURE){
			_tr._end = min(_tr._start + 1, CONFIG::MAX_TEMPERATURE);
		} else {
			_tr._end ++;
		}
		break;
	case CODE_INDEX:
		// TODO: update code
		break;
	}
}

void RemoteProgramMenuItem::read() {
	if(!isActive()) return;
	RemoteRXValue remoteValue =  _rx.get();
	if (remoteValue == NullRemoteRXValue) return;
	SerialPrint(F("Received code: "));
	SerialPrintln(remoteValue._command, HEX);

	if (currentIndex == CODE_INDEX && changeData == true) {
		if (_tr._hex._command != remoteValue._command) {
			_tr._hex = remoteValue;
			render();
		}
	}
}

const __FlashStringHelper* RemoteProgramMenuItem::getLabel(uint8_t index) {
	if (index > states - 1) return nullptr;
	return (const __FlashStringHelper *)RemoteProgramMenuLabels[index];
}
