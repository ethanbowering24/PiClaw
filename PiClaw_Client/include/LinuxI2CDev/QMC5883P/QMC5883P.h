#ifndef _QMC5883P_H_
#define _QMC5883P_H_

#include "I2Cdev.h"

#define QMC5883P_DEFAULT_ADDRESS 0x2C ///< Default I2C address

#define QMC5883P_RA_CHIPID 0x00   ///< Chip ID register
#define QMC5883P_RA_XOUT_LSB 0x01 ///< X-axis output LSB register
#define QMC5883P_RA_XOUT_MSB 0x02 ///< X-axis output MSB register
#define QMC5883P_RA_YOUT_LSB 0x03 ///< Y-axis output LSB register
#define QMC5883P_RA_YOUT_MSB 0x04 ///< Y-axis output MSB register
#define QMC5883P_RA_ZOUT_LSB 0x05 ///< Z-axis output LSB register
#define QMC5883P_RA_ZOUT_MSB 0x06 ///< Z-axis output MSB register
#define QMC5883P_RA_STATUS 0x09   ///< Status register
#define QMC5883P_RA_CONTROL1 0x0A ///< Control register 1
#define QMC5883P_RA_CONTROL2 0x0B ///< Control register 2

#define QMC5883_DRDY_BIT 0
#define QMC5883_OVFL_BIT 1

#define QMC5883P_CHIPID_BIT 7
#define QMC5883P_CHIPID_LENGTH 8

#define QMC5583P_CTRL1_MODE_BIT 1
#define QMC5583P_CTRL1_MODE_LENGTH 2

#define QMC5583P_CTRL1_ODR_BIT 3
#define QMC5583P_CTRL1_ODR_LENGTH 2

#define QMC5583P_CTRL1_OSR1_BIT 5
#define QMC5583P_CTRL1_OSR1_LENGTH 2

#define QMC5583P_CTRL1_OSR2_BIT 7
#define QMC5583P_CTRL1_OSR2_LENGTH 2

#define QMC5583P_CTRL2_RNG_BIT 3
#define QMC5583P_CTRL2_RNG_LENGTH 2

#define QMC5583P_CTRL2_SR_BIT 1
#define QMC5583P_CTRL2_SR_LENGTH 2

#define QMC5583P_CTRL2_SELFTEST_BIT 6
#define QMC5583P_CTRL2_SOFTRESET_BIT 7


class QMC5883P {
    public:
        QMC5883P(I2Cdev& i2cDev, uint8_t address=QMC5883P_DEFAULT_ADDRESS);
        void initialize();
        bool testConnection();
        uint8_t getDeviceID();
        
        void getRawMagnetic(int16_t *x, int16_t *y, int16_t *z);
        void getGaussField(float *x, float *y, float *z);
        bool isDataReady();
        bool isOverflow();
        void setMode(uint8_t mode);         
        uint8_t getMode();                  
        void setODR(uint8_t odr);           
        uint8_t getODR();                   
        void setOSR(uint8_t osr);           
        uint8_t getOSR();                   
        void setDSR(uint8_t dsr);           
        uint8_t getDSR();                   
        void softReset();                   
        bool selfTest();
        void setRange(uint8_t range);
        uint8_t getRange(); 
        void setSetResetMode(uint8_t mode); 
        uint8_t getSetResetMode();          

    protected:
        uint8_t devAddr;
        uint8_t buffer[14];
    
    private:
        I2Cdev& i2cDev; 
};


#endif /* _QMC5883P_H_ */