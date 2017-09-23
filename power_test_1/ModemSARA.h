/*
 * ModemSARA.h
 *
 *  Created on: 18 авг. 2017 г.
 *      Author: bekir
 */

#ifndef MODEMSARA_H_
#define MODEMSARA_H_

#define SARA_BUF_SIZE 4000

void Init_SARA(char* port_name, int port_speed);
void Call_SARA(char* number);
void Close_SARA(void);
void PPP_SARA(char* number, char* apn);
void EndCall_SARA(void);
void ICMPPing_SARA(char* apn);

#endif /* MODEMSARA_H_ */
