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
struct Sensor{
DigitalOut *xshutdown;
PinName interrupt;
int distanceMode;
uint8_t sensorAdress;
VL53L1X *obj;
uint32_t tempData;
uint8_t isDataReady;
vector<uint32_t> sensorData;
};
Sensor sensors[numberOfSensors];
Thread thread[numberOfSensors];
DigitalOut xshutdown[numberOfSensors]{PB_4, PB_5, PA_11, PA_8, PB_0, PB_7, PB_6, PB_1};
uint8_t sensorAdress[numberOfSensors]{0x54, 0x56, 0x58, 0x60, 0x62, 0x64, 0x66, 0x68};
int sensorNumber[numberOfSensors]{0, 1, 2, 3, 4, 5, 6, 7};
uint8_t timingBudgetInMs = 15;
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

void getdata( int *i){
    while (1){
        while (!sensors[*i].isDataReady){ 
            sensors[*i].obj->vl53l1x_check_for_data_ready(&sensors[*i].isDataReady);
        wait_us(3000);
        }
    sensors[*i].isDataReady = 0;
    sensors[*i].obj->get_distance(&sensors[*i].tempData);
    //printf("\nsens %d: %d\n", *i, sensors[*i].tempData);
    sensors[*i].sensorData.push_back(sensors[*i].tempData);
    printf("sens %d size: %d\n", *i, sensors[*i].sensorData.size());
    sensors[*i].obj->vl53l1x_clear_interrupt();
    wait_us(100000);
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
sensors[i].obj->vl53l1x_start_ranging();
}

int main()
{

    

    
 checkaddrs();
    printf("before init\n");
    preInitSensors();
    wait_us(10000);
    initSingleSensor(sensorNumber[0]);
    wait_us(10000);
    initSingleSensor(sensorNumber[1]);
    
thread[sensorNumber[0]].start(callback(getdata,&sensorNumber[0]));
thread[sensorNumber[1]].start(callback(getdata,&sensorNumber[1]));


}