#ifndef LEDCONTROL_H
#define LEDCONTROl_H
    static const int LED1=1<<5;//100000
    static const int LED2=1<<4;//010000
    static const int LED3=1<<3;//001000
    static const int LED4=1<<2;//000100
    static const int LED5=1<<1;//000010
    static const int LED6=1;   //000001

    static const int ROW1=1;
    static const int ROW2=2;
    static const int ROW3=3;
    static const int ROW4=4;
    static const int ROW5=5;
    static const int ROW6=6;

    //takes a variable amount of arguments which should each be one of
    //the following: LED1, LED2, LED3, LED4, LED5, LED6
    int activateLED(int arguments,...);
    void toggleRow(int row);

#endif