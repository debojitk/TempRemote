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

#ifdef	ENABLE_TEST
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


class TestMemory {
public:
	void setup();
	void loop() {}
};

} /* namespace TEST */
#endif /* SMARTREMOTETESTS_H_ */

