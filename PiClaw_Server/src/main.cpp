#include "udpReceiver.h"
#include "robot.h"
#include "servo.h"
#include "pigpio.h"

#include <iostream>
#include <iomanip> 
#include <chrono>

constexpr int PORT = 8080;

int64_t timeDelta(int64_t ref) {
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() -
            ref;
}


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
            int64_t networkDelay = timeDelta(packet.timestamp);
            lastPacketID = packet.id;
            robot.Move(packet);
            int64_t totalDelay = timeDelta(packet.timestamp);
            int64_t robotDelay = totalDelay - networkDelay;
            std::cout << packet.PacketToString() << std::endl;
            std::cout << "Network delay: " << networkDelay << "\n" 
                        << "Robot delay: " << robotDelay << "\n" 
                        << "Total delay: " << totalDelay << std::endl;
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