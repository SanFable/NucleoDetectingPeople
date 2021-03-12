#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include "CSensorVL53L1X.h"
#include "DebugFunctions.h"
    CSensorVL53L1X::CSensorVL53L1X(VL53L1X_DevI2C *device_i2c, DigitalOut *xshutdown, PinName interrupt, uint8_t sensorAdress) :
                     m_oSensor(device_i2c, xshutdown, interrupt), 
                     mXshutdown(xshutdown),
                     mInterrupt(interrupt),
                     mSensorAdress(sensorAdress),
                     mDevice_i2c(device_i2c)
    {

        //m_oSensor = VL53L1X(mDevice_i2c, mXshutdown, mInterrupt);
        
    }
    
CSensorVL53L1X::~CSensorVL53L1X() {}

 void CSensorVL53L1X::Init()
    {
        VL53L1X_ERROR status = 0;
        status = m_oSensor.init_sensor(mSensorAdress);
        status = m_oSensor.vl53l1x_set_distance_mode(1);
        status = m_oSensor.vl53l1x_set_timing_budget_in_ms(15);
	    status = m_oSensor.vl53l1x_set_inter_measurement_in_ms(15);
        status = m_oSensor.vl53l1x_start_ranging();
        printf("after whole init %d\n", status);
        /*wait_us(100000);
        while (1){
        uint8_t isDataReady = 0;
        while (isDataReady == 0)
        status = m_oSensor.vl53l1x_check_for_data_ready(&isDataReady);
        uint32_t piData = NULL;
        status = m_oSensor.get_distance(&piData);
        //printf("po dist %d\n", status);
        printf("dist: %d \n", piData);
        status = m_oSensor.vl53l1x_clear_interrupt();
        //printf("po interrupt %d\n", status);
       //DebugFunctions showi2css = DebugFunctions();   
       // showi2css.checkaddrs();
        }*/
    }
    
void CSensorVL53L1X::getData(){
    VL53L1X_ERROR status = 0;
    uint32_t piData = NULL;
    uint8_t isDataReady = 0;
while(1){
    while(isDataReady == 0){
    status = m_oSensor.vl53l1x_check_for_data_ready(&isDataReady);
    printf("waiting %d\n", status);
    //printf("data ready: %d\n", isDataReady);
    wait_us(1000000);
    
}
isDataReady = 0;
    status = m_oSensor.get_distance(&piData);
    //printf("po dist %d\n", status);
    printf("dist: %d \n", piData);
    status = m_oSensor.vl53l1x_clear_interrupt();
    //printf("po interrupt %d\n", status);
     
}
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