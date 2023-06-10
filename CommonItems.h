/*
 * CommonItems.h
 *
 *  Created on: 20-May-2023
 *      Author: debojitk
 */

#ifndef COMMONITEMS_H_
#define COMMONITEMS_H_

enum EventType: unsigned int {
	NoEvent = 0,
	SingleClickEvent,
	DoubleClickEvent,
	LongPressEvent
	// Add more event types as needed
};
struct CurrentTime {
  int hours;
  int minutes;
  int seconds;
};


#endif /* COMMONITEMS_H_ */
