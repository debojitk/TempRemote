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
void EventManager::setEventCallback(void (*eventCallback)(EventType)){
	_eventCallback = eventCallback;
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
		SerialPrint(F("Handling event "));
		SerialPrintln(event);
		this->eventReceiver->handleEvent(event);
	}
}
void EventManager::processEvents(){
	EventType event = eventSourceObserver->getLastEvent();
	if (event != NoEvent){
		SerialPrint(F("Event captured "));
		SerialPrintln(event);
		// process event
		handleEvent(event);
		if (_eventCallback != nullptr) {
			_eventCallback(event);
		}
		eventSourceObserver->clearLastEvent();
		SerialPrintln(F("Event cleared "));
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
			SerialPrint(F("Event Received "));
			SerialPrintln(input);
			if (input == 1) {
				lastEvent = SingleClickEvent;
			} else if (input == 2) {
				lastEvent = DoubleClickEvent;
			} else{
				SerialPrint(F("Unknown event"));
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
		SerialPrintln(F("ButtonInputObserver disabled"));
	}
}
void ButtonInputObserver::enable() {
	if (!enabled) {
		AbsEventSourceObserver::enable();
		Timer1.attachInterrupt(ButtonInputObserver::timerInterruptInvoker);
		SerialPrintln(F("ButtonInputObserver enabled"));
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
				SerialPrintln(F("SingleClickEvent"));
				lastEvent = SingleClickEvent;
				clickCount = 0;
				clickInstant = 0;
			}
		} else if (clickCount == 2) {
			SerialPrintln(F("DoubleClickEvent"));
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


SleepWakeupInterruptHandler *SleepWakeupInterruptHandler::_instance = nullptr;

SleepWakeupInterruptHandler::SleepWakeupInterruptHandler(): SleepWakeupInterruptHandler::SleepWakeupInterruptHandler(2,5000){
	//default private constructor
}

SleepWakeupInterruptHandler::SleepWakeupInterruptHandler(uint8_t pin,
		uint16_t disableDelay) {
	this->pin = pin;
	this->disableDelay = disableDelay;
	AbsEventSourceObserver::disable();
}

void SleepWakeupInterruptHandler::enable() {
	if(enabled) return;
	AbsEventSourceObserver::enable();
	SerialPrintln(F("SleepWakeupInterruptHandler enabled"));
	attachInterrupt(digitalPinToInterrupt(pin), SleepWakeupInterruptHandler::interruptHandlerInvoker, LOW);
}

void SleepWakeupInterruptHandler::disable() {
	if(!enabled) return;
	AbsEventSourceObserver::disable();
	SerialPrintln(F("SleepWakeupInterruptHandler disabled"));
	detachInterrupt(digitalPinToInterrupt(pin));
}

void SleepWakeupInterruptHandler::initialize() {
	if(hasInitialized) return;
	pinMode(pin, INPUT_PULLUP);// by default the value is high, need to be shorted with ground to generate a low input
	hasInitialized = true;
}

SleepWakeupInterruptHandler* SleepWakeupInterruptHandler::getInstance(uint8_t pin, uint16_t interval) {
	if(SleepWakeupInterruptHandler::_instance == nullptr){
		SleepWakeupInterruptHandler::_instance = new SleepWakeupInterruptHandler(pin, interval);
		SleepWakeupInterruptHandler::_instance->initialize();
	}
	return SleepWakeupInterruptHandler::_instance;
}

void SleepWakeupInterruptHandler::interruptHandlerInvoker() {
	SleepWakeupInterruptHandler::_instance->interruptHandler();
}

void SleepWakeupInterruptHandler::sleep() {
	enable();
	if (_sleepCallback) {
		_sleepCallback();
	}
}

void SleepWakeupInterruptHandler::wakeup() {
	disable();
	if (_wakeupCallback) {
		_wakeupCallback();
	}
}

void SleepWakeupInterruptHandler::interruptHandler() {
	SerialPrintln(F("interruptHandler executed"));
	wakeup();
	clearLastEvent();
}

void SleepWakeupInterruptHandler::clearLastEvent() {
	lastEventInstant = millis();
}

void SleepWakeupInterruptHandler::observeEvents() {
	if (!enabled) {
		if (millis()<lastEventInstant) lastEventInstant = millis();
		if ((millis() - lastEventInstant) > disableDelay) {
			sleep();
		}
	}
}

void SleepWakeupInterruptHandler::setSleepCallback(void (*cb)()){
	this->_sleepCallback = cb;
}
void SleepWakeupInterruptHandler::setWakeupCallback(void (*cb)()){
	this->_wakeupCallback = cb;
}

