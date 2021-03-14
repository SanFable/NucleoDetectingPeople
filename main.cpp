#include <stdio.h>
#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#define VL53L1_I2C_SDA   PA_10
#define VL53L1_I2C_SCL   PA_9
//static VL53L1X *sensor[5] = {NULL};
uint32_t piData[5] = {NULL};
uint8_t isDataReady[5] = {0};
VL53L1X_DevI2C *device_i2c = new VL53L1X_DevI2C(VL53L1_I2C_SDA, VL53L1_I2C_SCL);


struct Sensor{

DigitalOut *xshutdown;
PinName interrupt;
int timingBudgetInMs;
int distanceMode;
uint8_t sensorAdress;
VL53L1X *obj;

};

Sensor sensors[10];




void getdata( int *i){
    while (1){
while (!isDataReady[*i]){ 
    sensors[*i].obj->vl53l1x_check_for_data_ready(&isDataReady[*i]);
}

    isDataReady[*i] = 0;
    sensors[*i].obj->get_distance(&piData[*i]);
    printf("\nsens %d: %d\n", *i, piData[*i]);
    sensors[*i].obj->vl53l1x_clear_interrupt();
wait_us(100000);

}
}

Thread thread[5];
int main()
{

    

    
    uint16_t TimingBudgetInMs = 15;
    printf("before init\n");
    DigitalOut xshutdown[2]{PB_4,PB_5};
    sensors[0].obj = new VL53L1X(device_i2c, &xshutdown[0], PA_3);
    sensors[1].obj = new VL53L1X(device_i2c, &xshutdown[1], PA_3);

    sensors[0].obj->init_sensor(0x54);
    wait_us(1000000);
    sensors[1].obj->init_sensor(0x56);

    printf("after init\n");
    

    sensors[0].obj->vl53l1x_set_timing_budget_in_ms(TimingBudgetInMs);
    sensors[0].obj->vl53l1x_set_inter_measurement_in_ms(TimingBudgetInMs);
    sensors[0].obj->vl53l1x_set_distance_mode(1);
    sensors[0].obj->vl53l1x_start_ranging();
    
    sensors[1].obj->vl53l1x_set_timing_budget_in_ms(TimingBudgetInMs);
    sensors[1].obj->vl53l1x_set_inter_measurement_in_ms(TimingBudgetInMs);
    sensors[1].obj->vl53l1x_set_distance_mode(1);
    sensors[1].obj->vl53l1x_start_ranging();  
    
int u = 0;
int uw = 1;
thread[u].start(callback(getdata,&u));
thread[uw].start(callback(getdata,&uw));
while(1){
    
}


}