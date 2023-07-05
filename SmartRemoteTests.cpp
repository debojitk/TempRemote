/*
 * SmartRemoteTests.cpp
 *
 *  Created on: 02-Jul-2023
 *      Author: arnab
 */

#include "SmartRemoteTests.h"
#include <Arduino.h>
#include <MemoryFree.h>
#include "CommonItems.h"

namespace TEST {

void TestRemoteData::testDataAdd() {
	IRNode ir1{10, 20, 30};
	IRNode ir2{11, 21, 31};
	IRNode ir3{12, 22, 32};
	TemperatureRange t1{15,20, ir1};
	TemperatureRange t2{19,25, ir2};
	TemperatureRange t3{26,49, ir3};
	r.addRange(t1);
	r.addRange(t2);
	r.addRange(t3);

	for(uint8_t i = 15; i <= 49; ++i) {
		IRNode ref;
		IRNode irN = r.atTemperature(i);
		SerialPrint("Temp: "); SerialPrint(i);
		if(i >= 15 && i <= 18) {
			ref = ir1;
		}
		else if (i >= 19 && i <= 25) {
			ref = ir2;
		}
		else if (i >= 26 && i <= 49) {
			ref = ir3;
		}
		if(ref == irN) {
			SerialPrintln(" PASSED");
		}
		else {
			SerialPrintln(" FAILED");
		}
	}
}

void
TestRemoteData::testIterator() {
	IRNode ir1{10, 20, 30};
	IRNode ir2{11, 21, 31};
	IRNode ir3{12, 22, 32};
	for(auto it = r.beginRange(), itEnd = r.endRange();
			it != itEnd; ++it) {

		TemperatureRange tr = *it;
		if(tr == NullTemperatureRange) {
			SerialPrint("Null check passed");
		}
		bool pass = false;
		SerialPrint("Iter T Begin: "); SerialPrint(tr._start);
		SerialPrint("--Iter T End: "); SerialPrint(tr._end);
		if(tr._start == 15 && tr._end == 18 && tr._hex == ir1) {
			pass = true;
		}
		else if (tr._start == 19 && tr._end == 25 && tr._hex == ir2) {
			pass = true;
		}
		else if (tr._start == 26 && tr._end == 49 && tr._hex == ir3) {
			pass = true;
		}
		if(pass) {
			SerialPrintln(" PASSED");
		}
		else {
			SerialPrintln(" FAILED");
		}
	}
}

void
TestMemory::setup() {
	size_t lowerBound = 1;
	size_t upperBound = 5000;

	while (lowerBound <= upperBound) {
		size_t mid = lowerBound + (upperBound - lowerBound) / 2;
		void *ptr = malloc(sizeof(uint8_t) * mid);

		if (ptr) {
//			Serial.print(F("Allocated Bytes: "));
//			Serial.println(mid);
			free(ptr);
			lowerBound = mid + 1;
		} else {
//			Serial.print(F("Could not Allocate Bytes: "));
//			Serial.println(mid);
			upperBound = mid - 1;
		}
	}
	Serial.print(F("Maximum Allocated Bytes: "));
	Serial.println(upperBound);

	Serial.print(F("From FreeMemory(): "));
	Serial.println(freeMemory());
}

}

