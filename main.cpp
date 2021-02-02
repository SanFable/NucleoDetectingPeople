#include <stdio.h>
#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include "CSensorBase.h"
#include "CSensorVL53L1X.h"
#include "CSensorManager.h"
#define VL53L1_I2C_SDA   PA_10
#define VL53L1_I2C_SCL   PA_9

VL53L1X_DevI2C *device_i2c = new VL53L1X_DevI2C(VL53L1_I2C_SDA, VL53L1_I2C_SCL);


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
int main()
{

    
    CSensorManager oSensorManager;
    uint16_t TimingBudgetInMs = 15;
    printf("before init\n");
    checkaddrs();
    uint32_t piData[5] = {NULL};
    uint8_t isDataReady[5] = {0};
    DigitalOut xshutdown[8]{D12,D11,D10,D9,D8,D7,D6,D5};

    oSensorManager.Init();


    printf("after init\n");
    checkaddrs();

while(1){
if (!isDataReady[0]){ 
    sensor[0]->vl53l1x_check_for_data_ready(&isDataReady[0]);
}
else{
    isDataReady[0] = 0;
    sensor[0]->get_distance(&piData[0]);
    printf("sens 0: %d", piData[0]);
    sensor[0]->vl53l1x_clear_interrupt();
}


if (!isDataReady[1]){ 
    sensor[1]->vl53l1x_check_for_data_ready(&isDataReady[1]);
}
else{
    isDataReady[1] = 0;
    sensor[1]->get_distance(&piData[1]);
    printf(" sens 1: %d\n", piData[1]);
    sensor[1]->vl53l1x_clear_interrupt();
}


wait_us(100000);
}
    return 0;
}