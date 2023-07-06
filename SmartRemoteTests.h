/*
 * SmartRemoteTests.h
 *
 *  Created on: 02-Jul-2023
 *      Author: arnab
 */

#ifndef SMARTREMOTETESTS_H_
#define SMARTREMOTETESTS_H_

#include "HexProgrammer.h"



//class TEST {
//public:
//    TEST();
//    ~TEST();
//    void SETUP();
//    void LOOP();
//
//private:
//    constexpr uint8_t NUM_TEST = 1
//    TestBase* _tests[NUM_TEST];
//};
//
//
//
//
//struct TestBase {
//    virtual void setup() = 0;
//    virtual void run() = 0;
//    virtual ~TestBase() {
//        Serial.print("Tests Passed: ");
//        Serial.println(TestBase::passed());
//        Serial.print("Tests Faled: ");
//        Serial.println(TestBase::failed());
//    }
//    static void CHECK(bool val) {
//        if(val) {
//            ++_pass;
//        }
//        else {
//            ++_fail;
//        }
//    }
//    uint8_t passed() { return _pass; }
//    uint8_t failed() { return _fail; }
//private:
//    static uint8_t _pass;
//    static uint8_t _fail;
//};
//
//};   // namespace TEST

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



class TestMemory {
public:
	void setup();
	void loop() {}
};

} /* namespace TEST */
#endif /* SMARTREMOTETESTS_H_ */

