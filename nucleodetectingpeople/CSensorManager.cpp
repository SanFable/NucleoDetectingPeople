#include "mbed.h"
#include "CSensorManager.h"

CSensorManager::CSensorManager() {

    oSensorVector.push_back(CSensorVL53L1X(device_i2c, &xshutdown, interrupt, sensorAdress));
}
CSensorManager::~CSensorManager() {}
 
    void CSensorManager::initSensors()
    {
        
        oSensorVector[0].Init();
        
        
        //oSensorVector->push_back(CSensorVL53L1X(*device_i2c, *xshutdown, interrupt, sensorAdress, mTimingBudgetInMs));

    }

    void CSensorManager::ReadFromSensors(){
        oSensorVector[0].getData();
    }
