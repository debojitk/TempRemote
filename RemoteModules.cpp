#include "HexProgrammer.h"
#include "SensorTypes.h"
#include <Arduino.h>
#include "CommonItems.h"

#define F_INTERRUPTS                     15000
#define IRMP_IRSND_ALLOW_DYNAMIC_PINS
#define NO_LED_FEEDBACK_CODE
#define IRSND_IR_FREQUENCY          38000
#define IRMP_PROTOCOL_NAMES         1 // Enable protocol number mapping to protocol strings - requires some FLASH.
#define IRSND_PROTOCOL_NAMES        1 // Enable protocol number mapping to protocol strings - requires some FLASH.
// configure send protocols
#define IRSND_SUPPORT_SAMSUNG48_PROTOCOL        1
#define IRSND_SUPPORT_SAMSUNG_PROTOCOL          1       // Samsung + Samsung32  >= 10000                 ~300 bytes
#define IRSND_SUPPORT_NEC_PROTOCOL 			    1
// configure receive protocols
#define IRMP_SUPPORT_SAMSUNG_PROTOCOL           1       // Samsung + Samsg32    >= 10000                 ~300 bytes
#define IRMP_SUPPORT_SAMSUNG48_PROTOCOL         1       // Samsung48            >= 10000                 ~100 bytes (SAMSUNG must be enabled!)
#define IRMP_SUPPORT_NEC_PROTOCOL               1       // NEC + APPLE + ONKYO  >= 10000                 ~300 bytes

#define USE_ONE_TIMER_FOR_IRMP_AND_IRSND // otherwise we get an error on AVR platform: redefinition of 'void __vector_8()

#include <irmp.hpp>
#include <irsnd.hpp>



RemoteRXModule::RemoteRXModule()  {
	_data = new IRMP_DATA;
}

RemoteRXModule::~RemoteRXModule() {}

void
RemoteRXModule::setup() {
    irmp_init(PIN);
    irmp_irsnd_LEDFeedback(false); // Enable receive signal feedback at LED_BUILTIN for receive and send
}

RemoteRXValue
RemoteRXModule::get() {
    if (irmp_get_data(_data))
    {
        irmp_result_print(_data);
        IRNode node{
        	_data->protocol,
			_data->address,
			_data->command
        };
        // Flush repeats received
        irmp_get_data(_data);
        return node;
    }
	return NullRemoteRXValue;
}

RemoteTXModule::RemoteTXModule() {
	_data = new IRMP_DATA;
}

void
RemoteTXModule::setup() {
    irsnd_init(PIN);
}

bool
RemoteTXModule::set(RemoteRXValue node) {
	_data->protocol = node._protocol;
	_data->address = node._address;
	_data->command = node._command;
	_data->flags = 1;
    if (!irsnd_send_data(_data, true)) {
        SerialPrintln(F("Protocol not found")); // name of protocol is printed by irsnd_data_print()
        return false;
    }
#ifndef DISABLE_SERIAL_PRINT
    irsnd_data_print(&Serial, _data);
#endif
	return true;
}



