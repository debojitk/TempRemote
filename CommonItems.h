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

static const char hourLabel[] PROGMEM = "Hour";
static const char minLabel[]  PROGMEM = "Minute";
static const char secLabel[]  PROGMEM = "Second";
static const char dayLabel[] PROGMEM = "Day";
static const char monLabel[]  PROGMEM = "Month";
static const char yearLabel[]  PROGMEM = "year";
static const char saveLabel[] PROGMEM = "Save";
static const char backLabel[] PROGMEM = "Back";
static const char testLabel[] PROGMEM = "Test";
static const char okLabel[]   PROGMEM = "OK";
static const char remoteCodeLabel[]   PROGMEM = "Code";
static const char timeLabel[]   PROGMEM = "Time";
static const char DateLabel[]   PROGMEM = "Date";
static const char tempLabel[]   PROGMEM = "Temperature";
static const char humidityLabel[]   PROGMEM = "Humidity";

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

