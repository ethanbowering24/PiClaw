#pragma once
#include <utility>

class Servo
{
    private:
        int pinNumber;
        std::pair<int,int> limits;
        int powerupAngle;
        int currentAngle;
        int offset;


    public:
        Servo(int pinNumber, std::pair<int,int> limits, int powerupAngle);
        int writeAngle(int angle);
        //Is this doing anything?
        bool config();
        bool initialize();
        static int angleToPulseWidth(int angle);

};