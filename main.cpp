#include <cstdint>
#include <stdio.h>
#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include <vector>
#define VL53L1_I2C_SDA   PA_10
#define VL53L1_I2C_SCL   PA_9
VL53L1X_DevI2C *device_i2c = new VL53L1X_DevI2C(VL53L1_I2C_SDA, VL53L1_I2C_SCL);
const uint8_t numberOfSensors = 8;
Timer t;
struct Sensor{
DigitalOut *xshutdown;
PinName interrupt;
int distanceMode;
uint8_t sensorAdress;
VL53L1X *obj;
uint32_t tempData;
uint8_t isDataReady;
uint16_t sensorData[2][1000]{0};
uint32_t timestampData[2][100]{0};
uint8_t whichArray = 0;
uint16_t numofData = 0;

};
Sensor sensors[numberOfSensors];
Thread thread[numberOfSensors];
DigitalOut xshutdown[numberOfSensors]{PB_4, PB_5, PA_11, PA_8, PB_0, PB_7, PB_6, PB_1};
uint8_t sensorAdress[numberOfSensors]{0x54, 0x56, 0x58, 0x60, 0x62, 0x64, 0x66, 0x68};
uint8_t roiArray[]{151, 167, 183, 199, 215, 239};
uint8_t roiX = 4;
uint8_t roiY = 16;
int sensorNumber[numberOfSensors]{0, 1, 2, 3, 4, 5, 6, 7};
uint8_t timingBudgetInMs = 20;
uint8_t shortDistanceMode = 1;
 void checkaddrs(){
     I2C i2c(PA_10, PA_9); 
     int error, address;
     for(address = 0; address < 128; address++ ){
        thread_sleep_for(10);
        error = i2c.write(address  << 1, "1", 1);

        if (error == 0){
            printf("I2C device found at address 8bit: 0x%X 7bit: 0x%X\n", address, address << 1);

        }
    }
 }

void getROIData( int *i){
uint8_t zoneMeasured = 0;
    while (1){
        sensors[*i].obj->VL53L1X_SetROICenter(roiArray[zoneMeasured]); 
        wait_us(10000);
        while (!sensors[*i].isDataReady){ 
            sensors[*i].obj->vl53l1x_check_for_data_ready(&sensors[*i].isDataReady);
        wait_us(3000);
        }
    sensors[*i].isDataReady = 0;
    sensors[*i].obj->get_distance(&sensors[*i].tempData);
    //printf("sens %d: %d zone: %d\n", *i, sensors[*i].tempData, zoneMeasured);
    
    sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData] = sensors[*i].tempData;
    printf("array %d val %d index %d\n", sensors[*i].whichArray, sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData], sensors[*i].numofData);
    //sensors[*i].timestampData[sensors[*i].whichArray][sensors[*i].numofData] = (duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count());
       // printf("     The time taken was %llu milliseconds\n", duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count());
  
    //sensors[*i].sensorData.push_back(sensors[*i].tempData);
    //printf("sens %d dist: %d size: %d\n", *i, sensors[*i].tempData, sensors[*i].sensorData.size());
    sensors[*i].obj->vl53l1x_clear_interrupt();
    
    zoneMeasured++;
    sensors[*i].numofData++;
    if (zoneMeasured==6)
        zoneMeasured = 0;

    if (sensors[*i].numofData == 1000)
        {
            sensors[*i].numofData = 0;
            if(sensors[*i].whichArray == 0)
                sensors[*i].whichArray = 1;

            else if(sensors[*i].whichArray == 1)
                    sensors[*i].whichArray = 0;
        }
    //wait_us(100000);
    }
}


void preInitSensors(){
for (uint8_t i = 0;i<numberOfSensors;i++){
    xshutdown[i] = 0;
}
}
void initSingleSensor(int i){

sensors[i].obj = new VL53L1X(device_i2c, &xshutdown[i]);
sensors[i].obj->init_sensor(sensorAdress[i]);
sensors[i].obj->vl53l1x_set_timing_budget_in_ms(timingBudgetInMs);
sensors[i].obj->vl53l1x_set_inter_measurement_in_ms(timingBudgetInMs);
sensors[i].obj->vl53l1x_set_distance_mode(shortDistanceMode);
sensors[i].obj->vl53l1x_set_roi(roiX,roiY);
sensors[i].obj->vl53l1x_start_ranging();
}

int main()
{
t.start();

    

    
 
    printf("before init\n");
    preInitSensors();
    wait_us(10000); 
    initSingleSensor(sensorNumber[0]);
    wait_us(10000);
  // initSingleSensor(sensorNumber[3]);
    checkaddrs();
    
thread[sensorNumber[0]].start(callback(getROIData,&sensorNumber[0]));
//thread[sensorNumber[3]].start(callback(getROIData,&sensorNumber[3]));

}