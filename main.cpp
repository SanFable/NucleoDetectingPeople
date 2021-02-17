#include "mbed.h"
#include "CSensorManager.h"




int main()
{


CSensorManager oManager = CSensorManager();
printf("lol\n");
oManager.initSensors();
oManager.ReadFromSensors();

    
}