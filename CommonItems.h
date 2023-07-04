/*
 * CommonItems.h
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */

#ifndef COMMONITEMS_H_
#define COMMONITEMS_H_
//#define DISABLE_SERIAL_PRINT

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

const char hourLabel[] PROGMEM = "Hour";
const char minLabel[]  PROGMEM = "Minute";
const char secLabel[]  PROGMEM = "Second";
const char dayLabel[] PROGMEM = "Day";
const char monLabel[]  PROGMEM = "Month";
const char yearLabel[]  PROGMEM = "year";
const char saveLabel[] PROGMEM = "Save";
const char backLabel[] PROGMEM = "Back";
const char testLabel[] PROGMEM = "Test";
const char okLabel[]   PROGMEM = "OK";
const char remoteCodeLabel[]   PROGMEM = "Code";
const char timeLabel[]   PROGMEM = "Time";
const char DateLabel[]   PROGMEM = "Date";
const char tempLabel[]   PROGMEM = "Temperature";
const char humidityLabel[]   PROGMEM = "Humidity";

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

