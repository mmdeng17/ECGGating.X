/*
 * ECG Gating main code
 * 
 */

// Include header files
#include "Lcd.h"
#include "Fcns.h"
#include "OutFcns.h"
#include "QueueFcns.h"
#include <delays.h>
#include <p18f46k22.h>
#include <stdio.h>

// Config PIC
#pragma config FOSC = INTIO7   // Internal OSC block, CLKOUT RA6/7
#pragma config PLLCFG = ON      // enable 4x pll
#pragma config WDTEN = OFF      // Watch Dog Timer disabled. SWDTEN no effect
#pragma config XINST = OFF      // Instruction set Extension and indexed Addressing mode disabled

//Define statements
#define T1H  0x80   // Timer is 0xFFBF for 512Hz
#define T1L  0x00	// 0x8000 for 1s 
				    // 0xFFFF-0x8000 = 512 * (0xFFFF-0xFFBF)
#define T3H  0x00
#define T3L  0x00
#define QUEUE_SIZE 10
#define PQUEUE_SIZE 10

// Variable definitions
unsigned int Tick = 0;          // Count variable to store time
unsigned int prevPeak = 0;      // Timing of last peak

unsigned char ECGState = 0;     // State variable for ECG detection
unsigned char TrigState = 0;    // State variable for image trigger
unsigned char SettState = 0;    // State variable for controlling settings

unsigned int currQTDelay = 0;       // Delay before detecting another peak
unsigned int currImDelay  = 40;     // Delay before trigger
unsigned int currImLength = 10;     // Length of trigger
unsigned int currImCount = 0;       // Current timer count for trigger
unsigned int currThresh = 5000;     // Threshold for QRS detect

unsigned int currData = 0;      // Assorted temp variables
unsigned int currVolt = 0;

// Queue definitions
float dataQueue[QUEUE_SIZE];        // Queue to store raw ECG data
unsigned int dataInd = 0;
float derivQueue[QUEUE_SIZE];       // Queue to store ECG deriv
unsigned int derivInd = 0;
float peakQueue[PQUEUE_SIZE+5];     // Queue to store last peaks
unsigned int peakInd = 0;
float peakValQueue[QUEUE_SIZE];     // Queue to store deriv vals of last peaks
unsigned int pVInd = 0;


// Interrupt functions
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
        Delay10KTCYx(5);
    }
}


// Initialize necessary systems
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
    
    // Set up timer 1 for ECG data collection
    TMR1H  = T1H;
    TMR1L  = T1L;
    T1CON = 0x49;               // timer 1 is enabled, from system clock
    T1GCON = 0;                 // Timer 1 Gate function disabled
    RCONbits.IPEN=1;            // Allow interrupt priorities
    PIR1bits.TMR1IF = 0;        // Clear any pending Timer 1 Interrupt indication
    PIE1bits.TMR1IE = 1;        // Enable Timer 1 Interrupt
    
    //Set up A/D on AN0 for ECG data in
    ANSELAbits.ANSA0 = 1; // set bit 0 an channel A as analog
    TRISAbits.RA0 = 1; // Analog in
    ADCON2bits.ACQT=000; //Set acq time to 16 TAD (for 16MHz)
    ADCON2bits.ADCS=010; // Set conversion clock FOSC/32
    ADCON2bits.ADFM=1; //Format result to right justified
    ADCON0bits.ADON=1; // Turn on A/D
	ADCON0bits.CHS = 0b00000; // Set analog channel to AN0
    
    // Set up analog output on FVR RA2 to display ECG data out (optional)
    ANSELAbits.ANSA2 = 1;
    VREFCON1bits.DACEN = 1;
    VREFCON1bits.DACOE = 1;
    
	// Set up output
    //  E0 - Image trigger
    //  E1 - misc debugging
    //  B0 - Trigger State on
    //  B1 - ECG State on
    TRISEbits.TRISE0 = 0; // Set bit 1 on port E to output
    LATEbits.LATE0 = 0;	  // latch value of output on port E bit 1 to 1
    TRISEbits.TRISE1 = 0; // Set bit 1 on port E to output
    LATEbits.LATE1 = 0;	  // latch value of output on port E bit 1 to 1
    TRISBbits.TRISB0 = 0; 
    LATBbits.LATB0 = 0;	  
    TRISBbits.TRISB1 = 0; 
    LATBbits.LATB1 = 0;	 
	
    // Set up RB IOC interrupts for pushbuttons
    TRISBbits.RB4 = 1; //set RB0 as Input
    TRISBbits.RB5 = 1; //set RB0 as Input
    TRISBbits.RB6 = 1; //set RB0 as Input
    TRISBbits.RB7 = 1; //set RB0 as Input
    IOCBbits.IOCB4 = 1;
    IOCBbits.IOCB5 = 1;
    IOCBbits.IOCB6 = 1;
    IOCBbits.IOCB7 = 1;
    INTCONbits.RBIE = 1;
    
	// Misc data management
    Tick = 0;
    reset(dataQueue);
    reset(derivQueue);
    reset(peakQueue);
    reset(peakValQueue);
    
	INTCONbits.PEIE = 1;    // Enable peripheral interrupts
	INTCONbits.GIE = 1;     // Enable global interrupts
}

void TriggerInit(void) {
    TMR3H  = T3H;
    TMR3L  = T3L;
    T3CON = 0x49;               // timer 3 is enabled, from system clock
    T3GCON = 0;                 // Timer 3 Gate function disabled
    PIR2bits.TMR3IF = 0;        // Clear any pending Timer 3 Interrupt indication
}

// High priority interrupt processing
void RTC_ISR (void)
{
    if (PIR1bits.TMR1IF) {          // If timer1 overflowed
        INTCONbits.GIE = 0;         // disable global interrupts
        
        if (ECGState>=1) {          // If ECG collection on
            currVolt = readAVin();  // Read data in from A/D
            
            // Enqueue measurement in data/deriv queues
//            enqueueQ(dataQueue,&dataInd,currVolt);
            dataQueue[dataInd] = currVolt;
            dataInd = (dataInd+1)%QUEUE_SIZE;
//            enqueueQ(derivQueue,&derivInd,getDeriv(dataQueue,dataInd-1));
            derivQueue[derivInd] = getDeriv(dataQueue,dataInd-1);
            derivInd = (derivInd+1)%QUEUE_SIZE;
            
            // Fire trigger if:
            //  1) ECG deriv greater than threshold
            //  2) Timing of current peak is at least currQTDelay after last peak
            if (isQRS(derivQueue,currThresh,derivInd) && (Tick-(unsigned int)peekQ(peakQueue,peakInd))>currQTDelay)  {
                if (Tick>0) {
                    // Enqueue current deriv val in peakVal queue
                    peakValQueue[pVInd] = peekQ(derivQueue,derivInd);
                    pVInd = (pVInd+1)%QUEUE_SIZE;
                    
                    // Recalculate threshold using new peak value
                    currThresh = (int) getThresh(peakValQueue,pVInd);
                    
                    // Enqueue current peak time and record previous peak
                    prevPeak = peekQ(peakQueue,peakInd);
                    peakQueue[peakInd] = (float) Tick;
                    peakInd = (peakInd+1)%QUEUE_SIZE;
                    
                    // Calculate QT/Im delay using difference between peaks
//                    currData = getRRInterval(peakQueue,peakInd);
//                    currQTDelay = 0.75*currData;
                    currQTDelay = 0.6*(peekQ(peakQueue,peakInd)-prevPeak);
                    if (currQTDelay<=0)
                        currQTDelay = 25;
                    if (currQTDelay>=200)
                        currQTDelay = 200;
                    currImDelay  = 0.38*(peekQ(peakQueue,peakInd)-prevPeak)+6;
                    if (currImDelay<=0)
                        currImDelay = 20;
                    if (currImDelay>=150)
                        currImDelay = 150;
                    
                    // Restart count at 0 for imaging trigger
                    currImCount = 0;
                }
            }
            Tick++;
        }
        
        // Restart timer
        TMR1H  = T1H;
        TMR1L  = T1L;
        
        PIR1bits.TMR1IF = 0;  // Clear timer flag
        INTCONbits.GIE = 1;   // Enable global interrupts
    }
    
    if (INTCONbits.RBIF) {      // If pushbutton pressed
        INTCONbits.GIE = 0;     // Disable global interrupts
        
        // If both buttons pressed
        if (PORTBbits.RB7 && PORTBbits.RB6) {
            // Reset state and turn of indicators
            ECGState = 0;
            TrigState = 0;
            LATBbits.LATB1 = 0;
            LATBbits.LATB0 = 0;
            
            // Execute settings change menu
            SettingsMode(&currThresh, &currQTDelay, &currImDelay, &currImLength);
            Delay10KTCYx(100);
        }
        // If ECG State button pressed
        else if (PORTBbits.RB7) {
            if (ECGState>=1) {
                ECGState = 0;
                TrigState = 0;
                LATBbits.LATB1 = 0;
                LATBbits.LATB0 = 0;
            }
            else {
                ECGState = 1;
                LATBbits.LATB1 = 1;
            }
        }
        // If Trigger State button pressed
        else if (PORTBbits.RB6) {
            if (TrigState>=1) {
                TrigState = 0;
                PIE2bits.TMR3IE = 0;
                LATBbits.LATB0 = 0;
            }
            else {
                TrigState = 1;
                PIE2bits.TMR3IE = 1;
                LATBbits.LATB0 = 1;
            }
        }
        
        INTCONbits.RBIF = 0;    // Clear interrupt flag
        INTCONbits.GIE = 1;     // Re-enable global interrupts
    }
    
    if (PIR2bits.TMR3IF) {      // If Timer 3 overflow for trigger
        INTCONbits.GIE = 0;     // Enable global interrupts
        
        // If both ECG/Trigger modes on
        if (ECGState>=1 && TrigState>=1) {
            // Fire trigger after currImDelay for currImLength
            if (currImCount>=currImDelay && currImCount<=(currImDelay+currImLength))
                LATEbits.LATE0 = 1;
            else
                LATEbits.LATE0 = 0;

            currImCount++;
        }
        
        TMR3H  = T3H;   // Restart timer
        TMR3L  = T3L;
        
        PIR2bits.TMR3IF = 0;  // Clear timer interrupt flag
        INTCONbits.GIE = 1;   // Enable global interrupts
    }
}