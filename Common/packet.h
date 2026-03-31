#pragma once

#include <sstream>
#include <string>
#include <iomanip> // Required for std::fixed and std::setprecision


enum {
    WRIST_ROLL = 0,
    WRIST_PITCH,
    WRIST_YAW,
    FOREARM_ROLL,
    FOREARM_PITCH,
    FOREARM_YAW,
    UPARM_ROLL,
    UPARM_PITCH,
    UPARM_YAW,
    INDEX_COUNT  // bonus: tracks array size
};

struct Packet {
    int id;
    float values[INDEX_COUNT];

    std::string PacketToString()
    {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Packet ID: " << id << "\n";
        ss << "            Roll    Pitch    Yaw\n";
        ss << "Wrist:      " << values[WRIST_ROLL]      << "    " << values[WRIST_PITCH]    << "    " << values[WRIST_YAW]      << "\n";
        ss << "Forearm:    " << values[FOREARM_ROLL]    << "    " << values[FOREARM_PITCH]  << "    " << values[FOREARM_YAW]    << "\n";
        ss << "UpArm:      " << values[UPARM_ROLL]      << "    " << values[UPARM_PITCH]    << "    " << values[UPARM_YAW]      << "\n";

        return ss.str();
    }


};