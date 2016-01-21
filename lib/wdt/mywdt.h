/*
 * mywdt.h
 *
 *  Created on: Jan 21, 2016
 *      Author: ionel.g.popa
 */

#ifndef MYWDT_H_
#define MYWDT_H_

#define TIMEOUTPERIOD 9          	// You can make this time as long as you want,
                                       	// it's not limited to 8 seconds like the normal
                                       	// watchdog

void watchdogSetup();
void resetWDT();

#endif /* MYWDT_H_ */
