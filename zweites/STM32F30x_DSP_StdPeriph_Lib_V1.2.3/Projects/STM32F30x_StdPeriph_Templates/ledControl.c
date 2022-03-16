#include <stdarg.h>
#include "stm32f30x_conf.h"
#include "ledControl.h"
int activateLED(int arguments,...){
    int send = 0b11000000;
    va_list ap;
    va_start(ap,arguments);
    for (int i = 0; i < arguments; i++)
    {
        send|=va_arg(ap,int);
    }
    va_end(ap);
    return send;
    //SPI_SendData8(SPI1,send);
    
}

void toggleRow(int row){
    int send = 0b010000;
    send|=row;
    //SPI_SendData8(SPI1,send);
}