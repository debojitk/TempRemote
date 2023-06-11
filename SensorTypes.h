#pragma once

#include <DHT22.h>
#include <RtcDS1302.h>


struct TimeValue {
	uint8_t  _month;
	uint8_t  _day;
	uint16_t _year;

	uint8_t  _hour;
	uint8_t  _min;
	uint8_t  _sec;
    
    char* format(char* data) {
//        snprintf_P(data,
//			countof(data),
//			PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
//			_month,
//			_day,
//			_year,
//			_hour,
//			_min,
//			_sec );   
        return data;
    }
};

class TimeModule {
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

