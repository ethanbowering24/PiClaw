#include "servo.h"
#include <utility>
#include <iostream>
#include <pigpio.h>
#include <cstdlib>


//limits is of the form {min_angle, max_angle}
Servo::Servo(int pinNumber, std::pair<int,int> limits, int powerupAngle, int offset, std::string name): 
    pinNumber(pinNumber), limits(limits), powerupAngle(powerupAngle), offset(offset), name(name)
{
}

int Servo::writeAngle(int angle)
{

    angle += offset;
    if (angle > limits.second){
        //Angle past servo max, write max angle instead
        angle=limits.second;
    }
    else if (angle <limits.first){
        //Angle below servo min, write min angle instead
        angle=limits.first;
    }

    //if angle within 1 degree of last angle, dont upate motor to prevent jitters
    //When testing, comment out this code if the arm doesn't move as nicely as needed
    int angleDelta = std::abs(angle-currentAngle);
    if (angleDelta <= 1){
        //One degree difference, make no changes
        return 0;
    }

    int pulseWidth = angleToPulseWidth(angle);
    int returnCode = gpioServo(pinNumber, pulseWidth);
    if (returnCode == 0){
        currentAngle=angle;
    }
    return returnCode;
}

//Initializes servo to startup angle
int Servo::initialize()
{
    int ret = gpioServo(pinNumber, angleToPulseWidth(powerupAngle));
    if (ret == 0);
        currentAngle=powerupAngle;
    return ret;
}

//Utility functions
int Servo::angleToPulseWidth(int angle){
    //Min PW = 500us
    //Max PW = 2500us
    //Formula used: PW = MIN_PW+(angle/180deg)*(MAX_PW-MIN_PW)
    return static_cast<int>(500+(angle/180.0)*(2000));
}
