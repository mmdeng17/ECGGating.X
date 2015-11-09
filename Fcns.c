#include <p18f46k22.h>
#include <delays.h>
#include "Lcd.h"

// Function to read analog voltage in on AN0
unsigned int readAVin(void) {
    int volt=0;
    ADCON0bits.GO=1; // Start conversion
    while(ADCON0bits.GO==1){}; // Wait for finish
							   // look into possibility of doing this with interrupts
			// can do using PIR1bits.ADIF
			// and PIE1bits.ADIE
    volt = ADRESH; // get high byte of voltage
    volt = (volt<<8) | ADRESL; // combine with low byte of voltage
    if(volt==1023) //Fix roundoff error
        volt=1022;
    
    return volt;
}

// Function to write a 10-bit voltage value out on RA2 using onboard DAC
void DACVolt(unsigned int volt) {
    VREFCON2bits.DACR = volt>>5;
}

// Function to detect if RB7 (ECGState) is pressed
unsigned char isRB7(void)
{
    unsigned char i = 0;
    while (PORTBbits.RB7==1) {
        i++;
        if (i>3)
            return 1;
    }
    
    return 0;
}

// Function to detect if RB6 (TrigState) is pressed
unsigned char isRB6(void)
{
    unsigned char i = 0;
    while (PORTBbits.RB6==1) {
        i++;
        if (i>3)
            return 1;
    }
    
    return 0;
}

// Function to allow user to update settings
//  Press both buttons to advance from quantity to quantity
//  Press one button to increment value
//  RB7 (ECGState) - increase
//  RB6 (TrigSTate) - decrease
void SettingsMode(unsigned int *a1, unsigned int *a2, unsigned int *a3, unsigned int *a4) {
    int tmpState = 0;
    Delay10KTCYx(200);
    
    while(1)
    {   
        // Display current quantity
        LCDClear();
        if (tmpState==0) {
            LCDGoto(0,0);
            LCDWriteStr("ECG Threshold:");
            writeUInt(1,*a1);
        }
        else if (tmpState==1) {
            LCDGoto(0,0);
            LCDWriteStr("QT Delay:");
            writeUInt(1,*a2);
        }
        else if (tmpState==2) {
            LCDGoto(0,0);
            LCDWriteStr("Im. Delay:");
            writeUInt(1,*a3);
        }
        else if (tmpState==3) {
            LCDGoto(0,0);
            LCDWriteStr("Im. Length:");
            writeUInt(1,*a4);
        }
        else if (tmpState==4) {
            LCDClear();
            break;
        }
        
        // Read button presses to update values
        if (isRB7()==1 && isRB6()==1) {
            tmpState = (tmpState+1)%5;
            Delay10KTCYx(200);
        }
        else if (isRB6()==1) {
            switch (tmpState) {
                case 0:
                    if (*a1>1)
                        *a1 = *a1-2;
                case 1:
                    if (*a2>25)
                        *a2 = *a2-1;
                case 2:
                    if (*a3>20)
                        *a3 = *a3-1;
                case 3:
                    if (*a4>0)
                        *a4 = *a4-1;
            }
        }
        else if (isRB7()==1) {
            switch (tmpState) {
                case 0:
                    *a1 = *a1+2;
                case 1:
                    if (*a2<200)
                        *a2 = *a2+1;
                case 2:
                    if (*a3<150)
                        *a3 = *a3+1;
                case 3:
                    *a4 = *a4+1;
            }       
        }
        
        Delay10KTCYx(25);
    };
}
