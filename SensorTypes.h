#pragma once

#include <DHT22.h>
#include <RtcDS1302.h>
#include "HexProgrammer.h";
#include "i2c.h"

class IRNode;
class SimpleDS3231;
struct IRMP_DATA;
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
    bool operator<(const TimeValue& other) const {
        if (_year < other._year)
            return true;
        if (_year > other._year)
            return false;

        if (_month < other._month)
            return true;
        if (_month > other._month)
            return false;

        if (_day < other._day)
            return true;
        if (_day > other._day)
            return false;

        if (_hour < other._hour)
            return true;
        if (_hour > other._hour)
            return false;

        if (_min < other._min)
            return true;
        if (_min > other._min)
            return false;

        return _sec < other._sec;
    }

    // Method to compare two TimeValue objects for equality
    bool operator==(const TimeValue& other) const {
        return (_month == other._month && _day == other._day && _year == other._year &&
                _hour == other._hour && _min == other._min && _sec == other._sec);
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
	static constexpr uint8_t IO  = CONFIG::DS1307_PIN_IO;
	static constexpr uint8_t CLK = CONFIG::DS1307_PIN_CLK;
	static constexpr uint8_t CE  = CONFIG::DS1307_PIN_CE;

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
	static constexpr uint8_t DS3231_I2C_ADDRESS = CONFIG::DS3231_I2C_ADDRESS;
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
		noInterrupts();
		bool update =_dht22.update();
		interrupts();
		if (update) {
			SerialPrintln(F("Received value from DHT"));
			_last._t = (float)_dht22.lastT()/float(10);
			_last._h = (float)_dht22.lastH()/float(10);
		} else{
			SerialPrintln(F("No update from DHT, returning previous value"));
		}
		return _last;
	}
private:
	static constexpr uint8_t PIN = CONFIG::DHT22_PIN;
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
	static constexpr uint8_t PIN = CONFIG::IR_RECV_PIN;
	IRMP_DATA *_data = nullptr;
};

class RemoteTXModule {
public:
	RemoteTXModule();
	~RemoteTXModule(){}

	bool set(RemoteRXValue val);
	void setup();
private:
	static constexpr uint8_t PIN = CONFIG::IR_SEND_PIN;
	static constexpr uint8_t REPEATS = CONFIG::IR_SEND_REPEATS;
	IRMP_DATA * _data = nullptr;
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
