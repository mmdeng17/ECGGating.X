#include <p18f46k22.h>
#include "Lcd.h"
#include <stdio.h>

char dispStr[15];

// Function to write analog voltage to LCD using 10-bit to 5V conversion
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

// Function to write a bit value to LCD
void writeBin(unsigned int row, unsigned char outbin)
{
    LCDGoto(0,row);
    if (outbin==1)
        LCDWriteStr("1");
    else
        LCDWriteStr("0");
}

// Function to write unsigned char to LCD
void writeChar(unsigned int row, unsigned char outchar)
{
    LCDGoto(0,row);
    sprintf(dispStr,"%02d", outchar);
    LCDPutChar(dispStr[0]);
    LCDPutChar(dispStr[1]);
}

// Function to write unsigned int to LCD
void writeUInt(unsigned int row, unsigned int Tick)
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

// Debug function to right States to LCD
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