/*
 * CommonItems.h
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */

#ifndef COMMONITEMS_H_
#define COMMONITEMS_H_
//#define DISABLE_SERIAL_PRINT
//#define DS3231
//#define ENABLE_TEST

enum EventType: unsigned int {
	NoEvent = 0,
	SingleClickEvent,
	DoubleClickEvent,
	LongPressEvent
	// Add more event types as needed
};
struct CurrentTime {
	int hours;
	int minutes;
	int seconds;
};

class IEventReceiver { // @suppress("Class has a virtual method and non-virtual destructor")
public:
	virtual void handleEvent(EventType event) = 0;
};

class IRenderable { // @suppress("Class has a virtual method and non-virtual destructor")
	virtual void render() = 0;
};

const char TimeMenuLabels [5] [7] PROGMEM = {
		{ "Hour" },
		{ "Minute" },
		{ "Second" },
		{ "Save" },
		{ "Back" }
};

const char DateMenuLabels [5] [6] PROGMEM = {
		{ "Day" },
		{ "Month" },
		{ "Year" },
		{ "Save" },
		{ "Back" }
};

const char RemoteTestMenuLabels [5] [6] PROGMEM = {
		{ "Start" },
		{ "End" },
		{ "Code" },
		{ "Test" },
		{ "Back" }
};

const char RemoteProgramMenuLabels [5] [6] PROGMEM = {
		{ "Start" },
		{ "End" },
		{ "Code" },
		{ "Save" },
		{ "Back" }
};

const char ScheduleMenuLabels [6] [6] PROGMEM = {
		{ "HR1" },
		{ "MI1" },
		{ "HR2" },
		{ "MI2" },
		{ "Save" },
		{ "Back" }
};


namespace TEST{
	void testMemory();
}
	#ifdef DISABLE_SERIAL_PRINT
		#define SerialPrint(...)
		#define SerialPrintln(...)
		#define SerialPrintWithDelay(...)
		#define SerialPrintlnWithDelay(...)
	#else
		#define SerialPrint(...) Serial.print(__VA_ARGS__)
		#define SerialPrintln(...) Serial.println(__VA_ARGS__)
		#define SerialPrintWithDelay(...) Serial.print(__VA_ARGS__); delay(10)
		#define SerialPrintlnWithDelay(...) Serial.println(__VA_ARGS__); delay(10)
	#endif
#endif /* COMMONITEMS_H_ */

