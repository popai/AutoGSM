/*
 * pinDef.h
 *
 *  Created on: Aug 25, 2015
 *      Author: popai
 */
/***********************************************************
 *                  SMS Cotrol module
 This Software uses Arduino project to conect GSM modul SIM300L
 and it sends and recive a SMS to comand the I/O ports.

 Copyright (C) 2012  Popa Ionel  -- popai307@gmail.com --


 ***********************************************************/

// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.
// ATMEL ATMEGA8 & 328 / ARDUINO
//
//   		             +-\/-+
//      		   PC6  1|    |28  PC5 (AI 5)(D 19)	inA6		conf pc/sms
//		     (D 0) PD0  2|    |27  PC4 (AI 4)(D 18)	inA5		delEEPROM mode
//		     (D 1) PD1  3|    |26  PC3 (AI 3)(D 17)	inA4	Tx softSerial
//outD1      (D 2) PD2  4|    |25  PC2 (AI 2)(D 16)	inA3	Rx softSerial
//outD2 PWM+ (D 3) PD3  5|    |24  PC1 (AI 1)(D 15)	inA2	tmp sezor1
//outD3      (D 4) PD4  6|    |23  PC0 (AI 0)(D 14)	inA1	tmp senzor2
//		           VCC  7|    |22  GND
//		           GND  8|    |21  AREF
//		           PB6  9|    |20  AVCC
//                 PB7 10|    |19  PB5 (D 13)		errLED
//outD4 PWM+ (D 5) PD5 11|    |18  PB4 (D 12)		useLED
//outD5 PWM+ (D 6) PD6 12|    |17  PB3 (D 11) PWM	inD4
//GSM POWER  (D 7) PD7 13|    |16  PB2 (D 10) PWM	inD3
//GSM RST    (D 8) PB0 14|    |15  PB1 (D 9) PWM	inD2
//                       +----+
//
// (PWM+ indicates the additional PWM pins on the ATmega168.)

#ifndef PINDEF_H_
#define PINDEF_H_


void pinSetUp();

#endif /* PINDEF_H_ */
