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
#include <thread>

int main(int argc, char* argv[])
{

    Arm arm;

    arm.Connect();

    std::vector<std::string> args(argv, argv + argc);
    if (std::find(args.begin(), args.end(), "--calibrate") != args.end()) {
        arm.Calibrate();
        return 0;
    }

    arm.Initialize();
    int id = 0;
    while (true)
    {
        auto next_loop_time = std::chrono::steady_clock::now();
        Packet packet = arm.Read();
        packet.id = id++;
        //std::cout << packet.PacketToString() << std::endl;
       
        next_loop_time += std::chrono::milliseconds(10);
        std::this_thread::sleep_until(next_loop_time);
    }
    
    return 0;

}