#include <p18f46k22.h>
#include "Lcd.h"
#include <stdio.h>

char dispStr[15];

void writeState(unsigned int row,unsigned int ECGState) {
    LCDGoto(0,row);
    LCDWriteStr("ECG state is: ");
	LCDGoto(0,13);
    LCDPutByte(ECGState);
}

void writeVolt(unsigned int row, unsigned int volt) {
    LCDGoto(0,row);
    sprintf(dispStr,"%04d",volt*49/10); //Approximate conversion to 0-5V
    LCDPutChar(dispStr[0]);
    LCDPutChar('.');
    LCDPutChar(dispStr[1]);
    LCDPutChar(dispStr[2]);
    LCDPutChar(dispStr[3]);
    LCDPutChar('V');
}

void writeTime(unsigned int row, unsigned int Tick)
{
    LCDGoto(0,row);
    sprintf(dispStr,"%06d", Tick);
    LCDPutChar(dispStr[0]);
    LCDPutChar(dispStr[1]);
    LCDPutChar(dispStr[2]);
    LCDPutChar(dispStr[3]);
    LCDPutChar(dispStr[4]);
    LCDPutChar(dispStr[5]);
}

void DACVolt(unsigned int volt) {
    VREFCON2bits.DACR = volt>>5;
}

void writeSSP(unsigned int row, unsigned int data)
{
    LCDGoto(0,row);
    sprintf(dispStr,"%06d", data);
    LCDPutChar(dispStr[0]);
    LCDPutChar(dispStr[1]);
    LCDPutChar(dispStr[2]);
    LCDPutChar(dispStr[3]);
    LCDPutChar(dispStr[4]);
    LCDPutChar(dispStr[5]);
}