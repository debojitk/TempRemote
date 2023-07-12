/*
 * EventManager.h
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */

#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

enum EventType: uint8_t;
class IEventReceiver;
class SleepWakeupInterruptHandler;

class IEventSourceObserver{ // @suppress("Class has a virtual method and non-virtual destructor")
public:
	virtual EventType getLastEvent() = 0;
	virtual void clearLastEvent() = 0;
	virtual void observeEvents() = 0;
	virtual void initialize() = 0;
	virtual void disable() = 0;
	virtual void enable() = 0;
};

class EventManager {
public:
	EventManager(IEventSourceObserver *observer);
	void registereventReceiver(IEventReceiver *eventReceiver);
	IEventReceiver * geteventReceiver();
	void unregisterEventReceiver();
	void handleEvent(EventType event);
	void processEvents();
	void setEventCallback(void (*_eventCallback)(EventType));
private:
	IEventReceiver *eventReceiver;
	IEventSourceObserver *eventSourceObserver;
	void (*_eventCallback)(EventType) = nullptr;

};

class AbsEventSourceObserver: public IEventSourceObserver { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	EventType getLastEvent();
	void clearLastEvent();
	void observeEvents();
	void disable();
	void enable();
protected:
	EventType lastEvent = 0;
	bool enabled = false;
};

class SerialObserver: public AbsEventSourceObserver { // @suppress("Class has a virtual method and non-virtual destructor")

public:
	void initialize();
	void observeEvents();
};

class ButtonInputObserver: public  AbsEventSourceObserver { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	void initialize();
	void disable();
	void enable();
	static ButtonInputObserver *getInstance(int pin, int interval);
private:
	static void timerInterruptInvoker();

	ButtonInputObserver();
	ButtonInputObserver(int pin, int interval);
	void timerInterrupt();
	EventType getEvent();

	uint8_t buttonPin;
	unsigned long doubleClickInterval;
	static ButtonInputObserver *instance;
	bool hasInitialized = false;

	uint8_t buttonState = HIGH;
	uint8_t lastButtonState = 0;
	uint8_t currentButtonState = 0;
	uint8_t clickCount = 0;
	unsigned long lastDebounceTime = 0;
	unsigned long clickInstant = 0;
	bool clickPending = true;
	static constexpr uint8_t debounceDelay = 25;
	static constexpr uint16_t longPressDelay = 700;

};

class SleepWakeupInterruptHandler: public AbsEventSourceObserver { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	void enable();
	void disable();
	void initialize();
	void sleep();
	void wakeup();
	void clearLastEvent();
	void observeEvents();
	void setSleepCallback(void (*cb)());
	void setWakeupCallback(void (*cb)());
	void setAutoWakeupCallback(void (*cb)());
	void setAutoWakeupDelay(uint8_t autoWakeupDelay);
	static SleepWakeupInterruptHandler *getInstance(uint8_t pin, uint32_t interval, uint8_t autoWakeupDelay);
	static void WDInterruptHandlerInvoker();
private:
	SleepWakeupInterruptHandler();
	SleepWakeupInterruptHandler(uint8_t pin, uint32_t interval, uint8_t autoWakeupDelay);
	uint8_t pin;
	uint32_t disableDelay;
	uint32_t lastEventInstant = 0;
	uint8_t autoWakeupDelay = 30; // in sec
	uint8_t sleepCounterLimit = 0;
	uint8_t sleepCounter = 0;
	bool autoWakedUp = false;

	bool hasInitialized = false;
	void (*_sleepCallback)() = nullptr;
	void (*_wakeupCallback)() = nullptr;
	void (*_autoWakeupCallback)() = nullptr;
	static SleepWakeupInterruptHandler *_instance;
	static void interruptHandlerInvoker();
	void interruptHandler();

	void disableWDInterrupt();
	void enableWDInterrupt();
	void setupWDTimer(uint8_t delay = 8);
	void WDInterruptHandler();
	void setupSleep();
	void goToSleep();
	void disableADC();
	void enableADC();

	volatile bool wdtExecuted = false;
};

#endif /* EVENTMANAGER_H_ */
