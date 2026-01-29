#include "mpu.h"
#include "I2Cdev.h"
#include "wiringPi.h"
#include <iostream>

MPU::MPU():mpu6050()
{

};

MPU::MPU(uint8_t mpu_address):mpu6050(mpu_address)
{

};

bool MPU::Connect()
{
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
        std::cout << "MPU connection Try #" << ctr << "\nDMP Initialization failed (code " << StatStr[devStatus] << ")" << std::endl;
        delay(1000);
    }

    if (devStatus != 0)
    {
        std::cout << "Max MPU connection attempts reached" << std::endl;
        return false;
    }    

    mpu6050.setXAccelOffset(MPUOffsets[0]);
    mpu6050.setYAccelOffset(MPUOffsets[1]);
    mpu6050.setZAccelOffset(MPUOffsets[2]);
    mpu6050.setXGyroOffset(MPUOffsets[3]);
    mpu6050.setYGyroOffset(MPUOffsets[4]);
    mpu6050.setZGyroOffset(MPUOffsets[5]);

    //Serial.println(F("Enabling DMP..."));
    std::cout << "Enabling DMP..." << std::endl;
    mpu6050.setDMPEnabled(true);
    // enable Arduino interrupt detection
    std::cout << "Enabling interrupt detection (Arduino external interrupt pin 2 on the Uno)..." << std::endl;
    std::cout << "mpu.getInterruptDrive " << mpu6050.getInterruptDrive() << std::endl;
    //attachInterrupt(digitalPinToInterrupt(interruptPin), dmpDataReady, RISING);
   // wiringPiISR(interruptPin, INT_EDGE_RISING, &DmpDataReady);
    wiringPiISR2(interruptPin, INT_EDGE_RISING, &ISR, 3000, this);
    mpuIntStatus = mpu6050.getIntStatus(); // Same
    // get expected DMP packet size for later comparison
    packetSize = mpu6050.dmpGetFIFOPacketSize();
    delay(1000); // Let it Stabalize
    mpu6050.resetFIFO(); // Clear fifo buffer
    mpu6050.getIntStatus();
    interrupt = false; // wait for next interrupt
    return true;
}

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
    mpu6050.dmpGetYawPitchRoll(ypr, &q, &gravity);
    //float yaw = (ypr[0] * 180.0 / M_PI);
    //float pitch = (ypr[1] *  180.0 / M_PI);
    //float roll = (ypr[2] *  180.0 / M_PI);
    
    /*delay(100);
    Serial.print(q.w);Serial.print(",");
    Serial.print(q.x);Serial.print(",");
    Serial.print(q.y);Serial.print(",");
    Serial.print(q.z);
    Serial.print("\n");*/

    std::cout << q.w << ", " << q.x << ", " << q.y << ", " << q.z << std::endl;
};

void MPU::ISR(struct WPIWfiStatus wfiStatus, void* userdata)
{
    auto* mpu = static_cast<MPU*>(userdata);
    //mpu->GetDMP();
    mpu->interrupt = true;
};

void MPU::Loop()
{
    static unsigned long _ETimer;
    while (true)
        {
            if ( millis() - _ETimer >= (100)) {
            _ETimer += (10);
            interrupt = true;
        }
        if (interrupt ) { // wait for MPU interrupt or extra packet(s) available
            GetDMP();
        }
    }
    
    
}

int main()
{
    I2Cdev::initialize("/dev/i2c-1");

    MPU mpu;

    mpu.Connect();
    mpu.Loop();

}

