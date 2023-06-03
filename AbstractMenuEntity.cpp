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
#include "MenuRenderer.h"
#include <Arduino.h>

// define the stack
CustomStack<AbstractMenuEntity *, 10> AbstractMenuEntity::menuStack;

AbstractMenuEntity::AbstractMenuEntity(const char *name) {
	this->name = name;
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

MenuEntity::MenuEntity(MenuRenderer *renderer, const char* name, AbstractMenuEntity* items[], int numItems): AbstractMenuEntity(name) {
	this->items = items;
	this->numItems = numItems;
	this->renderer = renderer;
}

void MenuEntity::activate() {
	Serial.print(F("menu is not active, activating ->"));
	Serial.println(getName());
	if(eventManager != nullptr){
		eventManager->registereventReceiver(this);
	}
	this->setActive(true);
	renderer->renderMenu(this);
}

int MenuEntity::getNumItems() {
	return numItems;
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
	renderer->renderMenu(this);
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



MenuItem::MenuItem(const char *name): AbstractMenuEntity(name){

}

FunctionMenuItem::FunctionMenuItem(const char* name, void (*func)()): MenuItem(name) {
	this->func = func;
}

void FunctionMenuItem::select() {
	func();
}
