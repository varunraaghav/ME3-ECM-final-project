#include <xc.h>
#include "color.h"
#include "i2c.h"


// INITIALISATION FUNCTIONS, LINK WITH I2C __________________________________________________________________________________________________________________________
void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at Green = 0x18 low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at Blue = 0x1a low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

// master write values got from tcs3471 manual

unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at Clear = 0x14 low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
    
}    

//___________________________________________________________________________________________________________________________________________________________________

// INITIALISING ALL THE RATIOS OF THE RGB_VARIABLE STRUCT AS FLOATS AND = 0.5 INITIAL VALUE
void RGB_variables_set_zero(struct RGB_val *m){
    m->input_r_ratio = (float) 0.5;                   // FLOATS USED TO STORE THE RATIO OF THE R,G,B PROPORTIONS RESPECT TO THEIR CLEAR VALUE
    m->input_g_ratio = (float) 0.5;      // RAW R,G,B VALYE FROM LIGHT SENSORS NOT STORED
    m->input_b_ratio = (float) 0.5;
                                            // 1ST WORD = CARD COLOR, 2ND LETTER = R/G/B RATIO OVER CLEAR
    m->red_r_ratio = (float) 0.5;    
    m->red_g_ratio = (float) 0.5;
    m->red_b_ratio = (float) 0.5;

    m->green_r_ratio = (float) 0.5;    
    m->green_g_ratio = (float) 0.5;
    m->green_b_ratio = (float) 0.5;
    
    m->blue_r_ratio = (float) 0.5;    
    m->blue_g_ratio = (float) 0.5;
    m->blue_b_ratio = (float) 0.5;

    m->yellow_r_ratio = (float) 0.5;    
    m->yellow_g_ratio = (float) 0.5;
    m->yellow_b_ratio = (float) 0.5;    
    
    m->orange_r_ratio = (float) 0.5;    
    m->orange_g_ratio = (float) 0.5;
    m->orange_b_ratio = (float) 0.5;

    m->pink_r_ratio = (float) 0.5;    
    m->pink_g_ratio = (float) 0.5;
    m->pink_b_ratio = (float) 0.5;   
    
    m->lightblue_r_ratio = (float) 0.5;    
    m->lightblue_g_ratio = (float) 0.5;
    m->lightblue_b_ratio = (float) 0.5;

    m->green_r_ratio = (float) 0.5;    
    m->green_g_ratio = (float) 0.5;
    m->green_b_ratio = (float) 0.5;
    
    m->white_r_ratio = (float) 0.5;    
    m->white_g_ratio = (float) 0.5;
    m->white_b_ratio = (float) 0.5;

    m->black_r_ratio = (float) 0.5;    
    m->black_g_ratio = (float) 0.5;
    m->black_b_ratio = (float) 0.5;    

}


// COLOR FUNCTIONS : FIND RATIOS USING COLOR READ FUNCTIONS. STORE IN RGB_VARIABLE STRUCT _____________________________________________________________________________________________________________________________
void find_color(struct RGB_val *m){

    (m->input_r_ratio) = (float)(color_read_Red())   / (float)(color_read_Clear());
    (m->input_g_ratio) = (float)(color_read_Green()) / (float)(color_read_Clear());
    (m->input_b_ratio) = (float)(color_read_Blue()) / (float)(color_read_Clear());
    
}

void calibrate_red(struct RGB_val *m){
   (m->red_r_ratio) = (float)(color_read_Red())/(float)(color_read_Clear());
   (m->red_g_ratio) = (float)(color_read_Green())/(float)(color_read_Clear());
   (m->red_b_ratio) = (float)(color_read_Blue())/(float)(color_read_Clear());
}
void calibrate_green(struct RGB_val *m){
    (m->green_r_ratio) = (float)(color_read_Red())/(float)(color_read_Clear());
    (m->green_g_ratio) = (float)(color_read_Green())/(float)(color_read_Clear());
    (m->green_b_ratio) = (float)(color_read_Blue())/(float)(color_read_Clear());
}
void calibrate_blue(struct RGB_val *m){
    (m->blue_r_ratio) = (float)(color_read_Red())/(float)(color_read_Clear());
    (m->blue_g_ratio) = (float)(color_read_Green())/(float)(color_read_Clear());
    (m->blue_b_ratio) = (float)(color_read_Blue())/(float)(color_read_Clear());
}

void calibrate_yellow(struct RGB_val *m){
    (m->yellow_r_ratio) = (float)(color_read_Red())/(float)(color_read_Clear());
    (m->yellow_g_ratio) = (float)(color_read_Green())/(float)(color_read_Clear());
    (m->yellow_b_ratio) = (float)(color_read_Blue())/(float)(color_read_Clear());

}

void calibrate_pink(struct RGB_val *m){
    (m->pink_r_ratio) = (float)(color_read_Red())/(float)(color_read_Clear());
    (m->pink_g_ratio) = (float)(color_read_Green())/(float)(color_read_Clear());
    (m->pink_b_ratio) = (float)(color_read_Blue())/(float)(color_read_Clear());
}

void calibrate_orange(struct RGB_val *m){
    (m->orange_r_ratio) = (float)(color_read_Red())/(float)(color_read_Clear());
    (m->orange_g_ratio) = (float)(color_read_Green())/(float)(color_read_Clear());
    (m->orange_b_ratio) = (float)(color_read_Blue())/(float)(color_read_Clear());
}

void calibrate_lightblue(struct RGB_val *m){
    (m->lightblue_r_ratio) = (float)(color_read_Red())/(float)(color_read_Clear());
    (m->lightblue_g_ratio) = (float)(color_read_Green())/(float)(color_read_Clear());
    (m->lightblue_b_ratio) = (float)(color_read_Blue())/(float)(color_read_Clear());

}

void calibrate_white(struct RGB_val *m){
    (m->white_r_ratio) = (float)(color_read_Red())/(float)(color_read_Clear());
    (m->white_g_ratio) = (float)(color_read_Green())/(float)(color_read_Clear());
    (m->white_b_ratio) = (float)(color_read_Blue())/(float)(color_read_Clear());

}
void calibrate_black(struct RGB_val *m){
    (m->black_r_ratio) = (float)(color_read_Red())/(float)(color_read_Clear());
    (m->black_g_ratio) = (float)(color_read_Green())/(float)(color_read_Clear());
    (m->black_b_ratio) = (float)(color_read_Blue())/(float)(color_read_Clear());

}
// _________________________________________________________________________________________________________________________________

// PERFORM EXTENSIVE CALIBRATION ROUTINE FOR EACH COLOUR FOR EVERY TRIAL OF THE MAZE - TO GET MOST ACCURACY. AS LIGHT LEVELS DURING THE DAY MADE A SIGNIFICANT IMPACT 
void full_calibration_routine(struct RGB_val *m){
  
    LATDbits.LATD7 = 1;
    while (PORTFbits.RF2); 
    if (!PORTFbits.RF2){
        LATDbits.LATD7 = 0; 
        calibrate_red(m);
        __delay_ms(300);
        }   
    LATDbits.LATD7 = 1;
    while (PORTFbits.RF2); 
    if (!PORTFbits.RF2){
        LATDbits.LATD7 = 0; 
        calibrate_green(m);
        __delay_ms(300);
        }   
    LATDbits.LATD7 = 1;
    while (PORTFbits.RF2); 
    if (!PORTFbits.RF2){
        LATDbits.LATD7 = 0; 
        calibrate_blue(m);
        __delay_ms(300);
        }
    
    LATDbits.LATD7 = 1;
    while (PORTFbits.RF2); 
    if (!PORTFbits.RF2){
        LATDbits.LATD7 = 0; 
        calibrate_yellow(m);
        __delay_ms(300);
        }
    
    LATDbits.LATD7 = 1;
    while (PORTFbits.RF2); 
    if (!PORTFbits.RF2){
        LATDbits.LATD7 = 0; 
        calibrate_pink(m);
        __delay_ms(300);
        }
    
    LATDbits.LATD7 = 1;
    while (PORTFbits.RF2); 
    if (!PORTFbits.RF2){
        LATDbits.LATD7 = 0; 
        calibrate_orange(m);
        __delay_ms(300);
        }
    
    LATDbits.LATD7 = 1;
    while (PORTFbits.RF2); 
    if (!PORTFbits.RF2){
        LATDbits.LATD7 = 0; 
        calibrate_lightblue(m);
        __delay_ms(300);
        }
    
    LATDbits.LATD7 = 1;
    while (PORTFbits.RF2); 
    if (!PORTFbits.RF2){
        LATDbits.LATD7 = 0; 
        calibrate_white(m);
        __delay_ms(300);
        }   

    LATDbits.LATD7 = 1;
    while (PORTFbits.RF2); 
    if (!PORTFbits.RF2){
        LATDbits.LATD7 = 0; 
        calibrate_black(m);
        __delay_ms(300);
        } 
}
// _________________________________________________________________________________________________________________________________
   
// THRESHOLD FUNCTIONS THAT OUTPUT 1 IF VALUE IS WITHIN THE SPECIFIED RANGE
unsigned int threshold90(float value_in, float value1){ // 90 threshold = value between 90% and 100% of the actual value
    int output = 0;
    if (((value_in)*10 < (value1)*11)  && ((value_in)*10 > (value1)*9)) {output=1;}
    return output;
}
unsigned int threshold95(float value_in, float value1){ // 95 threshold = value between 95% and 105% of the actual value
    int output = 0;
    if (((value_in)*100 < (value1)*105)  && ((value_in)*100 > (value1)*95)) {output=1;}
    else {output=0;}
    return output;
}

// ______________________________________________________________________________________________________________________________

// COLOR DETERMINING FUNCTION TO FIND
unsigned int color_checker(struct RGB_val *m){
    signed int correct_color = 10;  // 10 AMBIENT,  1 = red, 2 = blue, 3 = green, 4 = white


    find_color(m); // performs find_color operation within larger function
    
    // threshold checking for each input R/G/B ratio in comparison with the R/G/B ratio of the pre-calibrated colors before 
    if ((threshold90((m->input_r_ratio), (m->red_r_ratio))==1) && (threshold90((m->input_g_ratio), (m->red_g_ratio))==1) && (threshold90((m->input_b_ratio), (m->red_b_ratio))==1)){
        correct_color=1; // correct_color variable updated to the designated value based on the thresholds
    }

    if ((threshold90((m->input_r_ratio), (m->green_r_ratio))==1) && (threshold90((m->input_g_ratio), (m->green_g_ratio))==1) && (threshold90((m->input_b_ratio), (m->green_b_ratio))==1)){
        correct_color=2;
    }
    
    if ((threshold90((m->input_r_ratio), (m->blue_r_ratio))==1) && (threshold90((m->input_g_ratio), (m->blue_g_ratio))==1) && (threshold90((m->input_b_ratio), (m->blue_b_ratio))==1)){
        correct_color=3;
    }

    if ((threshold95((m->input_r_ratio), (m->yellow_r_ratio))==1) && (threshold90((m->input_g_ratio), (m->yellow_g_ratio))==1) && (threshold90((m->input_b_ratio), (m->yellow_b_ratio))==1)){
        correct_color=4;
    }
    
    if ((threshold90((m->input_r_ratio), (m->pink_r_ratio))==1) && (threshold90((m->input_g_ratio), (m->pink_g_ratio))==1) && (threshold90((m->input_b_ratio), (m->pink_b_ratio))==1)){
        correct_color=5;
    }    
    
    if ((threshold95((m->input_r_ratio), (m->orange_r_ratio))==1) && (threshold95((m->input_g_ratio), (m->orange_g_ratio))==1) && (threshold95((m->input_b_ratio), (m->orange_b_ratio))==1)){
        correct_color=6;
    }
    
    
    if ((threshold90((m->input_r_ratio), (m->lightblue_r_ratio))==1) && (threshold90((m->input_g_ratio), (m->lightblue_g_ratio))==1) && (threshold90((m->input_b_ratio), (m->lightblue_b_ratio))==1)){
        correct_color=7;
    }
    
    if ((threshold90((m->input_r_ratio), (m->white_r_ratio))==1) && (threshold95((m->input_g_ratio), (m->white_g_ratio))==1) && (threshold95((m->input_b_ratio), (m->white_b_ratio))==1)){
        correct_color=8;
    }
    
    if ((threshold90((m->input_r_ratio), (m->black_r_ratio))==1) && (threshold90((m->input_g_ratio), (m->black_g_ratio))==1) && (threshold90((m->input_b_ratio), (m->black_b_ratio))==1)){
        correct_color=9;
    }
    
    return correct_color; //  value outputted to main.c
}

// _____________________________________________________________________________________________________________________