#include "UltrasonicDriver.h"
#include "mbed.h"

UltrasonicDriver::UltrasonicDriver(PinName tx, PinName rx) : _tx(tx),_rx(rx)
{
    _tx.input();
    t.reset();
    triger.attach(this, &UltrasonicDriver::triger_func, 0.1);
    _rx.rise(this, &UltrasonicDriver::start_func);
    _rx.fall(this, &UltrasonicDriver::end_func);
}

void UltrasonicDriver::triger_func()
{
    _tx.output();
    _tx = 1;
    wait_us(5);
    _tx = 0;
    _tx.input();

}

void UltrasonicDriver::start_func()
{
    t.reset();
    t.start();
}

void UltrasonicDriver::end_func()
{
    t.stop();
    if (( t > 0.00012 ) & ( t < 0.0187)) {
        eco = 331.5*(t/2.0)*1000; // 30mm ~ 3000mm
    }
}

float UltrasonicDriver::read()
{
    return eco;
}