#ifndef _color_H
#define _color_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/

struct RGB_val{

    float input_r_ratio;
    float input_g_ratio;
    float input_b_ratio;

    
    float red_r_ratio;
    float red_g_ratio;
    float red_b_ratio;
    
    
    float green_r_ratio;
    float green_g_ratio;
    float green_b_ratio;
    

    float blue_r_ratio;
    float blue_g_ratio;
    float blue_b_ratio;

    float yellow_r_ratio;
    float yellow_g_ratio;
    float yellow_b_ratio;

    
    float pink_r_ratio;
    float pink_g_ratio;
    float pink_b_ratio;

    float orange_r_ratio;
    float orange_g_ratio;
    float orange_b_ratio;
    
    
    float lightblue_r_ratio;
    float lightblue_g_ratio;
    float lightblue_b_ratio;
    

    
    float white_r_ratio;
    float white_g_ratio;
    float white_b_ratio;
    
    float black_r_ratio;
    float black_g_ratio;
    float black_b_ratio;
   
};



void find_color(struct RGB_val *m);
unsigned int color_read_Clear(void);
unsigned int color_read_Red(void);
unsigned int color_read_Green(void);
unsigned int color_read_Blue(void);

void calibrate_red(struct RGB_val *m);
void calibrate_green(struct RGB_val *m);
void calibrate_blue(struct RGB_val *m);
void calibrate_yellow(struct RGB_val *m);
void calibrate_pink(struct RGB_val *m);
void calibrate_orange(struct RGB_val *m);
void calibrate_lightblue(struct RGB_val *m);

void calibrate_white(struct RGB_val *m);
void calibrate_black(struct RGB_val *m);


unsigned int threshold90(float value_in, float value1);
unsigned int threshold95(float value_in, float value1);

unsigned int color_checker(struct RGB_val *m);
void full_calibration_routine(struct RGB_val *m);
void RGB_variables_set_zero(struct RGB_val *m);







#endif