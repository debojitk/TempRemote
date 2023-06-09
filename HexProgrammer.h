#pragma once

#include <stdint.h>
#include <Arduino.h>
#include "CommonItems.h"

class RemoteData;
class TimeValue;

namespace CONFIG {
    constexpr uint32_t NUM_INDEX         = 35;              // starting from START_TEMPERATURE
    constexpr uint32_t NUM_SCHEDULE      = 2;
    constexpr uint32_t MAX_HEX_CODES     = 7;
    constexpr uint32_t BAUD_RATE         = 115200;
    constexpr uint32_t START_TEMPERATURE = 15;
    constexpr uint32_t MAX_TEMPERATURE   = 100;
    constexpr uint32_t NULL_HOUR         = 24; // 0 - 23
    constexpr uint32_t NULL_MIN          = 60; // 0 - 59
};

struct SchedulerTime {
	uint8_t _hr = CONFIG::NULL_HOUR;
	uint8_t _min = CONFIG::NULL_MIN;
	bool greaterEq(const SchedulerTime& t) const {
		if(_hr == t._hr) {
			return (_min >= t._min);
		}
		return _hr > t._hr;
	}
	bool lessEq(const SchedulerTime& t) const {
		if(_hr == t._hr) {
			return (_min <= t._min);
		}
		return _hr < t._hr;
	}
	bool greater(const SchedulerTime& t) const {
		if(t == *this) {
			return false;
		}
		if(_hr == t._hr) {
			return (_min > t._min);
		}
		return _hr > t._hr;
	}
	bool operator ==(const SchedulerTime& s) const {
		return ((s._hr == _hr) && (s._min == _min));
	}
#ifdef ENABLE_TEST
    void p() {
        SerialPrint(F("Hr :")); SerialPrint(_hr);
        SerialPrint(F(" Min :")); SerialPrintln(_min);
    }
#endif
};
struct Schedule {
	SchedulerTime _begin;
	SchedulerTime _end;
	bool inRange(const SchedulerTime& t) const {
		if(_begin.greater(_end)) {
			if(_begin.lessEq(t) || _end.greaterEq(t)) {
				return true;
			}
		}
		else {
			return (_begin.lessEq(t) && _end.greaterEq(t));
		}
		return false;
	}
	bool operator ==(const Schedule& s) const {
		return ((s._begin == _begin) && (s._end == _end));
	}
#ifdef ENABLE_TEST
    void p() {
        _begin.p();
        _end.p();
    }
#endif
};

constexpr Schedule NullSchedule;

// 5 bytes per IR
struct IRNode {
    uint8_t  _protocol;
    uint16_t _address;
    uint16_t _command;
    bool operator ==(const IRNode& n) {
    	return ((n._protocol == _protocol) && (n._address == _address) && (n._command == _command));
    }
#ifdef ENABLE_TEST
    void p() {
    	SerialPrint(F("Protocol :")); SerialPrint(_protocol);
    	SerialPrint(F(", Address :")); SerialPrint(_address);
    	SerialPrint(F(", Command :")); SerialPrintln(_command);
    }
#endif
};
constexpr IRNode NullIRNode {0,0,0};

struct MemoryLayout {
  uint8_t   _index[CONFIG::NUM_INDEX];            // temperature index
  IRNode    _hexCodes[CONFIG::MAX_HEX_CODES];     // hexCode index
  Schedule  _schedules[CONFIG::NUM_SCHEDULE];

#ifdef ENABLE_TEST
  void p(){
	  for (uint8_t i=0; i<CONFIG::MAX_HEX_CODES; i++) {
		  _hexCodes[i].p();
	  }
  }
#endif
};

struct TemperatureRange {
    uint8_t       _start;
    uint8_t       _end;
    IRNode        _hex;
    bool operator==(const TemperatureRange& r) {
    	return ((_start == r._start) && (_end == r._end) && (_hex == r._hex));
    }
#ifdef ENABLE_TEST
    void p() {
    	SerialPrint(F("Start :")); SerialPrintln(_start);
    	SerialPrint(F("End :")); SerialPrintln(_end);
    	_hex.p();
    }
#endif
};
constexpr TemperatureRange NullTemperatureRange{CONFIG::MAX_TEMPERATURE, CONFIG::MAX_TEMPERATURE, NullIRNode};
constexpr TemperatureRange DefaultTemperatureRange{CONFIG::START_TEMPERATURE, CONFIG::START_TEMPERATURE, NullIRNode};

class RangeIterator {
public:
    RangeIterator(uint8_t posBegin, uint8_t posEnd, const RemoteData& rt) :
                _posBegin(posBegin), _posEnd(posEnd), _rt(rt) {}
    RangeIterator(const RemoteData& rt) : _rt(rt) {}
    TemperatureRange operator *() const;
    RangeIterator operator ++();
    bool operator ==(const RangeIterator& ri) const;
    bool operator !=(const RangeIterator& ri) const;
    RangeIterator& operator =(const RangeIterator& ri) {
    	if(*this != ri) {
    		_posBegin = ri._posBegin;
    		_posEnd = ri._posEnd;
    	}
    	return *this;
    }
private:
    uint8_t _posBegin = CONFIG::NUM_INDEX;
    uint8_t _posEnd = CONFIG::NUM_INDEX;
    const RemoteData& _rt;
    friend class RemoteData;
};

class RemoteData {
public:
  RemoteData();
  // EPROM has limited write cycles
  void save() const;
  void restore();
  uint8_t atIndex(uint8_t val) const;
  // 0xFFFFFF or correct value
  IRNode at(uint8_t t) const;
  IRNode atTemperature(uint8_t t) const;
  bool addRange(const TemperatureRange& r);

  RangeIterator beginRange();
  RangeIterator endRange();

  bool isScheduleOn(const TimeValue& time) const;
  bool addSchedule(const Schedule& s);
  Schedule& getSchedule(uint8_t i);
  void clear();

#ifdef ENABLE_TEST
  /// only for testing
  MemoryLayout& getLayout() { return _layout; }
  void p() {
      _layout.p();
  }
#endif

private:
  uint8_t findHex(const IRNode& node, uint8_t& pos) const;
  bool program(uint8_t beginTemp, uint8_t endTemp, uint8_t position);
  uint8_t addHex(const IRNode& node);

private:
  MemoryLayout _layout;
};

