
#include "HexProgrammer.h"
#include "SensorTypes.h"
#include <Arduino.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>


//#include <arduino.h>
#include <EEPROM.h>
#include "CommonItems.h"
#include <IRremote.hpp>



TimeModuleDS3231::TimeModuleDS3231():_rtc(Wire) {
}

void TimeModuleDS3231::setup() {
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

TimeValue TimeModuleDS3231::get() const {
	TimeValue t;
	RtcDateTime dt = const_cast<RtcDS3231<TwoWire>&>(_rtc).GetDateTime();
	t._day = dt.Day();
	t._hour = dt.Hour();
	t._min = dt.Minute();
	t._month = dt.Month();
	t._sec = dt.Second();
	t._year = dt.Year();
	return t;
}

bool TimeModuleDS3231::set(const TimeValue &val) {
	RtcDateTime dateTime(val._year, val._month, val._day, val._hour, val._min, val._sec);
	_rtc.SetDateTime(dateTime);
	return true;
}


RemoteRXModule::RemoteRXModule() :
		_rx(IrReceiver) {}

RemoteRXModule::~RemoteRXModule() {}

void
RemoteRXModule::setup() {
	_rx.begin(PIN);
}

RemoteRXValue
RemoteRXModule::get() {
	if (_rx.decode()) {

		IRNode node{_rx.decodedIRData.protocol,
		            _rx.decodedIRData.address,
					_rx.decodedIRData.command
		           };

		_rx.resume();
		return node;
	}
	return NullRemoteRXValue;
}

RemoteTXModule::RemoteTXModule() : _tx(IrSender) {}

void
RemoteTXModule::setup() {
	_tx.begin(PIN);
}

void
RemoteTXModule::set(RemoteRXValue node) {
	_tx.write((decode_type_t)node._protocol,
			  node._address,
			  node._command,
			  REPEATS);
}



