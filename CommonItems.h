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
#define ENABLE_TEST
namespace CONFIG {
    constexpr uint32_t NUM_INDEX         = 15;       // starting from START_TEMPERATURE
    constexpr uint32_t NUM_SCHEDULE      = 2;
    constexpr uint32_t MAX_HEX_CODES     = 12;
    constexpr uint32_t BAUD_RATE         = 115200;
    constexpr uint32_t START_TEMPERATURE = 20;
    constexpr uint32_t MAX_TEMPERATURE   = 100;
    constexpr uint32_t NULL_HOUR         = 24; 		// 0 - 23
    constexpr uint32_t NULL_MIN          = 60; 		// 0 - 59
    constexpr uint8_t  REMOTE_BANKS		 = 2; 		// 0 for Remote 1 (fan), 1 for Remote 2 (AC)

    constexpr uint8_t  DS1307_PIN_IO	 = 3;
    constexpr uint8_t  DS1307_PIN_CLK	 = 4;
    constexpr uint8_t  DS1307_PIN_CE	 = 7;
    constexpr uint16_t DS3231_I2C_ADDRESS= 0x68;
    constexpr uint8_t  DHT22_PIN		 = 6;
    constexpr uint8_t  IR_RECV_PIN		 = 4;
    constexpr uint8_t  IR_SEND_PIN		 = 5;
    constexpr uint8_t  BUTTON_PIN		 = 2;

    constexpr uint8_t  IR_SEND_REPEATS	 = 1;

    constexpr uint8_t  DEBOUNCE_DELAY_MSEC	 				= 25;
    constexpr uint16_t LONGPRESS_DELAY_MSEC	 				= 700;
    constexpr uint8_t  SINGLE_CLICK_DETECT_DELAY_MSEC	 	= 100;
    constexpr uint8_t  AUTOWAKEUP_DELAY_SEC	 				= 20;
    constexpr uint8_t  AUTOSLEEP_DELAY_SEC	 				= 20;



};


enum EventType: uint8_t {
	NoEvent = 0,
	SingleClickEvent,
	DoubleClickEvent,
	LongPressEvent
	// Add more event types as needed
};

enum RemoteType: uint8_t {
	Fan = 0,
	AC
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

const char RemoteTestMenuLabels [6] [6] PROGMEM = {
		{ "Type" },
		{ "Start" },
		{ "End" },
		{ "Code" },
		{ "Test" },
		{ "Back" }
};

const char RemoteProgramMenuLabels [6] [6] PROGMEM = {
		{ "Type" },
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
	uint16_t testMemory();
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

