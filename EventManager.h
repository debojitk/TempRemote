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

private:
	IEventReceiver *eventReceiver;
	IEventSourceObserver *eventSourceObserver;
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

#endif /* EVENTMANAGER_H_ */
