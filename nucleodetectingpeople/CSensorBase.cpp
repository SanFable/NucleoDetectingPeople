#include "mbed.h"
#include "CSensorBase.h"

 int Init(VL53L1X_DevI2C *mDevice_i2c, DigitalOut *mXshutdown, PinName mInterrupt, uint8_t mSensorAdress, uint16_t mTimingBudgetInMs)
    {
        CSensorBase oSensor = CSensorBase(*mDevice_i2c, *mXshutdown, mInterrupt, mSensorAdress, mTimingBudgetInMs); 
        oSensor.init_sensor(mSensorAdress);
        oSensor.vl53l1x_set_timing_budget_in_ms(mTimingBudgetInMs);
	    oSensor.vl53l1x_set_inter_measurement_in_ms(mTimingBudgetInMs);
        oSensor.vl53l1x_set_distance_mode(1);
        oSensor.vl53l1x_start_ranging();
    }