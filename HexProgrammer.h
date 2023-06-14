#pragma once

#include <stdint.h>

namespace CONFIG {
    constexpr uint32_t NUM_INDEX = 50;
    constexpr uint32_t NUM_HEX = 7;
    constexpr uint32_t BAUD_RATE = 9600;
};

struct MemoryLayout {
  uint8_t  _index[CONFIG::NUM_INDEX];    // temperature index
  uint32_t _hexCodes[CONFIG::NUM_HEX];  // hexCode index
};

template <typename SensorModule, typename Value>
class Sensor {
public:
    Value       get() const           { return _s.get(); }
    bool        set(const Value& val) { return _s.set(val); }
    const char* error() const         { return _s.error(); }
    void        setup()               { _s.setup(); }
private:
    SensorModule _s;
};



class RemoteData {
public:
  enum eButton {
    POWER = 0,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    BOOST,
    UNKNOWN
  };

public:
  RemoteData();
  // EPROM has limited write cycles
  void save() const;
  void restore();
  // 0xFFFFFF or correct value
  uint32_t at(uint8_t t) const;
  void program(uint8_t begin, uint8_t end, eButton e);
  void serialPrint() const;

private:
  MemoryLayout _layout;
};
