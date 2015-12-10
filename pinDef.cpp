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
//		     (D 0) PD0  2|    |27  PC4 (AI 4)(D 18)	inA5		monitorizare tensiune
//		     (D 1) PD1  3|    |26  PC3 (AI 3)(D 17)	inA4	Tx softSerial
//outD1      (D 2) PD2  4|    |25  PC2 (AI 2)(D 16)	inA3	Rx softSerial
//outD2 PWM+ (D 3) PD3  5|    |24  PC1 (AI 1)(D 15)	inA2	tmp sezor1
//outD3      (D 4) PD4  6|    |23  PC0 (AI 0)(D 14)	inA1	tmp senzor2
//		           VCC  7|    |22  GND
//		           GND  8|    |21  AREF
//	XTAL1		   PB6  9|    |20  AVCC
//  XTAL2          PB7 10|    |19  PB5 (D 13)		error LED
//outD4 PWM+ (D 5) PD5 11|    |18  PB4 (D 12)		user  LED
//outD5 PWM+ (D 6) PD6 12|    |17  PB3 (D 11) PWM	inD4
//outD6      (D 7) PD7 13|    |16  PB2 (D 10) PWM	inD3
//GSM POWER  (D 8) PB0 14|    |15  PB1 (D 9 ) PWM	inD2
//                       +----+
//
// (PWM+ indicates the additional PWM pins on the ATmega168.)
#include <avr/io.h>
#include <pinDef.h>

void pinSetUp()
{
	///Port D configure at output
	DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);
	//DDRD &= ~((1 << DDD2) | (1 << DDD3) | (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7));
	//PORTD |= (1 << PD0) | (1 << PD2) | (1 << PD4) | (1 << PD5) | (1 << PD6); // internal Pull UP switch on
	//DDRD = 0b11111110;
	//PORTD = 0b00000001;

	/**
	 * Port B:
	 * PB0 out GSM POWER
	 * PB1, PB2, PB3 configure at IN
	 * PB4 an PB5 OUT for LEDs
	 */
	DDRB |= (1 << DDB0) | (1 << DDB4) | (1 << DDB5);				// OUTs
	DDRB &= ~( (1 << DDB1) | (1 << DDB2) | (1 << DDB3));			// INs
	PORTB |= (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3); 		// internal Pull UP switch on
	//DDRB = 0b11110001;
	//PORTB = 0b00001110;

	/**
	 * Port C:
	 * PC0, PC1, PC4 analog IN, no pull up R
	 * PC3 digital software serial Rx
	 * PC4 digital software serial Tx
	 * PC5 digital IN, delete EEPROM and Config mode
	 */
	//DDRC |= (1 << DDC5) | (1 << DDC4);
	DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC4) |  (1 << DDC5));//(1 << DDC2) | | (1 << DDC3));

	PORTC |=  (1 << PORTC5); // internal Pull UP switch on
	//DDRC =  0b00100000;
	//PORTC = 0b00011100;
}

