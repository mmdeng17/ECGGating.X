#include <p18f46k22.h>
#include <delays.h>

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

