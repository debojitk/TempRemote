#include "HexProgrammer.h"
#include "SensorTypes.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "CommonItems.h"
#include "i2c.h"

#define F_INTERRUPTS                     15000
#define IRMP_IRSND_ALLOW_DYNAMIC_PINS
#define NO_LED_FEEDBACK_CODE
#define IRSND_IR_FREQUENCY          38000
#define IRMP_PROTOCOL_NAMES         1 // Enable protocol number mapping to protocol strings - requires some FLASH.
#define IRSND_PROTOCOL_NAMES        1 // Enable protocol number mapping to protocol strings - requires some FLASH.
// configure send protocols
#define IRSND_SUPPORT_SAMSUNG48_PROTOCOL        1
#define IRSND_SUPPORT_SAMSUNG_PROTOCOL          1       // Samsung + Samsung32  >= 10000                 ~300 bytes
#define IRSND_SUPPORT_NEC_PROTOCOL 			    1
// configure receive protocols
#define IRMP_SUPPORT_SAMSUNG_PROTOCOL           1       // Samsung + Samsg32    >= 10000                 ~300 bytes
#define IRMP_SUPPORT_SAMSUNG48_PROTOCOL         1       // Samsung48            >= 10000                 ~100 bytes (SAMSUNG must be enabled!)
#define IRMP_SUPPORT_NEC_PROTOCOL               1       // NEC + APPLE + ONKYO  >= 10000                 ~300 bytes

#define USE_ONE_TIMER_FOR_IRMP_AND_IRSND // otherwise we get an error on AVR platform: redefinition of 'void __vector_8()

#include <irmp.hpp>
#include <irsnd.hpp>


// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}

TimeModuleDS3231::TimeModuleDS3231():_i2c(i2c) {
}

void TimeModuleDS3231::setup() {
	i2c.init();
}

TimeValue TimeModuleDS3231::get() const {
	TimeValue time;
	i2c.start(DS3231_I2C_ADDRESS<<1 | 0);// opening for write
	i2c.write(0); // set DS3231 register pointer to 00h for read from 00 reg
	i2c.start(DS3231_I2C_ADDRESS<<1 | 1); // opening for read
	// request seven bytes of data from DS3231 starting from register 00h
	time._sec = bcdToDec(i2c.readack() & 0x7f);
	time._min = bcdToDec(i2c.readack());
	time._hour = bcdToDec(i2c.readack() & 0x3f);
	uint8_t dow = bcdToDec(i2c.readack());
	time._day = bcdToDec(i2c.readack());
	time._month = bcdToDec(i2c.readack());
	time._year = 2000 + bcdToDec(i2c.readnck());
	i2c.stop();
	return time;
}

bool TimeModuleDS3231::set(const TimeValue &val) {
	i2c.start(DS3231_I2C_ADDRESS<<1 | 0);
	i2c.write(0); // set next input to start at the seconds register
	i2c.write(decToBcd(val._sec)); // set seconds
	i2c.write(decToBcd(val._min)); // set minutes
	i2c.write(decToBcd(val._hour)); // set hours
	i2c.write(decToBcd(1)); // set day of week (1=Sunday, 7=Saturday)
	i2c.write(decToBcd(val._day)); // set date (1 to 31)
	i2c.write(decToBcd(val._month)); // set month
	i2c.write(decToBcd(val._year - 2000)); // set year (0 to 99)
	i2c.stop();
	return true;
}

TimeValue
TimeModuleDS1302::get() const {
	TimeValue t;
	RtcDateTime dt = _rtc.GetDateTime();
	t._day = dt.Day();
	t._hour = dt.Hour();
	t._min = dt.Minute();
	t._month = dt.Month();
	t._sec = dt.Second();
	t._year = dt.Year();
	return t;
}


bool
TimeModuleDS1302::set(const TimeValue &val) {
	RtcDateTime dateTime(val._year, val._month, val._day, val._hour, val._min, val._sec);
	_rtc.SetDateTime(dateTime);
	return true;
}

RemoteRXModule::RemoteRXModule()  {
	_data = new IRMP_DATA;
}

RemoteRXModule::~RemoteRXModule() {}

void
RemoteRXModule::setup() {
    irmp_init(PIN);
    irmp_irsnd_LEDFeedback(false); // Enable receive signal feedback at LED_BUILTIN for receive and send
}

RemoteRXValue
RemoteRXModule::get() {
    if (irmp_get_data(_data))
    {
        irmp_result_print(_data);
        IRNode node{
        	_data->protocol,
			_data->address,
			_data->command
        };
        // Flush repeats received
        irmp_get_data(_data);
        return node;
    }
	return NullRemoteRXValue;
}

RemoteTXModule::RemoteTXModule() {
	_data = new IRMP_DATA;
}

void
RemoteTXModule::setup() {
    irsnd_init(PIN);
}

bool
RemoteTXModule::set(RemoteRXValue node) {
	_data->protocol = node._protocol;
	_data->address = node._address;
	_data->command = node._command;
	_data->flags = 1;
    if (!irsnd_send_data(_data, true)) {
        SerialPrintln(F("Protocol not found")); // name of protocol is printed by irsnd_data_print()
        return false;
    }
#ifndef DISABLE_SERIAL_PRINT
    irsnd_data_print(&Serial, _data);
#endif
	return true;
}



