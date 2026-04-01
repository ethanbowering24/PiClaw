#include "robot.h"
#include "pigpio.h"
#include <iostream>

bool Robot::Initialize()
{

    if (!gpioInitialise())
    {
        std::cerr << "gpioInitialize failed!" << std::endl;
        return false;
    }
    
    for (Servo& servo : servos)
    {
        if (!servo.initialize())
        {
            std::cerr << servo.name << " initialization failed!" << std::endl;
            return false;
        }
        
    }

    return true;
}

bool Robot::Move(Packet& packet) //TODO this needs to now write angles to the servos
{
    return true;
}
