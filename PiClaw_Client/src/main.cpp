#include "Fusion.h"
#include "I2Cdev.h"
#include "arm.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

int main(int argc, char* argv[])
{

    Arm arm;

    arm.Connect();

    //wiringPiSetupPinType(WPI_PIN_PHYS);
    //pinMode(26, INPUT);
    std::vector<std::string> args(argv, argv + argc);
    if (std::find(args.begin(), args.end(), "--calibrate") != args.end()) {
        arm.Calibrate();
        return 0;
    }
    
    arm.Read();
    return 0;

}