/**************************************************************************/
/*!
    @file     Adafruit_ADS1115.cpp
    @author   K.Townsend (Adafruit Industries)

    @mainpage Adafruit ADS1115 ADC Breakout Driver

    @section intro_sec Introduction

    This is a library for the Adafruit ADS1115 ADC breakout boards.

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section author Author

    Written by Kevin "KTOWN" Townsend for Adafruit Industries.

    @section  HISTORY

    v1.0  - First release
    v1.1  - Added ADS1115 support - W. Earl
    v2.0  - Refactor - C. Nelson

    @section license License

    BSD license, all text here must be included in any redistribution
*/
/**************************************************************************/
#include "ADS1115.h"
/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1115 class w/appropriate properties
*/
/**************************************************************************/
ADS1115::ADS1115(I2Cdev& i2cDev, uint8_t address) : i2cDev(i2cDev), devAddr(address) {
}


/**************************************************************************/
/*!
    @brief  Sets the gain and input voltage range

    @param gain gain setting to use
*/
/**************************************************************************/
void ADS1115::setGain(adsGain_t gain) { m_gain = gain; }

/**************************************************************************/
/*!
    @brief  Gets a gain and input voltage range

    @return the gain setting
*/
/**************************************************************************/
adsGain_t ADS1115::getGain() { return m_gain; }

/**************************************************************************/
/*!
    @brief  Sets the data rate

    @param rate the data rate to use
*/
/**************************************************************************/
void ADS1115::setDataRate(uint16_t rate) { m_dataRate = rate; }

/**************************************************************************/
/*!
    @brief  Gets the current data rate

    @return the data rate
*/
/**************************************************************************/
uint16_t ADS1115::getDataRate() { return m_dataRate; }

/**************************************************************************/
/*!
    @brief  Gets a single-ended ADC reading from the specified channel

    @param channel ADC channel to read

    @return the ADC reading
*/
/**************************************************************************/
int16_t ADS1115::readADC_SingleEnded(uint8_t channel) {
  if (channel > 3) {
    return 0;
  }

  startADCReading(MUX_BY_CHANNEL[channel], /*continuous=*/false);

  // Wait for the conversion to complete
  while (!conversionComplete())
    ;

  // Read the conversion results
  return getLastConversionResults();
}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN1) input.  Generates
            a signed value since the difference can be either
            positive or negative.

    @return the ADC reading
*/
/**************************************************************************/
int16_t ADS1115::readADC_Differential_0_1() {
  startADCReading(ADS1115_REG_CONFIG_MUX_DIFF_0_1, /*continuous=*/false);

  // Wait for the conversion to complete
  while (!conversionComplete())
    ;

  // Read the conversion results
  return getLastConversionResults();
}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
    @return the ADC reading
*/
/**************************************************************************/
int16_t ADS1115::readADC_Differential_0_3() {
  startADCReading(ADS1115_REG_CONFIG_MUX_DIFF_0_3, /*continuous=*/false);

  // Wait for the conversion to complete
  while (!conversionComplete())
    ;

  // Read the conversion results
  return getLastConversionResults();
}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN1) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.
    @return the ADC reading
*/
/**************************************************************************/
int16_t ADS1115::readADC_Differential_1_3() {
  startADCReading(ADS1115_REG_CONFIG_MUX_DIFF_1_3, /*continuous=*/false);

  // Wait for the conversion to complete
  while (!conversionComplete())
    ;

  // Read the conversion results
  return getLastConversionResults();
}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN2) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.

    @return the ADC reading
*/
/**************************************************************************/
int16_t ADS1115::readADC_Differential_2_3() {
  startADCReading(ADS1115_REG_CONFIG_MUX_DIFF_2_3, /*continuous=*/false);

  // Wait for the conversion to complete
  while (!conversionComplete())
    ;

  // Read the conversion results
  return getLastConversionResults();
}

/**************************************************************************/
/*!
    @brief  Sets up the comparator to operate in basic mode, causing the
            ALERT/RDY pin to assert (go from high to low) when the ADC
            value exceeds the specified threshold.

            This will also set the ADC in continuous conversion mode.

    @param channel ADC channel to use
    @param threshold comparator threshold
*/
/**************************************************************************/
void ADS1115::startComparator_SingleEnded(uint8_t channel,
                                                   int16_t threshold) {
  // Start with default values
  uint16_t config =
      ADS1115_REG_CONFIG_CQUE_1CONV |   // Comparator enabled and asserts on 1
                                        // match
      ADS1115_REG_CONFIG_CLAT_LATCH |   // Latching mode
      ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1115_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
      ADS1115_REG_CONFIG_MODE_CONTIN |  // Continuous conversion mode
      ADS1115_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

  // Set PGA/voltage range
  config |= m_gain;

  // Set data rate
  config |= m_dataRate;

  config |= MUX_BY_CHANNEL[channel];

  // Set the high threshold register
  // Shift 12-bit results left 4 bits for the ADS1015
  writeRegister(ADS1115_REG_POINTER_HITHRESH, threshold << m_bitShift);

  // Write config register to the ADC
  writeRegister(ADS1115_REG_POINTER_CONFIG, config);
}

/**************************************************************************/
/*!
    @brief  In order to clear the comparator, we need to read the
            conversion results.  This function reads the last conversion
            results without changing the config value.

    @return the last ADC reading
*/
/**************************************************************************/
int16_t ADS1115::getLastConversionResults() {
  // Read the conversion results
  uint16_t res = readRegister(ADS1115_REG_POINTER_CONVERT) >> m_bitShift;
  if (m_bitShift == 0) {
    return (int16_t)res;
  } else {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if (res > 0x07FF) {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t)res;
  }
}

/**************************************************************************/
/*!
    @brief  Return the current fs range for the configured gain

    @return the fsRange for the configured gain, or zero.
            Zero should not be possible thereby indicating error
*/
/**************************************************************************/
float ADS1115::getFsRange() {
  // see data sheet Table 3
  switch (m_gain) {
  case GAIN_TWOTHIRDS:
    return 6.144f;
    break;
  case GAIN_ONE:
    return 4.096f;
    break;
  case GAIN_TWO:
    return 2.048f;
    break;
  case GAIN_FOUR:
    return 1.024f;
    break;
  case GAIN_EIGHT:
    return 0.512f;
    break;
  case GAIN_SIXTEEN:
    return 0.256f;
    break;
  default:
    return 0.0f;
  }
}

/**************************************************************************/
/*!
    @brief  Compute volts for the given raw counts.

    @param counts the ADC reading in raw counts

    @return the ADC reading in volts
*/
/**************************************************************************/
float ADS1115::computeVolts(int16_t counts) {
  return counts * (getFsRange() / (32768 >> m_bitShift));
}

/**************************************************************************/
/*!
    @brief  Non-blocking start conversion function

    Call getLastConversionResults() once conversionComplete() returns true.
    In continuous mode, getLastConversionResults() will always return the
    latest result.
    ALERT/RDY pin is set to RDY mode, and a 8us pulse is generated every
    time new data is ready.

    @param mux mux field value
    @param continuous continuous if set, otherwise single shot
*/
/**************************************************************************/
void ADS1115::startADCReading(uint16_t mux, bool continuous) {
  // Start with default values
  uint16_t config =
      ADS1115_REG_CONFIG_CQUE_1CONV |   // Set CQUE to any value other than
                                        // None so we can use it in RDY mode
      ADS1115_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
      ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1115_REG_CONFIG_CMODE_TRAD;    // Traditional comparator (default val)

  if (continuous) {
    config |= ADS1115_REG_CONFIG_MODE_CONTIN;
  } else {
    config |= ADS1115_REG_CONFIG_MODE_SINGLE;
  }

  // Set PGA/voltage range
  config |= m_gain;

  // Set data rate
  config |= m_dataRate;

  // Set channels
  config |= mux;

  // Set 'start single-conversion' bit
  config |= ADS1115_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister(ADS1115_REG_POINTER_CONFIG, config);

  // Set ALERT/RDY to RDY mode.
  writeRegister(ADS1115_REG_POINTER_HITHRESH, 0x8000);
  writeRegister(ADS1115_REG_POINTER_LOWTHRESH, 0x0000);
}

/**************************************************************************/
/*!
    @brief  Returns true if conversion is complete, false otherwise.

    @return True if conversion is complete, false otherwise.
*/
/**************************************************************************/
bool ADS1115::conversionComplete() {
  return (readRegister(ADS1115_REG_POINTER_CONFIG) & 0x8000) != 0;
}

/**************************************************************************/
/*!
    @brief  Writes 16-bits to the specified destination register

    @param reg register address to write to
    @param value value to write to register
*/
/**************************************************************************/
void ADS1115::writeRegister(uint8_t reg, uint16_t value) {
  /*
  buffer[0] = reg;
  buffer[1] = value >> 8;
  buffer[2] = value & 0xFF;
  m_i2c_dev->write(buffer, 3);
  */
  i2cDev.writeWord(devAddr, reg, value);
}

/**************************************************************************/
/*!
    @brief  Read 16-bits from the specified destination register

    @param reg register address to read from

    @return 16 bit register value read
*/
/**************************************************************************/
uint16_t ADS1115::readRegister(uint8_t reg) {
  
  /*
  buffer[0] = reg;
  m_i2c_dev->write(buffer, 1);
  m_i2c_dev->read(buffer, 2);
  return ((buffer[0] << 8) | buffer[1]);
  */
  uint16_t ret;
  i2cDev.readWord(devAddr, reg, &ret);
  return ret;
}
