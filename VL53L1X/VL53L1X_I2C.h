/* Define to prevent from recursive inclusion --------------------------------*/
#ifndef __DEV_53L1X_I2C_H
#define __DEV_53L1X_I2C_H

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include "pinmap.h"

//Class replacing DevI2C class as it was not implementing a 16bit address registers
class VL53L1X_DevI2C : public I2C
{
public:
    /** Create a DevI2C Master interface, connected to the specified pins
     *
     *  @param sda I2C data line pin
     *  @param scl I2C clock line pin
     */
    VL53L1X_DevI2C(PinName sda, PinName scl) : I2C(sda, scl) {}

    /**
     * @brief  Writes a buffer towards the I2C peripheral device.
     * @param  pBuffer pointer to the byte-array data to send
     * @param  DeviceAddr specifies the peripheral device slave address.
     * @param  RegisterAddr specifies the internal address register
     *         where to start writing to (must be correctly masked).
     * @param  NumByteToWrite number of bytes to be written.
     * @retval 0 if ok,
     * @retval -1 if an I2C error has occured, or
     * @retval -2 on temporary buffer overflow (i.e. NumByteToWrite was too high)
     * @note   On some devices if NumByteToWrite is greater
     *         than one, the RegisterAddr must be masked correctly!
     */
    int v53l1x_i2c_write(uint8_t *pBuffer, uint8_t DeviceAddr, uint16_t RegisterAddr,
                         uint16_t NumByteToWrite)
    {
        int ret;
        uint8_t tmp[TEMP_BUF_SIZE];

        if (NumByteToWrite >= TEMP_BUF_SIZE)
            return -2;

        /* First, send device address. Then, send data and STOP condition */
        tmp[0] = RegisterAddr >> 8;
        tmp[1] = RegisterAddr & 0x0FF;
        memcpy(tmp + 2, pBuffer, NumByteToWrite);

        ret = write(DeviceAddr, (const char *)tmp, NumByteToWrite + 2, false);

        if (ret)
            return -1;
        return 0;
    }

    /**
     * @brief  Reads a buffer from the I2C peripheral device.
     * @param  pBuffer pointer to the byte-array to read data in to
     * @param  DeviceAddr specifies the peripheral device slave address.
     * @param  RegisterAddr specifies the internal address register
     *         where to start reading from (must be correctly masked).
     * @param  NumByteToRead number of bytes to be read.
     * @retval 0 if ok,
     * @retval -1 if an I2C error has occured
     * @note   On some devices if NumByteToWrite is greater
     *         than one, the RegisterAddr must be masked correctly!
     */
    int v53l1x_i2c_read(uint8_t *pBuffer, uint8_t DeviceAddr, uint16_t RegisterAddr,
                        uint16_t NumByteToRead)
    {
        int ret;
        uint8_t ExpanderData[2];
        ExpanderData[0] = RegisterAddr >> 8;
        ExpanderData[1] = RegisterAddr & 0x0FF;
        /* Send device address, with no STOP condition */
        ret = write(DeviceAddr, (const char *)ExpanderData, 2, true);
        if (!ret) {
            /* Read data, with STOP condition  */
            ret = read(DeviceAddr, (char *)pBuffer, NumByteToRead, false);
        }

        if (ret)
            return -1;
        return 0;
    }

private:
    static const unsigned int TEMP_BUF_SIZE = 32;
};

#endif /* __DEV_53L1X_I2C_H */
