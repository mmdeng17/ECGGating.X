#include <p18f46k22.h>

unsigned int volt = 0;

unsigned int readAVin(void) {
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