#include "QMC5883P.h"

/** Specific address constructor.
 * @param address I2C address, uses default I2C address if none is specified
 */
QMC5883P::QMC5883P(I2Cdev& i2cDev, uint8_t address) : i2cDev(i2cDev), devAddr(address) {

}

void QMC5883P::initialize() {

}

bool QMC5883P::testConnection() {
    return getDeviceID() == 0x80;
}

/** Get Device ID.
 * This register is used to verify the identity of the device (0b110100, 0x34).
 * @return Device ID (6 bits only! should be 0x34)
 * @see MPU6050_RA_WHO_AM_I
 * @see MPU6050_WHO_AM_I_BIT
 * @see MPU6050_WHO_AM_I_LENGTH
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
 * The FS_SEL parameter allows setting the full-scale range of the gyro sensors,
 * as described in the table below.
 *
 * <pre>
 * 0 = +/- 30 Gauss
 * 1 = +/- 12 Gauss
 * 2 = +/- 8 Gauss
 * 3 = +/- 2 Gauss
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

