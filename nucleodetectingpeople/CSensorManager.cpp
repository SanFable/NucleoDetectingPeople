#include "mbed.h"
#include "CSensorManager.h"

CSensorManager::CSensorManager() {}
CSensorManager::~CSensorManager() {}
 
    void CSensorManager::initSensors()
    {
        oSensorVector.push_back(CSensorVL53L1X(device_i2c, &xshutdown, interrupt, sensorAdress));
        oSensorVector[0].Init();
        
        
        //oSensorVector->push_back(CSensorVL53L1X(*device_i2c, *xshutdown, interrupt, sensorAdress, mTimingBudgetInMs));

    }

    void CSensorManager::ReadFromSensors(){
        oSensorVector[0].getData();
    }
