# Course project - Mine navigation search and rescue

## Overview of Files

**main.c**: 
- Performs initialisation functions. 
- Initiates calibration sequences for light and colour (actual calibrations done in separate file however)
- Runs main maze navigation, colour detection and returning home functions in a recurring while (1) loop. 
- A flow chart has been attched to the project outlining the main logic behind the code.

**color.c**: 
- Sets up the use of the color click hardware and obtains a continuous and live stream of data that defines the colours observed (find_color and color_checker functions)
- Colour calibration within this function normlises the input data (the RGBC values) between a scale of 0:1, as explained below 
- Continuous data inputs are also normlised and compared with the preset values taken from the colour calibration to identify if a coloured wall has been observed within the maze.

**dc_motors.c:** 
- Definition of the motor structs as done in Lab 6. 
- Turning mechanisms for each card are generated.
- Also holds the motor calibration for left and right 90 degree turns routines

**serial.c**: 
- File used to output values through the serial communication software as done in Lab 5. 
- _This was mainly done for testing purposes and is not required in the running of the final project._

**I2C.c:** 
- Contains functions which allow the RGBC values to be transferred over from the color click to the main hardware.

**interrupts.c**: 
- Used for timer TMR0 interrupt, in the same method as lab 2, to measure the time between different turning movements. 
- This is used when the buggy must return home when it has seen the white flag or it has performed too many turns and must come back. 


## Colour Calibration routine

- It was discovered that colour sensing was dependent on many different factors such as room light level and battery level. To mitigate the need to manually enter values for whatever room testing was being conducted in a colour calibration algorithm was created. 

- Colour calibration is performed each time before the buggy runs through the maze. The process of colour calibration involves placing a different coloured card in front of the colour sensor, recording what RGBC values are taken and then normalising these values by storing R/C, G/C and B/C. 
- The order for cards that are placed in front of the sensor during the calibration process is: red, green, blue, yellow, pink, orange, light blue, white and black.
- Continuous streams of data are also normlised and then compared to these previously recorded values, if the current value lies within threshold (normally 10%) of the pre-recorded value then a colour is identified and the appropriate movement is conducted. 
- This process was reliable when used in a constant light setting, however, at times when light is concentrated in certain areas of the room different RGBC values reach the buggy containing different normalised values. 
- A flow chart of the colour calibration process is attached to this repo. As mentioned earlier, colour calibration is required each time the buggy is set up, however, we found that this entire process took aroud 20 seconds so accepted doing this on each run.

## Motor Calibration routine
- Similar to colour sensing, motor rotation and more generally buggy movement was dependant on a plethora of different factors such as floor roughness and wheel smoothness. One of the aims of this task was for the buggy to turn reliably when a flag is dependent so calibration was required.

- When motor calibration is activated, button RF2 needs to be pushed which will prompt the buggy to initially conduct a left turn. 
- The turning angle can be increased by pressing RF2 more times until the angle is exactly 90 degrees. 
- When RF3 is pressed the buggy will stop turning left and instead start turning right, when the same process is conducted for the right turn and RF3 is pressed again then the motor calibration process is done. 
- This process is increasing the right_cal_count and left_cal_count variables and feeding it into the turn functions, this adjusts the length of time each turning movement will delay for. 
- The variable right_cal_count and left_cal_count represent how long left and right 90 degree turning functions are delayed for. This delay defines the length of time the buggy is rotating, if the correct time is chosen then the buggy will stop to conduct a perfect 90 degree turn. 
- To gain the 135 degree turn from this calibration the delay time is multiplied by 1.5 (since 1.5 x 90 = 135), the relationship between delay time and angle of turn was assumed to be approximately linear. Similarly the 180 degree turn is the delay time multiplied by 2. 


## Responses to Brief

**Task 1, Navigate towards a coloured card and stop before hitting the card:**

- This was done by having the buggy constantly moving forwards, using the FullSpeedAhead function from Lab 6. 

- The color click is constantly recieving input RGBC values and the PIC is constanlty checking if these values lie within a threshold range of any of the preset colours that the program is meant to look for. 
- If a colour is found then the buggy will immediately stop and then slowly start to move forwards so that it will readjust itslef and become perpendicular to the wall it is facing. It will then move backwards to give space for turning and then conduct its turn.

**Task 2, Read the Card Colour and Task 3, Interpret the card colour using a predefined code and perform the navigation instruction:**

- The buggy will only stop when the card colour has been read. It is read by comparing normalised input values of: R/C, B/C and G/C to previously calibrated normalised threshold values for each colour. 

- If all three inputs lie within the threshold for the same colour then the buggy will stop and conduct the appropriate movement. The color_from_func function is able to obtain a numerical indexed value of the colour that has been red (e.g. red = 1, green = 2, blue = 3....). 
- This is then sent to a maze_navigation_sequence function which is a set of conditional statements that define the motor output for any numerical input.

**Task 4, When the final card is reached, navigate back to the starting position:**

- The final card reached in this problem is pre-defined to be white coloured. If white is detected then the return_home_sequence function is called which defines the movements the buggy has to make to get back to its starting point while also providing the timings between movements so that the journey back is calibrated with the maze. 

- Timings are done through a global time variable, similar to the logic used in our miniproject. The timer overflow interrupt from Lab2 is used and the prescaler and range was updated so that the overflow would occur every millisecond. The starting values of TMR0H and TMR0L are adjusted each time to have an extremely accurate 1ms overflow over 16 bits. 
- The amount of time between turns is placed in a time_turns_taken array and whenever time is added to this array, time is reset to zero so that each value in the array represents an amount of time that has passed between turns. 
- On top of this during every turn the color_from_func int value is added to a turn_array_storage array. This allows us to keep track of all of the turns that have occurred. 
- When navigating back to the starting position the opposite turning motion to what is recorded are used. e.g. for a 90 degree right turn on the way back there will be a 90 degree left turn instead. 

**Task 5, Handle exceptions and return back to the starting position if final card cannot be found:**

- It is decided that the final card cannot be found if there are too many turns that have occured at a certain time. 

- This can be decided either when the total time (sum of all the values in the time_turns_array) has exceeded a certain amount or when the number of variables within the array exceeds it memory limitation. For example if the array can only contain 15 data points then the buggy will return home once 15 turns have been made.


## Code snippets of important functions

**while (1) loop that handles overall buggy sequences**
```C
    while (1){
        fullSpeedAhead(&motorL, &motorR);  // constantly move forward
        int color_from_func = color_checker(&RGB_variable);   // constantly detect color. Could have multiple checks as a future improvement
        
	if (color_from_func != 8) {                           
            maze_navigation_sequence(&motorL, &motorR, color_from_func);   // perform maze navigation as long as color is not white or 15 turns have been achieved
	    } 
        else if ((color_from_func == 8 || (turn_counter=20))){ // return home sequence initiated when colour detected==white or number of turns has exceeded 19
            return_home_sequence(&motorL, &motorR);  // perform return home sequence
            } 
        
	stop(&motorL, &motorR);  // temporary stop for smooth fullSpeedAhead forward function  
    }
```

**color_checker() function and find_color() function that constantly detects input colours from sensor**
```C
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
	
	//  ......... threshold checking is performed for every colour in a similar manner
    }
    return correct_color;
```

```C
void find_color(struct RGB_val *m){

    // ratio (float) of each R,G,B reading from light sensors measured against Clear reading is stored in the particular parts of the RGB_variable in the struct
    
    (m->input_r_ratio) = (float)(color_read_Red())   / (float)(color_read_Clear()); // input_red_ratio = ratio of R in from sensor input: amount of clear
    (m->input_g_ratio) = (float)(color_read_Green()) / (float)(color_read_Clear());
    (m->input_b_ratio) = (float)(color_read_Blue()) / (float)(color_read_Clear());       
}
```

**Navigating maze sequence**

```C
void maze_navigation_sequence(struct DC_motor *mL, struct DC_motor *mR, int color_from_func){  
if (color_from_func == 1){ // 1 = red
     
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



    else if (color_from_func == 2){  // 2 = green
    
    // ... CODE REPEATED ACCORDINGLY FOR EACH COLOUR 
    }
```
**bits of return home sequence** 
```C
void return_home_sequence(struct DC_motor *mL, struct DC_motor *mR){
        time_turns_taken[turn_counter] = time; // store final fullSpeadAhead time
        time = 0;
            
        turn_array_storage[turn_counter] = 8; //stored 8 = white function 
        turn_counter = turn_counter + 1;

	// [CODE PRESENT HERE FOR 180 DEGREE TURN. REFER ACTUAL FILE FOR CODE, NOT INCLUDED README FOR CONCISENESS]
        
        signed int i = turn_counter;  //  PERFORM return home sequence inside while loop where i : turn_counter --> 0. therefore array parsed from last index value
        while (i >= 0){  // until i = 0

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
        } 
	
	// ..... DEDICATED SEQUENCES FOR EVERY VALUE STORED WHEN COMING BACK. NOT INCLUDED IN README FOR CONCISENESS
	
            i = i - 1;   // reduce i value by 1 each time to iterate from turn_counter --> 0
    }
```

**Colour Inputs:**

The following code represents the structure is 

	struct RGB_val{
	    float input_r_ratio;
	    float input_g_ratio;
	    float input_b_ratio;


	    float red_r_ratio;
	    float red_g_ratio;
	    float red_b_ratio;
	};
This is an extract of the overal struct that is used, the floating red ratioed variables represent the same form of every other variable within the struct for each RGB component of each colour. Floating points are used to represent the decimal valued normalisation. Integers could have been used in this application, however, given that memory was not too large a constraint for the completed code, accuracy was preferred which is gained by floating points. 


## Project Development

- Upon assignment we decided to split the initial bulk of the work into two streams. One for motor initialisation and calibration and one for colour sensing. Initially, two separate branches were created on github, one for each team member (Mukund-Initial-motor-callibration and Varun-initial-light-sensing) to allow them to create and initialise their functions. 
- Following commits that required merging of the motor and colour functions, new feature tree branches were created and named accordingly. The commits from the initial branches was merged.
- As testing day approached, certain additional branches were created for the last few day of development to improvise calibration routines etc and enhance buggy performance. 


## Performance

The following two video links represent two completed runs through a medium difficulty maze in room 771 in the morning of 15/12/2022:

https://youtu.be/4ibD3u-7JSM

https://youtu.be/zYGusJ8QJPw

## Possible improvements for the future

- We could perform multiple checks for each time a colour is detected to increase accuracy of colour detection
- We could check battery % levels of buggy and calibrate the turns based on battery levels instead of by visual detection so that buggy turns exactly 90/135/180 degrees each time   
