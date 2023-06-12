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

enum ValueType{
	IntType = 1,
	FloatType,
	TimeType,
	DateType,
	StringType,
	RemoteHexType,
	ScheduleType

};
class KeyValueStoreClass {
public:
	//category is used to have same key for different data type
	template<typename T>
	T &get(ValueType category, const char* key, T &value) {
		size_t address = findKeyAddress(category, key);
		if (address != 0) {
			address += sizeof(category) + MAX_KEY_LENGTH; // Move past the category and key
			EEPROM.get(address, value);
		}
		return value;
	}

	template <typename T>
	void set(ValueType category, const char* key, const T& value) {
		size_t address = findKeyAddress(category, key);
		if (address != 0) {
			address += sizeof(category) + MAX_KEY_LENGTH; // Move past the key
			EEPROM.put(address, value);
		} else {
			address = findEmptyAddress();
			if (address == 0) {
				return;
			}

			// Store the category
			EEPROM.put(address, category);
			address += sizeof(category);

			// Store the key
			puts(address, key);
			address += MAX_KEY_LENGTH;
			// Store the value
			EEPROM.put(address, value);
		}
	}


private:
	static constexpr int MAX_KEY_LENGTH = 15;
	static constexpr int EEPROM_START_ADDRESS = 1;
	size_t findKeyAddress(ValueType category, const char* key) {
		size_t address = EEPROM_START_ADDRESS;
		ValueType storedCategory;
		char storedKey[MAX_KEY_LENGTH];

		while (address < EEPROM.length()) {
			EEPROM.get(address, storedCategory);
			if (storedCategory == category) {
				address += sizeof(storedCategory);
				EEPROM.get(address, storedKey);

				bool keyMatches = true;
				for (size_t i = 0; i < MAX_KEY_LENGTH; ++i) {
					if (key[i] == '\0' || key[i] != storedKey[i]) {
						keyMatches = false;
						break;
					}
				}

				if (keyMatches) {
					return address;
				}

				address += MAX_KEY_LENGTH;
			} else {
				// Skip to the next entry
				size_t entrySize = sizeof(storedCategory) + MAX_KEY_LENGTH;
				address += entrySize;
			}
		}

		return 0; // Key not found
	}


	size_t findEmptyAddress() {
		size_t address = EEPROM_START_ADDRESS;
		ValueType storedCategory;

		while (address < EEPROM.length()) {
			EEPROM.get(address, storedCategory);
			if (storedCategory == 0) {
				return address; // Found an empty address
			} else {
				// Skip to the next entry
				address += sizeof(storedCategory) + MAX_KEY_LENGTH;
			}
		}

		return 0; // No empty address found
	}

	void puts(size_t address, const char* data) {
		size_t dataLength = strlen(data);
		for (size_t i = 0; i < dataLength; i++) {
			EEPROM.write(address + i, data[i]);
		}
		for (size_t i = dataLength; i < MAX_KEY_LENGTH; i++) {
			EEPROM.write(address + i, 0xFF); // Fill with non-zero value
		}
	}
};

static KeyValueStoreClass KeyValueStore;
#endif /* KEYVALUESTORE_H_ */
