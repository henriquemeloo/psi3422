#include "MotorDriver.h"

MotorDriver::MotorDriver(PinName leftClk, PinName leftNClk,
            PinName rightClk, PinName rightNClk){
    this->motorLeftClk = new DigitalOut(leftClk);
    this->motorLeftNClk = new DigitalOut(leftNClk);
    this->motorRightClk = new DigitalOut(rightClk);
    this->motorRightNClk = new DigitalOut(rightNClk);
};

void MotorDriver::stop(){
    *motorLeftClk = 0;
    *motorLeftNClk = 0;
    *motorRightClk = 0;
    *motorRightNClk = 0;
}

void MotorDriver::fwd(){
    *motorLeftClk = 1;
    *motorLeftNClk = 0;
    *motorRightClk = 1;
    *motorRightNClk = 0;
};

void MotorDriver::bkw(){
    *motorLeftClk = 0;
    *motorLeftNClk = 1;
    *motorRightClk = 0;
    *motorRightNClk = 1;
};

void MotorDriver::left(){
    *motorLeftClk = 1;
    *motorLeftNClk = 0;
    *motorRightClk = 0;
    *motorRightNClk = 0;
};

void MotorDriver::right(){
    *motorLeftClk = 0;
    *motorLeftNClk = 0;
    *motorRightClk = 1;
    *motorRightNClk = 0;
};

void MotorDriver::clk(){
    *motorLeftClk = 0;
    *motorLeftNClk = 1;
    *motorRightClk = 1;
    *motorRightNClk = 0;
};

void MotorDriver::cclk(){
    *motorLeftClk = 1;
    *motorLeftNClk = 0;
    *motorRightClk = 0;
    *motorRightNClk = 1;
};
