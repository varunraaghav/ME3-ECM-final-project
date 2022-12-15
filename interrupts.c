#include <xc.h>
#include "interrupts.h"
#include "serial.h"

extern int time;

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    INTCONbits.PEIE=1; 	//enable peripheral interrupts
    PIE4bits.RC4IE=1;	//receive interrupt
    PIE4bits.TX4IE=0;	//transmit interrupt (only turn on when you have more than one byte to send)

    PIE0bits.TMR0IE = 1; //make use of timer interrupt 
    IPR0bits.TMR0IP=1;  // im assigning tmr0 as high priority input

    INTCONbits.GIE=1; 	//enable interrupts globally 
    
}

void Timer0_init(void) {
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b0000; // 1:256 //// multiply by 2^16 to get time taken to fill out 16 bit clock
    T0CON0bits.T016BIT=1;	//16 bit mode	
    
    // it's a good idea to initialise the timer registers so we know we are at 0
    TMR0H=0b11000001;    //write High reg first, update happens when low reg is written to
    TMR0L=0b10000000;    // starting off at 49536 to get exactly 1 second
    T0CON0bits.T0EN=1;	//start the timer
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR(){   //add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...    
    if (PIR4bits.RC4IF == 1){ 
        putCharToRxBuf(RC4REG);   
    }
    if (PIR4bits.TX4IF  == 1 && PIE4bits.TX4IE==1){ // TX4REG is empty
        if (isDataInTxBuf()==1 ) {TX4REG = getCharFromTxBuf();} // Set next char 
        else {PIE4bits.TX4IE=0;              
                //  when transmitter interrupt is triggered, data is sent to computer via eusart
        }
    }
    
        if(PIR0bits.TMR0IF){ 					//check the interrupt source: tmr0
        
            time = time + 1;

            PIR0bits.TMR0IF=0; 						//clear the interrupt flag!
            TMR0H=0b11000001;    //write High reg first, update happens when low reg is written to
            TMR0L=0b10000000;    // starting off at 49536 to get exactly 1 second
        
    }

}