#include "udpReceiver.h"
#include "robot.h"
#include "servo.h"
#include "pigpio.h"

#include <iostream>
#include <iomanip> 

constexpr int PORT = 8080;


int main()
{
    std::cout << std::fixed << std::setprecision(2);
    Robot robot;
    if (robot.Initialize() != 0)
    {
        std::cerr << "Initilazation failed, exiting" << std::endl;
        return -1;
    }
    
    UdpReceiver sockRecv;
    if (!sockRecv.init(PORT))
    {
        std::cerr << "Socket initialization failed, exiting" << std::endl;
        return -1;
    }
    
    int lastPacketID = -1;
    while (true)
    {
        Packet packet;
        if (sockRecv.receive(packet) && packet.id > lastPacketID)
        {
            lastPacketID = packet.id;
            robot.Move(packet);
            std::cout << packet.PacketToString() << std::endl;
        }
        else if (packet.id <= lastPacketID)
        {
            std::cerr << "Stale packet received" << std::endl;
        }
        else
        {
            std::cerr << "recv failed" << std::endl;
        }
    }
    
}