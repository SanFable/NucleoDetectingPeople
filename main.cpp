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
#include "gl.h"
#define VL53L1_I2C_SDA   PB_9
#define VL53L1_I2C_SCL   PB_8
VL53L1X_DevI2C *device_i2c = new VL53L1X_DevI2C(VL53L1_I2C_SDA, VL53L1_I2C_SCL);
const uint8_t numberOfSensors = 8;
Timer t;
Timer detectionTimer;
bool detectionBoolflag = 0;
int saveDelay = 0;
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
uint16_t avgSensorData;
uint16_t dist150cm = 0;
uint16_t dist100cm = 0;
uint16_t dist50cm = 0;
Timer internalTimer1;
Timer internalTimer2;
Timer internalTimer3;
bool timerRunning[3] = {0, 0, 0};
uint16_t detectedAt50cm = 0;
uint16_t detectedAt100cm = 0;
uint16_t detectedAt150cm = 0;
uint16_t pdetectedAt50cm = 0;
uint16_t pdetectedAt100cm = 0;
uint16_t pdetectedAt150cm = 0;

};

uint32_t totalNumOfData = 0;
uint8_t lDetectionFlagAt50cm = 0;
uint8_t lDetectionFlagAt100cm = 0;
uint8_t lDetectionFlagAt150cm = 0;
uint8_t rDetectionFlagAt50cm = 0;
uint8_t rDetectionFlagAt100cm = 0;
uint8_t rDetectionFlagAt150cm = 0;
uint8_t passedLeftAt50 = 0;
uint8_t passedRightAt50 = 0;
uint8_t passedLeftAt100 = 0;
uint8_t passedRightAt100 = 0;
uint8_t passedLeftAt150 = 0;
uint8_t passedRightAt150 = 0;

SDBlockDevice   blockDevice(PB_15, PB_14, PI_1, PA_8);  // mosi, miso, sck, cs
FATFileSystem   fileSystem("fs");
FILE*   f;
char fileName[10];
Sensor sensors[numberOfSensors];
Thread thread[numberOfSensors];
Thread handleSD;
Thread countPeopleThread;
Thread printMeasurementsThread;

DigitalOut xshutdown[numberOfSensors]{PC_7, PC_6, PB_4, PG_7, PH_6, PI_3, PI_2, PA_15};
uint8_t sensorAdress[numberOfSensors]{0x54, 0x56, 0x58, 0x60, 0x62, 0x64, 0x66, 0x68};
uint8_t roiArray[]{151, 167, 183, 199, 215, 239};
uint8_t roiX = 4;
uint8_t roiY = 16;
int sensorNumber[numberOfSensors]{0, 1, 2, 3, 4, 5, 6, 7};
uint8_t timingBudgetInMs = 20;
uint8_t shortDistanceMode = 2; //short distance mode can be made with 15ms budget and sleep delay under 15/10ms but only to 1.3/1.5m distance
uint8_t text[numberOfSensors][30];
int linearLocation = 0;
uint16_t xDist= 39;
//480x272 lcd size two modes, linear scheme vs avg with sensor displayeed
//detecting people based on measuring time of short distance per sensor
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

void getROIData(int* i)
{
    int eh = 0;
    uint8_t zoneMeasured = 0;
    while (1)
    {
        sensors[*i].obj->VL53L1X_SetROICenter(roiArray[zoneMeasured]);
        // wait_us(20000);
        totalNumOfData++;
        ThisThread::sleep_for(25ms);
        while (!sensors[*i].isDataReady)
        {
            sensors[*i].obj->vl53l1x_check_for_data_ready(&sensors[*i].isDataReady);
            eh++;
        }
        if (eh != 1)
            printf("%d  %d\n", *i, eh);
        eh = 0;
        sensors[*i].isDataReady = 0;
        sensors[*i].obj->get_distance(&sensors[*i].tempData);
        sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData]
            = sensors[*i].tempData;
        sensors[*i].timestampData[sensors[*i].whichArray][sensors[*i].numofData]
            = (duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count());
        sensors[*i].obj->vl53l1x_clear_interrupt();

        zoneMeasured++;
        sensors[*i].numofData++;
        if (zoneMeasured == 6)
        {
            sensors[*i].avgSensorData = 
            (sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-6] +
            sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-5] +
            sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-4] +
            sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-3] +
            sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-2] +
            sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-1])/6;
            for (int j = 0; j < 6; j++)
            {
                if (sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData - j] < 500)
                {
                    sensors[*i].dist50cm++;
                }
                if (sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData - j] < 1000)
                {
                    sensors[*i].dist100cm++;
                }
                if (sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData - j] < 1500)
                {
                    sensors[*i].dist150cm++;
                }
            }
            if (sensors[*i].dist50cm >= 4)
            {
                if (sensors[*i].timerRunning[0] == 0)
                {
                    sensors[*i].internalTimer1.start();
                    sensors[*i].timerRunning[0] = 1;
                    
                }
            }

           // printf("%lld sen0\n", duration_cast<std::chrono::milliseconds>( sensors[0].internalTimer1.elapsed_time()).count());
            //printf("%lld sen1\n", duration_cast<std::chrono::milliseconds>( sensors[1].internalTimer1.elapsed_time()).count());
                if (sensors[*i].timerRunning[0] == 1)
                {
                    if ((duration_cast<std::chrono::milliseconds>(sensors[*i].internalTimer1.elapsed_time()).count()) <= 1500  && sensors[*i].dist50cm <= 1)
                    {
                        sensors[*i].detectedAt50cm++;
                        sensors[*i].internalTimer1.stop();
                        sensors[*i].internalTimer1.reset();
                        sensors[*i].timerRunning[0] = 0;
                    }
                    else if (duration_cast<std::chrono::milliseconds>( sensors[*i].internalTimer1.elapsed_time()).count() > 1500)
                    {
                        sensors[*i].internalTimer1.stop();
                        sensors[*i].internalTimer1.reset();
                        sensors[*i].timerRunning[0] = 0;
                    }
                
                                   
            }

            if (sensors[*i].dist100cm >= 4)
            {
                if (sensors[*i].timerRunning[1] == 0)
                {
                    sensors[*i].internalTimer2.start();
                    sensors[*i].timerRunning[1] = 1;
                    
                }
            }

                if (sensors[*i].timerRunning[1] == 1 )
                {
                    if ((duration_cast<std::chrono::milliseconds>(sensors[*i].internalTimer2.elapsed_time()).count()) <= 1500 && sensors[*i].dist100cm <= 1)
                    {
                        sensors[*i].detectedAt100cm++;
                         sensors[*i].internalTimer2.stop();
                        sensors[*i].internalTimer2.reset();
                        sensors[*i].timerRunning[1] = 0;
                    }
                    else if (duration_cast<std::chrono::milliseconds>(sensors[*i].internalTimer2.elapsed_time()).count() > 1500)
                    {
                        sensors[*i].internalTimer2.stop();
                        sensors[*i].internalTimer2.reset();
                        sensors[*i].timerRunning[1] = 0;
                    }
                

            }

            if (sensors[*i].dist150cm >= 4)
            {
                if (sensors[*i].timerRunning[2] == 0)
                {
                    sensors[*i].internalTimer3.start();
                    sensors[*i].timerRunning[2] = 1;
                    
                }
            }

                if (sensors[*i].timerRunning[2] == 1 )
                {
                    if ((duration_cast<std::chrono::milliseconds>(sensors[*i].internalTimer3.elapsed_time()).count()) <= 1500 && sensors[*i].dist150cm <= 1)
                    {
                        sensors[*i].detectedAt150cm++;
                        sensors[*i].internalTimer3.stop();
                        sensors[*i].internalTimer3.reset();
                        sensors[*i].timerRunning[2] = 0;
                    }
                    else if (duration_cast<std::chrono::milliseconds>(sensors[*i].internalTimer3.elapsed_time()).count() > 1500)
                    {
                        sensors[*i].internalTimer3.stop();
                        sensors[*i].internalTimer3.reset();
                        sensors[*i].timerRunning[2] = 0;
                    }
                

            }

            sensors[*i].dist50cm = 0;
            sensors[*i].dist100cm = 0;
            sensors[*i].dist150cm = 0;
            zoneMeasured = 0;

            // printf("sens: %d numOfData: %d %d  %d  %d  %d  %d  %d\n", *i,
            // sensors[*i].numofData,
            // sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-5],
            // sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-6],
            // sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-4]
            // ,
            // sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-3],
            // sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-2],
            // sensors[*i].sensorData[sensors[*i].whichArray][sensors[*i].numofData-1]);
        }
        if (sensors[*i].numofData == 1000)
        {
            sensors[*i].numofData = 0;
            if (sensors[*i].whichArray == 0)
                sensors[*i].whichArray = 1;

            else if (sensors[*i].whichArray == 1)
                sensors[*i].whichArray = 0;
        }
        // wait_us(100000);
    }
}

void preInitSensors(){
for (uint8_t i = 0;i<numberOfSensors;i++){
    xshutdown[i] = 0;
}
}

void mountSDCard(){
    printf("Mounting the filesystem... ");
    fflush(stdout);
    fileSystem.mount(&blockDevice);
}


void SaveToSD(int arrayNumber, int sensorNum){
printf("arraynum %d sensnum %d\n", arrayNumber, sensorNum);
fflush(stdout);
    
    sprintf(fileName, "/fs/%d.txt", sensorNum);
    f = fopen(fileName, "a");
 int err = 0;
        for (int i = 0; i < 1000; i++) {
            fflush(stdout);
            err = fprintf(f,"%d,%d\n", sensors[sensorNum].timestampData[arrayNumber][i], sensors[sensorNum].sensorData[arrayNumber][i]);
            if (err < 0) {
                printf("saveFailed\n");
            }
 
    }

    printf("Closing \"/fs/%d.txt\"\n",sensorNum);
    fflush(stdout);
    if (fclose(f)) {
        printf("closingFail\n");
    }


}
void SavePassedPeopleToSD(){
int err = 0;
fflush(stdout);
    

    f = fopen("/fs/passedPeople.txt", "a");
            fflush(stdout);
            err = fprintf(f,"%llu, %d,%d,%d,%d,%d,%d\n", duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count(), passedLeftAt50, passedLeftAt100, passedLeftAt150, passedRightAt50, passedRightAt100, passedRightAt100);
            if (err < 0) {
                printf("saveFailed\n");
            }
    printf("Closing \"/fs/passedPeople.txt\"\n");
    fflush(stdout);
    if (fclose(f)) {
        printf("closingFail\n");
    }



// to be fixed, not working https://pastebin.com/ELC0xZ2D?fbclid=IwAR11s8d0CJJjr92JyfcjjN4ijSGyIBvdspw5xam4VJXA4PKULnvLORB5IKA
}

void checkForSDWrite(){
    uint8_t lastWhichArray[8] ={sensors[0].whichArray, sensors[1].whichArray, sensors[2].whichArray, sensors[3].whichArray, 
                                sensors[4].whichArray, sensors[5].whichArray, sensors[6].whichArray, sensors[7].whichArray};
    while(1){
        saveDelay++;
        printf("checking for write\n");
        for (int i = 0; i<numberOfSensors; i++){           
            if (lastWhichArray[i] != sensors[i].whichArray){
                SaveToSD(lastWhichArray[i], i);
                lastWhichArray[i] = sensors[i].whichArray;
                
            }
        }
        if (saveDelay == 4){
        SavePassedPeopleToSD();
        saveDelay = 0 ;
        }
ThisThread::sleep_for(3s);
    }

}

void cleanUpSD(){
    char fileToRemoveName[10];
            fflush(stdout);
        fileSystem.remove("/passedPeople.txt");
        for (int i=0;i<numberOfSensors;i++){
        fflush(stdout);
        sprintf(fileToRemoveName, "/%d.txt", i);
        fileSystem.remove(fileToRemoveName);
        printf("\ncleanup %d executed\n", i);
        }

}
void initSingleSensor(int i){

sensors[i].obj = new VL53L1X(device_i2c, &xshutdown[i]);
sensors[i].obj->init_sensor(sensorAdress[i]);
sensors[i].obj->vl53l1x_set_distance_mode(shortDistanceMode);
sensors[i].obj->vl53l1x_set_timing_budget_in_ms(timingBudgetInMs);
sensors[i].obj->vl53l1x_set_inter_measurement_in_ms(timingBudgetInMs);

sensors[i].obj->vl53l1x_set_roi(roiX,roiY);
sensors[i].obj->vl53l1x_start_ranging();
}

void countPeople(){
   /* char cm[10];
    int ypos = 15;
    
    BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"50cm", LEFT_MODE); 
    BSP_LCD_DisplayStringAt(100, 0, (uint8_t *)"100cm", LEFT_MODE); 
    BSP_LCD_DisplayStringAt(200, 0, (uint8_t *)"150cm", LEFT_MODE); 
    */
    
while(1){
    for (int y=0;y<8;y++){

    /*sprintf((char*)cm, "%d", sensors[y].detectedAt50cm); //detectedat
    BSP_LCD_DisplayStringAt(0, ypos, (uint8_t *)&cm, LEFT_MODE); 
    sprintf((char*)cm, "%d", sensors[y].detectedAt100cm);
    BSP_LCD_DisplayStringAt(100, ypos, (uint8_t *)&cm, LEFT_MODE); 
    sprintf((char*)cm, "%d", sensors[y].detectedAt150cm);
    BSP_LCD_DisplayStringAt(200, ypos, (uint8_t *)&cm, LEFT_MODE); 
*/

    if ( y < 4)
    {
        if(sensors[y].pdetectedAt50cm != sensors[y].detectedAt50cm){
            if (detectionBoolflag == 0){
                detectionTimer.start();
                detectionBoolflag = 1;
            }
            lDetectionFlagAt50cm++;
        }
        if(sensors[y].pdetectedAt100cm != sensors[y].detectedAt100cm){
            if (detectionBoolflag == 0){
                detectionTimer.start();
                detectionBoolflag = 1;
            }           
            lDetectionFlagAt100cm++;
        }
        if(sensors[y].pdetectedAt150cm != sensors[y].detectedAt150cm){
            if (detectionBoolflag == 0){
                detectionTimer.start();
                detectionBoolflag = 1;
            }             
            lDetectionFlagAt150cm++;      
        }
                              
    }
    else
    {
        if(sensors[y].pdetectedAt50cm != sensors[y].detectedAt50cm){
            if (detectionBoolflag == 0){
                detectionTimer.start();
                detectionBoolflag = 1;
            }   
            rDetectionFlagAt50cm++;      
        }
            
        if(sensors[y].pdetectedAt100cm != sensors[y].detectedAt100cm){
            if (detectionBoolflag == 0){
                detectionTimer.start();
                detectionBoolflag = 1;
            }
             rDetectionFlagAt100cm++;           
        }
           
         if(sensors[y].pdetectedAt150cm != sensors[y].detectedAt150cm){
            if (detectionBoolflag == 0){
                detectionTimer.start();
                detectionBoolflag = 1;
            }  
            rDetectionFlagAt150cm++;                
         }
                              
    }


    sensors[y].pdetectedAt50cm = sensors[y].detectedAt50cm;
    sensors[y].pdetectedAt100cm = sensors[y].detectedAt100cm;
    sensors[y].pdetectedAt150cm = sensors[y].detectedAt150cm;

//ypos = ypos+15;

    } 



if (detectionBoolflag == 1 && duration_cast<std::chrono::milliseconds>(detectionTimer.elapsed_time()).count() > 1500){

if (lDetectionFlagAt50cm >= 2)
    passedLeftAt50++;
else if (lDetectionFlagAt100cm >= 2)
    passedLeftAt100++;
else if (lDetectionFlagAt150cm >= 2)
    passedLeftAt150++;    

if (rDetectionFlagAt50cm >= 2)
    passedRightAt50++;
else if (rDetectionFlagAt100cm >= 2)
    passedRightAt100++;
else if (rDetectionFlagAt150cm >= 2)
    passedRightAt150++; 

lDetectionFlagAt50cm = 0;
lDetectionFlagAt100cm = 0;
lDetectionFlagAt150cm = 0;
rDetectionFlagAt50cm = 0;
rDetectionFlagAt100cm = 0;
rDetectionFlagAt150cm = 0;

detectionTimer.stop();
detectionTimer.reset();
detectionBoolflag = 0;
}

   

//printf("pass %d %d\n",passedLeftAt50, passedRightAt50);

//ypos = 15;
ThisThread::sleep_for(200ms); ///may be edited to adjust scanning frequency
}
    
}
void printMeasurements(){
    while(1)
    {
    char cm[10];
    BSP_LCD_SetFont(&Font12);
    int16_t xpos = -179;
    xDist = 39;
    //BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    sprintf((char*)cm, "%llu s", duration_cast<std::chrono::seconds>(t.elapsed_time()).count());
    BSP_LCD_DisplayStringAt(0, 3, (uint8_t *)&cm, LEFT_MODE); 

     sprintf((char*)cm, "%u M.", totalNumOfData/8);
    BSP_LCD_DisplayStringAt(0, 3, (uint8_t *)&cm, RIGHT_MODE);    
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    for (int sens=0;sens<numberOfSensors;sens++){
        
    sprintf((char*)cm, "  %d  ", sensors[sens].avgSensorData);//sensors[y].detectedAt50cm); //detectedat
    BSP_LCD_DisplayStringAt(xpos, 259, (uint8_t *)&cm, CENTER_MODE); 

        linearLocation = 0 + (sensors[sens].avgSensorData - 0) * 200 / 2600;
        if (linearLocation>=200)
            linearLocation = 200;
        if (sensors[sens].avgSensorData < 3000){
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_FillRect(xDist, 31, 38, 200-linearLocation);      
        BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
         BSP_LCD_FillRect(xDist, 231-linearLocation, 38, linearLocation); 
        }
    xDist = xDist + 52;
    xpos = xpos + 52;
    }
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    sprintf((char*)cm, "%d", passedLeftAt50);//sensors[y].detectedAt50cm); //detectedat
    BSP_LCD_DisplayStringAt(-221, 195, (uint8_t *)&cm, CENTER_MODE); 
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
    sprintf((char*)cm, "%d", passedLeftAt100);//sensors[y].detectedAt50cm); //detectedat
    BSP_LCD_DisplayStringAt(-221, 171, (uint8_t *)&cm, CENTER_MODE); 
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    sprintf((char*)cm, "%d", passedLeftAt150);//sensors[y].detectedAt50cm); //detectedat
    BSP_LCD_DisplayStringAt(-221, 147, (uint8_t *)&cm, CENTER_MODE); 

    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    sprintf((char*)cm, "%d", passedRightAt50);//sensors[y].detectedAt50cm); //detectedat
    BSP_LCD_DisplayStringAt(225, 195, (uint8_t *)&cm, CENTER_MODE);
    BSP_LCD_SetTextColor(LCD_COLOR_YELLOW); 
    sprintf((char*)cm, "%d", passedRightAt100);//sensors[y].detectedAt50cm); //detectedat
    BSP_LCD_DisplayStringAt(225, 171, (uint8_t *)&cm, CENTER_MODE); 
    BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
    sprintf((char*)cm, "%d", passedRightAt150);//sensors[y].detectedAt50cm); //detectedat
    BSP_LCD_DisplayStringAt(225, 147, (uint8_t *)&cm, CENTER_MODE); 

    //BSP_LCD_FillRect(39, 31, 402, 200);

//result = minOutRange + (in - minInRange) * OutSize / InSize where OutSize and InSize are maxOutRange - minOutRange and maxInRange - minInRange


    ThisThread::sleep_for(100ms);
}
}

int main()
{
mountSDCard();
cleanUpSD();

BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
handleSD.start(callback(checkForSDWrite));
    
//BSP_LCD_DisplayStringAt(0, 0, (uint8_t *)"&'$%", LEFT_MODE); 

BSP_LCD_DrawBitmap(0, 0, (uint8_t *)gl_file);
    preInitSensors();
    wait_us(10000); 
       

 for (int i=0;i<8;i++){
    initSingleSensor(sensorNumber[i]);
    wait_us(100000); 
 }
  checkaddrs();
 t.start();   
 for (int i=0;i<8;i++){
 thread[sensorNumber[i]].start(callback(getROIData,&sensorNumber[i]));

 countPeopleThread.start(callback(countPeople));
 printMeasurementsThread.start(callback(printMeasurements));

 }




//thread[sensorNumber[b]].start(callback(getROIData,&sensorNumber[b]));

}
    