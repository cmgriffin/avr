#ifndef __ADS111X__
#define __ADS111X__

#include <avrlibdefs.h>

#define ADS111X_REG_CONV               0x0
#define ADS111X_REG_CONFIG             0x1
#define ADS111X_REG_LO_THRESH          0x2
#define ADS111X_REG_HI_THRESH          0x3

#define ADS111X_CONFIG_OS_MASK         (1 << 15)

#define ADS111X_CONFIG_MUX_MASK        (7 << 12)
#define ADS111X_CONFIG_MUX_A0_A1       (0 << 12)
#define ADS111X_CONFIG_MUX_A0_A3       (1 << 12)
#define ADS111X_CONFIG_MUX_A1_A3       (2 << 12)
#define ADS111X_CONFIG_MUX_A2_A3       (3 << 12)
#define ADS111X_CONFIG_MUX_A0_GND      (4 << 12)
#define ADS111X_CONFIG_MUX_A1_GND      (5 << 12)
#define ADS111X_CONFIG_MUX_A2_GND      (6 << 12)
#define ADS111X_CONFIG_MUX_A3_GND      (7 << 12)

#define ADS111X_CONFIG_PGA_MASK        (7 << 9)
#define ADS111X_CONFIG_PGA_6144FSR     (0 << 9)
#define ADS111X_CONFIG_PGA_4096FSR     (1 << 9)
#define ADS111X_CONFIG_PGA_2048FSR     (2 << 9)
#define ADS111X_CONFIG_PGA_1024FSR     (3 << 9)
#define ADS111X_CONFIG_PGA_0512FSR     (4 << 9)
#define ADS111X_CONFIG_PGA_0256FSR     (5 << 9)

#define ADS111X_CONFIG_MODE_MASK       (1 << 8)
#define ADS111X_CONFIG_MODE_CONTINUOUS (0 << 8)
#define ADS111X_CONFIG_MODE_SINGLE     (1 << 8)

#define ADS111X_CONFIG_DR_MASK         (3 << 5)
#define ADS111X_CONFIG_DR_8SPS         (0 << 5)
#define ADS111X_CONFIG_DR_16SPS        (1 << 5)
#define ADS111X_CONFIG_DR_32SPS        (2 << 5)
#define ADS111X_CONFIG_DR_64SPS        (3 << 5)
#define ADS111X_CONFIG_DR_128SPS       (4 << 5)
#define ADS111X_CONFIG_DR_250PS        (5 << 5)
#define ADS111X_CONFIG_DR_475SPS       (6 << 5)
#define ADS111X_CONFIG_DR_860SPS       (7 << 5)

#define ADS111X_CONFIG_COMP_MODE_MASK  (1 << 4)
#define ADS111X_CONFIG_COMP_POL_MASK   (1 << 3)
#define ADS111X_CONFIG_COMP_LAT_MASK   (1 << 2)
#define ADS111X_CONFIG_COMP_QUE_MASK   (3 << 0)

/**
 * @brief write val to a register in the device
 *
 * @param i2cAddr 7-bit style address of the device
 * @param regAddr register to write to
 * @param val value to be written
 * @return uint8_t i2c transaction status
 */
uint8_t ADS111X_writeReg(uint8_t i2cAddr, uint8_t regAddr, uint16_t val);

/**
 * @brief read a val from a register in the device
 *
 * @param i2cAddr 7-bit style address of the device
 * @param regAddr register to read from
 * @param readVal pointer to variable in which to store value read
 * @return uint8_t i2c transaction status
 */
uint8_t ADS111X_readReg(uint8_t i2cAddr, uint8_t regAddr, uint16_t *readVal);

/**
 * @brief start a conversion withe curent configuration
 *
 * @param i2cAddr 7-bit style address of the device
 */
void ADS111X_startConversion(uint8_t i2cAddr);

/**
 * @brief check if the conversion has completed
 *
 * @param i2cAddr 7-bit style address of the device
 * @return true conversion is complete
 * @return false conversion is ongoing
 */
bool ADS111X_conversionComplete(uint8_t i2cAddr);

/**
 * @brief get the raw measurement from the current config
 * this is a blocking routine that starts a conversion then
 * waits for it to finish
 *
 * @param i2cAddr 7-bit style address of the device
 * @return int16_t raw signed measurement
 */
int16_t ADS111X_measureCurrent(uint8_t i2cAddr);

/**
 * @brief fetch the current value in the conversion register
 *
 * @param i2cAddr 7-bit style address of the device
 * @return int16_t raw signed measurement
 */
int16_t ADS111X_fetchCurrent(uint8_t i2cAddr);

/**
 * @brief measure voltage from current config (blocking)
 *
 * @param i2cAddr 7-bit style address of the device
 * @param fsrVolts the configured full scale range in volts
 * @return double measurement in volts
 */
double ADS111X_measureVolts(uint8_t i2cAddr, double fsrVolts);

/**
 * @brief update the config register based on the values provided
 *
 * @param i2cAddr 7-bit style address of the device
 * @param config_val new values to update the config register with
 * @param config_mask mask of new value fields
 */
void ADS111X_updateConfig(uint8_t i2cAddr, uint16_t config_val,
                          uint16_t config_mask);

#endif // __ADS111X__