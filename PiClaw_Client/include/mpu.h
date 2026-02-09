#include "MPU6050_6Axis_MotionApps20.h"
#include "wiringPi.h"
#include "MPU6050.h"

class MPU
{
private:
    MPU6050 mpu6050;

    //int fifoAlive = 0; // tests if the interrupt is triggering
    //int isAlive = -20;     // counts interrupt start at -20 to get 20+ good values before assuming connected
    // MPU control/status vars
    //uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    //uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer

    // orientation/motion vars
    Quaternion q;           // [w, x, y, z]         quaternion container
    //VectorInt16 aa;         // [x, y, z]            accel sensor measurements
    //VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
    //VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
    VectorFloat gravity;    // [x, y, z]            gravity vector
    float euler[3];         // [psi, theta, phi]    Euler angle container
    float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
    //uint8_t startUP = 100; // lets get 100 readings from the MPU before we start trusting them (Bot is not trying to balance at this point it is just starting up.)

    //volatile bool interrupt = false;     // indicates whether MPU interrupt pin has gone high
    
    int MPUOffsets[6] = {-262, 1702, 1307 , 193, 103, 74};// vert board
    //int interruptPin = 26;

public:
    MPU();
    MPU(uint8_t mpu_address);
    bool Connect();
    //void GetDMP();
    //void MPUMath();
    //void DmpDataReady();
    void Loop();

    //static void ISR(struct WPIWfiStatus wfiStatus, void* userdata);
};
