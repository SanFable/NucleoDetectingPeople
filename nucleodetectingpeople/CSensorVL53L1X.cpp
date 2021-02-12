#include "mbed.h"
#include "CSensorVL53L1X.h"


    int defineSensor(VL53L1X_DevI2C *mDevice_i2c, DigitalOut *mXshutdown, PinName mInterrupt, uint8_t mSensorAdress, uint16_t mTimingBudgetInMs)
    {
       mDevice_i2c = mDevice_i2c;
       mXshutdown = new DigitalOut(*mXshutdown);
       mInterrupt = *new PinName(mInterrupt);
       mSensorAdress = mSensorAdress;
       mTimingBudgetInMs = mTimingBudgetInMs;
       return 0;
    }


    


    int GetData();