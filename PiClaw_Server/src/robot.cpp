#include "robot.h"
#include "pigpio.h"
#include "packet.h"
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
    servos[SERVO_WRIST_ROLL].writeAngle(packet.values[WRIST_ROLL]);
    servos[SERVO_WRIST_PITCH].writeAngle(packet.values[WRIST_PITCH]-packet.values[FOREARM_PITCH]-packet.values[UPARM_PITCH]);
    servos[SERVO_FOREARM_PITCH].writeAngle(packet.values[FOREARM_PITCH]-packet.values[UPARM_PITCH]);
    servos[SERVO_UPARM_PITCH].writeAngle(packet.values[UPARM_PITCH]);
    servos[SERVO_UPARM_YAW].writeAngle(packet.values[FOREARM_YAW]);
    servos[SERVO_CLAW].writeAngle(packet.values[CLAW]);
    return true;
}
