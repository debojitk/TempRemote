#pragma once

#include <DHT22.h>
#include <RtcDS1302.h>
#include <RtcDS3231.h>
#include <Wire.h>
#include "HexProgrammer.h";

class IRrecv;
class IRsend;
class decode_results;
class IRNode;

struct TimeValue {
	uint8_t  _month;
	uint8_t  _day;
	uint16_t _year;

	uint8_t  _hour;
	uint8_t  _min;
	uint8_t  _sec;

	const char* formatDate() {
		static char timeStore[12];
		snprintf_P(timeStore,
				sizeof(timeStore),
				PSTR("%02u/%02u/%04u"),
				_day,
				_month,
				_year
		);
		return timeStore;
	}
	const char* formatTime() {
		static char timeStore[10];
		snprintf_P(timeStore,
				sizeof(timeStore),
				PSTR("%02u:%02u:%02u"),
				_hour,
				_min,
				_sec );
		return timeStore;
	}
};



class TimeModuleDS1302 {
public:
	TimeModuleDS1302() : _myWire(IO, CLK, CE), _rtc(_myWire) {}
	void setup() {
		RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
		RtcDateTime now = _rtc.GetDateTime();
		if (now < compiled) {
			_rtc.SetDateTime(compiled);
		}
	}
private:
	static constexpr uint8_t IO  = 3;
	static constexpr uint8_t CLK = 4;
	static constexpr uint8_t CE  = 8;

	ThreeWire            _myWire; // (IO, CLK, CE); // IO, SCLK, CE
	RtcDS1302<ThreeWire> _rtc;
};

class TimeModuleDS3231 {
public:
	TimeModuleDS3231();
	void setup();
	TimeValue get() const;
	bool set(const TimeValue& val);

private:
	RtcDS3231<TwoWire> _rtc;
};


using TemperatureValue = float;
using HumidityValue = float;

class TemperatureModule {
public:
	TemperatureModule() : _dht22(PIN) {}
	TemperatureValue get() {
		return _dht22.getTemperature();
	}
private:
	static constexpr uint8_t PIN = 6;
	DHT22 _dht22;
};

class HumidityModule {
public:
	HumidityModule() : _dht22(PIN) {}
	HumidityValue get() {
		return _dht22.getHumidity();
	}
private:
	static constexpr uint8_t PIN = 6;
	DHT22 _dht22;
};

using RemoteRXValue = IRNode;
constexpr RemoteRXValue NullRemoteRXValue = NullIRNode;

class RemoteRXModule {
public:
	RemoteRXModule();
	~RemoteRXModule();
	// The get API returns a ptr which it will not delete
	// Its a caller's responsibility to delete the ptr
	RemoteRXValue get();
	void setup();
private:
	static constexpr uint8_t PIN = 11;
	IRrecv& _rx;
};

class RemoteTXModule {
public:
	RemoteTXModule();
	~RemoteTXModule(){}

	void set(RemoteRXValue val);
	void setup();
private:
	static constexpr uint8_t PIN = 5;
	static constexpr uint8_t REPEATS = 5;
	IRsend& _tx;
};

