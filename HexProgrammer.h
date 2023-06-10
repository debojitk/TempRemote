#pragma once

#include <stdint.h>

constexpr uint32_t NUM_INDEX = 50;
constexpr uint32_t NUM_HEX = 7;


struct MemoryLayout {
  uint8_t _index[NUM_INDEX];    // temperature index
  uint32_t _hexCodes[NUM_HEX];  // hexCode index
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
