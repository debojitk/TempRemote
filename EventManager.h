/*
 * EventManager.h
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */

#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

enum EventType: unsigned int;
class IEventReceiver;
class SleepWakeupInterruptHandler;

class IEventSourceObserver{
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

class AbsEventSourceObserver: public IEventSourceObserver {
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
	bool hasClicked();

	int buttonPin;
	unsigned long doubleClickInterval;
	static ButtonInputObserver *instance;
	bool hasInitialized = false;

	int counter = 0;
	int buttonState = 0;
	int lastButtonState = 0;
	int lastClickCount = 0;
	int currentButtonState = 0;
	unsigned long lastDebounceTime = 0;
	int debounceDelay = 25;
	int clickCount = 0;
	unsigned long clickInstant =0;


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
	static SleepWakeupInterruptHandler *getInstance(uint8_t pin, uint16_t interval);
private:
	SleepWakeupInterruptHandler();
	SleepWakeupInterruptHandler(uint8_t pin, uint16_t interval);
	uint8_t pin;
	uint16_t disableDelay;
	uint32_t lastEventInstant = 0;
	bool hasInitialized = false;
	void (*_sleepCallback)() = nullptr;
	void (*_wakeupCallback)() = nullptr;
	static SleepWakeupInterruptHandler *_instance;
	static void interruptHandlerInvoker();
	void interruptHandler();
};

#endif /* EVENTMANAGER_H_ */
