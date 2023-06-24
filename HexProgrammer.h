

#pragma once

#include <stdint.h>
//class IRData;

namespace CONFIG {
    constexpr uint32_t NUM_INDEX = 50;
    constexpr uint32_t NUM_SCHEDULE = 4;
    constexpr uint32_t MAX_HEX_CODES = 20;
    constexpr uint32_t BAUD_RATE = 9600;
    constexpr uint32_t HEX_BUFFER_LENGTH = 112;
};
struct SchedulerTime {
	uint8_t _hr = 0;
	uint8_t _min = 0;
};
struct Schedule {
	SchedulerTime _begin;
	SchedulerTime _end;
};

struct IRNode {                        // extensions for sendRaw
  uint8_t _rawCode[CONFIG::HEX_BUFFER_LENGTH]; // The durations if raw
  uint8_t _rawCodeLength;              // The length of the code
};

struct MemoryLayout {
  uint8_t   _index[CONFIG::NUM_INDEX];            // temperature index
  uint8_t   _numHex = 0;
  IRNode    _hexCodes[CONFIG::MAX_HEX_CODES];     // hexCode index
  Schedule  _schedules[CONFIG::NUM_SCHEDULE];
};


class RemoteData {
public:
  RemoteData();
  // EPROM has limited write cycles
  void save() const;
  void restore();
  // 0xFFFFFF or correct value
  IRNode* at(uint8_t t) const;
  void program(uint8_t begin, uint8_t end, uint8_t position);
  uint8_t addHex(const IRNode* node);
  // The API runs after wakeup
  void run();
//  void serialPrint() const; // only for debug

private:
  MemoryLayout _layout;
};
