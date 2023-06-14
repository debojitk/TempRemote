#include "SensorTypes.h"
#include <Arduino.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
#include <IRremote.h>
#if defined(__RTC_DS_3231__)
TimeModule::TimeModule():_rtc(Wire) {
}

void TimeModule::setup() {
	RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
	_rtc.Begin();
	if (!_rtc.IsDateTimeValid())
	{
		_rtc.SetDateTime(compiled);
	}
	if (!_rtc.GetIsRunning())
	{
		_rtc.SetIsRunning(true);
	}
	RtcDateTime now = _rtc.GetDateTime();
	if (now < compiled)
	{
		_rtc.SetDateTime(compiled);
	}

}

TimeValue TimeModule::get() const {
	TimeValue t;
	RtcDateTime dt((const RtcDateTime&)_rtc.GetDateTime());
	t._day = dt.Day();
	t._hour = dt.Hour();
	t._min = dt.Minute();
	t._month = dt.Month();
	t._sec = dt.Second();
	t._year = dt.Year();
	return t;
}

bool TimeModule::set(const TimeValue &val) {
	RtcDateTime dateTime(val._year, val._month, val._day, val._hour, val._min, val._sec);
	_rtc.SetDateTime(dateTime);
}
#endif

RemoteRXModule::RemoteRXModule() :
		_rx(new IRrecv(PIN)),
		_results(new decode_results) {}

RemoteRXModule::~RemoteRXModule() {
	delete _rx;
	delete _results;
}

void
RemoteRXModule::setup() {
	_rx->enableIRIn();
}

RemoteRXValue
RemoteRXModule::get() {
	if (_rx->decode(_results)) {  //this line checks if we received a signal from the IR receiver
		RemoteRXValue irVal = _results->value;
		if (irVal == NullRemoteRXValue) {
			_rx->resume();
			return NullRemoteRXValue;
		}
		_rx->resume();
		return irVal;
	}
	return NullRemoteRXValue;
}
