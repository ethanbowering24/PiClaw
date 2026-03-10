#pragma once
#include <utility>

class Servo
{
    private:
        int pinNumber;
        std::pair<int,int> limits;
        int powerupAngle;


    public:
        Servo(int pinNumber, std::pair<int,int> limits, int powerupAngle);
        bool writeAngle(int angle);
        bool config();
        bool initialize();

}