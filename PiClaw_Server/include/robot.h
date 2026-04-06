#pragma once
#include "packet.h"
#include "servo.h"
#include <array>

constexpr int CLAW_PIN          = 2;
constexpr int WRISTROLL_PIN     = 3;
constexpr int WRISTPITCH_PIN    = 4;
constexpr int FOREARMPITCH_PIN  = 5;
constexpr int UPARMPITCH_PIN    = 6;
constexpr int UPARMYAW_PIN      = 7;


class Robot
{
    private:

        enum ServoIndex 
        { 
            SERVO_CLAW, 
            SERVO_WRIST_ROLL, 
            SERVO_WRIST_PITCH, 
            SERVO_FOREARM_PITCH, 
            SERVO_UPARM_PITCH, 
            SERVO_UPARM_YAW, 
            SERVO_COUNT 
        };

        std::array<Servo, SERVO_COUNT> servos {{
            {CLAW_PIN,         std::pair<int,int>{0, 180},  0,  0, "Claw"},
            {WRISTROLL_PIN,    std::pair<int,int>{0, 180},  0,  0, "WristRoll"},
            {WRISTPITCH_PIN,   std::pair<int,int>{0, 180}, 90,  90, "WristPitch"},
            {FOREARMPITCH_PIN, std::pair<int,int>{0, 180},  0,	0, "ForearmPitch"},
            {UPARMPITCH_PIN,   std::pair<int,int>{0, 180}, 90,  0, "UpArmPitch"},
            {UPARMYAW_PIN,     std::pair<int,int>{0, 180}, 90,  90, "UpArmYaw"},
        }};

    public:
        int Initialize();
        bool Move(Packet &packet);

};
