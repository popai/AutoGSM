/*
 * cmd.c
 *
 *  Created on: Apr 15, 2013
 *      Author: popai
 *  Modify on Aug 26, 2015
 */
#include "Arduino.h"
#include "cmd.h"
#include "pinDef.h"
#include <MyGSM.h>

//#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/io.h>
//#include <string.h>
#include <stdio.h>
//#include <math.h>

GSM gsm;			//gsm handler class
//extern uint8_t nr_pfonnr;
void static StareIN(char *nrtel);
void static StareOUT(char *nrtel);
void static StareTMP(char *nrtel);

//"Flash store comands etc are strings to store
const prog_char IN1[] PROGMEM = "IN1";			//adresa 18*1
const prog_char IN2[] PROGMEM = "IN2";			//adresa 18*2
const prog_char IN3[] PROGMEM = "IN3";			//adresa 18*3
const prog_char OUT1L[] PROGMEM = "OUT1L";		//adresa 18*4
const prog_char OUT1H[] PROGMEM = "OUT1H";		//adresa 18*5
const prog_char OUT1P[] PROGMEM = "OUT1P";		//adresa 18*6
const prog_char OUT2L[] PROGMEM = "OUT2L";		//adresa 18*7
const prog_char OUT2H[] PROGMEM = "OUT2H";		//adresa 18*8
const prog_char OUT2P[] PROGMEM = "OUT2P";		//adresa 18*9
const prog_char OUT3L[] PROGMEM = "OUT3L";		//adresa 18*10
const prog_char OUT3H[] PROGMEM = "OUT3H";		//adresa 18*11
const prog_char OUT3P[] PROGMEM = "OUT3P";		//adresa 18*12
const prog_char OUT4L[] PROGMEM = "OUT4L";		//adresa 18*13
const prog_char OUT4H[] PROGMEM = "OUT4H";		//adresa 18*14
const prog_char OUT4P[] PROGMEM = "OUT4P";		//adresa 18*15
const prog_char OUT5L[] PROGMEM = "OUT5L";		//adresa 18*16
const prog_char OUT5H[] PROGMEM = "OUT5H";		//adresa 18*17
const prog_char OUT5P[] PROGMEM = "OUT5P";		//adresa 18*18
const prog_char OUT6L[] PROGMEM = "OUT6L";		//adresa 18*19
const prog_char OUT6H[] PROGMEM = "OUT6H";		//adresa 18*20
const prog_char OUT6P[] PROGMEM = "OUT6P";		//adresa 18*21
const prog_char TMP1[] PROGMEM = "TMP1";		//adresa 18*22
const prog_char TMP2[] PROGMEM = "TMP2";		//adresa 18*23
const prog_char PASS[] PROGMEM = "PASS";		//adresa 18*24
const prog_char LOGIN[] PROGMEM = "LOGIN";		//adresa
const prog_char DEL[] PROGMEM = "DEL";		//adresa
const prog_char STARE_IN[] PROGMEM = "STARE IN";	//predefined command
const prog_char STARE_OUT[] PROGMEM = "STARE OUT";	//predefined command
const prog_char STARE_TMP[] PROGMEM = "STARE TMP";	//predefined command
const prog_char STARE_ALL[] PROGMEM = "STARE ALL";	//predefined command
//const prog_char OK[] PROGMEM = "OK";	//adresa 18*22

// The table to refer to my strings.
const char *comenzi[]PROGMEM =
{ IN1, IN2, IN3, OUT1L, OUT1H, OUT1P, OUT2L, OUT2H, OUT2P, OUT3L, OUT3H, OUT3P,
		OUT4L, OUT4H, OUT4P, OUT5L, OUT5H, OUT5P, OUT6L, OUT6H, OUT6P, TMP1,
		TMP2, PASS };

//int8_t in1 = 1, in2 = 1, in3 = 1, in4 = 1;
bool in1 = true;
bool in2 = true;
bool in3 = true;
//bool in4 = true;

//read data from eeprom on specified adres
void ReadEprom(char* str_citit, int const adresa)
{
	//char str_citit[18];
	eeprom_read_block(str_citit, (int*) adresa, 18);
//#if DEBUG
	//Serial.print("citesc la adresa: ");
	Serial.print(adresa);
	Serial.print(": ");
	Serial.println(str_citit);
//#endif
	//return str_citit;
}

void DellEprom()
{
	int i;
	for (i = 0; i < 512; i++)
		eeprom_write_byte((uint8_t*) i, 0);
}
/*
 void DellPass()
 {
 for (int i = 18 * 19; i < 18 * 20; i++)
 EEPROM.update(i, 0);
 }
 */

/**
 * @brief : Write user defined commands on eeprom
 *
 * @param : char *inbuffer = command send by user
 * @return: 0 = writed not succeed, 1 = succeed write
 */
int8_t CfgCmd(char *inbuffer)
{
	int adresa = 18;
	//char *scriu;
	char comanda[7];
	int8_t i;

	for (i = 0; i < 24; ++i)
	{
		strcpy_P(comanda, (char*) pgm_read_word(&(comenzi[i]))); // Necessary casts and dereferencing, just copy.
		if (strstr(inbuffer, comanda) != 0)
		{

			inbuffer += strlen(comanda) + 1;
			adresa = 18 * (1 + i);
//#if DEBUG
			//Serial.print("Scriu la adresa ");
			Serial.print(adresa);
			Serial.print(": ");
			Serial.println(inbuffer);
//#endif
			eeprom_write_block(inbuffer, (int*) adresa, 18);
			//EEPROM.updateBlock(inbuffer, adresa, 18);
			return 1;
		}
	}
	return 0;
}

/**
 * @brief : write the sms string for commands
 *
 * @param : char *nrtel -> sms sender phone number
 * 			char *inmsg -> sms message
 * @return: no return
 */
void Config(char *nrtel, char *inmsg)
{
	char buffer[64];
	int adr = 18;
	if ((strlen(nrtel) != 0) && (strlen(inmsg) != 0))
	{
		//TODO mtetod to delete number from pozition

		//add number on authorized slot;
		if (strstr_P(inmsg, LOGIN) != 0)
		{
			int8_t nr_pfonnr = 1;
			if (nr_pfonnr < 7)
			{
				byte error = gsm.WritePhoneNumber(nr_pfonnr, nrtel);
				if (error != 0)
				{
					sprintf_P(buffer,
							PSTR("Number %s writed in Phone Book position %d"),
							nrtel, nr_pfonnr);
					Serial.println(buffer);
					++nr_pfonnr;

					strcpy_P(buffer, PSTR("Acceptat"));
					gsm.SendSMS(nrtel, buffer);

				}
				else
				{
					strcpy_P(buffer, PSTR("Writing error"));
					Serial.println(buffer);
					gsm.SendSMS(nrtel, buffer);
				}
			}
			else
			{
				strcpy_P(buffer, PSTR("No free slot"));
				gsm.SendSMS(nrtel, buffer);
			}

			//CfgCmd(inmsg);

		}
		else if (strstr_P(inmsg, DEL) != 0)
		{
			byte i = 1;
			byte error = 0;
			for (i = 1; i < 7; i++)
				if (gsm.ComparePhoneNumber(i, nrtel))
					break;
			if (i < 7)
				error = gsm.DelPhoneNumber(i);
			if (error != 0)
			{
				sprintf_P(buffer, " %s: %d %s", PSTR("Phone number position "), i, PSTR(" deleted"));
				//Serial.print("Phone number position ");
				//Serial.print(i);
				//Serial.println(" deleted");
				Serial.println(buffer);

				strcpy_P(buffer, PSTR("Sters"));
				gsm.SendSMS(nrtel, buffer);
			}
			else
			{
				strcpy_P(buffer, PSTR("Ne Sters"));
				gsm.SendSMS(nrtel, buffer);
			}

		}
		else
		{
			CfgCmd(inmsg);
		}
	}
}

void Comand(char *nrtel, char *inmsg)
{
	char buffer[24];
	char OK[3] = "OK";
	//uint8_t pin_state = 0b00000000;		//variable to write in eeprom pins state

	ReadEprom(buffer, 18 * 4);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD1, LOW);
		PORTD &= ~(1 << PIND2);
		//pin_state &= ~(1 << PIND2);
		////eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 5);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD1, HIGH);
		PORTD |= (1 << PIND2);
		////pin_state |= (1 << PIND2);
		////eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 6);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD1, HIGH);
		PORTD |= (1 << PIND2);
		delay(1000);
		PORTD &= ~(1 << PIND2);
		////pin_state |= (1 << PIND2);
		////eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 7);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD2, LOW);
		PORTD &= ~(1 << PIND3);
		////pin_state |= (1 << PIND3);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 8);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD2, HIGH);
		PORTD |= (1 << PIND3);
		//pin_state |= (1 << PIND3);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 9);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD1, HIGH);
		PORTD |= (1 << PIND3);
		delay(1000);
		PORTD &= ~(1 << PIND3);
		////pin_state |= (1 << PIND2);
		////eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 10);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD3, LOW);
		PORTD &= ~(1 << PIND4);
		//pin_state |= (1 << PIND4);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 11);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD3, HIGH);
		PORTD |= (1 << PIND4);
		//pin_state |= (1 << PIND4);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 12);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD1, HIGH);
		PORTD |= (1 << PIND4);
		delay(1000);
		PORTD &= ~(1 << PIND4);
		////pin_state |= (1 << PIND2);
		////eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 13);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD4, LOW);
		PORTD &= ~(1 << PIND5);
		//pin_state |= (1 << PIND5);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 14);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD4, HIGH);
		PORTD |= (1 << PIND5);
		//pin_state |= (1 << PIND5);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 15);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD1, HIGH);
		PORTD |= (1 << PIND5);
		delay(1000);
		PORTD &= ~(1 << PIND5);
		////pin_state |= (1 << PIND2);
		////eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 16);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD5, LOW);
		PORTD &= ~(1 << PIND6);
		//pin_state |= (1 << PIND6);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 17);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD5, HIGH);
		PORTD |= (1 << PIND6);
		//pin_state |= (1 << PIND6);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 18);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD1, HIGH);
		PORTD |= (1 << PIND6);
		delay(1000);
		PORTD &= ~(1 << PIND6);
		////pin_state |= (1 << PIND2);
		////eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 19);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD5, LOW);
		PORTD &= ~(1 << PIND7);
		//pin_state |= (1 << PIND7);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 20);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD5, HIGH);
		PORTD |= (1 << PIND7);
		//pin_state |= (1 << PIND7);
		//eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	ReadEprom(buffer, 18 * 21);
	if (strcasecmp(buffer, inmsg) == 0)
	{
		//digitalWrite(outD1, HIGH);
		PORTD |= (1 << PIND7);
		delay(1000);
		PORTD &= ~(1 << PIND7);
		////pin_state |= (1 << PIND2);
		////eeprom_write_byte((uint8_t*) 396, pin_state);
		gsm.SendSMS(nrtel, OK);
		return;
	}

	if (strcasecmp_P(inmsg, STARE_IN) == 0)
	{
		StareIN(nrtel);
		return;
	}

	//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[18])));
	//if (strcasecmp(buffer, inmsg) == 0)
	if (strcasecmp_P(inmsg, STARE_OUT) == 0)
	{
		StareOUT(nrtel);
		return;
	}
	//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[19])));
	//if (strcasecmp(buffer, inmsg) == 0)
	if (strcasecmp_P(inmsg, STARE_TMP) == 0)
	{
		StareTMP(nrtel);
		return;
	}

	//strcpy_P(buffer, (char*) pgm_read_word(&(comenzi[20])));
	//if (strcasecmp(buffer, inmsg) == 0)
	if (strcasecmp_P(inmsg, STARE_ALL) == 0)
	{
		StareIN(nrtel);
		StareOUT(nrtel);
		StareTMP(nrtel);
		return;
	}

	if (strcasecmp_P(inmsg, DEL) == 0)
	{
		byte i = 1;
		byte error = 0;
		for (i = 1; i < 7; i++)
			if (gsm.ComparePhoneNumber(i, nrtel))
				break;
		if (i < 7)
			error = gsm.DelPhoneNumber(i);
		if (error != 0)
		{
			char tmpbuffer[64];
			sprintf_P(tmpbuffer, " %s: %d %s", PSTR("Phone number position "), i, PSTR(" deleted"));
			//Serial.print("Number position ");
			//Serial.print(i);
			//Serial.println(" deleted");
			Serial.println(buffer);

			strcpy_P(buffer, PSTR("Sters"));
			gsm.SendSMS(nrtel, tmpbuffer);
		}
		else
		{
			strcpy_P(buffer, PSTR("Ne Sters"));
			gsm.SendSMS(nrtel, buffer);
		}
		return;
	}

	ReadEprom(buffer, 486);
	gsm.SendSMS(nrtel, buffer);
	Serial.println(buffer);
	return;

}

/**
 * @brief :	Send a sms with state of output pins
 *
 * @param : char *nrtel = phone number who interrogate the state
 * @return: no return
 */
void static StareOUT(char *nrtel)
{
	char mesage[256];
	char buffer[18];
	//int i = 108;
	*mesage = 0x00;
	//if (digitalRead(outD1) == LOW)
	if ((PIND & (1 << PIND2)) == 0)
	{
		ReadEprom(buffer, 18 * 4);
		//Serial.print(strlen(buffer));
		//Serial.println(buffer);

		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}

	}
	else
	{
		ReadEprom(buffer, 18 * 5);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}

	//if (digitalRead(outD2) == LOW)
	if ((PIND & (1 << PIND3)) == 0)
	{
		ReadEprom(buffer, 18 * 7);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 8);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}

	//if (digitalRead(outD3) == LOW)
	if ((PIND & (1 << PIND4)) == 0)
	{
		ReadEprom(buffer, 18 * 10);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 11);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}

	//if (digitalRead(outD4) == LOW)
	if ((PIND & (1 << PIND5)) == 0)
	{
		ReadEprom(buffer, 18 * 13);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 14);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}

	//if (digitalRead(outD5) == LOW)
	if ((PIND & (1 << PIND6)) == 0)
	{
		ReadEprom(buffer, 18 * 16);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 17);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}

	if ((PIND & (1 << PIND7)) == 0)
	{
		ReadEprom(buffer, 18 * 19);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}
	}
	else
	{
		ReadEprom(buffer, 18 * 20);
		if (strlen(buffer) != 0)
		{
			strcat(mesage, buffer);
			//strcat(mesage, "\n");
		}
	}

	if (strlen(mesage) != 0)
		gsm.SendSMS(nrtel, mesage);
}

/**
 * @brief : Send a sms with state of input pins
 *
 * @param :nrtel -> telephone number to send sms
 * @return: no return
 */
void static StareIN(char *nrtel)
{

	char mesage[96];
	char buffer[18];
	*mesage = 0x00;
//if (digitalRead(inD1) == LOW && in1)
	if ((PINB & (1 << PINB1)) == 0)
	{
		ReadEprom(buffer, 18 * 1);
		if (strlen(buffer) != 0)
		{
			strcat(buffer, " on");
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}

	}
	else
	{
		ReadEprom(buffer, 18 * 1);
		if (strlen(buffer) != 0)
		{
			strcat(buffer, " off");
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));

		}

	}

//if (digitalRead(inD2) == LOW && in2)
	if ((PINB & (1 << PINB2)) == 0)
	{
		ReadEprom(buffer, 18 * 2);
		if (strlen(buffer) != 0)
		{
			strcat(buffer, " on");
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}

	}
	else
	{
		ReadEprom(buffer, 18 * 2);
		if (strlen(buffer) != 0)
		{
			strcat(buffer, " off");
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));

		}

	}

//if (digitalRead(inD3) == LOW && in3)
	if ((PINB & (1 << PINB3)) == 0)
	{
		ReadEprom(buffer, 18 * 3);
		if (strlen(buffer) != 0)
		{
			strcat(buffer, " on");
			strcat(mesage, buffer);
			strcat_P(mesage, PSTR("\r\n"));
		}

	}
	else
	{
		ReadEprom(buffer, 18 * 3);
		if (strlen(buffer) != 0)
		{
			strcat(buffer, " off");
			strcat(mesage, buffer);
			//strcat_P(mesage, PSTR("\n\r"));

		}

	}
	if (strlen(mesage) != 0)
		gsm.SendSMS(nrtel, mesage);

}
/**
 * @brief : Thermistor calculate the temperature in grades Celsius dead by a 10K thermistor
 *
 * @param : analog pin where connect the thermistor
 * @return: temperature in grade Celsius
 */
float Thermistor(int Tpin)
{
	float Vo;
	float logRt, Rt, T;
	float R = 9870; // fixed resistance, measured with multimeter
	// c1, c2, c3 are calibration coefficients for a particular thermistor
	float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
	Vo = analogRead(Tpin);
	//Vo = ReadADC(Tpin);
	Rt = R * (1024.0 / Vo - 1.0);
	logRt = log(Rt);
	T = (1.0 / (c1 + c2 * logRt + c3 * logRt * logRt * logRt)) - 273.15;
	return T;
}

/**
 * @brief : Send a sms with temperature read from senzors
 *
 * @param : char *nrtel = phone number who interrogate the state
 * @return: no return
 */
static void StareTMP(char *nrtel)
{
	char mesage[96];
	char buffer[18];
	char tmpe[32];
	int tmp, tmp1, tmp2;
	*mesage = 0x00;

	tmp = Thermistor(PINC0);
	delay(10);
	tmp1 = Thermistor(PINC0);
	delay(10);
	tmp2 = Thermistor(PINC0);
	tmp = (tmp + tmp1 + tmp2) / 3;

	ReadEprom(buffer, 18 * 22);
	if (strlen(buffer) != 0)
	{
		sprintf(tmpe, " %s: %d %s", buffer, tmp, "C\r\n");
		strcat(mesage, tmpe);
	}

	tmp = Thermistor(PINC1);
	delay(10);
	tmp1 = Thermistor(PINC1);
	delay(10);
	tmp2 = Thermistor(PINC1);
	tmp = (tmp + tmp1 + tmp2) / 3;

	ReadEprom(buffer, 18 * 23);
	if (strlen(buffer) != 0)
	{
		sprintf(tmpe, " %s: %d %s", buffer, tmp, "C\r\n");
		strcat(mesage, tmpe);
	}

	if (strlen(mesage) != 0)
		gsm.SendSMS(nrtel, mesage);
}

/**
 * @brief : Verify the state of input pins
 *
 * @param : no parameters
 * @return: no return
 */
void VerificIN()
{
	//char mesage[80];
	char number[20];
	char buffer[32];
	int error = 0;

	//if (digitalRead(inD1) == LOW && in1)
	if ((PINB & (1 << PINB1)) == 0)
	{
		if (in1)
		{
			in1 = false;
			ReadEprom(buffer, 18 * 1);
			if (strlen(buffer) != 0)
			{
				strcat(buffer, " on");
				for (byte i = 1; i < 7; i++)
				{
					error = gsm.GetPhoneNumber(i, number);
					if (error == 1)  //Find number in specified position
						gsm.SendSMS(number, buffer);
				}
			}
		}
	}
	else
	{
		if (in1 == false)
		{
			ReadEprom(buffer, 18 * 1);
			if (strlen(buffer) != 0)
			{
				strcat(buffer, " off");
				for (byte i = 1; i < 7; i++)
				{
					error = gsm.GetPhoneNumber(i, number);
					if (error == 1)  //Find number in specified position
						gsm.SendSMS(number, buffer);
				}
			}
		}
		in1 = true;
	}

	//if (digitalRead(inD2) == LOW && in2)
	if ((PINB & (1 << PINB2)) == 0)
	{
		if (in2)
		{
			in2 = false;
			ReadEprom(buffer, 18 * 2);
			if (strlen(buffer) != 0)
			{
				strcat(buffer, " on");
				for (byte i = 1; i < 7; i++)
				{
					error = gsm.GetPhoneNumber(i, number);
					if (error == 1)  //Find number in specified position
						gsm.SendSMS(number, buffer);
				}
			}
		}
	}
	else
	{
		if (in2 == false)
		{
			ReadEprom(buffer, 18 * 2);
			if (strlen(buffer) != 0)
			{
				strcat(buffer, " off");
				for (byte i = 1; i < 7; i++)
				{
					error = gsm.GetPhoneNumber(i, number);
					if (error == 1)  //Find number in specified position
						gsm.SendSMS(number, buffer);
				}
			}
		}
		in2 = true;
	}

	//if (digitalRead(inD3) == LOW && in3)
	if ((PINB & (1 << PINB3)) == 0)
	{
		if (in3)
		{
			in3 = false;
			ReadEprom(buffer, 18 * 3);
			if (strlen(buffer) != 0)
			{
				strcat(buffer, " on");
				for (byte i = 1; i < 7; i++)
				{
					error = gsm.GetPhoneNumber(i, number);
					if (error == 1)  //Find number in specified position
						gsm.SendSMS(number, buffer);
				}
			}
		}
	}
	else
	{
		if (in3 == false)
		{
			ReadEprom(buffer, 18 * 3);
			if (strlen(buffer) != 0)
			{
				strcat(buffer, " off");
				for (byte i = 1; i < 7; i++)
				{
					error = gsm.GetPhoneNumber(i, number);
					if (error == 1)  //Find number in specified position
						gsm.SendSMS(number, buffer);
				}
			}
		}
		in3 = true;
	}

}

/**
 * XXX de verificat daca merge, momentet renuntat la idee
 * @brief : In case of reset out/in don't lose state
 *
 * @param : no parameters
 * @return: no return
 */
void SetPort()
{
	uint8_t pin_state = 0b00000000;
	pin_state = eeprom_read_byte((const uint8_t *) 396);
	//Serial.println(pin_state);
	PORTD |= pin_state;
}
