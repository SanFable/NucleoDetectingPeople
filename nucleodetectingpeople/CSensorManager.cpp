#include "mbed.h"
#include "CSensorManager.h"

CSensorManager::CSensorManager() {}
CSensorManager::~CSensorManager() {}
 
    int CSensorManager::initSensors()
    {
        oSensorVector.push_back(CSensorVL53L1X(device_i2c, &xshutdown, interrupt, sensorAdress));
        oSensorVector[0].Init(VL53L1X(device_i2c, &xshutdown, interrupt));
        
        
        //oSensorVector->push_back(CSensorVL53L1X(*device_i2c, *xshutdown, interrupt, sensorAdress, mTimingBudgetInMs));
        return 0;
    }

    void CSensorManager::ReadFromSensors(){
        oSensorVector[0].getData(VL53L1X(device_i2c, &xshutdown, interrupt));
    }
