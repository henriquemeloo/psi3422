#include "EncoderDriver.h"
#include <math.h>

EncoderDriver::EncoderDriver(PinName encoderPin) {
    this->encoder = new DigitalIn(encoderPin);
}

EncoderDriver::wait(int pulses) {
    // Wait for pulses to be sensed by encoder.
    
}

int EncoderDriver::distanceToPulses (double distance) {
    double radius = 4.2;
    int totalPulses = 18;

    int pulses = distance / ((2 * pi * radius) / totalPulses);
    
    return pulses;
}