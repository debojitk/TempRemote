/*
 * KeyValueStore.h
 *
 *  Created on: 12-Jun-2023
 *      Author: debojitk
 */

#ifndef KEYVALUESTORE_H_
#define KEYVALUESTORE_H_

#include <Arduino.h>
#include <EEPROM.h>

template <typename T>
struct KeyValuePair {
  char* key;
  T value;
};

template <typename T>
class KeyValueStore {
private:
  const int MAX_KEY_LENGTH = 32;
  const int EEPROM_START_ADDRESS = 0;

public:
  void set(KeyValuePair<T> kvp) {
    int address = findKeyAddress(kvp.key);
    if (address == -1) {
      address = findEmptyAddress();
      if (address == -1) {
        // EEPROM is full, handle the error accordingly
        return;
      }
      writeKey(address, kvp.key);
    }
    writeValue(address, kvp.value);
  }

  T get(const char* key) {
    int address = findKeyAddress(key);
    if (address == -1) {
      // Key not found, handle the error accordingly
      T defaultValue;
      return defaultValue;
    }
    return readValue(address);
  }

private:
  int findKeyAddress(const char* key) {
    for (int address = EEPROM_START_ADDRESS; address < EEPROM.length(); address += MAX_KEY_LENGTH + sizeof(T)) {
      if (strcmp(key, readKey(address)) == 0) {
        return address;
      }
    }
    return -1;  // Key not found
  }

  int findEmptyAddress() {
    for (int address = EEPROM_START_ADDRESS; address < EEPROM.length(); address += MAX_KEY_LENGTH + sizeof(T)) {
      if (readKey(address)[0] == '\0') {
        return address;
      }
    }
    return -1;  // EEPROM is full
  }

  char* readKey(int address) {
    static char key[MAX_KEY_LENGTH + 1];
    for (int i = 0; i < MAX_KEY_LENGTH; i++) {
      key[i] = EEPROM.read(address + i);
    }
    key[MAX_KEY_LENGTH] = '\0';
    return key;
  }

  void writeKey(int address, const char* key) {
    for (int i = 0; i < MAX_KEY_LENGTH; i++) {
      EEPROM.write(address + i, key[i]);
    }
  }

  T readValue(int address) {
    T value;
    for (int i = 0; i < sizeof(T); i++) {
      *((uint8_t*)&value + i) = EEPROM.read(address + MAX_KEY_LENGTH + i);
    }
    return value;
  }

  void writeValue(int address, const T& value) {
    for (int i = 0; i < sizeof(T); i++) {
      EEPROM.write(address + MAX_KEY_LENGTH + i, *((uint8_t*)&value + i));
    }
  }
};




#endif /* KEYVALUESTORE_H_ */
