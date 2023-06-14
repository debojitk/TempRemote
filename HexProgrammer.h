#pragma once

#include <stdint.h>

namespace CONFIG {
    constexpr uint32_t NUM_INDEX = 50;
    constexpr uint32_t NUM_HEX = 7;
    constexpr uint32_t NUM_SCHEDULE = 4;
    constexpr uint32_t BAUD_RATE = 9600;
};
struct SchedulerTime {
	uint8_t _hr = 0;
	uint8_t _min = 0;
};
struct Schedule {
	SchedulerTime _begin;
	SchedulerTime _end;
};
struct MemoryLayout {
  uint8_t   _index[CONFIG::NUM_INDEX];    // temperature index
  uint32_t  _hexCodes[CONFIG::NUM_HEX];  // hexCode index
  Schedule  _schedules[CONFIG::NUM_SCHEDULE];
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
