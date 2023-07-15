#pragma once

#include <DHT22.h>
#include <RtcDS1302.h>
#include "HexProgrammer.h";
#include "i2c.h"

class IRrecv;
class IRsend;
class decode_results;
class IRNode;
class SimpleDS3231;
struct TimeValue {
	uint8_t  _month;
	uint8_t  _day;
	uint16_t _year;

	uint8_t  _hour;
	uint8_t  _min;
	uint8_t  _sec;

	const char* formatDate(char *buffer) {
		sprintf_P(buffer,
				PSTR("%02u-%02u-%02u"),
				_day,
				_month,
				_year - 2000
		);
		return buffer;
	}
	const char* formatTime(char * buffer) {
		sprintf_P(buffer,
				PSTR("%02u:%02u:%02u"),
				_hour,
				_min,
				_sec );
		return buffer;
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
	TimeValue get() const;
	bool set(const TimeValue&);

private:
	static constexpr uint8_t IO  = 3;
	static constexpr uint8_t CLK = 4;
	static constexpr uint8_t CE  = 7;

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
	static constexpr uint8_t DS3231_I2C_ADDRESS = 0x68;
	iic &_i2c;
};



struct TemperatureValue {
	float _t;
	float _h;
};


class TemperatureModule {
public:
	TemperatureModule() : _dht22(PIN) {}
	TemperatureValue get() {
		if (_dht22.update()) {
			_last._t = (float)_dht22.lastT()/float(10);
			_last._h = (float)_dht22.lastH()/float(10);
		}
		return _last;
	}
private:
	static constexpr uint8_t PIN = 6;
	DHT22 _dht22;
	TemperatureValue _last;
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

	bool set(RemoteRXValue val);
	void setup();
private:
	static constexpr uint8_t PIN = 5;
	static constexpr uint8_t REPEATS = 5;
	IRsend& _tx;
};


template <typename SensorModule, typename Value>
class Sensor;

#ifdef DS3231
	using TimeSensor = Sensor<TimeModuleDS3231, TimeValue>;
#else
using TimeSensor = Sensor<TimeModuleDS1302, TimeValue>;
#endif
using TempSensor = Sensor<TemperatureModule, TemperatureValue>;
using RXSensor = Sensor<RemoteRXModule, RemoteRXValue>;
using TXSensor = Sensor<RemoteTXModule, RemoteRXValue>;
