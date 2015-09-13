/*
 * ECG Gating main code
 * 
 */


#include "Lcd.h"
#include <delays.h>
#include <p18f46k22.h>

#pragma config FOSC = INTIO67   // Internal OSC block, Port Function on RA6/7
#pragma config WDTEN = OFF      // Watch Dog Timer disabled. SWDTEN no effect
#pragma config XINST = OFF      // Instruction set Extension and indexed Addressing mode disabled

//Define statements
#define T1H  0x80   // Timer is 0xFFBF for 512Hz
#define T1L  0x00	// 0x8000 for 1s 
				    // 0xFFFF-0x8000 = 512 * (0xFFFF-0xFFBF)
// Timer 1 clock source is crystal oscillator on T1OS1/T1OS0, 1:1,
// Dedicated enabled, Do Not Synch, Enable Timer1
#define Timer1  0x89

//Variable definitions
unsigned char Sec; //Variables for keeping time
unsigned char Min;
unsigned char Hour;
unsigned char ECGState = 0;
unsigned char GateState = 0;
unsigned int volt =0;

//Function definitions
void RTC_ISR(void);
void SysInit(void);
void High_Priority_ISR(void);
void RTCIncSec(void);
void RTCIncMin(void);
void RTCIncHour(void);
void WriteTime(void);
unsigned char isLeftBtnPressed(void);
unsigned char isRightBtnPressed(void);

void readAVin(void);
void writeVolt(void);
void readSSP1(void);


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
    char str[4];
    //Initialize
    SysInit();
    LCDClear();
    //Loop (except while servicing interrupts)
    while(1)
    {   
        WriteTime();
        writeVolt();
        
        Delay10KTCYx(10);
    };
}

//Initialize necessary systems
void SysInit(void)
{
    OSCCON=0b01010110; //4 MHz internal oscillator

    //Set up buttons (B as interruptible input)
    ANSELBbits.ANSB0=0; //Digital
    TRISAbits.RA4=1; //Input
    TRISBbits.RB0=1; //Input
    INT0E = 1;
    INTCON2bits.INTEDG0 = 1; //Interrupt on rising edge of RB0/


    //Set up LCD
    ANSELD = 0x00;
    TRISD = 0x00; //Digital out

    LCDInit(); //Start LCD
    
    //Set up timer
    TMR1H  = T1H;
    TMR1L  = T1L;
    T1CON  = Timer1;		// Configure Timer 1
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
    
	// Set up output
    TRISEbits.TRISE1 = 0; // Set bit 1 on port E to output
    LATEbits.LATE1 = 1;	  // latch value of output on port E bit 1 to 1
    
	// Set up MSSP1
	PIR1bits.SSP1IF = 0; // Clear SSP1 interrupt
	PIE1.SSP1IE = 0;	 // Enable SSP1 interrupts
	SSP1CON1bits.SSPM = 0x2; //Set SSP1 mode to master w/ FOSC/64
	SSP1CON1bits.SSPEN = 1; // Enable serial port
	

	// Misc
    Sec = 0;
    Hour = 0;
    Min = 0;

	INTCONbits.PEIE = 1 // Enable peripheral interrupts
	INTCONbits.GIE=1;   // Enable global interrupts
}

// High priority interrupt processing
void RTC_ISR (void)
{
    if (PIR1bits.TMR1IF) {          // If timer overflowed
        PIR1bits.TMR1IF = 0;        // Clear timer flag
        INTCONbits.INT0IF = 0;      // Clear interrupt flag

		readAVin();
        RTCIncSec();
    
    
        TMR1H  = T1H;
        TMR1L  = T1L;
        
    }
	else if (PIR1bits.SSP1IF) {			// SSP1 read
		if (SSP1STAT.BF) 			// if SSP1 buffer full
			readSSP1();
		// Do some stuff
		PIR1bits.TMR1IF = 0;        // Clear timer flag
        INTCONbits.INT0IF = 0;      // Clear interrupt flag	
	}
	else {					// RB0 button press
		ECGState = -ECGState+1;		// Flip ECG state
		INTCONbits.INT0IF = 0;      // Clear interrupt flag
	}
}

//Return 1 if RA4 button has been pressed, 0 otherwise
//The button is accessed using PORTAbits.RA4, a 0 means the button is pressed
unsigned char isLeftBtnPressed(void)
{
    unsigned char i = 0;
    while (PORTAbits.RA4==0)
        i++;
    
    return (i>5)?1:0;
}

//Return 1 if RB0 button has been pressed, 0 otherwise
//The button is accessed using PORTBbits.RB0, a 0 means the button is pressed
unsigned char isRightBtnPressed(void)
{
    unsigned char i = 0;
    while (PORTBbits.RB0==0)
        i++;
    
    return (i>5)?1:0;
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
}

void writeVolt(void) {
    LCDPutByte((volt&0xF0)>>4);
    LCDPutByte((volt&0x0F));
}

void writeState(void) {
	LCDWriteStr(sprintf("ECG state is: %i",ECGState));
}

void readSSP1(void) {
	unsigned char data;
	data = SSP1BUF;
}

//Increment Sec variable, correctly roll over Sec, Min and Hour
void RTCIncSec(void)
{
    Sec++;
    if(Sec==60)
    {
        Sec=0;
        RTCIncMin();
    }
}

//Increment Min variable, correctly roll over Min and Hour
void RTCIncMin(void)
{
    Min++;
    if(Min==60)
    {
        Min=0;
        RTCIncHour();
    }
}

//Increment Hour variable, correctly roll over Hour
void RTCIncHour(void)
{
    Hour++;
    if(Hour==24)
        Hour=0;
}

//Display time on LCD as HH:MM:SS at row and column 0
//The hour is in variable Hour
//The minute is in variable Min
//The second is in variable Sec
void WriteTime(void)
{
    LCDGoto(0,0);
    LCDPutByte(Hour);
    LCDPutChar(':');
    LCDPutByte(Min);
    LCDPutChar(':');
    LCDPutByte(Sec);
}
