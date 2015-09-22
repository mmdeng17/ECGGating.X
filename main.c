/*
 * ECG Gating main code
 * 
 */

#include "Lcd.h"
#include <delays.h>
#include <p18f46k22.h>
#include <stdlib.h>
#include <stdio.h>

#pragma config FOSC = INTIO7   // Internal OSC block, CLKOUT RA6/7
#pragma config PLLCFG = ON      // enable 4x pll
#pragma config WDTEN = OFF      // Watch Dog Timer disabled. SWDTEN no effect
#pragma config XINST = OFF      // Instruction set Extension and indexed Addressing mode disabled

//Define statements
#define T1H  0xF0   // Timer is 0xFFBF for 512Hz
#define T1L  0x00	// 0x8000 for 1s 
				    // 0xFFFF-0x8000 = 512 * (0xFFFF-0xFFBF)

//Variable definitions
char dispStr[15];
char dispTick[15];
char dispVolt[15];
unsigned int Tick;
unsigned char count;
unsigned char curr;
unsigned char ECGState = 0;
unsigned char GateState = 0;
unsigned int volt = 0;

//Function definitions
void RTC_ISR(void);
void SysInit(void);
void High_Priority_ISR(void);
void RTCIncSec(void);
void RTCIncMin(void);
void RTCIncHour(void);
void writeTime(void);
void readAVin(void);
void writeVolt(void);
void readSSP1(void);
void writeState(void);

//High priority interrupt
#pragma code InterruptVectorHigh = 0x08
void InterruptVectorHigh (void)
{
  _asm
    goto High_Priority_ISR
  _endasm
}
#pragma interrupt High_Priority_ISR
void High_Priority_ISR(void) 
{
    RTC_ISR(); //Call real-time clock service routine
}


void main(void)
{
    //Initialize
    SysInit();
    LCDClear();


    while(1)
    {   
        writeState();
        writeVolt();
        
        Delay10KTCYx(100);
    };
}


//Initialize necessary systems
void SysInit(void)
{
	// Set up internal oscillator
    OSCCON = 0b01110110;
    OSCTUNEbits.PLLEN = 1;      // enable 4x pll
    OSCTUNEbits.INTSRC = 1;     // use HF-INTOSC as LF source
    OSCCON2bits.MFIOSEL = 0;    

    // Set up LCD
    ANSELD = 0x00;
    TRISD = 0x00; //Digital out
    LCDInit(); //Start LCD
    
    // Set up timer
    TMR1H  = T1H;
    TMR1L  = T1L;
    T1CON = 0x49;           // timer 1 is enabled, from system clock
    T1GCON = 0;                 // Timer 1 Gate function disabled
    RCONbits.IPEN=1;            // Allow interrupt priorities
    PIR1bits.TMR1IF = 0;        // Clear any pending Timer 1 Interrupt indication
    PIE1bits.TMR1IE = 1;        // Enable Timer 1 Interrupt
    
    //Set up A/D on AN0
    ANSELAbits.ANSA0 = 1; // set bit 0 an channel A as analog
    TRISAbits.RA0 = 1; // Analog in
    ADCON2bits.ACQT=001; //Set acq time to 2 TAD
    ADCON2bits.ADCS=010; // Set conversion clock FOSC/32
    ADCON2bits.ADFM=1; //Format result to right justified
    ADCON0bits.ADON=1; // Turn on A/D
	ADCON0bits.CHS = 0b00000; // Set analog channel to AN0
    
    // Set up analog output on FVR
    ANSELAbits.ANSA2 = 1;
    //TRISAbits.TRISA2 = 0;
    VREFCON1bits.DACEN = 1;
    VREFCON1bits.DACOE = 1;
    
    //ANSELAbits.ANSA3 = 1;
    //TRISAbits.TRISA3 = 1;
    
	// Set up output
    TRISEbits.TRISE1 = 0; // Set bit 1 on port E to output
    LATEbits.LATE1 = 1;	  // latch value of output on port E bit 1 to 1
    
	// Set up MSSP1
	PIR1bits.SSP1IF = 0; // Clear SSP1 interrupt
	PIE1bits.SSP1IE = 0;	 // Enable SSP1 interrupts
	SSP1CON1bits.SSPM = 0x2; //Set SSP1 mode to master w/ FOSC/64
	SSP1CON1bits.SSPEN = 1; // Enable serial port
	
    // Set up RB0 interrupt as high priority interrupt
    TRISBbits.RB0 = 1; //set RB0 as Input
    INTCONbits.INT0E = 1; //enable Interrupt 0 (RB0 as interrupt)
    INTCON2bits.INTEDG0 = 0; //cause interrupt at falling edge
    INTCONbits.INT0F = 0; //reset interrupt flag
    
	// Misc
    Tick = 0;

	//INTCONbits.PEIE = 1 // Enable peripheral interrupts
	INTCONbits.GIE = 1;   // Enable global interrupts
    INTCONbits.PEIE = 1;
}

// High priority interrupt processing
void RTC_ISR (void)
{
    if (PIR1bits.TMR1IF) {          // If timer overflowed
        PIR1bits.TMR1IF = 0;        // Clear timer flag

		readAVin();
        Tick++;
        
        LATEbits.LATE1 = !LATEbits.LATE1;
        TMR1H  = T1H;
        TMR1L  = T1L;
    }
	else if (PIR1bits.SSP1IF) {			// SSP1 read
		if (SSP1STATbits.BF) 			// if SSP1 buffer full
			readSSP1();
		// Do some stuff
		PIR1bits.TMR1IF = 0;        // Clear timer flag
	}
	else if (INTCONbits.INT0IF) {	// RB0 button press
		ECGState = ~ECGState&0x01;		// Flip ECG state
		INTCONbits.INT0IF = 0;      // Clear interrupt flag
        LATEbits.LATE1 = !LATEbits.LATE1;
	}
}

void readAVin(void) {
    ADCON0bits.GO=1; // Start conversion
    while(ADCON0bits.GO==1){}; // Wait for finish
							   // look into possibility of doing this with interrupts
			// can do using PIR1bits.ADIF
			// and PIE1bits.ADIE
    volt= ADRESH; // get high byte of voltage
    volt= (volt<<8) | ADRESL; // combine with low byte of voltage
    if(volt==1023) //Fix roundoff error
        volt=1022;
    
    VREFCON2bits.DACR = volt>>5;
}

void writeVolt(void) {
    LCDGoto(0,1);
	count = 15;
//	while (volt!=0) {
//		LCDGoto(0,count);
//		LCDPutByte(volt%10);
//		volt = volt/10;
//		count--;
//	}
    sprintf(dispVolt,"%04d",volt*49/10); //Approximate conversion to 0-5V
    LCDPutChar(dispVolt[0]);
    LCDPutChar('.');
    LCDPutChar(dispVolt[1]);
    LCDPutChar(dispVolt[2]);
    LCDPutChar(dispVolt[3]);
    LCDPutChar('V');

    //LCDPutByte((volt&0xF0)>>4);
    //LCDPutByte((volt&0x0F));
}

void writeState(void) {
    LCDGoto(0,0);
    LCDWriteStr("ECG state is: ");
	LCDGoto(0,13);
    LCDPutByte(ECGState);
}

void readSSP1(void) {
	unsigned char data;
	data = SSP1BUF;
}

void writeTime(void)
{
    LCDGoto(0,0);
    sprintf(dispTick,"%d", Tick);
    dispTick[15] = '\0';
    LCDWriteStr(dispTick);
}
