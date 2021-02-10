#include "mbed.h"
#include "CSensorVL53L1X.h"


    CSensorVL53L1X::CSensorVL53L1X(VL53L1X_DevI2C *device_i2c, DigitalOut xshutdown, PinName interrupt, uint8_t sensorAdress, uint16_t timingBudgetInMs) : CSensorBase()
    {
       mDevice_i2c = device_i2c;
       mXshutdown = new DigitalOut(xshutdown);
       mInterrupt = new DigitalIn(interrupt);
       mSensorAdress = sensorAdress;
       mTimingBudgetInMs = timingBudgetInMs;
       
    }


    


    int GetData();