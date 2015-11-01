#include <p18f46k22.h>
#include "Lcd.h"
#include <stdio.h>

char dispStr[15];

void writeStates(unsigned int row, unsigned int ECGState, unsigned int GateState, unsigned int SettState) {
    LCDGoto(0,row);
    if (ECGState==1)
        LCDWriteStr("1");
    else
        LCDWriteStr("0");
    
    if (GateState==1)
        LCDWriteStr("1");
    else
        LCDWriteStr("0");
    
    if (SettState==1)
        LCDWriteStr("1");
    else
        LCDWriteStr("0");
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

void writeFloat(unsigned int row, float data) {
    LCDGoto(0,row);
    sprintf(dispStr,"%08d",data);
    LCDPutChar(dispStr[0]);
    LCDPutChar(dispStr[1]);
    LCDPutChar(dispStr[2]);
    LCDPutChar(dispStr[3]);
    LCDPutChar(dispStr[4]);
    LCDPutChar(dispStr[5]);
    LCDPutChar(dispStr[6]);
    LCDPutChar(dispStr[7]);
//    LCDPutChar('.');
//    LCDPutChar(dispStr[8]);
//    LCDPutChar(dispStr[9]);
    
//    LCDPutChar(dispStr[6]);
//    LCDPutChar(dispStr[7]);
//    LCDPutChar(dispStr[8]);
//    LCDPutChar(dispStr[9]);
//    LCDPutChar(dispStr[10]);
//    LCDPutChar(dispStr[11]);
}


void writeButton(unsigned int row, unsigned int Tick)
{
    LCDGoto(0,row);
    if (PORTBbits.RB4)
        LCDWriteStr("1");
    else
        LCDWriteStr("0");
    
    if (PORTBbits.RB5)
        LCDWriteStr("1");
    else
        LCDWriteStr("0");
    
    if (PORTBbits.RB6)
        LCDWriteStr("1");
    else
        LCDWriteStr("0");
    
    if (PORTBbits.RB7)
        LCDWriteStr("1");
    else
        LCDWriteStr("0");
}

void writeChar(unsigned int row, unsigned char outchar)
{
    LCDGoto(0,row);
    sprintf(dispStr,"%02d", outchar);
    LCDPutChar(dispStr[0]);
    LCDPutChar(dispStr[1]);
}

void writeBin(unsigned int row, unsigned char outbin)
{
    LCDGoto(0,row);
    if (outbin==1)
        LCDWriteStr("1");
    else
        LCDWriteStr("0");
}