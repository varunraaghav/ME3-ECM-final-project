
// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)



#include <xc.h>
#include <stdio.h>

#include "color.h"
#include "i2c.h"
#include "serial.h"
#include "interrupts.h"
#include "dc_motor.h"
#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


// GLOBAL VARIABLES INITIALISATION
signed int left_cal_count = 150;     // for left turn calibration
signed int right_cal_count = 150;  // for left turn calibration

int time = 0;     // timer 
unsigned int time_turns_taken[15] = {}; // saves the amount of ms in the fullSpeedAhead between turns
unsigned int turn_counter = 0;   //  number of turns taken
unsigned int turn_array_storage[15] = {};    // stores the exact turn , eg. turn90Left, turn90Right etc.

void main(void){
    
    //initialisation functions
    color_click_init();
        
    initUSART4(); 
    Interrupts_init();   
    initDCmotorsPWM();  
// ________________________________________________________________________________________________________________________________________


// LED AND BUTTON INITIALISATION _________________________________________________________________________________________
    
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin
    
    // Initialise Front LEDs on ColourClick 
    
    LATGbits.LATG1=1;   //set initial output state
    TRISGbits.TRISG1=0; //set TRIS value for pin (output)
    LATAbits.LATA4=1;   //set initial output state
    TRISAbits.TRISA4=0; //set TRIS value for pin (output)
    LATFbits.LATF7=1;   //set initial output state
    TRISFbits.TRISF7=0; //set TRIS value for pin (output)
    
    // Indicator LEDs on board
    TRISDbits.TRISD7 = 0; LATDbits.LATD7 = 0;
    TRISHbits.TRISH3 = 0; LATHbits.LATH3 = 0;
    
            LATDbits.LATD3=0;
        TRISDbits.TRISD3=0;
    
// _______________________________________________________________________________________________________________________________________
    
    // RGB STRUCT INITIALISATION
    struct RGB_val RGB_variable;
    RGB_variables_set_zero(&RGB_variable); // RGB variables init function
   
    
    //  Motor initialisation
//    ______________________________________________________________________________________________________________________________________
    
    struct DC_motor { //definition of DC_motor structure
	unsigned char power;         //motor power, out of 100
	unsigned char direction;     //motor direction, forward(1), reverse(0)
	unsigned char brakemode;	//slow or fast decay (braking or coasting)
	unsigned int PWMperiod; //base period of PWM cycle
	unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
	unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
    };
    
    struct DC_motor motorL, motorR; 		//declare two DC_motor structures 
    unsigned int PWMcycle = 99;

    motor_variables_init(&motorL, &motorR, PWMcycle); // motor variables init function
        
// COLOR CALIBRATION SEQUENCE _________________________________________________________________________________________________________________
    int calibration_checker = 0;
        while (calibration_checker==0){
            full_calibration_routine(&RGB_variable);  // color calibration
            LATHbits.LATH3 = 1;
            while (PORTFbits.RF3); 
            if (!PORTFbits.RF3){
                LATHbits.LATH3 = 0;
                calibration_checker = 1;
            }
        }
     
// MOTOR CALIBRATION SEQUENCE FOR L&R MOTORS _______________________________________________________________________
    
    int cal_left = 0;
    LATHbits.LATH3=1;
    while (cal_left==0){
        cal_left = calibration_sequence_left(&motorL, &motorR);   // left motor calibration
    }
    LATHbits.LATH3=0;
    int cal_right = 0;
    LATDbits.LATD7=1;
    while (cal_right==0){
        cal_right = calibration_sequence_right(&motorL, &motorR);   // right motor calibration
    }
    LATHbits.LATH3=0;
    LATDbits.LATD7=0;
    
    

// CALIBRATIONS COMPLETED________________________________________________________________________________________________________________    
    Timer0_init();   // initialise timer
    time = 0;
    
    while (1){

        fullSpeedAhead(&motorL, &motorR);  // constantly move forward
        
        int color_from_func = color_checker(&RGB_variable);   // constantly detect color. Could have multiple checks
        
        if (color_from_func != 8){                           
            maze_navigation_sequence(&motorL, &motorR, color_from_func);   // perform maze navigation as long as color is not white or 15 turns have been achieved
        } 
        else if ((color_from_func == 8 || (turn_counter=15))){
            
            return_home_sequence(&motorL, &motorR);  // perform return home sequence
            
        } else {
            __delay_ms(1000);   // if nothing is detected, ie ambient, delay for 1s and continue in while(1) loop
        }
        
        stop(&motorL, &motorR);  // temporary stop for smooth fullSpeedAhead forward function
        
    }
 
}