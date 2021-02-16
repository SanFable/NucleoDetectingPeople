#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include <cstdint>
class CSensorVL53L1X
{
    public:
    VL53L1X m_oSensor;
    CSensorVL53L1X(VL53L1X_DevI2C *device_i2c, DigitalOut *xshutdown, PinName interrupt, uint8_t sensorAdress);
    ~CSensorVL53L1X();

    VL53L1X_DevI2C *mDevice_i2c;
    DigitalOut * mXshutdown;
    PinName mInterrupt;
    uint8_t mSensorAdress;   
    uint16_t mTimingBudgetInMs;
    int Init();

    private:



    
    
};
