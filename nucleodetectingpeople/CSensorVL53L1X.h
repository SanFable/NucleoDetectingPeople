#include "CSensorBase.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include <cstdint>
class CSensorVL53L1X : public CSensorBase
{
    public:
    CSensorVL53L1X(VL53L1X_DevI2C *device_i2c, DigitalOut xshutdown, PinName interrupt, uint8_t sensorAdress, uint16_t timingBudgetInMs);
    ~CSensorVL53L1X();
    VL53L1X oSensor(VL53L1X_DevI2C*, DigitalOut, PinName); 
   

private:
    VL53L1X_DevI2C *mDevice_i2c;
    DigitalOut * mXshutdown;
    DigitalIn * mInterrupt;
    uint8_t mSensorAdress;   
    uint16_t mTimingBudgetInMs;

    
};
