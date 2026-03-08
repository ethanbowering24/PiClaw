#include "QMC5883P.h"
#include <time.h>


/** Specific address constructor.
 * @param address I2C address, uses default I2C address if none is specified
 */
QMC5883P::QMC5883P(I2Cdev& i2cDev, uint8_t address) : i2cDev(i2cDev), devAddr(address) {

}

void QMC5883P::initialize() {
    softReset();
}

bool QMC5883P::testConnection() {
    return getDeviceID() == 0x80;
}

/** Get Device ID.
 * This register is used to verify the identity of the device (0b10000000, 0x80).
 * @return Device ID (should be 0x80)
 */
uint8_t QMC5883P::getDeviceID() {
    i2cDev.readBits(devAddr, QMC5883P_RA_CHIPID, QMC5883P_CHIPID_BIT, QMC5883P_CHIPID_LENGTH, buffer, i2cDev.readTimeout);
    return buffer[0];
}

/** Get raw magetic readings.
 * Retrieves all currently available motion sensor values.
 * @param x 16-bit signed integer container for magetometer X-axis value
 * @param y 16-bit signed integer container for magetometer y-axis value
 * @param z 16-bit signed integer container for magetometer Z-axis value
 */
void QMC5883P::getRawMagnetic(int16_t *x, int16_t *y, int16_t *z) {
    i2cDev.readBytes(devAddr, QMC5883P_RA_XOUT_LSB, 6, buffer, i2cDev.readTimeout);
    *x = (((int16_t)buffer[1]) << 8) | buffer[0];
    *y = (((int16_t)buffer[3]) << 8) | buffer[2];
    *z = (((int16_t)buffer[5]) << 8) | buffer[4];
}

void QMC5883P::getGaussField(float *x, float *y, float *z) {
  int16_t raw_x, raw_y, raw_z;

  // Get raw magnetic data
  getRawMagnetic(&raw_x, &raw_y, &raw_z);


  // Get current range to determine conversion factor
  uint8_t range = getRange();
  float lsb_per_gauss;

  switch (range) {
  case 0x00:
    lsb_per_gauss = 1000.0;
    break;
  case 0x01:
    lsb_per_gauss = 2500.0;
    break;
  case 0x02:
    lsb_per_gauss = 3750.0;
    break;
  case 0x03:
    lsb_per_gauss = 15000.0;
    break;
  }

  // Convert to Gauss
  *x = (float)raw_x / lsb_per_gauss;
  *y = (float)raw_y / lsb_per_gauss;
  *z = (float)raw_z / lsb_per_gauss;

}

/** Get magnetometer range.
 * The RNG parameter allows setting the range of the magnetometer sensors,
 * as described in the table below.
 *
 * <pre>
 * 0x00 = +/- 30 Gauss
 * 0x01 = +/- 12 Gauss
 * 0x02 = +/- 8 Gauss
 * 0x03 = +/- 2 Gauss
 * </pre>
 *
 * @return Current magnetometer range setting
 */
uint8_t QMC5883P::getRange() {
    i2cDev.readBits(devAddr, QMC5883P_RA_CONTROL2, QMC5583P_CTRL2_RNG_BIT, QMC5583P_CTRL2_RNG_LENGTH, buffer, i2cDev.readTimeout);
    return buffer[0];
}

/** Set magnetometer range.
 * @param range New magnetometer range value
 * @see getRange()
 */
void QMC5883P::setRange(uint8_t range) {
    i2cDev.writeBits(devAddr, QMC5883P_RA_CONTROL2, QMC5583P_CTRL2_RNG_BIT, QMC5583P_CTRL2_RNG_LENGTH, range);
}


/** Get magnetometer mode.
 * The MODE parameter allows setting the mode of the magnetometer sensors,
 * as described in the table below.
 *
 * <pre>
 * 0x00 = Suspend
 * 0x01 = Normal mode
 * 0x02 = Single
 * 0x03 = Continuous mode
 * </pre>
 *
 * @return Current magnetometer mode setting
 */
uint8_t QMC5883P::getMode() {
    i2cDev.readBits(devAddr, QMC5883P_RA_CONTROL1, QMC5583P_CTRL1_MODE_BIT, QMC5583P_CTRL1_MODE_LENGTH, buffer, i2cDev.readTimeout);
    return buffer[0];
}

/** Set magnetometer mode.
 * @param mode New magnetometer mode value
 * @see getMode()
 */
void QMC5883P::setMode(uint8_t mode) {
    i2cDev.writeBits(devAddr, QMC5883P_RA_CONTROL1, QMC5583P_CTRL1_MODE_BIT, QMC5583P_CTRL1_MODE_LENGTH, mode);
}

/** Get magnetometer ODR
 * @return Current magentometer ODR setting
 * 
 * <pre>
 * 0x00 = 10Hz
 * 0x01 = 50Hz
 * 0x02 = 100Hz
 * 0x03 = 200Hz
 * </pre>
 * 
 */
uint8_t QMC5883P::getODR() {
    i2cDev.readBits(devAddr, QMC5883P_RA_CONTROL1, QMC5583P_CTRL1_ODR_BIT, QMC5583P_CTRL1_ODR_LENGTH, buffer, i2cDev.readTimeout);
    return buffer[0];
}

/** Set magnetometer ODR.
 * @param odr New magnetometer ODR value
 * @see getODR()
 */
void QMC5883P::setODR(uint8_t odr) {
    i2cDev.writeBits(devAddr, QMC5883P_RA_CONTROL1, QMC5583P_CTRL1_ODR_BIT, QMC5583P_CTRL1_ODR_LENGTH, odr);
}

/** Get magnetometer OSR
 * @return Current magentometer OSR setting
 * 
 * <pre>
 * 0x00 = 8
 * 0x01 = 4
 * 0x02 = 2
 * 0x03 = 1
 * </pre>
 * 
 */
uint8_t QMC5883P::getOSR() {
    i2cDev.readBits(devAddr, QMC5883P_RA_CONTROL1, QMC5583P_CTRL1_OSR1_BIT, QMC5583P_CTRL1_OSR1_LENGTH, buffer, i2cDev.readTimeout);
    return buffer[0];
}

/** Set magnetometer OSR
 * @param osr New magnetometer OSR value
 * @see getOSR() 
 */
void QMC5883P::setOSR(uint8_t osr) {
    i2cDev.writeBits(devAddr, QMC5883P_RA_CONTROL1, QMC5583P_CTRL1_OSR1_BIT, QMC5583P_CTRL1_OSR1_LENGTH, osr);
}

/** Get magnetometer DSR
 * @return Current magnetometer DSR setting
 * 
 * <pre>
 * 0x00 = 1
 * 0x01 = 2
 * 0x02 = 4
 * 0x03 = 8
 * </pre>
 * 
 */
uint8_t QMC5883P::getDSR() {
    i2cDev.readBits(devAddr, QMC5883P_RA_CONTROL1, QMC5583P_CTRL1_OSR2_BIT, QMC5583P_CTRL1_OSR2_LENGTH, buffer, i2cDev.readTimeout);
    return buffer[0];
}

/** Set magnetometer DSR
 * @param dsr New magnetometer DSR value
 * @see getDSR() 
 */
void QMC5883P::setDSR(uint8_t dsr) {
    i2cDev.writeBits(devAddr, QMC5883P_RA_CONTROL1, QMC5583P_CTRL1_OSR2_BIT, QMC5583P_CTRL1_OSR2_LENGTH, dsr);
}

/** Check if data is ready
 * @return True if data is ready, false otherwise
 */
bool QMC5883P::isDataReady() {
    i2cDev.readBit(devAddr, QMC5883P_RA_STATUS, QMC5883_DRDY_BIT, buffer, i2cDev.readTimeout);
    return buffer[0];
}

/** Check if buffer has overflown
 * @return True if buffer has overflown, false otherwise
 */
bool QMC5883P::isOverflow() {
    i2cDev.readBit(devAddr, QMC5883P_RA_STATUS, QMC5883_OVFL_BIT, buffer, i2cDev.readTimeout);
    return buffer[0];
}

/** Get current magnetometer Set/Reset mode
 *
 * <pre>
 * 0x00 = Set and reset on
 * 0x01 = Set on only
 * 0x02 = Set and reset off
 * 0x03 = Set and reset off
 * </pre>
 *
 * @return Current magnetometer Set/Reset mode
 */
uint8_t QMC5883P::getSetResetMode() {
    i2cDev.readBits(devAddr, QMC5883P_RA_CONTROL2, QMC5583P_CTRL2_SR_BIT, QMC5583P_CTRL2_SR_LENGTH, buffer, i2cDev.readTimeout);
    return buffer[0];
}

/** Set magnetometer set/Reset mode
 * @param mode New magnetometer Set/Reset mode
 * @see getSetResetMode()
 */
void QMC5883P::setSetResetMode(uint8_t mode) {
    i2cDev.writeBits(devAddr, QMC5883P_RA_CONTROL2, QMC5583P_CTRL2_SR_BIT, QMC5583P_CTRL2_SR_LENGTH, mode);
}

/*!
 *  @brief  Performs a soft reset of the chip
 *  @return True if reset is successful
 */
void QMC5883P::softReset() {
    i2cDev.writeBit(devAddr, QMC5883P_RA_CONTROL2, QMC5583P_CTRL2_SOFTRESET_BIT, 1);
    nanosleep((const struct timespec[]){{0, 50000000L}}, NULL);
    i2cDev.writeBit(devAddr, QMC5883P_RA_CONTROL2, QMC5583P_CTRL2_SOFTRESET_BIT, 0); //I don't know why but this seems to be needed or else the device will always be stuck
    //i2cDev.readBit(devAddr, QMC5883P_RA_CONTROL2, QMC5583P_CTRL2_SOFTRESET_BIT, buffer);
    //return buffer[0]==0;
}

/*!
 *  @brief  Performs self-test of the chip
 *  @return True if self-test passed, false otherwise
 */
bool QMC5883P::selfTest() {
    i2cDev.writeBit(devAddr, QMC5883P_RA_CONTROL2, QMC5583P_CTRL2_SELFTEST_BIT, 1);
    nanosleep((const struct timespec[]){{0, 5000000L}}, NULL);
    i2cDev.readBit(devAddr, QMC5883P_RA_CONTROL2, QMC5583P_CTRL2_SELFTEST_BIT, buffer);
    return buffer[0]==0;
}