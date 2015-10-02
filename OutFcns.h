/* 
 * File:   OutFcns.h
 * Author: BME464
 *
 * Created on October 2, 2015, 10:16 AM
 */

#ifndef OUTFCNS_H
#define	OUTFCNS_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#include <p18f46k22.h>
#include "Lcd.h"
#include <stdio.h>

void writeState(unsigned int row, unsigned int ECGState);

void writeVolt(unsigned int row, unsigned int volt);

void writeTime(unsigned int row, unsigned int Tick);

void DACVolt(unsigned int volt);

#endif	/* OUTFCNS_H */

