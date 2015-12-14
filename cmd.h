/*
 * cmd.h
 *
 *  Created on: Apr 15, 2013
 *      Author: popai
 *  Modify on Aug 26, 2015
 */

#ifndef CMD_H_
#define CMD_H_


int8_t CfgCmd(char *buffer);
void ReadEprom(char* str_citit, int const adresa);
void DellEprom();
void Config(char *nrtel, char *inmsg);
void Comand(char *nrtel, char *inmsg);
float Thermistor(int RawADC);
void VerificIN();
//void SetPort();

#endif /* CMD_H_ */
