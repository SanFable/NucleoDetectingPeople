#include <cstdint>
#include <stdio.h>
#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <vector>
#define VL53L1_I2C_SDA   PB_9
#define VL53L1_I2C_SCL   PB_8
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
uint32_t timestampData[2][1000]{0};
uint8_t whichArray = 0;
uint16_t numofData = 0;
};
Sensor sensors[numberOfSensors];
Thread thread[numberOfSensors];
DigitalOut xshutdown[numberOfSensors]{PC_7, PC_6, PB_4, PG_7, PH_6, PI_3, PI_2, PA_15};
uint8_t sensorAdress[numberOfSensors]{0x54, 0x56, 0x58, 0x60, 0x62, 0x64, 0x66, 0x68};
uint8_t roiArray[]{151, 167, 183, 199, 215, 239};
uint8_t roiX = 4;
uint8_t roiY = 16;
int sensorNumber[numberOfSensors]{0, 1, 2, 3, 4, 5, 6, 7};
uint8_t timingBudgetInMs = 20;
uint8_t shortDistanceMode = 1;
uint8_t text[numberOfSensors][30];

 void checkaddrs(){
     I2C i2c(VL53L1_I2C_SDA, VL53L1_I2C_SCL); 
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
        //wait_us(20000);
        while (!sensors[*i].isDataReady){ 
            sensors[*i].obj->vl53l1x_check_for_data_ready(&sensors[*i].isDataReady);
        ThisThread::sleep_for(3ms);
        }
    sensors[*i].isDataReady = 0;
    sensors[*i].obj->get_distance(&sensors[*i].tempData);
    printf("sens %d: %d zone: %d\n", *i, sensors[*i].tempData, zoneMeasured);
    //
    if(*i % 2 == 0) {
        sprintf((char*)text[*i], "dist: %d", sensors[*i].tempData);
        BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)&text[*i], RIGHT_MODE); }
    if(*i % 2 == 1)
    {
        sprintf((char*)text[*i], "dist: %d", sensors[*i].tempData);
        BSP_LCD_DisplayStringAt(0, 90, (uint8_t *)&text[*i], RIGHT_MODE); }

    ThisThread::sleep_for(100ms);
    sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData] = sensors[*i].tempData;
    //printf("array %d val %d index %d\n", sensors[*i].whichArray, sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData], sensors[*i].numofData);
    sensors[*i].timestampData[sensors[*i].whichArray][sensors[*i].numofData] = (duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count());
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
BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
    //BSP_LCD_DisplayStringAt(0, 1, (uint8_t *)"ELOooo", CENTER_MODE);

    

    int a = 0;
    int b = 1;
 
    printf("before init\n");
    preInitSensors();
    wait_us(10000); 
        
    initSingleSensor(sensorNumber[a]);
    wait_us(1000000);
    initSingleSensor(sensorNumber[b]);

checkaddrs();
    
thread[sensorNumber[a]].start(callback(getROIData,&sensorNumber[a]));
thread[sensorNumber[b]].start(callback(getROIData,&sensorNumber[b]));

}