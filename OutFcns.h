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

void writeVolt(unsigned int row, unsigned int volt);
void writeBin(unsigned int row,unsigned char outbin);
void writeChar(unsigned int row,unsigned char outchar);
void writeUInt(unsigned int row, unsigned int Tick);
void writeStates(unsigned int row, unsigned int ECGState, unsigned int GateState, unsigned int SettState);

#endif	/* OUTFCNS_H */

