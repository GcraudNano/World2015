#ifndef __GCKicker__
#define __GCKicker__

#include "GCRobot.h"

class GCKicker {
    static constexpr int kickTime = 45;
    static constexpr int kickInterval = 150;
    DigitalOut pin;
    bool readyToKick = true;
    RtosTimer unkickTimer   = RtosTimer(unkick, osTimerOnce, this);
    RtosTimer validateTimer = RtosTimer(validate, osTimerOnce, &readyToKick);
    static void unkick(void const *arg);
    static void validate(void const *arg);
    
public:
    GCKicker(PinName pin);
    void kick();
};

#endif
