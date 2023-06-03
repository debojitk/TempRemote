/*
 * EventManager.cpp
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */
#include <Arduino.h>
#include <TimerOne.h>
#include "CommonItems.h"
#include "EventManager.h"
#include "AbstractMenuEntity.h"

EventManager::EventManager(IEventSourceObserver *observer){
	eventReceiver = nullptr;
	this->eventSourceObserver = observer;
}
void EventManager::registereventReceiver(IEventReceiver *eventReceiver){
	this->eventReceiver = eventReceiver;
}
IEventReceiver* EventManager::geteventReceiver(){
	return this->eventReceiver;
}
void EventManager::unregisterEventReceiver(){
	this->eventReceiver = nullptr;
}
void EventManager::handleEvent(EventType event){
	if(this->eventReceiver != nullptr){
		Serial.print(F("Handling event "));
		Serial.println(event);
		this->eventReceiver->handleEvent(event);
	}
}
void EventManager::processEvents(){
	EventType event = eventSourceObserver->getLastEvent();
	if (event != NoEvent){
		Serial.print(F("Event captured "));
		Serial.println(event);
		// process event
		handleEvent(event);
		eventSourceObserver->clearLastEvent();
		Serial.print(F("Event cleared "));
	} else{
		eventSourceObserver->observeEvents();
	}
}




EventType AbsEventSourceObserver::getLastEvent(){
	return lastEvent;
}
void AbsEventSourceObserver::clearLastEvent() {
	lastEvent = NoEvent;
}
void AbsEventSourceObserver::observeEvents(){
	// does nothing
}
void AbsEventSourceObserver::disable(){
	enabled = false;
}
void AbsEventSourceObserver::enable(){
	enabled = true;
}


void SerialObserver::initialize(){

}
void SerialObserver::observeEvents() {
	if(enabled && lastEvent == NoEvent){
		if (Serial.available()) {
			int input = Serial.parseInt();
			Serial.print(F("Event Received "));
			Serial.println(input);
			if (input == 1) {
				lastEvent = SingleClickEvent;
			} else if (input == 2) {
				lastEvent = DoubleClickEvent;
			} else{
				Serial.print(F("Unknown event"));
			}
		}
	}
}


ButtonInputObserver::ButtonInputObserver(){
	//default private constructor
}

ButtonInputObserver::ButtonInputObserver(int pin, int interval){
	buttonPin = pin;
	doubleClickInterval = interval;
	waitingForDoubleClick = false;
	lastButtonClickTime = 0;

}
void ButtonInputObserver::initialize(){
	pinMode(buttonPin, INPUT_PULLUP);
	Timer1.initialize(1000);
	Timer1.attachInterrupt(ButtonInputObserver::timerInterruptInvoker);
	enabled = true;
}
void ButtonInputObserver::disable(){
	if(enabled) {
		AbsEventSourceObserver::disable();
		Timer1.detachInterrupt();
	}
}
void ButtonInputObserver::enable() {
	if (!enabled) {
		AbsEventSourceObserver::disable();
		Timer1.attachInterrupt(ButtonInputObserver::timerInterruptInvoker);
	}
}

void ButtonInputObserver::timerInterrupt() {
	// scan for events only if last event is handled
	if(enabled && lastEvent == NoEvent) {
		unsigned long currentTime = millis();
		bool buttonState = digitalRead(buttonPin);

		if (buttonState == LOW) {
			if (!waitingForDoubleClick) {
				// First click detected, start waiting for double-click
				waitingForDoubleClick = true;
				lastButtonClickTime = currentTime;
			} else {
				// Double-click detection window
				if (currentTime - lastButtonClickTime <= doubleClickInterval) {
					// Double-click detected
					Serial.println(F("DoubleClickEvent"));
					lastEvent = DoubleClickEvent;
					waitingForDoubleClick = false;
				} else {
					// Timeout for double-click detection, treat as single click
					Serial.println(F("SingleClickEvent"));
					lastEvent = SingleClickEvent;
					waitingForDoubleClick = false;
				}
			}
		}
	}
}

void ButtonInputObserver::timerInterruptInvoker(){
	ButtonInputObserver::instance->timerInterrupt();
}

ButtonInputObserver * ButtonInputObserver::getInstance(int pin, int interval){
	if(ButtonInputObserver::instance == nullptr){
		ButtonInputObserver::instance = new ButtonInputObserver(pin, interval);
	}
	return ButtonInputObserver::instance;
}
