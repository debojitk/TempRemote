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

class SerialObserver: public AbsEventSourceObserver {

public:
	void initialize();
	void observeEvents();
};

class ButtonInputObserver: public  AbsEventSourceObserver {
public:
	void initialize();
	void disable();
	void enable();
	static ButtonInputObserver *getInstance(int pin, int interval);
private:
	ButtonInputObserver();
	ButtonInputObserver(int pin, int interval);
	int buttonPin;
	unsigned long doubleClickInterval;
	bool waitingForDoubleClick;
	unsigned long lastButtonClickTime;
	void timerInterrupt();
	static void timerInterruptInvoker();
	static ButtonInputObserver *instance;
};

#endif /* EVENTMANAGER_H_ */
