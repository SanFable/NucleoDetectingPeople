#include <stdio.h>
#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#define VL53L1_I2C_SDA   PA_10
#define VL53L1_I2C_SCL   PA_9


 
int main()
{

    VL53L1X_DevI2C *device_i2c = new VL53L1X_DevI2C(VL53L1_I2C_SDA, VL53L1_I2C_SCL);

static VL53L1X *sensor = NULL;
uint16_t TimingBudgetInMs = 15;
    
    uint32_t piData = NULL;
    uint8_t isDataReady = 1;
    DigitalOut xshutdown(PB_4);
    sensor = new VL53L1X(device_i2c, &xshutdown, PA_3);
 
    sensor->vl53l1_off();
    /* initialise sensor */
    sensor->init_sensor(0x52);
    //range_measure(device_i2c, 0);  // start continuous measures
    
    sensor->vl53l1x_set_timing_budget_in_ms(TimingBudgetInMs);
	sensor->vl53l1x_set_inter_measurement_in_ms(TimingBudgetInMs);
    sensor->vl53l1x_set_distance_mode(1);
    sensor->vl53l1x_start_ranging();

while(1){
while(isDataReady){
    sensor->vl53l1x_check_for_data_ready(&isDataReady);
    printf("waiting\n");
}
    sensor->get_distance(&piData);
printf("dist: %d \n", piData);
    sensor->vl53l1x_clear_interrupt();

}
    return 0;
}