#include "mbed.h"
#include "CSensorVL53L1X.h"


    int Init(VL53L1X_DevI2C *mDevice_i2c, DigitalOut mXshutdown, PinName mInterrupt, uint8_t mSensorAdress, uint16_t mTimingBudgetInMs)
    {
        oSensorVector.add(CSensorVL53L1X(mDevice_i2c, &mXshutdown, PA_3, 0x52,15));
        oSensorVector.add(CSensorVL53L1X(mDevice_i2c, &mXshutdown, PA_3));
    }