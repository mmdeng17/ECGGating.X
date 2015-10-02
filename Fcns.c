#include <p18f46k22.h>

unsigned char data;
unsigned char volt;

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

unsigned int readSSP1(void) {
    PORTAbits.NOT_SS = 1; // do not select

    //while ( !SSPSTATbits.BF );
    
	data = SSP1BUF;
    
    return data;
}