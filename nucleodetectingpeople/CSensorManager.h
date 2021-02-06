#include "CSensorVL53L1X.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
class CSensorManager : CSensorVL53L1X
{
public:
    CSensorManager();
    ~CSensorManager();
    int Init(VL53L1X_DevI2C *mDevice_i2c, DigitalOut mXshutdown, PinName mInterrupt, uint8_t mSensorAdress, uint16_t mTimingBudgetInMs);

private:
    vector<CSensorBase> oSensorVector;



};
