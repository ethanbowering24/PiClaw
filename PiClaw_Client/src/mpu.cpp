#include "mpu.h"
#include "I2Cdev.h"
#include "wiringPi.h"
#include <iostream>
#include <bitset>
#include <algorithm>
#include <iterator>

MPU::MPU():mpu6050()
{

};

MPU::MPU(uint8_t mpu_address):mpu6050(mpu_address)
{

};

bool MPU::Connect()
{
    //TODO: add device ID
    if (mpu6050.testConnection())
    {
        std::cout << "MPU6050 Connection Successful" << std::endl;
    }
    else
    {
        std::cout << "MPU6050 Connection Failed" << std::endl;
        return false;
    }
    

    for (int ctr = 1; ctr <= 10; ctr++)
    {
        mpu6050.initialize();
        devStatus = mpu6050.dmpInitialize();
        if (devStatus == 0)
        {
            break;
        }
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        char * StatStr[5] { "No Error", "initial memory load failed", "DMP configuration updates failed", "3", "4"};
        std::cout << "MPU init Try #" << ctr << "\nDMP Init failed (code " << StatStr[devStatus] << ")" << std::endl;

        if (ctr == 10)
        {
            std::cout << "Max MPU connection attempts reached" << std::endl;
            return false;
        }
        
        delay(1000);
    }   

    mpu6050.setXAccelOffset(MPUOffsets[0]);
    mpu6050.setYAccelOffset(MPUOffsets[1]);
    mpu6050.setZAccelOffset(MPUOffsets[2]);
    mpu6050.setXGyroOffset(MPUOffsets[3]);
    mpu6050.setYGyroOffset(MPUOffsets[4]);
    mpu6050.setZGyroOffset(MPUOffsets[5]);

    std::cout << "Enabling DMP..." << std::endl;
    mpu6050.setDMPEnabled(true);

    std::cout << "Enabling interrupt detection" << std::endl;
    mpu6050.setInterruptDrive(0);
    std::cout << "mpu.getInterruptDrive " << mpu6050.getInterruptDrive() << std::endl;

    std::cout << std::bitset<8>(mpu6050.getIntEnabled()) << std::endl;
    //wiringPiISR2(interruptPin, INT_EDGE_RISING, &ISR, 3000, this);
    //mpuIntStatus = mpu6050.getIntStatus();

    // get expected DMP packet size for later comparison
    packetSize = mpu6050.dmpGetFIFOPacketSize();
    //delay(1000); // Let it Stabalize

    //mpu6050.resetFIFO(); // Clear fifo buffer
    //mpu6050.getIntStatus();
    //interrupt = false; // wait for next interrupt
    return true;
}
/*
void MPU::GetDMP() { // Best version I have made so far
  // Serial.println(F("FIFO interrupt at:"));
  // Serial.println(micros());
  static unsigned long LastGoodPacketTime;
  interrupt = false;
  fifoAlive = 1;
  fifoCount = mpu6050.getFIFOCount();
  if ((!fifoCount) || (fifoCount % packetSize)) { // we have failed Reset and wait till next time!
    mpu6050.resetFIFO();// clear the buffer and start over
  } else {
    while (fifoCount  >= packetSize) { // Get the packets until we have the latest!
      mpu6050.getFIFOBytes(fifoBuffer, packetSize); // lets do the magic and get the data
      fifoCount -= packetSize;
    }
    LastGoodPacketTime = millis();
    MPUMath(); // <<<<<<<<<<<<<<<<<<<<<<<<<<<< On success MPUMath() <<<<<<<<<<<<<<<<<<<
  }
}

void MPU::DmpDataReady(void)
{
    interrupt=true;
}

void MPU::MPUMath()
{
    mpu6050.dmpGetQuaternion(&q, fifoBuffer);
    mpu6050.dmpGetGravity(&gravity, &q);
    //mpu6050.dmpGetYawPitchRoll(ypr, &q, &gravity);
    //float yaw = (ypr[0] * 180.0 / M_PI);
    //float pitch = (ypr[1] *  180.0 / M_PI);
    //float roll = (ypr[2] *  180.0 / M_PI);
    
    delay(100);
    Serial.print(q.w);Serial.print(",");
    Serial.print(q.x);Serial.print(",");
    Serial.print(q.y);Serial.print(",");
    Serial.print(q.z);
    Serial.print("\n");

    std::cout << q.w << ", " << q.x << ", " << q.y << ", " << q.z << std::endl;
};

void MPU::ISR(struct WPIWfiStatus wfiStatus, void* userdata)
{
    auto* mpu = static_cast<MPU*>(userdata);
    //mpu->GetDMP();
    mpu->interrupt = true;
};
*/
void MPU::Loop()
{
    std::cout << "Discarding 100 readings..." << std::endl;
    for (int i = 0; i < 100; i++)   
    {
        mpu6050.dmpGetCurrentFIFOPacket(fifoBuffer);
        delay(500);
    } 

    const int samples = 20000;
    float yaw[samples];
    float pitch[samples];
    float roll[samples];

    for (int i = 0; i < samples; i++)   
    {
        std::cout << "\nLOOP " << i << ":" << std::endl;
        if (mpu6050.dmpGetCurrentFIFOPacket(fifoBuffer))
        {
            mpu6050.dmpGetQuaternion(&q, fifoBuffer);
            std::cout << "Quaternion: " << q.w << ", " << q.x << ", " << q.y << ", " << q.z << std::endl;

            mpu6050.dmpGetGravity(&gravity, &q);
            std::cout << "Gravity :" << gravity.x << ", " << gravity.y << ", " << gravity.z << std::endl;

            mpu6050.dmpGetYawPitchRoll(ypr, &q, &gravity);
            yaw[i] = (ypr[0] * 180.0 / M_PI);
            pitch[i] = (ypr[1] *  180.0 / M_PI);
            roll[i] = (ypr[2] *  180.0 / M_PI);
            std::cout << "YPR: " << yaw[i] << ", " << pitch[i] << ", " << roll[i] << std::endl;  
        }
        else
        {
            std::cout << "READ " << i << "FAILED" << std::endl;
        }
        delay(500);
    } 

    std::cout << std::endl;

    std::cout << "YAW START: " << yaw[0] << ", YAW END: " << yaw[samples-1] << std::endl;
    auto yawMin = *std::min_element(std::begin(yaw), std::end(yaw));
    auto yawMax = *std::max_element(std::begin(yaw), std::end(yaw));
    std::cout << "YAW MIN: " << yawMin << ", YAW MAX: " << yawMax << ", RANGE: " << abs(yawMax-yawMin) << "\n" << std::endl;

    std::cout << "PITCH START: " << pitch[0] << ", PITCH END: " << pitch[samples-1] << std::endl;
    auto pitchMin = *std::min_element(std::begin(pitch), std::end(pitch));
    auto pitchMax = *std::max_element(std::begin(pitch), std::end(pitch));
    std::cout << "PITCH MIN: " << pitchMin << ", PITCH MAX: " << pitchMax << ", RANGE: " << abs(pitchMax-pitchMin) << "\n" << std::endl;

    std::cout << "ROLL START: " << roll[0] << ", ROLL END: " << roll[samples-1] << std::endl;
    auto rollMin = *std::min_element(std::begin(roll), std::end(roll));
    auto rollMax = *std::max_element(std::begin(roll), std::end(roll));
    std::cout << "ROLL MIN: " << rollMin << ", ROLL MAX: " << rollMax << ", RANGE: " << abs(rollMax-rollMin) << "\n" << std::endl;

}

int main()
{

    wiringPiSetupPinType(WPI_PIN_PHYS);
    pinMode(26, INPUT);
    I2Cdev::initialize("/dev/i2c-1");

    MPU mpu;

    mpu.Connect();
    delay(1000);
    mpu.Loop();

}

