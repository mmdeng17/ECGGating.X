/* 
 * File:   OutFcns.h
 * Author: BME464
 *
 * Created on October 2, 2015, 10:16 AM
 */

#ifndef OUTFCNS_H
#define	OUTFCNS_H

#include <p18f46k22.h>
#include "Lcd.h"
#include <stdio.h>

void writeStates(unsigned int row, unsigned int ECGState, unsigned int GateState, unsigned int SettState);
void writeVolt(unsigned int row, unsigned int volt);
void writeTime(unsigned int row, unsigned int Tick);
void DACVolt(unsigned int volt);
void writeSSP(unsigned int row, unsigned int data);
void writeFloat(unsigned int row, float data);
void writeButton(unsigned int row);

#endif	/* OUTFCNS_H */

