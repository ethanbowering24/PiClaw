#include "robot.h"
#include "pigpio.h"
#include "packet.h"
#include <iostream>

int Robot::Initialize()
{

    int ret = gpioInitialise();
    if (ret != 0)
    {
        std::cerr << "gpioInitialize failed!" << std::endl;
        return ret;
    }
    
    for (Servo& servo : servos)
    {
        ret = servo.initialize();
        if (ret != 0)
        {
            std::cerr << servo.name << " initialization failed!" << std::endl;
            return ret;
        }
        
    }

    return 0;
}

bool Robot::Move(Packet& packet) //TODO this needs to now write angles to the servos
{
    int ret = 0;
    ret |= servos[SERVO_WRIST_ROLL].writeAngle(packet.values[WRIST_ROLL]);
    ret |= servos[SERVO_WRIST_PITCH].writeAngle(packet.values[WRIST_PITCH]-packet.values[FOREARM_PITCH]-packet.values[UPARM_PITCH]);
    ret |= servos[SERVO_FOREARM_PITCH].writeAngle(packet.values[FOREARM_PITCH]-packet.values[UPARM_PITCH]);
    ret |= servos[SERVO_UPARM_PITCH].writeAngle(packet.values[UPARM_PITCH]);
    ret |= servos[SERVO_UPARM_YAW].writeAngle(packet.values[FOREARM_YAW]);
    ret |= servos[SERVO_CLAW].writeAngle(packet.values[CLAW]);
    return (ret==0);
}
