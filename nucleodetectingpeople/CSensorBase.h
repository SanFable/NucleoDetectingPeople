
#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
class CSensorBase : VL53L1X
{
    public:
    CSensorBase(VL53L1X_DevI2C *mDevice_i2c, DigitalOut *mXshutdown, PinName mInterrupt, uint8_t mSensorAdress, uint16_t mTimingBudgetInMs) : VL53L1X(mDevice_i2c, mXshutdown, mInterrupt, mSensorAdress)
       //CDataHandler* DataHandlerPtr) : poDataHandler(DataHandlerPtr)
    {
        
    }
    ~CSensorBase();

    int Init(VL53L1X_DevI2C *mDevice_i2c, DigitalOut *mXshutdown, PinName mInterrupt, uint8_t mSensorAdress, uint16_t mTimingBudgetInMs);
    virtual int GetData();
    virtual int SetDataToDataHandler();
    //jakies inne metody

   // private:
    //CDataHandler* poDataHandler;

};