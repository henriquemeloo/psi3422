#ifndef MOTORDRIVER_H
#define MOTORFRIVER_H
#include "mbed.h"

class MotorDriver {
    public:
        MotorDriver(
            PinName leftClk, PinName leftNClk,
            PinName rightClk, PinName rightNClk
            );
        void stop();
        void fwd();
        void bkw();
        void left();
        void right();
    

    private:
        DigitalOut *motorLeftClk;
        DigitalOut *motorLeftNClk;
        DigitalOut *motorRightClk;
        DigitalOut *motorRightNClk;
    
};

#endif // MOTORDRIVER_H