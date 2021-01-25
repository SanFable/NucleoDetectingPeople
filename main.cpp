#include <stdio.h>
#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
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

    

    static VL53L1X *sensor[5] = {NULL};
    uint16_t TimingBudgetInMs = 15;
    printf("before init\n");
    checkaddrs();
    uint32_t piData[5] = {NULL};
    uint8_t isDataReady[5] = {0};
    DigitalOut xshutdown[8]{D12,D11,D10,D9,D8,D7,D6,D5};
    sensor[0] = new VL53L1X(device_i2c, &xshutdown[0], PA_3);
    sensor[1] = new VL53L1X(device_i2c, &xshutdown[1], PA_3);

	sensor[0]->init_sensor(0x54);
    sensor[1]->init_sensor(0x56);

    printf("after init\n");
    checkaddrs();
    

    sensor[0]->vl53l1x_set_timing_budget_in_ms(TimingBudgetInMs);
	sensor[0]->vl53l1x_set_inter_measurement_in_ms(TimingBudgetInMs);
    sensor[0]->vl53l1x_set_distance_mode(1);
    sensor[0]->vl53l1x_start_ranging();
    
    sensor[1]->vl53l1x_set_timing_budget_in_ms(TimingBudgetInMs);
	sensor[1]->vl53l1x_set_inter_measurement_in_ms(TimingBudgetInMs);
    sensor[1]->vl53l1x_set_distance_mode(1);
    sensor[1]->vl53l1x_start_ranging();  
    

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