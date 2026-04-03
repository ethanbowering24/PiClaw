#pragma once
#include "I2Cdev.h"
#include "ADS1115.h"

class ADC
{
    private:
        I2Cdev &i2cDev;
        ADS1115 adc1115;

        static float ReadingToPercentage(float reading);
        


    public:
        ADC(I2Cdev &i2cDev);
        bool Connect();
        float Read();

};