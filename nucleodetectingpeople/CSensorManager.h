#include "mbed.h"
#include "CSensorVL53L1X.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include <vector>

class CSensorManager
{
public:
    CSensorManager();
    ~CSensorManager();
    int initSensors();
    void ReadFromSensors();
    vector<CSensorVL53L1X> oSensorVector;

//to be moved out...
#define VL53L1_I2C_SDA   PA_10
#define VL53L1_I2C_SCL   PA_9
    VL53L1X_DevI2C *device_i2c = new VL53L1X_DevI2C(VL53L1_I2C_SDA, VL53L1_I2C_SCL);
    DigitalOut xshutdown = PB_4;
    PinName interrupt = PA_3;
    uint8_t sensorAdress = 0x52;
    uint16_t mTimingBudgetInMs = 15;
//private:
    



};
