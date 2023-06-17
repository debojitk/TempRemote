#include "HexProgrammer.h"
#include <arduino.h>
#include <EEPROM.h>
#include "CommonItems.h"

static_assert(sizeof(MemoryLayout) == ((sizeof(uint8_t) * CONFIG::NUM_INDEX) +
		                               (sizeof(uint32_t) * CONFIG::NUM_HEX)) +
									   (sizeof(Schedule) * CONFIG::NUM_SCHEDULE));
static_assert(RemoteData::UNKNOWN == CONFIG::NUM_HEX);



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
  _layout._hexCodes[RemoteData::POWER]     = 0xCF8976;
  _layout._hexCodes[RemoteData::ONE]       = 0xCFD12E;
  _layout._hexCodes[RemoteData::TWO]       = 0xCF09F6;
  _layout._hexCodes[RemoteData::THREE]     = 0xCF51AE;
  _layout._hexCodes[RemoteData::FOUR]      = 0xCFC936;
  _layout._hexCodes[RemoteData::FIVE]      = 0xCF11EE;
  _layout._hexCodes[RemoteData::BOOST]     = 0xCFF10E;

  for (uint8_t i = 0; i < CONFIG::NUM_INDEX; ++i) {
    _layout._index[i] = CONFIG::NUM_HEX;
  }
}

void RemoteData::program(uint8_t begin, uint8_t end, eButton e) {
  for (uint8_t i = begin; i <= end; ++i) {
    _layout._index[i] = e;
  }
}

uint32_t
RemoteData::at(uint8_t t) const {
  if (t >= CONFIG::NUM_INDEX) {
    return 0xFFFFFF;
  }
  uint8_t hexIndex = _layout._index[t];
  if (hexIndex >= CONFIG::NUM_HEX) {
    return 0xFFFFFF;
  }
  return _layout._hexCodes[hexIndex];
}

void RemoteData::serialPrint() const {
  const char* NAMES[] = { "POWER", "ONE", "TWO", "THREE", "FOUR", "FIVE", "BOOST", "UNKNOWN" };
  char buffer[75];
  char* name = nullptr;
  for (uint8_t i = 0; i < CONFIG::NUM_INDEX; ++i) {
    uint32_t val = at(i);
    char* name = NAMES[_layout._index[i]];

    sprintf(buffer, "Temperature: %dC : Mode: %s : Hex: 0x", i, name);
    SerialPrint(buffer);
    SerialPrintln(val, HEX);
  }
}

/*
RemoteData remote;

void setup() {

  Serial.begin(9600);
  remote.serialPrint();  // All UNKNOWN

  remote.program(0 , 19, RemoteData::UNKNOWN);
  remote.program(20, 21, RemoteData::ONE);
  remote.program(22, 23, RemoteData::TWO);
  remote.program(24, 29, RemoteData::THREE);
  remote.program(30, 35, RemoteData::FOUR);
  remote.program(36, 39, RemoteData::FIVE);
  remote.program(40, 49, RemoteData::BOOST);

  remote.serialPrint();  // As PROGRAMMED
}
*/
/* void loop() {

} */

