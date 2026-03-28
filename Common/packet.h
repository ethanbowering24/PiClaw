#pragma once

enum {
    WRIST_PITCH = 0,
    WRIST_ROLL,
    WRIST_YAW,
    FOREARM_PITCH,
    FOREARM_ROLL,
    FOREARM_YAW,
    UPARM_PITCH,
    UPARM_ROLL,
    UPARM_YAW,
    INDEX_COUNT  // bonus: tracks array size
};

struct Packet {
    int id;
    float values[INDEX_COUNT];
};