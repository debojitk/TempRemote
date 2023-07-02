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
CustomStack<AbstractMenuEntity *, 10> AbstractMenuEntity::menuStack;
AbstractMenuEntity *AbstractMenuEntity::CurrentMenu = nullptr;

// Definition for AbstractMenuEntity
AbstractMenuEntity::AbstractMenuEntity(const char *name, IMenuRenderer *renderer) {
	this->name = name;
	this->renderer = renderer;
}
const char* AbstractMenuEntity::getName() {
	return this->name;
}

int AbstractMenuEntity::getBackIndex(){
	return this->backIndex;
}

void AbstractMenuEntity::setCurrentIndex(int index){
	currentIndex = index;
}
int AbstractMenuEntity::getCurrentIndex(){
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

int MenuEntity::getNumItems() {
	return numItems;
}

void MenuEntity::activate(){
	AbstractMenuEntity::activate();
}

AbstractMenuEntity* MenuEntity::getItem(int index) {
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
	SerialPrint(F("currentIndex = "));
	SerialPrintln(currentIndex);
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
		SerialPrintln(F("HomeMenu detachInterrupt"));
		Timer1.detachInterrupt();
	}
}

void HomeMenu::back(){
	// does nothing
}

void HomeMenu::setTime(uint8_t hour, uint8_t min, uint8_t sec){

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

// Definition for SingleFieldMenuItem

SingleFieldMenuItem::SingleFieldMenuItem(IMenuRenderer *renderer,const char *name, const char *label, uint8_t maxValue):MenuItem(name, renderer) {
	this->label = label;
	this->value = 0;
	this->backIndex = SingleFieldMenuItem::BACK_INDEX;
	this->maxValue = maxValue;
}

int SingleFieldMenuItem::getValue() {
	return this->value;
}

void SingleFieldMenuItem::setValue(int value) {
	this->value = value;
}

const char* SingleFieldMenuItem::getLabel() {
	return this->label;
}

void SingleFieldMenuItem::handleClick() {
	if (!this->isActive())
		return;
	if(changeData){
		value = (value + 1)%(maxValue+1);
	} else {
		currentIndex = (currentIndex + 1)%3;
	}
	SerialPrint(F("Rendering on click:"));
	SerialPrintln(currentIndex);
	render();
}

void SingleFieldMenuItem::handleDoubleClick() {

	switch(currentIndex){
	case SingleFieldMenuItem::BACK_INDEX:
		back();
		break;
	case SingleFieldMenuItem::DATA_INDEX:
		changeData = !changeData;
		break;
	case SingleFieldMenuItem::SAVE_INDEX:
		save();
		break;
	default:
		break;
	}
}

void SingleFieldMenuItem::save() {
	//TODO: Need to implement a IKeyValuePairRepository
}

void SingleFieldMenuItem::back() {
	AbstractMenuEntity::back();
}


const char* TimeMenuItem::labels[] = {"Hour", "Minutes", "Seconds"};
TimeMenuItem::TimeMenuItem(IMenuRenderer *renderer, const char *name,
		TimeSensor &timeModule): MenuItem(name, renderer),_timeModule(timeModule) {
	this->name = name;
}

void TimeMenuItem::handleClick() {
	if (!this->isActive())
		return;
	if(changeData){
		switch(currentIndex){
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
	} else {
		currentIndex = (currentIndex + 1)%TimeMenuItem::STATES;
	}
	render();
}

void TimeMenuItem::handleDoubleClick() {
	switch(currentIndex){
	case TimeMenuItem::BACK_INDEX:
		back();
		break;
	case TimeMenuItem::SAVE_INDEX:
		save();
		break;
	case TimeMenuItem::HOUR_INDEX:
	case TimeMenuItem::MIN_INDEX:
	case TimeMenuItem::SEC_INDEX:
		changeData = !changeData;
		break;
	default:
		break;
	}
}

struct TimeValue TimeMenuItem::getTime() {
	return _timeValue;
}

void TimeMenuItem::save() {
	_timeModule.set(_timeValue);
}

void TimeMenuItem::back() {
	AbstractMenuEntity::back();
}

void TimeMenuItem::activate() {
	_timeValue = _timeModule.get();
	AbstractMenuEntity::activate();
}

int TimeMenuItem::getValue(uint8_t index) {
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

const char* TimeMenuItem::getLabel(uint8_t index) {
	if (index > getFieldCount() - 1) return nullptr;
	return TimeMenuItem::labels[index];
}

uint8_t TimeMenuItem::getFieldCount() {
	return sizeof(TimeMenuItem::labels)/ sizeof(char *);
}

/**
 *  DynamicMenuEntity definition
 */

DynamicMenuEntity::DynamicMenuEntity(
		IMenuRenderer *renderer,
		const char *name,
		IDynamicMenuItemProvider &valueProvider
	): MenuEntity(renderer, name, nullptr, 0), _valueProvider(valueProvider) {

}

void DynamicMenuEntity::activate() {
	setItems(_valueProvider.getValues(), _valueProvider.getSize());
	AbstractMenuEntity::activate();
}

void DynamicMenuEntity::setItems(AbstractMenuEntity *incomingItems[], uint8_t incomingNumItems) {
	if (this->items != incomingItems) {
		// need to modify, delete the existing array first
		for (int i = 0; i<numItems; i++) {
			delete this->items[i];
		}
		delete[] this->items;
		numItems = incomingNumItems;
		this->items = incomingItems;
	}
}


/**
 * RemoteProgramMenuItem definition
 */
RemoteProgramMenuItem::RemoteProgramMenuItem(IMenuRenderer *renderer): MenuItem(nullptr, renderer) {
}

void RemoteProgramMenuItem::handleClick() {
	if (!isActive()) return;
	if (testMode) {
		currentIndex = (currentIndex + 1)%STATES;
	} else {
		if (!testMode && changeData) {
			switch (currentIndex) {
			case START_RANGE_INDEX:
				rangeStart = (rangeStart + 1)%30;
				break;
			case END_RANGE_INDEX:
				rangeEnd = (rangeEnd + 1)%30;
				break;
			case CODE_INDEX:
				//TODO: handle Remote code
				break;
			default:
				break;
			}
		} else {
			currentIndex = (currentIndex + 1)%STATES;
		}
	}
	render();
}

void RemoteProgramMenuItem::handleDoubleClick() {
	if (!isActive()) return;

	if ( currentIndex ==  STATES - 2) {
		testMode? test(): save();
	} else if (currentIndex == STATES - 1) {
		back();
	} else if (currentIndex != -1){
		if (!testMode)	changeData = !changeData;
	}
}

void RemoteProgramMenuItem::test() {
}

void RemoteProgramMenuItem::back() {
	AbstractMenuEntity:: back();
}

void RemoteProgramMenuItem::activate() {
	AbstractMenuEntity:: activate();
}

uint8_t RemoteProgramMenuItem::getRangeEnd() const {
	return rangeEnd;
}

void RemoteProgramMenuItem::setRangeEnd(uint8_t rangeEnd = 0) {
	this->rangeEnd = rangeEnd;
}

uint8_t RemoteProgramMenuItem::getRangeStart() const {
	return rangeStart;
}

void RemoteProgramMenuItem::setRangeStart(uint8_t rangeStart = 0) {
	this->rangeStart = rangeStart;
}

bool RemoteProgramMenuItem::isTestMode() const {
	return testMode;
}

void RemoteProgramMenuItem::save() {
}

void RemoteProgramMenuItem::setTestMode(bool testMode = true) {
	this->testMode = testMode;
	if (testMode == true) {
		STATES = 2; //TEST and BACK
	} else {
		STATES = 5;
	}
}

const char* RemoteProgramMenuItem::getName() {
	static char buffer[6];
	sprintf(buffer, "%2d-%2d", rangeStart, rangeEnd);
	return buffer;
}

AbstractMenuEntity**
RemoteMenuItemProvider::getValues() {
	free();
	_size = 0;
	for(auto it = _rd.beginRange(), itEnd = _rd.endRange();
	        it != itEnd; ++it) {
	    TemperatureRange tr = *it;
	    RemoteProgramMenuItem* ptr = new RemoteProgramMenuItem(nullptr);
	    ptr->setRangeStart(tr._start);
	    ptr->setRangeEnd(tr._end);
	    _values[_size] = ptr;
	    ++_size;
	}
	return _values;
}

