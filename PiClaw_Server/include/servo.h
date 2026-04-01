#pragma once
#include <string>
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
        std::string name;
        Servo(int pinNumber, std::pair<int,int> limits, int powerupAngle, int offset=0, std::string name="");
        int writeAngle(int angle);
        bool initialize();
        static int angleToPulseWidth(int angle);

};