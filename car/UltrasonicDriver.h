#ifndef ULTRASONICDRIVER_H
#define ULTRASONICDRIVER_H

#include "mbed.h"

class Ultrasonic
{
public:
    Ultrasonic(PinName tx, PinName rx);
    float read();

private:
    DigitalInOut _tx;
    InterruptIn _rx;
    Timer t;
    Ticker triger;
    float eco;
    void triger_func(void);
    void start_func(void);
    void end_func(void);
};

#endif
