#include "mbed.h"
#include "DebugFunctions.h"
DebugFunctions::DebugFunctions() {}
DebugFunctions::~DebugFunctions() {}
 void DebugFunctions::checkaddrs(){
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