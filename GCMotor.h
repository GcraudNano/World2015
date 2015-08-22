#ifndef __GCMotor__
#define __GCMotor__

#include "GCRobot.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
class GCMotor {
public:
    static constexpr int numOfSteps = 6;
    
private:
    DigitalOut digital1;
    DigitalOut digital2;
    PwmOut analog;
    RtosTimer stepTimer = RtosTimer(timerCallback, osTimerPeriodic, this);
    Mutex mutex = Mutex();
    float currentSpeed = 0;
    float steps[numOfSteps];
    bool shouldHold = false;
    bool shouldFree = false;
    float *stepsPoint = steps;
    float *stepsTerminalPoint = steps + numOfSteps - 1;
    bool forcedToStop = false;
    
    static void timerCallback(const void *arg);
    void setPower(float power);
    void step();
    
public:
    GCMotor(PinName dig1, PinName dig2, PinName ana);
    void forward(float power);
    void hold(void);
    void free(void);
    void holdImmediately();
    void freeImmediately();
    float fixPower = 0;
    float getCurrentSpeed();
};
#pragma GCC diagnostic pop

#endif
