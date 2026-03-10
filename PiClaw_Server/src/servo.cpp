#include "servo.h"
#include <utility>
#include <iostream>
#include <pigpio.h>


Servo::Servo(int pinNumber, std::pair<int,int> limits, int powerupAngle): pinNumber(pinNumber), limits(limits), powerupAngle(powerupAngle)
{
}

bool Servo::writeAngle(int angle)
{
    int pulseWidth = angleToPulseWidth(angle);
    gpioServo(pinNumber, pulseWidth)
}

//Initializes servo to startup angle
bool Servo::initialize()
{
    gpioServo(pinNumber, powerupAngle);
}

//Utility functions
int angleToPulseWidth(int angle){
    //Min PW = 500us
    //Max PW = 2500us
    //Formula used: PW = MIN_PW+(angle/180deg)*(MAX_PW-MIN_PW)
    return static_cast<int>(500+(angle/180)*(2000));
}
