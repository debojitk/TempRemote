
#include "HexProgrammer.h"
#include <arduino.h>
#include <EEPROM.h>

  /***
POWER: 0xCF8976 : 13601142
ONE  : 0xCFD12E : 13619502
TWO  : 0xCF09F6 : 13568502
THREE: 0xCF51AE : 13586862
FOUR : 0xCFC936 : 13617462
FIVE : 0xCF11EE : 13570542
BOOST: 0xCFF10E : 13627662
***/

static_assert(sizeof(MemoryLayout) ==
		                   ((sizeof(uint8_t) * CONFIG::NUM_INDEX) +
		                   (sizeof(IRNode) * CONFIG::MAX_HEX_CODES))
						   );

static_assert(sizeof(IRNode) == (sizeof(uint8_t) * 5));


void RemoteData::save() const {
  EEPROM.put(0, _layout);
}
void RemoteData::restore() {
  EEPROM.get(0, _layout);
}

RemoteData::RemoteData() {
  for (uint8_t i = 0; i < CONFIG::NUM_INDEX; ++i) {
    _layout._index[i] = CONFIG::MAX_HEX_CODES;
  }
  for (uint8_t i = 0; i < CONFIG::MAX_HEX_CODES; ++i) {
    _layout._hexCodes[i] = NullIRNode;
  }
}

uint8_t
RemoteData::findHex(const IRNode& node, uint32_t& pos) const {
	for(pos = 0; pos < CONFIG::MAX_HEX_CODES; ++pos) {
		if(_layout._hexCodes[pos] == node) {
			return pos;
		}
		else if(_layout._hexCodes[pos] == NullIRNode) {
			return CONFIG::MAX_HEX_CODES;
		}
	}
	return CONFIG::MAX_HEX_CODES;
}

uint8_t
RemoteData::addHex(const IRNode& node) {
	uint32_t pos;
	uint8_t index = findHex(node, pos); // is Already present

	if(index != CONFIG::MAX_HEX_CODES) { // found an existing one
		return index;
	}

	_layout._hexCodes[pos] = node;
	return pos;
}

bool
RemoteData::program(uint8_t beginTemp, uint8_t endTemp, uint8_t pos) {
  beginTemp = beginTemp - CONFIG::START_TEMPERATURE;
  endTemp = endTemp - CONFIG::START_TEMPERATURE;

  if(endTemp >= CONFIG::NUM_INDEX || beginTemp >= CONFIG::NUM_INDEX) {
	  return false;
  }

  for (uint8_t i = beginTemp; i <= endTemp; ++i) {
    _layout._index[i] = pos;
  }
  return true;
}

IRNode
RemoteData::atTemperature(uint8_t t) const {
	return at(t - CONFIG::START_TEMPERATURE);
}

IRNode
RemoteData::at(uint8_t t) const {
  if (t >= CONFIG::NUM_INDEX) {
    return NullIRNode;
  }
  uint8_t hexIndex = _layout._index[t];
  if(hexIndex >= CONFIG::MAX_HEX_CODES) {
	return NullIRNode;
  }
  return (_layout._hexCodes[hexIndex]);
}

uint8_t
RemoteData::atIndex(uint8_t val) const {
	  return _layout._index[val];
}

TemperatureRange
RangeIterator::operator *() const {
    uint8_t beginIndex = _rt.atIndex(_posBegin);
    uint8_t endIndex = _rt.atIndex(_posEnd);

    if(beginIndex != endIndex) {
        return NullTemperatureRange;
    }

    IRNode nBeg = _rt.at(_posBegin);
    if(nBeg == NullIRNode) {
        return NullTemperatureRange;
    }
    return TemperatureRange{(_posBegin + CONFIG::START_TEMPERATURE),
                            (_posEnd + CONFIG::START_TEMPERATURE),
                            nBeg};
    
}

RangeIterator
RangeIterator::operator ++() {
    if((_posBegin >= CONFIG::NUM_INDEX) || (_posEnd >= CONFIG::NUM_INDEX)) {
        return RangeIterator(_rt);
    }
    _posBegin = _posEnd + 1;
    _posEnd = _posBegin;

    IRNode nBeg = _rt.at(_posBegin);
    if(nBeg == NullIRNode) {
        return RangeIterator(_rt);
    }

    IRNode nEnd = _rt.at(_posEnd);
    while(nBeg == nEnd) {
        nEnd = _rt.at(++_posEnd);
    }
    --_posEnd; // retract

    return *this;
}

bool
RangeIterator::operator ==(const RangeIterator& ri) const {
    return ((_posBegin == ri._posBegin) && (_posEnd == ri._posEnd));
}

bool
RangeIterator::operator !=(const RangeIterator& ri) const {
    return (!(*this == ri));
}

RangeIterator 
RemoteData::beginRange() {
    RangeIterator ri(*this);
    ri._posEnd = ri._posBegin = 0;

    IRNode nBeg = at(ri._posBegin);
    if(nBeg == NullIRNode) {
        return RangeIterator(*this);
    }

    IRNode nEnd = at(ri._posEnd);
    while(nBeg == nEnd) {
        nEnd = at(++(ri._posEnd));
    }
    --(ri._posEnd); // retract
    return ri;
}

RangeIterator
RemoteData::endRange() {
    return RangeIterator(*this);
}

bool
RemoteData::addRange(TemperatureRange& r) {
	uint8_t pos = addHex(r._hex);
	if(pos == CONFIG::MAX_HEX_CODES) {
		return false;
	}
	return program(r._start, r._end, pos);
}

