#include "mbed.h"
static BufferedSerial serial_port(USBTX, USBRX, 9600);

int main(void)
{
   for (int i=0;i<5;i++)
    printf("hello\n");

    
}