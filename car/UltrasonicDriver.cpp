#include "UltrasonicDriver.h"
#include "mbed.h"

Ultrasonic::Ultrasonic(PinName tx, PinName rx) : _tx(tx),_rx(rx)
{
    _tx.input();
    t.reset();
    triger.attach(this, &Ultrasonic::triger_func, 0.1);
    _rx.rise(this, &Ultrasonic::start_func);
    _rx.fall(this, &Ultrasonic::end_func);
}

void Ultrasonic::triger_func()
{
    _tx.output();
    _tx = 1;
    wait_us(5);
    _tx = 0;
    _tx.input();

}

void Ultrasonic::start_func()
{
    t.reset();
    t.start();
}

void Ultrasonic::end_func()
{
    t.stop();
    if (( t > 0.00012 ) & ( t < 0.0187)) {
        eco = 331.5*(t/2.0)*1000; // 30mm ~ 3000mm
    }
}

float Ultrasonic::read()
{
    return eco;
}