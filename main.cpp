#include "mbed.h"
#include "CSensorManager.h"




int main()
{


CSensorManager oManager = CSensorManager();

oManager.initSensors();
//wait_us(100000);

//oManager.ReadFromSensors();

    
}