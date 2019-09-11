#include "UltrasonicDriver.h"
#include "mbed.h"
    
UltrasonicDriver::UltrasonicDriver(
    PinName trigPin, PinName echoPin, float updateSpeed, float timeout
    ):_trig(trigPin), _echo(echoPin) {
    _updateSpeed = updateSpeed;
    _timeout = timeout;
}

UltrasonicDriver::UltrasonicDriver(
    PinName trigPin, PinName echoPin, float updateSpeed, float timeout, void onUpdate(int)
    ):_trig(trigPin), _echo(echoPin) {
    _onUpdateMethod=onUpdate;
    _updateSpeed = updateSpeed;
    _timeout = timeout;
    _t.start ();
}

void UltrasonicDriver::_startT() {
    if(_t.read()>600)
    {
        _t.reset ();
    }
    start = _t.read_us ();
}
    
void UltrasonicDriver::_updateDist() {
    end = _t.read_us ();
    done = 1;
    _distance = (end - start)/6;       
    _tout.detach();
    _tout.attach(this,&UltrasonicDriver::_startTrig, _updateSpeed);   
}

void UltrasonicDriver::_startTrig(void) {
    _tout.detach();
    _trig=1;             
    wait_us(10);        
    done = 0;            
    _echo.rise(this,&UltrasonicDriver::_startT);   
    _echo.fall(this,&UltrasonicDriver::_updateDist);
    _echo.enable_irq ();
    _tout.attach(this,&UltrasonicDriver::_startTrig,_timeout);
    _trig=0;
}

int UltrasonicDriver::getCurrentDistance(void) {
    return _distance;
}

void UltrasonicDriver::pauseUpdates(void) {
    _tout.detach();
    _echo.rise(NULL);
    _echo.fall(NULL);
}

void UltrasonicDriver::startUpdates(void) {
    _startTrig();
}

void UltrasonicDriver::attachOnUpdate(void method(int)) {
    _onUpdateMethod = method;
}

void UltrasonicDriver::changeUpdateSpeed(float updateSpeed) {
    _updateSpeed = updateSpeed;
}

float UltrasonicDriver::getUpdateSpeed() {
    return _updateSpeed;
}

int UltrasonicDriver::isUpdated(void) {
    //printf("%d", done);
    d=done;
    done = 0;
    return d;
}

void UltrasonicDriver::checkDistance(void) {
    if(isUpdated()) {
        (*_onUpdateMethod)(_distance);
    }
}

/* UltrasonicDriver::UltrasonicDriver(PinName tx, PinName rx) : _tx(tx),_rx(rx)
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
} */