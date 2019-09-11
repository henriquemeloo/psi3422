#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H
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
        void clk();
        void cclk();    

    private:
        DigitalOut *motorLeftClk;
        DigitalOut *motorLeftNClk;
        DigitalOut *motorRightClk;
        DigitalOut *motorRightNClk;
    
};

#endif // MOTORDRIVER_H