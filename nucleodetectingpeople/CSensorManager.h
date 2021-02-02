#include "CSensorBase.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
class CSensorManager
{
public:
    CSensorManager();
    ~CSensorManager();
    int Init()
    {
        oSensorVector.add(CSensorVL53L1X(device_i2c, &xshutdown, PA_3));
        oSensorVector.add(CSensorVL53L1X(device_i2c, &xshutdown, PA_3));
    }


private:
    vector<CSensorBase> oSensorVector;


};