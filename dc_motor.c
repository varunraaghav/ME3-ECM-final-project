#include <xc.h>
#include "dc_motor.h"
#include "i2c.h"

// EXTERN GLOBAL VARIABLES_____________________________________________________________________________________________________________________________
extern int time;
extern unsigned int turn_counter;
extern unsigned int time_turns_taken[15];
extern unsigned int turn_array_storage[15];
extern int left_cal_count;
extern int right_cal_count;


// function initialise T2 and CCP for DC motor control__________________________________________________________________________________________________
void initDCmotorsPWM(){
    //initialise your TRIS and LAT registers for PWM  
    TRISEbits.TRISE2=0; //output on RE2
    TRISCbits.TRISC7=0; //output on RC7
    TRISGbits.TRISG6=0; //output on RG6
    TRISEbits.TRISE4=0; //output on RE4
    LATEbits.LATE2=0; // 0 output on RE2
    LATCbits.LATC7=0; // 0 output on RC7
    LATGbits.LATG6=0; // 0 output on RG6
    LATEbits.LATE4=0; // 0 output on RE4
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS=0b100; // 1 : 16 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/16 -1 = 99
    T2PR=99; //Period reg 10kHz base period       // timer period register
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
}
// INITIALISE MOTOR STRUCT ______________________________________________________________________________________________________________________________
void motor_variables_init(struct DC_motor *mL, struct DC_motor *mR, int PWMcycle){
    
    mL->power=0; 						//zero power to start
    mL->direction=0; 					//set default motor direction
    mL->brakemode=1;						// brake mode (slow decay)
    mL->posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
    mL->negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    mL->PWMperiod=PWMcycle; 			//store PWMperiod for motor (value of T2PR in this case)

    //same for motorR but different CCP registers
    
    mR->power=0; 						//zero power to start
    mR->direction=0; 					//set default motor direction
    mR->brakemode=1;						// brake mode (slow decay)
    mR->posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP1 duty high byte
    mR->negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP2 duty high byte
    mR->PWMperiod=PWMcycle; 			//store PWMperiod for motor (value of T2PR in this case)
}



// function to set CCP PWM output from the values in the motor structure _____________________________________________________________________
void setMotorPWM(DC_motor *m){  // from lab 6
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time
		negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

//function to stop the robot gradually___________________________ from lab 6 
void stop(DC_motor *mL, DC_motor *mR)
{
    while ((mL->power)>0 && (mR->power)>0){
        if ((mL->power) > 0){
            (mL->power) -= 1;
        }
        if ((mR->power) > 0){
            (mR->power) -= 1;
        }
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    } 
}

//function to make the robot go straight : FORWARD MOTION ______________________________________________________________________________________________
void fullSpeedAhead(DC_motor *mL, DC_motor *mR)
{
    (mL->direction) = 1;
    (mR->direction) = 1;
    for (unsigned int i = 0; i < 35; ++i){  // 30 = PWM GRADUAL INCREASE
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(5);
    }
}

//function to make the robot go in reverse : BACKWARD MOTION ______________________________________________________________________________________________
void slowSpeedReverse(DC_motor *mL, DC_motor *mR)
{
    (mL->direction) = 0;
    (mR->direction) = 0;
    for (unsigned int i = 0; i < 20; ++i){  // 20 = PWM
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(5);
    }
}

// 90 Degree left turn , BASED ON CALIBRATION _________________________________________________________________________________________
void turn90Left(struct DC_motor *mL, struct DC_motor *mR)
{
    stop(mL, mR);
    __delay_ms(200);//  
    
    (mL->direction) = 0; //0 means reverse direction FOR LEFT SIDE OF BUGGY
    (mR->direction) = 1;
    for (unsigned int i = 0; i < 80; ++i){ // 80 = POWER
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
    
    for (unsigned int j = 0; j<left_cal_count; ++j){ // adjust delay called 'left_cal_count' that is calibrated for each turn
        __delay_ms(1); //Adjust Timing
    }
    stop(mL, mR);
    __delay_ms(50);
    
}

// 135 Degree left turn , BASED ON CALIBRATION of 90 degree turn ___________________________________________________________________________________
void turn135Left(struct DC_motor *mL, struct DC_motor *mR)    
{
    stop(mL, mR);
    __delay_ms(200);//  
    
    (mL->direction) = 0; //0 means reverse direction
    (mR->direction) = 1;
    for (unsigned int i = 0; i < 80; ++i){ // 80 = power
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
    
    int big_left_cal_count = 1.5*left_cal_count;   // calibration for 135 turn based on 1.5 * calibration for 90degree turn
    for (unsigned int j = 0; j<big_left_cal_count; ++j){
        __delay_ms(1); //Adjust Timing
    }
    stop(mL, mR);
    __delay_ms(50);
    
}

// 180 Degree left turn , BASED ON CALIBRATION of 90 degree turn ___________________________________________________________________________________
void turn180Left(struct DC_motor *mL, struct DC_motor *mR)
{
    
    stop(mL, mR);
    __delay_ms(200);//  
    
    (mL->direction) = 0; //0 means reverse direction
    (mR->direction) = 1;
    for (unsigned int i = 0; i < 78; ++i){   // 78 = power
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
    
    int huge_left_cal_count = 2*left_cal_count; // calibration for 180 turn based on 2 * calibration for 90degree turn
    for (unsigned int j = 0; j<huge_left_cal_count; ++j){
        __delay_ms(1); //Adjust Timing
    }
    stop(mL, mR);
    __delay_ms(50);
    
}


// 90 Degree right turn , BASED ON CALIBRATION _______________________________________________________________________________________________
void turn90Right(struct DC_motor *mL, struct DC_motor *mR)
{
    stop(mL, mR);
    __delay_ms(200);//      
    
    (mL->direction) = 1; //0 means reverse direction
    (mR->direction) = 0;
    for (unsigned int i = 0; i < 75; ++i){ //75 = power 
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
    
    for (unsigned int j = 0; j<right_cal_count; ++j){  // adjust delay called 'right_cal_count' that is calibrated for each turn
        __delay_ms(1); //Adjust Timing
    }
    stop(mL, mR);
    __delay_ms(50);
    
}
// 135 Degree right turn , BASED ON CALIBRATION of 90 degree turn ___________________________________________________________________________________

void turn135Right(struct DC_motor *mL, struct DC_motor *mR) // 
{
    stop(mL, mR);
    __delay_ms(200);//  

    slowSpeedReverse(mL, mR);
    __delay_ms(200);
    stop(mL, mR);
    __delay_ms(300);    
    
    (mL->direction) = 1; //0 means reverse direction
    (mR->direction) = 0;
    for (unsigned int i = 0; i < 75; ++i){ 
        (mL->power) += 1;
        (mR->power) += 1;
        setMotorPWM(mL);
        setMotorPWM(mR);
        __delay_us(50);
    }
    
    int big_right_cal_count = 1.5*right_cal_count; // based on 90 degree calibration
    for (unsigned int j = 0; j<big_right_cal_count; ++j){
        __delay_ms(1); //Adjust Timing
    }
    stop(mL, mR);
    __delay_ms(50);
}


// MOTOR CALIBRATION SEQUENCE _______________________________________________________________________________________________________________________
unsigned int calibration_sequence_left(struct DC_motor *mL, struct DC_motor *mR){ // left turn initial calibration sequence
    int cal_left = 0;
    stop(mL,mR);
    __delay_ms(200);
    fullSpeedAhead(mL, mR);
    __delay_ms(200);
    stop(mL,mR);
    __delay_ms(200);                            // INITIAL MOTIONS TO DETECT DEVIATION FRO 90 DEGREE 
    turn90Left(mL,mR);
    __delay_ms(100);
    stop(mL,mR);
    __delay_ms(200);
    fullSpeedAhead(mL, mR);
    __delay_ms(200);
    stop(mL,mR);
    __delay_ms(200);
    
    while (PORTFbits.RF2 && PORTFbits.RF3); //empty while loop (wait for button press)
    if (!PORTFbits.RF2)  {
        left_cal_count = left_cal_count + 6;     // keep pressing RF2 until you see buggy turn exactly 90 degrees, then press RF3 button to move onto next stage
    }
    if (!PORTFbits.RF3){
        cal_left = 1;
    }
    return cal_left;
    
}

unsigned int calibration_sequence_right(struct DC_motor *mL, struct DC_motor *mR){
    int cal_right = 0;
    stop(mL,mR);
    __delay_ms(200);
    fullSpeedAhead(mL, mR);
    __delay_ms(200);
    stop(mL,mR);
    __delay_ms(200);
    turn90Right(mL,mR);
    __delay_ms(100);
    stop(mL,mR);
    __delay_ms(200);
    fullSpeedAhead(mL, mR);
    __delay_ms(200);
    stop(mL,mR);
    __delay_ms(200);
    
    while (PORTFbits.RF2 && PORTFbits.RF3); //empty while loop (wait for button press)
    if (!PORTFbits.RF2)  {
        right_cal_count = right_cal_count + 6;   // keep pressing RF2 until you see buggy turn exactly 90 degrees, then press RF3 button to move onto next stage
    }
    if (!PORTFbits.RF3){
        cal_right = 1;
    }
    return cal_right;
    
}

// MAIN  MAZE  NAVIGATION  SEQUENCE ________________________________________________________________________________________________________________
void maze_navigation_sequence(struct DC_motor *mL, struct DC_motor *mR, int color_from_func){  
    if (color_from_func == 2){   //2 = color green
        time_turns_taken[turn_counter] = time;

        turn_array_storage[turn_counter] = color_from_func;
        turn_counter = turn_counter + 1;
        stop(mL,mR);
        __delay_ms(100);
        fullSpeedAhead(mL, mR);   // use wall for realignment
        __delay_ms(800);
        stop(mL,mR);
        __delay_ms(100);
        slowSpeedReverse(mL, mR);
        __delay_ms(300);
        turn90Left(mL, mR);  // turn LEFT for green
        __delay_ms(1000);

        time = 0;            
        
        LATDbits.LATD3=1;
        TRISDbits.TRISD3=1;
        
        fullSpeedAhead(mL, mR);
        
    }
    
    
    else if (color_from_func == 1){ // 1 = red
     
        time_turns_taken[turn_counter] = time;   // everytime a colour is detected, the time taken from TIMER is stored into the array, to be used for return home sequence 
        turn_array_storage[turn_counter] = color_from_func;   // opposite of turn right is turn left, therefore opposite 
        turn_counter = turn_counter + 1;  // adds a turn, to keep track of for return home sequence 
        
        stop(mL,mR);
        __delay_ms(100);        
        fullSpeedAhead(mL, mR);   // use wall for realignment
        __delay_ms(800);
        stop(mL,mR);
        __delay_ms(100);
        slowSpeedReverse(mL, mR);  // reverse back 
        __delay_ms(300);
        stop(mL,mR);
        __delay_ms(100);
        turn90Right(mL, mR);  // turn right for ref
        __delay_ms(1000);
        stop(mL, mR);
        __delay_ms(200);
        

        time = 0;            // reset time
        fullSpeedAhead(mL, mR);  // continually keep running fullSpeedAhead function
    }



    else if (color_from_func == 3){  // 3 = blue
        
        time_turns_taken[turn_counter] = time;

        turn_array_storage[turn_counter] = color_from_func;
        turn_counter = turn_counter + 1;

        stop(mL,mR);
        __delay_ms(100);        
        fullSpeedAhead(mL, mR);   // use wall for realignment
        __delay_ms(800);
        stop(mL,mR);
        __delay_ms(100);        
        slowSpeedReverse(mL, mR);
        __delay_ms(400);        
        turn90Left(mL, mR);
        __delay_ms(500);
        stop(mL, mR);
        __delay_ms(300);
        turn90Left(mL, mR);
        __delay_ms(1000);
        
//        turn180Left(mL, mR);   // u-turn 180 degree for blue
//        __delay_ms(1000);
      
        time = 0;            
        fullSpeedAhead(mL, mR);

    }
    else if (color_from_func == 4){ // 4 = yellow
            
        time_turns_taken[turn_counter] = time;

        turn_array_storage[turn_counter] = color_from_func;
        turn_counter = turn_counter + 1;
        
        stop(mL,mR);
        __delay_ms(100);        
        fullSpeedAhead(mL, mR);   // use wall for realignment
        __delay_ms(800);       
        stop(mL, mR);
        __delay_ms(200);
        slowSpeedReverse(mL, mR);  // reverse ~ 1 square
        __delay_ms(1000);     // CALIBRATION FOR 1 SQUARE --> 800ms delay
        __delay_ms(1000);  
        __delay_ms(1000);  
        stop(mL, mR);
        __delay_ms(200);
        
        turn90Right(mL, mR);  // turn 90 degree right for yellow
        __delay_ms(1000);
      
        time = 0;            
        fullSpeedAhead(mL, mR);
        
        }
    
        else if (color_from_func == 5){  // 5 = pink

        time_turns_taken[turn_counter] = time;

        turn_array_storage[turn_counter] = color_from_func;   // separate pink reverse 
        turn_counter = turn_counter + 1;
        
        stop(mL,mR);
        __delay_ms(100);        
        fullSpeedAhead(mL, mR);   // use wall for realignment
        __delay_ms(800);       
        stop(mL, mR);
        __delay_ms(200);
        slowSpeedReverse(mL, mR);  // reverse ~ 1 square
        __delay_ms(1000);     // CALIBRATION FOR 1 SQUARE --> 800ms delay
        __delay_ms(1000);  
        __delay_ms(1000);  
        stop(mL, mR);
        __delay_ms(200);
        
        turn90Left(mL, mR);  // turn 90 degree left for pink
        __delay_ms(1000);
      
        time = 0;            
        fullSpeedAhead(mL, mR);
        
    }

        else if (color_from_func == 6){ // 6 = orange
        
        time_turns_taken[turn_counter] = time;

        turn_array_storage[turn_counter] = color_from_func;
        turn_counter = turn_counter + 1;

        stop(mL,mR);
        __delay_ms(100);        
        fullSpeedAhead(mL, mR);   // use wall for realignment
        __delay_ms(800);       
        stop(mL, mR);
        __delay_ms(200);        
        slowSpeedReverse(mL, mR);
        __delay_ms(200);        
        turn135Right(mL, mR);  // 135 degree right turn for orange
        __delay_ms(1000);
      
        time = 0;            
        fullSpeedAhead(mL, mR);
    }

        else if (color_from_func == 7){ // 7 = light blue

        time_turns_taken[turn_counter] = time;

        turn_array_storage[turn_counter] = color_from_func;
        turn_counter = turn_counter + 1;

        stop(mL,mR);
        __delay_ms(100);        
        fullSpeedAhead(mL, mR);   // use wall for realignment
        __delay_ms(800);       
        stop(mL, mR);
        __delay_ms(200);        
        slowSpeedReverse(mL, mR);
        __delay_ms(200);        
        turn135Left(mL, mR); // 135 degree left turn for light blue
        __delay_ms(1000);
      
        time = 0;            
        fullSpeedAhead(mL, mR);
    }
    
    // 8 = white, separate function
    // 9 = black, nothing required
    // 10 = ambient, while(1) loop in main.c is re executed
}
// ____________________________________________________________________________________________________________________________________________________________

// RETURN HOME SEQUENCE IF WHITE IS DETECTED
void return_home_sequence(struct DC_motor *mL, struct DC_motor *mR){
        time_turns_taken[turn_counter] = time; // store final fullSpeadAhead time
        time = 0;
            
        turn_array_storage[turn_counter] = 8; //stored 8 = white function 
        turn_counter = turn_counter + 1;

        LATGbits.LATG1=0;   // TURN OFF BUGGY LED LIGHTS
        LATAbits.LATA4=0;   
        LATFbits.LATF7=0;   
        
        fullSpeedAhead(mL, mR);   // use wall for realignment
        __delay_ms(500);       
        stop(mL, mR);
        __delay_ms(200);        
        slowSpeedReverse(mL, mR);
        __delay_ms(400);             
        stop(mL, mR);
        __delay_ms(200);
        
        turn90Left(mL, mR);
        __delay_ms(500);
        stop(mL, mR);
        __delay_ms(400);
        turn90Left(mL, mR);
        __delay_ms(500);
        
        stop(mL, mR);
        __delay_ms(400);
        
        signed int i = turn_counter;  //  PERFORM return home sequence inside while loop where i : turn_counter --> 0. therefore array parsed from last index value
        while (i >= 0){  // until i = 0

            stop(mL, mR);
            __delay_ms(200);            

            unsigned int temp_turn = turn_array_storage[i]; // temporary variables for storage
            unsigned int temp_time = time_turns_taken[i];

            if (temp_turn==8){  //checking for white function, will always be the first turn performed in getting_home_sequence as white is last card
                
                fullSpeedAhead(mL, mR);  
                for (unsigned int j = 0; j<temp_time; ++j){  // perform fullSpeedAhead function for the given time from array, --> temp_time
                    __delay_us(650); // in the forward maze_sequence, there is a delay as it is constantly performing color_checker function. So time is not consistent with forward and return sequence. so the delay for the returning home is scaled by a factor of 0.5 --> tested empirically
                } //  time is store in ms, so technically should be __delay_ms(1) until temp time. however , 0.5ms used to scale to account for intrinsic delay in the forward maze function due to colorchecker continually beong performed
                stop(mL, mR);
                __delay_ms(200);       
                
            } else if (temp_turn==1){ 
                turn90Left(mL, mR);  // if red was stored, opposite = green = turn90 Left
                fullSpeedAhead(mL, mR);  
                for (unsigned int j = 0; j<temp_time; ++j){
                    __delay_us(650); // in the forward maze_sequence, there is a delay as it is constantly performing color_checker function. So time is not consistent with forward and return sequence. so the delay for the returning home is scaled by a factor of 0.5 --> tested empirically
                } //  time is store in ms, so technically should be __delay_ms(1) until temp time. however , 0.5ms used to scale to account for intrinsic delay in the forward maze function due to colorchecker continually beong performed
                
            } else if (temp_turn==2){
                turn90Left(mL, mR);  // if green was stored in sequence order, opposite = red = turn90Right  
                fullSpeedAhead(mL, mR);  
                for (unsigned int j = 0; j<temp_time; ++j){
                    __delay_us(650); // in the forward maze_sequence, there is a delay as it is constantly performing color_checker function. So time is not consistent with forward and return sequence. so the delay for the returning home is scaled by a factor of 0.5 --> tested empirically
                } //  time is store in ms, so technically should be __delay_ms(1) until temp time. however , 0.5ms used to scale to account for intrinsic delay in the forward maze function due to colorchecker continually beong performed
            
            } else if (temp_turn==3){ // if blue was stored, opposite is same function, turn 180 deg
//                turn180Left(mL, mR); 
                turn90Left(mL, mR);
                stop(mL, mR);
                __delay_ms(400);
                turn90Left(mL, mR);
                stop(mL, mR);
                __delay_ms(200);
                
                fullSpeedAhead(mL, mR);  
                for (unsigned int j = 0; j<temp_time; ++j){
                    __delay_us(575); // in the forward maze_sequence, there is a delay as it is constantly performing color_checker function. So time is not consistent with forward and return sequence. so the delay for the returning home is scaled by a factor of 0.5 --> tested empirically
                } //  time is store in ms, so technically should be __delay_ms(1) until temp time. however , 0.5ms used to scale to account for intrinsic delay in the forward maze function due to colorchecker continually beong performed
            
            } else if (temp_turn==6){ // if orange was stored, opposite = light blue  = turn135left
                turn135Left(mL, mR);
                fullSpeedAhead(mL, mR);  
                for (unsigned int j = 0; j<temp_time; ++j){
                    __delay_us(575); // in the forward maze_sequence, there is a delay as it is constantly performing color_checker function. So time is not consistent with forward and return sequence. so the delay for the returning home is scaled by a factor of 0.5 --> tested empirically
                } //  time is store in ms, so technically should be __delay_ms(1) until temp time. however , 0.5ms used to scale to account for intrinsic delay in the forward maze function due to colorchecker continually beong performed

            } else if (temp_turn ==7){ //if lightblue was stored, opposite = orange = turn135Right
                turn135Right(mL, mR);
                fullSpeedAhead(mL, mR);  
                for (unsigned int j = 0; j<temp_time; ++j){
                    __delay_us(575); // in the forward maze_sequence, there is a delay as it is constantly performing color_checker function. So time is not consistent with forward and return sequence. so the delay for the returning home is scaled by a factor of 0.5 --> tested empirically
                } //  time is store in ms, so technically should be __delay_ms(1) until temp time. however , 0.5ms used to scale to account for intrinsic delay in the forward maze function due to colorchecker continually beong performed

            } else if (temp_turn ==4){  // custom reverse sequence for yellow 
                turn90Left(mL, mR);
                slowSpeedReverse(mL, mR);  // reverse ~ 1 square
                __delay_ms(1000);     // CALIBRATION FOR 1 SQUARE --> 800ms delay
                __delay_ms(1000);  
                __delay_ms(1000);
                stop(mL, mR);
                fullSpeedAhead(mL, mR);  
                for (unsigned int j = 0; j<temp_time; ++j){
                    __delay_us(575); // in the forward maze_sequence, there is a delay as it is constantly performing color_checker function. So time is not consistent with forward and return sequence. so the delay for the returning home is scaled by a factor of 0.5 --> tested empirically
                } //  time is store in ms, so technically should be __delay_ms(1) until temp time. however , 0.5ms used to scale to account for intrinsic delay in the forward maze function due to colorchecker continually beong performed
                
            } else if (temp_turn ==5){  // custom reverse pink sequence
                turn90Right(mL, mR);
                slowSpeedReverse(mL, mR);  // reverse ~ 1 square
                __delay_ms(1000);     // CALIBRATION FOR 1 SQUARE --> 800ms delay
                __delay_ms(1000);  
                __delay_ms(1000);
                stop(mL, mR);
                fullSpeedAhead(mL, mR);  
                for (unsigned int j = 0; j<temp_time; ++j){
                    __delay_us(575); // in the forward maze_sequence, there is a delay as it is constantly performing color_checker function. So time is not consistent with forward and return sequence. so the delay for the returning home is scaled by a factor of 0.5 --> tested empirically
                } //  time is store in ms, so technically should be __delay_ms(1) until temp time. however , 0.5ms used to scale to account for intrinsic delay in the forward maze function due to colorchecker continually beong performed

            } 
            
            i = i - 1;   // reduce i value by 1 each time to iterate from turn_counter --> 0
            
    
        }
        
        while (1){                          // once all reverse iterations are done, buggy stops
            stop(mL, mR);
            LATDbits.LATD7 = 1;
            LATHbits.LATH3 = 1;
        }
        
}
// ________________________________________________________________________________________________________________________


