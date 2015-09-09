/*
 * BME 464 - Lab B
 *
 * Run a real-time clock and display the time on the LCD
 * Template: Nick Bottenus - 9/3/12
 * Student: Michael Deng, Ruth Godbey, Mitchel Zhang
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
#define T1L  0x00
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
unsigned char isLeftBtnPressed(void);
unsigned char isRightBtnPressed(void);
void readAVin(void);
void writeVolt(void);
void RTCIncSec(void);
void RTCIncMin(void);
void RTCIncHour(void);
void WriteTime(void);

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

    //Set up buttons
    ANSELBbits.ANSB0=0; //Digital
    TRISAbits.RA4=1; //Input
    TRISBbits.RB0=1; //Input

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
    INTCONbits.GIE=1;           // Enable interrupts

    //Set up A/D on AN0
    TRISA = 0x00;
    ANSELAbits.ANSA0 = 1;
    TRISAbits.RA0 = 1; //Analog in
    ADCON2bits.ACQT=001; //2 TAD
    ADCON2bits.ADCS=010; //FOSC/32
    ADCON2bits.ADFM=1; //Left justified
    ADCON0bits.ADON=1; //Turn on A/D
    
    TRISEbits.TRISE1 = 0;
    LATEbits.LATE1 = 1;
    
    Sec = 0;
    Hour = 0;
    Min = 0;
}

//Called every second by the interrupt
void RTC_ISR (void)
{
    if (PIR1bits.TMR1IF)            // If timer overflowed
    {
        readAVin();
        
        RTCIncSec();
    
    
        TMR1H  = T1H;
        TMR1L  = T1L;
        PIR1bits.TMR1IF = 0;        // Clear timer flag
        INTCONbits.INT0IF = 0;      // Clear interrupt flag
    }
}

//Return 1 if RA4 button has been pressed, 0 otherwise
//
//Be sure to include some form of debouncing
//
//The button is accessed using PORTAbits.RA4, a 0 means the button is pressed
unsigned char isLeftBtnPressed(void)
{
    unsigned char i = 0;
    while (PORTAbits.RA4==0)
        i++;
    
    return (i>5)?1:0;
}

//Return 1 if RB0 button has been pressed, 0 otherwise
//
//Be sure to include some form of debouncing
//
//
//The button is accessed using PORTBbits.RB0, a 0 means the button is pressed
unsigned char isRightBtnPressed(void)
{
    unsigned char i = 0;
    while (PORTBbits.RB0==0)
        i++;
    
    return (i>5)?1:0;
}

void readAVin(void) {
    ADCON0bits.GO=1; //Start conversion
    while(ADCON0bits.GO==1){}; //Wait for finish
    volt=ADRESH;
    volt=(volt<<8) | ADRESL; //Math needs to be done in the int variable
    if(volt==1023) //Fix roundoff error
        volt=1022;
}

void writeVolt(void) {
    LCDGoto(0,1);
    LCDPutByte((volt&0xF0)>>4);
    LCDPutByte((volt&0x0F));
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