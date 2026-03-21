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
    UdpSender sockSend;

    arm.Connect();

    if(!sockSend.init("Claw-Server", "8080"))
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

    sockSend.init("Client-Server", "8080");
    int id = 0;
    while (true)
    {
        auto next_loop_time = std::chrono::steady_clock::now();
        Packet packet;
        arm.Read(packet);
        packet.id = id++;
        if(!sockSend.send(packet))
        {
            std::cerr << "send failed" << std::endl;
        }
        next_loop_time += std::chrono::milliseconds(5);
        std::this_thread::sleep_until(next_loop_time);
    }
    
    return 0;

}