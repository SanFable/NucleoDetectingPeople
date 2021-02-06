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


    

 int CSensorBase::Init(VL53L1X_DevI2C *mDevice_i2c, DigitalOut mXshutdown, PinName mInterrupt, uint8_t mSensorAdress, uint16_t mTimingBudgetInMs)
    {
        VL53L1X oSensor(mDevice_i2c, mXshutdown, mInterrupt) 
        oSensor.init_sensor(mSensorAdress);
        oSensor.vl53l1x_set_timing_budget_in_ms(mTimingBudgetInMs);
	    oSensor.vl53l1x_set_inter_measurement_in_ms(mTimingBudgetInMs);
        oSensor.vl53l1x_set_distance_mode(1);
        oSensor.vl53l1x_start_ranging();
    }
    int GetData();