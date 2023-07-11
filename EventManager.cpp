/*
 * EventManager.cpp
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */
#include <Arduino.h>
#include "CommonItems.h"
#include "EventManager.h"
#include "AbstractMenuEntity.h"
//#include <MSTimer2.h>
#include <TimerOne.h>

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
		//SerialPrint(F("Handling event "));
		//SerialPrintln(event);
		this->eventReceiver->handleEvent(event);
	}
}
void EventManager::processEvents(){
	EventType event = eventSourceObserver->getLastEvent();
	if (event != NoEvent){
		//SerialPrint(F("Event captured "));
		//SerialPrintln(event);
		// process event
		handleEvent(event);
		if (_eventCallback != nullptr) {
			_eventCallback(event);
		}
		eventSourceObserver->clearLastEvent();
		//SerialPrintln(F("Event cleared "));
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
	Timer1.initialize(10000);
	//Timer1.attachInterrupt(ButtonInputObserver::timerInterruptInvoker);
	//MsTimer2::set(10, ButtonInputObserver::timerInterruptInvoker);
	hasInitialized = true;
}
void ButtonInputObserver::disable(){
	if(enabled) {
		AbsEventSourceObserver::disable();
		Timer1.detachInterrupt();
		//MsTimer2::stop();
		SerialPrintln(F("ButtonInputObserver disabled"));
	}
}
void ButtonInputObserver::enable() {
	initialize();
	if (!enabled) {
		AbsEventSourceObserver::enable();
		Timer1.attachInterrupt(ButtonInputObserver::timerInterruptInvoker);
		//MsTimer2::start();
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
	}
	return ButtonInputObserver::instance;
}


SleepWakeupInterruptHandler *SleepWakeupInterruptHandler::_instance = nullptr;

SleepWakeupInterruptHandler::SleepWakeupInterruptHandler(): SleepWakeupInterruptHandler::SleepWakeupInterruptHandler(2,5000, 30){
	//default private constructor
}

SleepWakeupInterruptHandler::SleepWakeupInterruptHandler(uint8_t pin, uint32_t disableDelay, uint8_t autoWakeupDelay) {
	this->pin = pin;
	this->disableDelay = disableDelay;
	setAutoWakeupDelay(autoWakeupDelay);

	AbsEventSourceObserver::disable();
}

void SleepWakeupInterruptHandler::setAutoWakeupDelay(uint8_t autoWakeupDelay){
	this->autoWakeupDelay = autoWakeupDelay;
	sleepCounterLimit = ceil(autoWakeupDelay*1.0 / 8.0f);
}

void SleepWakeupInterruptHandler::enable() {
	if(enabled) return;
	AbsEventSourceObserver::enable();
	SerialPrintln(F("SleepWakeupInterruptHandler enabled"));
	attachInterrupt(digitalPinToInterrupt(pin), SleepWakeupInterruptHandler::interruptHandlerInvoker, LOW);
	// real sleep code
	SerialPrintln(F("WDInterrupt enabled"));
	enableWDInterrupt();

}

void SleepWakeupInterruptHandler::disable() {
	if(!enabled) return;
	AbsEventSourceObserver::disable();
	SerialPrintln(F("SleepWakeupInterruptHandler disabled"));
	detachInterrupt(digitalPinToInterrupt(pin));
	// real wakeup code
	SerialPrintln(F("WDInterrupt disabled"));
	disableWDInterrupt();

}

void SleepWakeupInterruptHandler::initialize() {
	if(hasInitialized) return;
	pinMode(pin, INPUT_PULLUP);// by default the value is high, need to be shorted with ground to generate a low input
	setupSleep();
	setupWDTimer();
	hasInitialized = true;
}

SleepWakeupInterruptHandler* SleepWakeupInterruptHandler::getInstance(uint8_t pin, uint32_t interval, uint8_t autoWakeupDelay) {
	if(SleepWakeupInterruptHandler::_instance == nullptr){
		SleepWakeupInterruptHandler::_instance = new SleepWakeupInterruptHandler(pin, interval, autoWakeupDelay);
		SleepWakeupInterruptHandler::_instance->initialize();
	}
	return SleepWakeupInterruptHandler::_instance;
}

void SleepWakeupInterruptHandler::interruptHandlerInvoker() {
	SleepWakeupInterruptHandler::_instance->interruptHandler();
}

void SleepWakeupInterruptHandler::sleep() {
	if (_sleepCallback) {
		_sleepCallback();
	}

	enable();
	goToSleep();
}

void SleepWakeupInterruptHandler::wakeup() {

	disable();
	if (_wakeupCallback) {
		_wakeupCallback();
	}
}

void SleepWakeupInterruptHandler::interruptHandler() {
	SerialPrintln(F("interruptHandler executed"));
	clearLastEvent();
	wakeup();
}

void SleepWakeupInterruptHandler::clearLastEvent() {
	//SerialPrintln(F("lastEventInstant reset"));
	lastEventInstant = millis();
}

void SleepWakeupInterruptHandler::observeEvents() {
	if (wdtExecuted) {
		wdtExecuted = false;
		enableADC();
		// check if enough slept
		if ((sleepCounter + 1) >= sleepCounterLimit) {
			// perform necessary activity
			noInterrupts();
			_autoWakeupCallback();
			interrupts();
			sleepCounter = 0;
		}
		sleepCounter ++;
		autoWakedUp = true;
	}
	if (!enabled) {
		if (millis()<lastEventInstant) lastEventInstant = millis();
		if ((millis() - lastEventInstant) > disableDelay) {
			// sleeping due to timeout
			sleep();
		}
	}
	if (autoWakedUp) {
		//SerialPrintln(F("Should go to sleep"));
		autoWakedUp = false;
		goToSleep();
	}
}

void SleepWakeupInterruptHandler::setSleepCallback(void (*cb)()){
	this->_sleepCallback = cb;
}
void SleepWakeupInterruptHandler::setWakeupCallback(void (*cb)()){
	this->_wakeupCallback = cb;
}
void SleepWakeupInterruptHandler::setAutoWakeupCallback(void (*cb)()){
	this->_autoWakeupCallback = cb;
}


// arduino low power code
/**
 * Disables WD timer interrupt
 */
void SleepWakeupInterruptHandler::disableWDInterrupt(){
	  cli(); // Disable interrupts during register update
	  WDTCSR &= ~(1 << WDIE); // Disable watchdog timer interrupt
	  sei(); // Enable interrupts after register update
	  // reset the conuter so that while enabled it can sleep to full time
	  sleepCounter = 0;
	  autoWakedUp = false;
}
/**
 * Enables watchdog timer interrupt
 */
void SleepWakeupInterruptHandler::enableWDInterrupt(){
	  cli(); // Disable interrupts during register update
	  WDTCSR |= (1 << WDIE); // Enable watchdog timer interrupt
	  sei(); // Enable interrupts after register update
}
/**
 * Configures watchdog timer to invoke WD ISR every 8 secs
 */
void SleepWakeupInterruptHandler::setupWDTimer(uint8_t delay){
	//enable Timer to generate interrupt every 8 seconds
	WDTCSR = (1 << WDCE) | (1 << WDE); // enable configuration changes
	WDTCSR = (1 << WDP3) | (1 << WDP0); // set prescaler bits for 8-second timeout
	//WDTCSR |= (1 << WDIE); // enable watchdog interrupt
	disableWDInterrupt();
}
void SleepWakeupInterruptHandler::WDInterruptHandlerInvoker(){
	_instance->WDInterruptHandler();
}

/**
 * Configures sleep mode to go to power down mode
 */
void SleepWakeupInterruptHandler::setupSleep(){
	//ENABLE SLEEP - this enables the sleep mode
	SMCR |= (1 << 2); //power down mode
	SMCR |= 1;//enable sleep
}
/**
 * Goes to sleep for 8 sec, if wd interrupt awakes it
 */
void SleepWakeupInterruptHandler::goToSleep(){
	SerialPrint(F("Going to sleep - "));
	SerialPrintlnWithDelay(millis());
	disableADC();
	//BOD DISABLE - this must be called right before the __asm__ sleep instruction
	MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
	MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
	__asm__  __volatile__("sleep");//in line assembler to go to sleep

}
void SleepWakeupInterruptHandler::disableADC(){
	//Disable ADC - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
	ADCSRA &= ~(1 << 7);
}
void SleepWakeupInterruptHandler::enableADC(){
	ADCSRA |= (1 << 7);
}

ISR(WDT_vect){
	//DON'T FORGET THIS!  Needed for the watch dog timer.  This is called after a watch dog timer timeout - this is the interrupt function called after waking up
	 SleepWakeupInterruptHandler::WDInterruptHandlerInvoker();
}


void SleepWakeupInterruptHandler::WDInterruptHandler(){
	wdtExecuted = true;
}
