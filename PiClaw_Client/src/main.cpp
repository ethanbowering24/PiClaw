#include "ADS1115.h"
#include "I2Cdev.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>

#include <string>
#include <vector>
#include <algorithm>

#define I2C_ADDRESS 0x48          // ADDR pin: 0 => 0x48, 1 => 0x49
#define MIN_CLAW_SERVO_ANGLE 120  // Input the minimum servo angle for the claw
#define MAX_CLAW_SERVO_ANGLE 180  // Input the maximum servo angle for the claw
#define MAX_VOLTAGE 5             // peak output voltage of 5V

int convert_to_servo_angle(double voltage);

int main(int argc, char* argv[])
{
    I2Cdev i2c1("/dev/i2c-1");
    ADS1115 adc(i2c1);
    
    adc.setGain(GAIN_TWOTHIRDS); // 6.144 V max
    adc.setDataRate(RATE_ADS1115_128SPS); // 128 samples/sec

    while (true) {
        // read raw ADC counts from channel 0
        int16_t counts = adc.readADC_SingleEnded(0);

        // convert adc values to voltage 
        float voltage = adc.computeVolts(counts);

        // map volts to servo angle
        int angle = convert_to_servo_angle(static_cast<double>(voltage));

        // print results
        std::cout << "Counts: " << counts
                  << ", Voltage: " << voltage
                  << " V, Servo Angle: " << angle << std::endl;

        // delay for 100ms
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

int convert_to_servo_angle(double voltage) {
    double scale = static_cast<double>(MAX_CLAW_SERVO_ANGLE - MIN_CLAW_SERVO_ANGLE) / static_cast<double>(MAX_VOLTAGE);
    double angle = static_cast<double>(MIN_CLAW_SERVO_ANGLE) + scale * voltage;
    if (angle < MIN_CLAW_SERVO_ANGLE) angle = MIN_CLAW_SERVO_ANGLE;
    if (angle > MAX_CLAW_SERVO_ANGLE) angle = MAX_CLAW_SERVO_ANGLE;
    return static_cast<int>(std::round(angle));
}


