#include "Fusion.h"
#include "I2Cdev.h"
#include "arm.h"
#include "udpSender.h"
#include "packet.h"

#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <memory>

int main(int argc, char* argv[])
{

    Arm arm;
    UdpSender sockSend;

    if(!sockSend.init("Claw-Sevrer", "8080"))
    {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }
    //wiringPiSetupPinType(WPI_PIN_PHYS);
    //pinMode(26, INPUT);
    std::vector<std::string> args(argv, argv + argc);
    if (std::find(args.begin(), args.end(), "--calibrate") != args.end()) {
        arm.Calibrate();
        return 0;
    }

    while (true)
    {
        /* code */
    }
    
    
    arm.Read();
    return 0;

}