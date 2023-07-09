/*
 * SmartRemoteTests.cpp
 *
 *  Created on: 02-Jul-2023
 *      Author: arnab
 */

#include "SmartRemoteTests.h"
#include <Arduino.h>
#include "CommonItems.h"


namespace TEST {

void CHECK(bool cond, const char* c) {
	Serial.print(c);
	if(cond) {
		Serial.println(" : PASSED");
	}
	else {
		Serial.println(" : FAILED");
	}
}


void TestRemoteData::testDataAdd() {
	IRNode ir1{10, 20, 30};
	IRNode ir2{11, 21, 31};
	IRNode ir3{12, 22, 32};
	TemperatureRange t1{15,20, ir1};
	TemperatureRange t2{19,25, ir2};
	TemperatureRange t3{26,49, ir3};
	_r.addRange(t1);
	_r.addRange(t2);
	_r.addRange(t3);

	for(uint8_t i = 15; i <= 49; ++i) {
		IRNode ref;
		IRNode irN = _r.atTemperature(i);
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
	for(auto it = _r.beginRange(), itEnd = _r.endRange();
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
}

#ifndef	DISABLE_SERIAL_PRINT
void
TestSaveRestore::resetTest() {


	checkNull();

	Serial.println("Saving null....");

	_r.save();

	Serial.println("Saved....");

	Serial.println("Filling Remote data with garbage....");

	setData();

	Serial.println("Restoring ....");

	_r.restore();

	Serial.println("Restored....NULL");

	checkNull();

	//------------------------------------------------------------

	setData();

	_r.save();

	setNull();

	_r.p();

	_r.restore();

	checkData();

	_r.p();
}

void
TestSaveRestore::checkNull() {
	for(auto i = 0; i < CONFIG::NUM_INDEX; ++i) {
		CHECK(_r.getLayout()._index[i] == CONFIG::MAX_HEX_CODES, "NULL Index check");
	}

	for(auto i = 0; i < CONFIG::MAX_HEX_CODES; ++i) {
		CHECK(_r.getLayout()._hexCodes[i] == NullIRNode, "NULL Hex code check");
	}

	for(auto i = 0; i < CONFIG::NUM_SCHEDULE; ++i) {
		CHECK(_r.getLayout()._schedules[i] == NullSchedule, "NULL schedule check");
	}
}

void
TestSaveRestore::setData() {

	for(auto i = 0; i < CONFIG::NUM_INDEX; ++i) {
		_r.getLayout()._index[i] = CONFIG::MAX_HEX_CODES - 1;
		CHECK((_r.getLayout()._index[i] == CONFIG::MAX_HEX_CODES - 1), "Index not NULL");
	}

	IRNode ir{12, 22, 32};
	for(auto i = 0; i < CONFIG::MAX_HEX_CODES; ++i) {
		_r.getLayout()._hexCodes[i] = ir;
		CHECK(!(_r.getLayout()._hexCodes[i] == NullIRNode), "Hex code not NULL");
	}

	SchedulerTime st1;
	st1._hr = 1;
	st1._min = 2;
	SchedulerTime st2;
	st2._hr = 3;
	st2._min = 4;
	Schedule s;
	s._begin = st1;
	s._end = st2;
	for(auto i = 0; i < CONFIG::NUM_SCHEDULE; ++i) {
		_r.getLayout()._schedules[i] = s;
		CHECK((_r.getLayout()._schedules[i] == s), "Schedule not NULL");
	}
}

void
TestSaveRestore::setNull() {

	for(auto i = 0; i < CONFIG::NUM_INDEX; ++i) {
		_r.getLayout()._index[i] = CONFIG::MAX_HEX_CODES;
		CHECK((_r.getLayout()._index[i] == CONFIG::MAX_HEX_CODES), "Index is set NULL");
	}

	IRNode ir{12, 22, 32};
	for(auto i = 0; i < CONFIG::MAX_HEX_CODES; ++i) {
		_r.getLayout()._hexCodes[i] = NullIRNode;
		CHECK((_r.getLayout()._hexCodes[i] == NullIRNode), "Hex code is set NULL");
	}


	for(auto i = 0; i < CONFIG::NUM_SCHEDULE; ++i) {
		_r.getLayout()._schedules[i] = NullSchedule;
		CHECK((_r.getLayout()._schedules[i] == NullSchedule), "Schedule is set NULL");
	}
}

void
TestSaveRestore::checkData() {

	for(auto i = 0; i < CONFIG::NUM_INDEX; ++i) {
		CHECK((_r.getLayout()._index[i] == CONFIG::MAX_HEX_CODES - 1), "Index check");
	}


	IRNode ir{12, 22, 32};
	for(auto i = 0; i < CONFIG::MAX_HEX_CODES; ++i) {
		CHECK((_r.getLayout()._hexCodes[i] == ir), "Hex code check");
	}

	SchedulerTime st1;
	st1._hr = 1;
	st1._min = 2;
	SchedulerTime st2;
	st2._hr = 3;
	st2._min = 4;
	Schedule s;
	s._begin = st1;
	s._end = st2;
	for(auto i = 0; i < CONFIG::NUM_SCHEDULE; ++i) {
		CHECK((_r.getLayout()._schedules[i] == s), "Schedule check");
	}
}
#endif

} // namespace



