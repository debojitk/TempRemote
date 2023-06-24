
#include "HexProgrammer.h"
#include "SensorTypes.h"
#include <Arduino.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS3231.h>
#include <IRremote.h>

//#include <arduino.h>
#include <EEPROM.h>
#include "CommonItems.h"
//#include <IRremote.h>

static_assert(sizeof(MemoryLayout) == ((sizeof(uint8_t) * CONFIG::NUM_INDEX) +
		                                sizeof(uint8_t) +
		                               (sizeof(IRNode) * CONFIG::MAX_HEX_CODES) +
									   (sizeof(Schedule) * CONFIG::NUM_SCHEDULE)));

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
		IRNode* node = new IRNode();
		node->_rawCodeLength = _rx->decodedIRData.rawDataPtr->rawlen - 1;
		_rx->compensateAndStoreIRResultInArray(node->_rawCode);

		_rx->resume();
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
	_tx.sendRaw(node->_rawCode, node->_rawCodeLength, 38);
}

void
RemoteData::run() {
	TemperatureModule temp;
	TemperatureValue val = temp.get();
	RemoteRXValue data = at(static_cast<uint8_t>(val));

	if(data != nullptr) {
		RemoteTXModule mod;
		mod.set(data);
	}

}


void RemoteData::save() const {
  EEPROM.put(0, _layout);
}
void RemoteData::restore() {
  EEPROM.get(0, _layout);
}

RemoteData::RemoteData() {
  /***
POWER: 0xCF8976 : 13601142
ONE  : 0xCFD12E : 13619502
TWO  : 0xCF09F6 : 13568502
THREE: 0xCF51AE : 13586862
FOUR : 0xCFC936 : 13617462
FIVE : 0xCF11EE : 13570542
BOOST: 0xCFF10E : 13627662
***/
//  _layout._hexCodes[0]       = 0xCF8976;
//  _layout._hexCodes[1]       = 0xCFD12E;
//  _layout._hexCodes[2]       = 0xCF09F6;
//  _layout._hexCodes[3]       = 0xCF51AE;
//  _layout._hexCodes[4]       = 0xCFC936;
//  _layout._hexCodes[5]       = 0xCF11EE;
//  _layout._hexCodes[6]       = 0xCFF10E;

  for (uint8_t i = 0; i < CONFIG::NUM_INDEX; ++i) {
    _layout._index[i] = CONFIG::MAX_HEX_CODES;
  }
}

uint8_t
RemoteData::addHex(const IRNode* node) {
	uint8_t& num = _layout._numHex;
	if(num >= CONFIG::MAX_HEX_CODES) {
		return CONFIG::MAX_HEX_CODES;
	}
	memcpy(_layout._hexCodes[num]._rawCode, node->_rawCode, CONFIG::HEX_BUFFER_LENGTH);
	_layout._hexCodes[num]._rawCodeLength = node->_rawCodeLength;
	return num++;
}

void
RemoteData::program(uint8_t begin, uint8_t end, uint8_t position) {
  for (uint8_t i = begin; i <= end; ++i) {
    _layout._index[i] = position;
  }
}

IRNode*
RemoteData::at(uint8_t t) const {
  if (t >= CONFIG::NUM_INDEX) {
    return nullptr;
  }
  uint8_t hexIndex = _layout._index[t];
  if (hexIndex >= _layout._numHex) {
    return nullptr;
  }
  return &(_layout._hexCodes[hexIndex]);
}

