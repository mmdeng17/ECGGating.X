/*
 * ECG Gating main code
 * 
 */

#include "Lcd.h"
#include "Fcns.h"
#include "OutFcns.h"
#include "QueueFcns.h"
#include <delays.h>
#include <p18f46k22.h>
#include <stdio.h>

#pragma config FOSC = INTIO7   // Internal OSC block, CLKOUT RA6/7
#pragma config PLLCFG = ON      // enable 4x pll
#pragma config WDTEN = OFF      // Watch Dog Timer disabled. SWDTEN no effect
#pragma config XINST = OFF      // Instruction set Extension and indexed Addressing mode disabled

//Define statements
#define T1H  0x80   // Timer is 0xFFBF for 512Hz
#define T1L  0x00	// 0x8000 for 1s 
				    // 0xFFFF-0x8000 = 512 * (0xFFFF-0xFFBF)
#define QUEUE_SIZE 10
#define PQUEUE_SIZE 10

// Variable definitions
unsigned int Tick = 0;
unsigned int currData = 0;
unsigned char count;
unsigned char curr;
unsigned char ECGState = 0;
unsigned char GateState = 0;
unsigned char SettState = 0;
unsigned int currVolt = 0;
unsigned int currImDelay  = 40;
unsigned int currImLength = 5;
unsigned int currImCount = 0;

// Queue definitions
float dataQueue[QUEUE_SIZE+3];
float derivQueue[QUEUE_SIZE+3];
float peakQueue[PQUEUE_SIZE+3];

// Function definitions
void RTC_ISR(void);
void SysInit(void);
void TriggerInit(void);
void High_Priority_ISR(void);

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
    TriggerInit();
    LCDClear();

    
    while(1)
    {  
 //       writeTime(0,currImCount);
//        writeStates(1,ECGState,GateState,SettState);
        
        Delay10KTCYx(10);
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
    ADCON2bits.ACQT=000; //Set acq time to 16 TAD (for 16MHz)
    ADCON2bits.ADCS=010; // Set conversion clock FOSC/32
    ADCON2bits.ADFM=1; //Format result to right justified
    ADCON0bits.ADON=1; // Turn on A/D
	ADCON0bits.CHS = 0b00000; // Set analog channel to AN0
    
    // Set up analog output on FVR
    ANSELAbits.ANSA2 = 1;
    VREFCON1bits.DACEN = 1;
    VREFCON1bits.DACOE = 1;
    
    //ANSELAbits.ANSA3 = 1;
    //TRISAbits.TRISA3 = 1;
    
	// Set up output
    TRISEbits.TRISE0 = 0; // Set bit 1 on port E to output
    LATEbits.LATE0 = 0;	  // latch value of output on port E bit 1 to 1
    TRISEbits.TRISE1 = 0; // Set bit 1 on port E to output
    LATEbits.LATE1 = 0;	  // latch value of output on port E bit 1 to 1
    
	// Set up MSSP1
    TRISAbits.TRISA5 = 1;
    ANSELAbits.ANSA5 = 0; // set up SS1
    PORTAbits.NOT_SS = 1; // do not select
    TRISCbits.TRISC3 = 0;
    ANSELCbits.ANSC3 = 0; // set up sck1 output
    TRISCbits.TRISC4 = 1;
    ANSELCbits.ANSC4 = 0; // set up sdi input
    TRISCbits.TRISC5 = 0;
    ANSELCbits.ANSC5 = 0; // set up sdo output
    
	PIR1bits.SSP1IF = 0; // Clear SSP1 interrupt
	PIE1bits.SSP1IE = 0;	 // Enable SSP1 interrupts
	SSP1CON1bits.SSPM = 0x0; //Set SSP1 mode to master w/ FOSC/4
	SSP1CON1bits.SSPEN = 1; // Enable serial port
	
    // Set up RB0 interrupt as high priority interrupt
    TRISBbits.RB4 = 1; //set RB0 as Input
    TRISBbits.RB5 = 1; //set RB0 as Input
    TRISBbits.RB6 = 1; //set RB0 as Input
    TRISBbits.RB7 = 1; //set RB0 as Input
    IOCBbits.IOCB4 = 1;
    IOCBbits.IOCB5 = 1;
    IOCBbits.IOCB6 = 1;
    IOCBbits.IOCB7 = 1;
    INTCONbits.RBIE = 1;
    
	// Misc
    Tick = 0;
    reset(dataQueue);
    reset(derivQueue);
    reset(peakQueue);
    
	//INTCONbits.PEIE = 1 // Enable peripheral interrupts
	INTCONbits.GIE = 1;   // Enable global interrupts
    INTCONbits.PEIE = 1;
}

void TriggerInit(void) {
    TMR3H  = 0x00;
    TMR3L  = 0x00;
    T3CON = 0x49;           // timer 3 is enabled, from system clock
    T3GCON = 0;                 // Timer 3 Gate function disabled
    PIR2bits.TMR3IF = 0;        // Clear any pending Timer 3 Interrupt indication
    PIE2bits.TMR3IE = 1;        // Enable Timer 3 Interrupt
}

// High priority interrupt processing
void RTC_ISR (void)
{
    if (PIR1bits.TMR1IF) {          // If timer overflowed
        INTCONbits.GIE = 0;   // disable global interrupts
        
        PIR1bits.TMR1IF = 0;        // Clear timer flag
        
		currVolt = readAVin();
        //DACVolt(currVolt);
        enqueue(dataQueue,currVolt);
        enqueue(derivQueue,getDeriv(dataQueue,dataQueue[QUEUE_SIZE+1]));
        
        if (isQRS(derivQueue,5000.0) && (Tick-(unsigned int)peek(peakQueue))>getQTDelay(peakQueue))  {
            if (Tick>0) {
                enqueue(peakQueue,(float) Tick);
                currImCount = 0;
            }
        }
        
        Tick++;
        
        TMR1H  = T1H;
        TMR1L  = T1L;
        
        INTCONbits.GIE = 1;   // Enable global interrupts
    }
    
    if (INTCONbits.RBIF) { 
        INTCONbits.GIE = 0;
        
        LATEbits.LATE1 = !LATEbits.LATE1;
                
        if (PORTBbits.RB4) {
            if (ECGState==1)
                ECGState = 0;
            else
                ECGState = 1;
        }

        if (PORTBbits.RB5) {
            if (GateState==1)
                GateState = 0;
            else
                GateState = 1;
        }

        if (PORTBbits.RB6) {
            if (SettState==1)
                SettState = 0;
            else
                SettState = 1;
        }

        INTCONbits.RBIF = 0;
        
        INTCONbits.GIE = 1;
    }
    
    if (PIR2bits.TMR3IF) {
        INTCONbits.GIE = 0;   // Enable global interrupts
        
        if (currImCount>=currImDelay && currImCount<=(currImDelay+currImLength))
            LATEbits.LATE0 = 1;
        else
            LATEbits.LATE0 = 0;
        
        currImCount++;
        //LATEbits.LATE0 = !LATEbits.LATE0;
        TMR3H  = 0x00;
        TMR3L  = 0x00;
        PIR2bits.TMR3IF = 0;        // Clear any pending Timer 3 Interrupt indication
    
        INTCONbits.GIE = 1;   // Enable global interrupts
    }
}
