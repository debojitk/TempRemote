/*
 * AbstractMenuEntity.cpp
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */
#include "CommonItems.h"
#include "CustomStack.h"
#include "AbstractMenuEntity.h"
#include "EventManager.h"
#include "MenuItemRenderer.h"
#include <Arduino.h>
#include "IMenuRenderer.h"

// define the stack
CustomStack<AbstractMenuEntity *, 10> AbstractMenuEntity::menuStack;

// Definition for AbstractMenuEntity
AbstractMenuEntity::AbstractMenuEntity(const char *name, IMenuRenderer *renderer) {
	this->name = name;
	this->renderer = renderer;
}
const char* AbstractMenuEntity::getName() {
	return this->name;
}

void AbstractMenuEntity::setBackIndex(int index){
	this->backIndex = index;
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
	Serial.print(F("menu is not active, activating ->"));
	Serial.println(getName());
	if(eventManager != nullptr){
		eventManager->registereventReceiver(this);
	}
	this->setActive(true);
	render();
}

void AbstractMenuEntity::render(){
	this->renderer->renderMenu(this);
}


// Definition for MenuEntity
MenuEntity::MenuEntity(IMenuRenderer *renderer, const char* name, AbstractMenuEntity* items[], int numItems): AbstractMenuEntity(name, renderer) {
	this->items = items;
	this->numItems = numItems;
	this->renderer = renderer;
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
	//renderer->renderMenu(this);
	render();
}
void MenuEntity::goToNextItem() {
	currentIndex = (currentIndex + 1)%numItems;
	Serial.print(F("currentIndex = "));
	Serial.println(currentIndex);
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
				this->setActive(false);
				AbstractMenuEntity::menuStack.push(this);
				Serial.print(F("Pushing menu->"));
				Serial.println(this->getName());
				item->activate();
			}
		}
	}
}

void MenuEntity::back(){
	Serial.print(F("menu is deactivated ->"));
	Serial.println(getName());
	this->setActive(false);
	AbstractMenuEntity *prevMenu = AbstractMenuEntity::menuStack.pop();
	if (prevMenu != nullptr) {
		Serial.print(F("Popped menu->"));
		Serial.println(prevMenu->getName());
		prevMenu->activate();
	}
}



// Definition for MenuItem

MenuItem::MenuItem(const char *name, IMenuRenderer *renderer): AbstractMenuEntity(name, renderer){

}

// Definition for HomeMenu

HomeMenu::HomeMenu(HomeMenuItemRenderer *renderer, const char *name, AbstractMenuEntity *child):MenuItem(name, renderer) {
	this->child = child;
	this->setTime(12, 15, 17);
	this->temperature = 30;
}

void HomeMenu::handleClick(){
	// does nothing
}
void HomeMenu::handleDoubleClick(){
	if (this->child && !this->child->isActive()){
		AbstractMenuEntity::menuStack.push(this);
		this->child->activate();
	}
}
void HomeMenu::activate(){
	Serial.println(F("Activating menu"));
	AbstractMenuEntity::activate();
}
void HomeMenu::back(){
	// does nothing
}

void HomeMenu::setTime(uint8_t hour, uint8_t min, uint8_t sec){
	cTime.hours = hour;
	cTime.minutes = min;
	cTime.seconds = sec;
}

double HomeMenu::getTemperature(){
	return temperature;
}
struct CurrentTime HomeMenu::getTime(){
	return cTime;
}


