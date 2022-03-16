#include <stdio.h>
#include "ledControl.h"
int main(int argc, char const *argv[])
{
    int sent=activateLED(3,LED1,LED2,LED3);
    printf("led 1, 2, 3 should be 248 and is: %d",sent);   
    int sent=activateLED(3,LED4,LED5,LED6);
    printf("led 4, 5, 6 should be 199 and is: %d",sent);   
    int sent=activateLED(3,LED1,LED2,LED3,LED4,LED5,LED6);
    printf("led 1, 2, 3,4 ,5 ,6 should be 255 and is: %d",sent);   
    return 0;
}
