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
		Serial.println(F("Event cleared "));
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


ButtonInputObserver *ButtonInputObserver::instance = nullptr;

ButtonInputObserver::ButtonInputObserver(): ButtonInputObserver::ButtonInputObserver(2,500){
	//default private constructor
}

ButtonInputObserver::ButtonInputObserver(int pin, int interval){
	buttonPin = pin;
	doubleClickInterval = interval;

}
void ButtonInputObserver::initialize(){
	if (hasInitialized) return;
	pinMode(buttonPin, INPUT_PULLUP);// by default the value is high, need to be shorted with ground to generate a low input
	Timer1.initialize(100000);
	//Timer1.attachInterrupt(ButtonInputObserver::timerInterruptInvoker);
	hasInitialized = true;
}
void ButtonInputObserver::disable(){
	if(enabled) {
		AbsEventSourceObserver::disable();
		Timer1.detachInterrupt();
	}
}
void ButtonInputObserver::enable() {
	if (!enabled) {
		AbsEventSourceObserver::enable();
		Timer1.attachInterrupt(ButtonInputObserver::timerInterruptInvoker);
	}
}

bool ButtonInputObserver::hasClicked() {
	lastClickCount = counter;
	currentButtonState = digitalRead(buttonPin);

	if (currentButtonState != lastButtonState) {
		lastDebounceTime = millis();
	}

	if ((millis() - lastDebounceTime) > debounceDelay) {
		if (currentButtonState != buttonState) {
			buttonState = currentButtonState;
			if (buttonState == LOW) {
				counter++;
				//Serial.println(counter);
			}
		}
	}
	lastButtonState = currentButtonState;

	return counter-lastClickCount;
}
void ButtonInputObserver::timerInterrupt() {
	// scan for events only if last event is handled
	if(enabled && lastEvent == NoEvent) {
		unsigned long currentTime = millis();
		bool clicked = hasClicked();
		if (clicked) {
			clickInstant = millis();
			currentTime = clickInstant;
			clickCount++;
		}
		if (clickCount == 1) {
			if ((currentTime - clickInstant) > doubleClickInterval){
				Serial.println(F("SingleClickEvent"));
				lastEvent = SingleClickEvent;
				clickCount = 0;
				clickInstant = 0;
			}
		} else if (clickCount == 2) {
			Serial.println(F("DoubleClickEvent"));
			lastEvent = DoubleClickEvent;
			clickCount = 0;
		}
	}
}

void ButtonInputObserver::timerInterruptInvoker(){
	ButtonInputObserver::instance->timerInterrupt();
}

ButtonInputObserver * ButtonInputObserver::getInstance(int pin, int interval){
	if(ButtonInputObserver::instance == nullptr){
		ButtonInputObserver::instance = new ButtonInputObserver(pin, interval);
		ButtonInputObserver::instance->initialize();
	}
	return ButtonInputObserver::instance;
}
