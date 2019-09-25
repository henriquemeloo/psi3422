#include "UltrasonicDriver.h"
#include "mbed.h"
    
UltrasonicDriver::UltrasonicDriver(
    PinName trigPin, PinName echoPin, float updateSpeed, float timeout
    ):_trig(trigPin), _echo(echoPin) {
    _updateSpeed = updateSpeed;
    _timeout = timeout;
}

UltrasonicDriver::UltrasonicDriver(
    PinName trigPin, PinName echoPin, float updateSpeed, float timeout, bool onUpdate(int)
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

void UltrasonicDriver::attachOnUpdate(bool method(int)) {
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

bool UltrasonicDriver::checkDistance(void) {
    if(isUpdated()) {
        return (*_onUpdateMethod)(_distance);
    }
}
