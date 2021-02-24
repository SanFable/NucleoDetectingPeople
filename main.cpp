#include "mbed.h"
#include "CSensorManager.h"




int main()
{


CSensorManager oManager = CSensorManager();

oManager.initSensors();
while(1){
oManager.ReadFromSensors();
}
    
}