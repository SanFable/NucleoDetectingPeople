#include <stdio.h>
#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#define VL53L1_I2C_SDA   PA_10
#define VL53L1_I2C_SCL   PA_9
 
//#if TARGET_STM  // we are cross compiling for an STM32-Nucleo    
//    InterruptIn stop_button(USER_BUTTON);
//#endif

 
static VL53L1X *sensor[5] = {NULL};
//Serial pc(SERIAL_TX, SERIAL_RX);
 
/* flags that handle interrupt request for sensor and user blue button*/
volatile bool int_sensor = false;
volatile bool int_stop = false;
 
 
/* ISR callback function of the sensor */
void sens1(void)
{
    int_sensor = true;
    sensor[0]->disable_interrupt_measure_detection_irq();
}
 
/* ISR callback function of the user blue button to switch measuring sensor. */
void measuring_stop_irq(void)
{
    int_stop = true;
}
 
/* Start the sensor ranging */
int start_ranging(int sensor_id)
{
    int status = 0;
    /* start the measure on the sensor */
    if (NULL != sensor[sensor_id]) {
        status = sensor[sensor_id]->stop_measurement();
        if (status != 0) {
                return status;
        }
 
        status = sensor[sensor_id]->start_measurement(&sens1);
        if (status != 0) {
            return status;
        }
    }
    return status;
}
 
int range_measure(VL53L1X_DevI2C *device_i2c, int sensor_id)
{
    int status = 0;
    uint16_t distance = 0;
    /* Create a xshutdown pin */
    DigitalOut xshutdown(PB_4);
 
    /* create instance of sensor class */
    sensor[sensor_id] = new VL53L1X(device_i2c, &xshutdown, PA_3);
 
    sensor[sensor_id]->vl53l1_off();
    /* initialise sensor */
    sensor[sensor_id]->init_sensor(0x52);
 
    if (status) {
        delete sensor[sensor_id];
        sensor[sensor_id]= NULL;
        printf("Sensor centre not present\n\r");
    }
 
    /* init an array with chars to id the sensors */
    status = start_ranging(sensor_id);
    if (status != 0) {
        printf("Failed to start ranging!\r\n");
        return status;
    }
 
    if (NULL != sensor[sensor_id]) {
        printf("Entering loop mode\r\n");
        /* Main ranging interrupt loop */
        while (true) {
            if (int_sensor) {
                int_sensor = false;
                status = sensor[sensor_id]->handle_irq(&distance);
                printf("distance: %d\r\n", distance);
            }
 
            if (int_stop) {
                printf("\r\nEnding loop mode \r\n");
                break;
            }
        }
    }
 
    return status;
 
}


 
/*=================================== Main ==================================
=============================================================================*/
int main()
{
    //stop_button.rise(&measuring_stop_irq);
    
    VL53L1X_DevI2C *device_i2c = new VL53L1X_DevI2C(VL53L1_I2C_SDA, VL53L1_I2C_SCL);

    range_measure(device_i2c, 0);  // start continuous measures
    
    return 0;
}