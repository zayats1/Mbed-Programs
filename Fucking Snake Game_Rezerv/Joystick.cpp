/**
@file Joystick.cpp

@brief Member functions implementations
@brief Fixed button

*/
#include "Joystick.h"
#include "mbed.h"

Joystick::Joystick(PinName x_axis_pin, PinName y_axis_pin, PinName button_pin)
{
    x_axis_ = new AnalogIn(x_axis_pin);
    y_axis_ = new AnalogIn(y_axis_pin);
    button_ = new DigitalIn(button_pin);
}

Joystick::~Joystick()
{
    delete x_axis_;
    delete y_axis_;
    delete button_;
}

void Joystick::init()
{
    //Sets up the button ISR
    button_->mode(PullUp);
    //Initalises the vairables and flags
    x_offset_ = 0;
    y_offset_ = 0;
    g_button_flag_ = 0;

    //Samples the joystick 5 times and takes an average to get the offset
    float x_sum = 0;
    float y_sum = 0;

    for (int i = 0; i < 5; ++i) {
        x_sum += x_axis_->read();
        y_sum += y_axis_->read();
    }

    x_offset_ = 0.5f - x_sum/5.0f;
    y_offset_ = 0.5f - y_sum/5.0f;
}

float Joystick::GetXValue()
{

    float x_sum = 0;
    //Iterates 5 times and calculates an average
    for (int i = 0; i < 5; ++i) {
        x_sum += x_axis_->read();
    }

    float x_value = x_sum/5.0f + x_offset_;

    //Caps the value for the POT between 0 and 1
    if (x_value < 0.0f) {
        return 0;
    } else if (x_value > 1.0f) {
        return 1;
    } else {
        return x_value;
    }
}

float Joystick::GetYValue()
{

    float y_sum = 0;
    //Iterates 5 times and calculates an average
    for (int i = 0; i < 5; ++i) {
        y_sum += y_axis_->read();
    }

    float y_value = y_sum/5.0f + y_offset_;

    //Caps the value for the POT between 0 and 1
    if (y_value < 0.0f) {
        return 0;
    } else if (y_value > 1.0f) {
        return 1;
    } else {
        return y_value;
    }
}

bool Joystick::get_button_flag()
{
    g_button_flag_ = button_ -> read();
        return g_button_flag_;
}
