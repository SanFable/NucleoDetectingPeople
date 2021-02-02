#include "CSensorBase.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include <cstdint>
class CSensorVL53L1X : public CSensorBase
{
    public:
    CSensorVL53L1X(VL53L1X_DevI2C *device_i2c, DigitalOut xshutdown, DigitalIn interrupt, uint8_t sensorAdress, uint16_t timingBudgetInMs) : CSensorBase()
    {
       this->device_i2c = device_i2c;
       this->xshutdown = xshutdown;
       this->interrupt = interrupt;
       this->sensorAdress = sensorAdress;
       this->timingBudgetInMs = timingBudgetInMs;
       
    }
    ~CSensorVL53L1X();
    
    int Init() override
    {
        VL53L1X oSensor(this->device_i2c, this->xshutdown, this->interrupt);
        oSensor.init_sensor(this->sensorAdress);
        oSensor.vl53l1x_set_timing_budget_in_ms(this->timingBudgetInMs);
	    oSensor.vl53l1x_set_inter_measurement_in_ms(this->timingBudgetInMs);
        oSensor.vl53l1x_set_distance_mode(1);
        oSensor.vl53l1x_start_ranging();
    }
    int GetData() override;

private:
    VL53L1X_DevI2C *device_i2c;
    DigitalOut xshutdown;
    DigitalIn interrupt;
    uint8_t sensorAdress;   
    uint16_t timingBudgetInMs;

    VL53L1X oSensor(VL53L1X_DevI2C*, DigitalOut, DigitalIn); 
};
