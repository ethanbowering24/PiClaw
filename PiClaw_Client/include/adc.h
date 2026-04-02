#pragma once
#include "I2Cdev.h"
#include "ADS1115.h"

class ADC
{
    private:
        I2Cdev &i2cDev;
        ADS1115 adc1115;

        static int ReadingToPercentage(int reading);
        


    public:
        ADC(I2Cdev &i2cDev);
        bool Connect();
        int16_t Read();

};