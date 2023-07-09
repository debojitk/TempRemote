/*
 * SmartRemoteTests.h
 *
 *  Created on: 02-Jul-2023
 *      Author: arnab
 */

#ifndef SMARTREMOTETESTS_H_
#define SMARTREMOTETESTS_H_

#include "HexProgrammer.h"


namespace TEST {

class TestRemoteData {
public:
	TestRemoteData(RemoteData &r):_r(r){}
	void loop()  {};
	void setup() {
		testDataAdd();
		testIterator();
	}


private:
	void testDataAdd();
	void testIterator();
	RemoteData &_r;
private:

};

#ifndef	DISABLE_SERIAL_PRINT
class TestSaveRestore {
public:
	TestSaveRestore() {}
	void loop()  {};
	void setup() {
		resetTest();
	}

private:
	void resetTest();
	void checkNull();
	void setData();
	void setNull();
	void checkData();
	RemoteData _r;
};
#endif

//class TestSchedule {
//public:
//	TestRemoteData(RemoteData &r):_r(r){}
//	void loop()  {};
//	void setup() {
//		addSchedules();
//		testIterator();
//	}
//
//
//private:
//	void testDataAdd();
//	void testIterator();
//	RemoteData &_r;
//private:
//
//};


class TestMemory {
public:
	void setup();
	void loop() {}
};

} /* namespace TEST */
#endif /* SMARTREMOTETESTS_H_ */

