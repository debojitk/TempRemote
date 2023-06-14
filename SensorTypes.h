#pragma once

#include <DHT22.h>
#include <RtcDS1302.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>

#define __RTC_DS_3231__
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
				_month,
				_day,
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

#if defined(__RTC_DS1302__)
class TimeModule {
public:
	TimeModule() : _myWire(IO, CLK, CE), _rtc(_myWire) {}
	void setup() {
		RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

	}
private:
	static constexpr uint8_t IO  = 3;
	static constexpr uint8_t CLK = 4;
	static constexpr uint8_t CE  = 2;

	ThreeWire            _myWire; // (IO, CLK, CE); // IO, SCLK, CE
	RtcDS1302<ThreeWire> _rtc;
};
#elif defined(__RTC_DS_3231__)
class TimeModule {
public:
	TimeModule();
	void setup();
	TimeValue get() const;
	bool set(const TimeValue& val);

private:
	RtcDS3231<TwoWire> _rtc;
};

#endif
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

