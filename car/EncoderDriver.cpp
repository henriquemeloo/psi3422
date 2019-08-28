#include "EncoderDriver.h"
const double pi = 3.14159265358979323846;

EncoderDriver::EncoderDriver(PinName encoderPin) {
    this->encoder = new DigitalIn(encoderPin);
}

void EncoderDriver::waitPulses (int pulses) {
    // Wait for pulses to be sensed by encoder.
    int lastEncoderSignal = (*encoder).read();
    int pulsesCount = 0;

    while (pulsesCount < pulses) {
        wait_ms(5);
        if((*encoder).read() != lastEncoderSignal){
            lastEncoderSignal = (*encoder).read();
            pulsesCount++;
        }
    }
}

int EncoderDriver::distanceToPulses (double distance) {
    double radius = 4.2;
    int totalPulses = 18;

    int pulses = distance / ((2 * pi * radius) / totalPulses);
    
    return pulses;
}