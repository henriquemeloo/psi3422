#ifndef ENCODERDRIVER_H
#define ENCODERFRIVER_H
#include "mbed.h"

class EncoderDriver {
    public:
        EncoderDriver(PinName encoderPin);
        void wait(int pulses);
        int distanceToPulses(double distance);

    private:
        DigitalIn *encoder;
};

#endif // ENCODERDRIVER_H