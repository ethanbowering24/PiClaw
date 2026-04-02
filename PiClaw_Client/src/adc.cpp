#include "adc.h"
#include "ADS1115.h"
#include "I2Cdev.h"

bool ADC::Connect()
{

    /*
    if (!adc1115.testConnection())
    {
        return false;
    }

    adc1115.initialize();
    */ //TODO: THESE ARENT IMPLEMENTED!!
    
    adc1115.setGain(GAIN_TWOTHIRDS); // 6.144 V max
    adc1115.setDataRate(RATE_ADS1115_128SPS); // 128 samples/sec
    return true;

}

int16_t ADC::Read()
{
    //adc1115.computeVolts(adc1115.readADC_SingleEnded(0));
    return adc1115.readADC_SingleEnded(0);
}

//TODO: scale this reading into a percentage