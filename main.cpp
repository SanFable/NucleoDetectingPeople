#include <cstdint>
#include <stdio.h>
#include "mbed.h"
#include "VL53L1X_I2C.h"
#include "VL53L1X_Class.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <errno.h>
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
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


SDBlockDevice   blockDevice(PB_15, PB_14, PI_1, PA_8);  // mosi, miso, sck, cs
FATFileSystem   fileSystem("fs");
Sensor sensors[numberOfSensors];
Thread thread[numberOfSensors];
Thread handleSD;
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
    //printf("sens %d: %d zone: %d\n", *i, sensors[*i].tempData, zoneMeasured);
    //
    if(*i % 2 == 0) {
        sprintf((char*)text[*i], "dist: %d", sensors[*i].tempData);
        BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)&text[*i], RIGHT_MODE); }
    if(*i % 2 == 1)
    {
        sprintf((char*)text[*i], "dist: %d", sensors[*i].tempData);
        BSP_LCD_DisplayStringAt(0, 90, (uint8_t *)&text[*i], RIGHT_MODE); }

    ThisThread::sleep_for(50ms);
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
    {
        zoneMeasured = 0;
        printf("sens: %d numOfData: %d %d  %d  %d  %d  %d  %d\n", *i, sensors[*i].numofData, sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-5], sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-6], sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-4]
        , sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-3], sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-2], sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-1]);
    }
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

void SaveToSD(int arrayNumber, int sensorNum){
// to be fixed, not working https://pastebin.com/ELC0xZ2D?fbclid=IwAR11s8d0CJJjr92JyfcjjN4ijSGyIBvdspw5xam4VJXA4PKULnvLORB5IKA


printf("Mounting the filesystem... ");
    fflush(stdout);
 
    int err = fileSystem.mount(&blockDevice);
    printf("%s\n", (err ? "Fail :(" : "OK"));
    if (err) {
        // Reformat if we can't mount the filesystem
        // this should only happen on the first boot
        printf("No filesystem found, formatting... ");
        fflush(stdout);
        err = fileSystem.reformat(&blockDevice);
        printf("%s\n", (err ? "Fail :(" : "OK"));
        if (err) {
            error("error: %s (%d)\n", strerror(-err), err);
        }
    }
 
    // Open the hehe file
    printf("Opening \"/fs/hehe.txt\"... ");
    fflush(stdout);
 
    FILE*   f = fopen("/fs/hehe.txt", "r+");
    printf("%s\n", (!f ? "Fail :(" : "OK"));
    if (!f) {
        // Create the hehe file if it doesn't exist
        printf("No file found, creating a new file... ");
        fflush(stdout);
        f = fopen("/fs/hehe.txt", "w+");
        printf("%s\n", (!f ? "Fail :(" : "OK"));
        if (!f) {
            error("error: %s (%d)\n", strerror(errno), -errno);
        }
 
        for (int i = 0; i < 1000; i++) {
            printf("\rWriting hehe (%d/%d)... ", i, 1000);
            fflush(stdout);
            err = fprintf(f, "%d,%d\n", sensors[sensorNum].timestampData[arrayNumber][i], sensors[sensorNum].sensorData[arrayNumber][i]);
            if (err < 0) {
                printf("Fail :(\n");
                error("error: %s (%d)\n", strerror(errno), -errno);
            }
        }
 
        printf("\rWriting hehe (%d/%d)... OK\n", 1000, 1000);
 
        printf("Seeking file... ");
        fflush(stdout);
        err = fseek(f, 0, SEEK_SET);
        printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
        if (err < 0) {
            error("error: %s (%d)\n", strerror(errno), -errno);
        }
    }
  
    // Close the file which also flushes any cached writes
    printf("Closing \"/fs/hehe.txt\"... ");
    fflush(stdout);
    err = fclose(f);
    printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
    if (err < 0) {
        error("error: %s (%d)\n", strerror(errno), -errno);
    }
 
 
    // Tidy up
    printf("Unmounting... ");
    fflush(stdout);
    err = fileSystem.unmount();
    printf("%s\n", (err < 0 ? "Fail :(" : "OK"));
    if (err < 0) {
        error("error: %s (%d)\n", strerror(-err), err);
    }
 
    printf("Initializing the block device... ");
    fflush(stdout);
 
    err = blockDevice.init();
    printf("%s\n", (err ? "Fail :(" : "OK"));
    if (err) {
        error("error: %s (%d)\n", strerror(-err), err);
    }
 
    printf("Erasing the block device... ");
    fflush(stdout);
    err = blockDevice.erase(0, blockDevice.size());
    printf("%s\n", (err ? "Fail :(" : "OK"));
    if (err) {
        error("error: %s (%d)\n", strerror(-err), err);
    }
 
    printf("Deinitializing the block device... ");
    fflush(stdout);
    err = blockDevice.deinit();
    printf("%s\n", (err ? "Fail :(" : "OK"));
    if (err) {
        error("error: %s (%d)\n", strerror(-err), err);
    }
 
    printf("\r\n");
 
    printf("Mbed OS filesystem example done!\n");


}


void checkForSDWrite(){
    uint8_t lastWhichArray[8] ={sensors[0].whichArray, sensors[1].whichArray, sensors[2].whichArray, sensors[3].whichArray, 
                                sensors[4].whichArray, sensors[5].whichArray, sensors[6].whichArray, sensors[7].whichArray};
    while(1){
        
        for (int i = 0; i<numberOfSensors; i++){
            printf("checking for write\n");
            if (lastWhichArray[i] != sensors[i].whichArray){
                SaveToSD(lastWhichArray[i], i);
                lastWhichArray[i] = sensors[i].whichArray;
            }
        ThisThread::sleep_for(3s);
        }

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
    handleSD.start(callback(checkForSDWrite));
    //BSP_LCD_DisplayStringAt(0, 1, (uint8_t *)"ELOooo", CENTER_MODE);

    

    int a = 0;
    int b = 1;
 
    printf("before init\n");
    preInitSensors();
    wait_us(10000); 
        
    initSingleSensor(sensorNumber[a]);
    wait_us(1000000);
    //initSingleSensor(sensorNumber[b]);

checkaddrs();
    
thread[sensorNumber[a]].start(callback(getROIData,&sensorNumber[a]));
//thread[sensorNumber[b]].start(callback(getROIData,&sensorNumber[b]));

}