
#include "mbed.h"
#include "VL53L1X_I2C.h"
class CSensorBase
{
    public:
    CSensorBase( )//CDataHandler* DataHandlerPtr) : poDataHandler(DataHandlerPtr)
    {
    }
    ~CSensorBase();

    virtual int Init(VL53L1X_DevI2C *mDevice_i2c, DigitalOut mXshutdown, PinName mInterrupt, uint8_t mSensorAdress, uint16_t mTimingBudgetInMs);
    virtual int GetData();
    virtual int SetDataToDataHandler();
    //jakies inne metody

   // private:
    //CDataHandler* poDataHandler;

};