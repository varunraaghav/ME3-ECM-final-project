#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

//function prototypes
void initDCmotorsPWM(); // function to setup PWM
void setMotorPWM(DC_motor *m);

void motor_variables_init(struct DC_motor *mL, struct DC_motor *mR, int PWMcycle);

void stop(DC_motor *mL, DC_motor *mR);


void fullSpeedAhead(DC_motor *mL, DC_motor *mR);
void slowSpeedReverse(DC_motor *mL, DC_motor *mR);
void turn90Left(struct DC_motor *mL, struct DC_motor *mR);
void turn180Left(struct DC_motor *mL, struct DC_motor *mR);

void turn135Left(struct DC_motor *mL, struct DC_motor *mR);
void turn135Right(struct DC_motor *mL, struct DC_motor *mR);


void turn90Right(struct DC_motor *mL, struct DC_motor *mR);

unsigned int calibration_sequence_left(struct DC_motor *mL, struct DC_motor *mR);
unsigned int calibration_sequence_right(struct DC_motor *mL, struct DC_motor *mR);

void maze_navigation_sequence (struct DC_motor *mL, struct DC_motor *mR, int color_from_func);
void return_home_sequence (struct DC_motor *mL, struct DC_motor *mR);






#endif