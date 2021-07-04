/**
@file Joystick.h
@brief Joysitck header file containing member functions and variables
*/
//Joystick class - Header file
//Define guards
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "mbed.h"

/**
@brief Library for interfacing with a Joystick (http://proto-pic.co.uk/thumb-joystick-analogue/)
@brief Joystick is just 2 potentiometers and a button so can be interfaced with AnalogIn and DigitalIn/InterruptIn
@brief The library contains a method to prevent button debounce with a Timeout

@author Avinash Patel and fixer zayats1
@date April 2016
 * Example
 * @code

#include "mbed.h"
#include "Joystick.h"
//I use STM32Nucleo L476 RG
//               Xaxis,Yaxis,Button 
Joystick joystick(PTB3, PTB2, PTB11);
DigitalOut r_led(LED_RED);
Serial pc(USBTX, USBRX);

#include "Joystick.h"
//Led1 switching task
//I tired, so no fixation for button click
Thread thread1;
const int PWMPERIODMS = 20; //0,02s
Joystick joy(PA_4,PB_0,PC_1);
Serial pc(USBTX, USBRX);
DigitalOut Led1(PB_10);
PwmOut Led2(PB_4);
PwmOut Led3(PB_3);
void ledToggler()
{
    while(1) {
    int button = joy.get_button_flag();
        //Switching Led
        if (button == 0) Led1 = 1;   
        else   Led1 = 0; 
    }
}
int main()
{
    Led2.period_ms(PWMPERIODMS);
    Led3.period_ms(PWMPERIODMS);
    joy.init();
    thread1.start(ledToggler);
    while(1) {
        float x = joy.GetXValue();
        float y = joy.GetYValue();
        pc.printf("X: %f, Y: %f\n", x, y);
        //in Mbed OS PWM uses analogue values
        Led2 = x;
        Led3 = y;
    }
}
 * @endcode
*/

class Joystick
{
public:
    /** Creates a Joystick object connected to the given pins
    * Dynamically allocates AnalogIn for input potentiometers, InterruptIn for the joystick buton and the debounce Timeout
    *
    * @param x_axis_pin connected to the Joystick's x potentiometer output
    * @param y_axis_pin connected to the Joystick's y potentiometer output
    * @param button_pin connected to the Joystick's button
    *
    */
    Joystick(PinName x_axis_pin, PinName y_axis_pin, PinName button_pin);

    /** Destroys the Joystick object
    * Clears the AnalogIn's, InterruptIn and Timeout from memory
    */
    ~Joystick();

    /** Initalises the Joystick
    * Sets up the InterruptIn Mode ISR
    * Initalises the offset vairables and ISR flags
    * Samples the AnalogIn's 5 times and takes an average to get the offset
    */
    void init();

    /** Gets the value of the x potentiometer
    * Takes 5 readings from the potentiometer 
    * Calculates the average measurement, accounting for joystick offset
    * Caps the average between 0 and 1
    * 
    * @returns the average value of the x potentiometer
    */
    float GetXValue();

    /** Gets the value of the y potentiometer
    * Takes 5 readings from the potentiometer 
    * Calculates the average measurement, accounting for joystick offset
    * Caps the average between 0 and 1
    * 
    * @returns the average value of the y potentiometer
    */
    float GetYValue();

    /** Reads the state of the button flag
    * @returns the button flag
    */
    bool get_button_flag();

    /** Sets the button flag
    * @param value The value the flag will be set to
    */

private:
    //Pin inputs
    AnalogIn* x_axis_;
    AnalogIn* y_axis_;
    DigitalIn* button_;

    //Stores X and Y offsets
    float x_offset_;
    float y_offset_;

    //returns one or zero
    volatile bool g_button_flag_;
};

#endif