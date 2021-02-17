#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include "CSensorVL53L1X.h"

    CSensorVL53L1X::CSensorVL53L1X(VL53L1X_DevI2C *device_i2c, DigitalOut *xshutdown, PinName interrupt, uint8_t sensorAdress) : m_oSensor(device_i2c, xshutdown, interrupt, sensorAdress)
    {
        mDevice_i2c = device_i2c;
        mXshutdown = xshutdown;
        mInterrupt = interrupt;
        mSensorAdress = sensorAdress;
        m_oSensor = VL53L1X(mDevice_i2c, mXshutdown, mInterrupt, mSensorAdress);
        
    }
    
CSensorVL53L1X::~CSensorVL53L1X() {}

 int CSensorVL53L1X::Init()
    {
        
        m_oSensor.init_sensor(mSensorAdress);
        m_oSensor.vl53l1x_set_timing_budget_in_ms(mTimingBudgetInMs);
	    m_oSensor.vl53l1x_set_inter_measurement_in_ms(mTimingBudgetInMs);
        m_oSensor.vl53l1x_set_distance_mode(1);
        m_oSensor.vl53l1x_start_ranging();
        return 0;
    }
    
void CSensorVL53L1X::getData(){
    uint32_t piData = NULL;
    uint8_t isDataReady = 1;

    while(isDataReady){
    m_oSensor.vl53l1x_check_for_data_ready(&isDataReady);
    printf("waiting\n");
}
    m_oSensor.get_distance(&piData);
printf("dist: %d \n", piData);
    m_oSensor.vl53l1x_clear_interrupt();
}



/*int defineSensor(VL53L1X_DevI2C *mDevice_i2c, DigitalOut *mXshutdown, PinName mInterrupt, uint8_t mSensorAdress, uint16_t mTimingBudgetInMs)
    {
       mDevice_i2c = mDevice_i2c;
       mXshutdown = new DigitalOut(*mXshutdown);
       mInterrupt = *new PinName(mInterrupt);
       mSensorAdress = mSensorAdress;
       mTimingBudgetInMs = mTimingBudgetInMs;
       return 0;
    }*/