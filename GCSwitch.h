#ifndef __GCSwitch__
#define __GCSwitch__

#include "GCRobot.h"
#include "GCSharedObjects.h"
#include "GCRunLoop.h"

class GCSwitch;

class GCSwitchDelegate {
public:
    virtual void switch_didRise(GCSwitch *s) = 0;
    virtual void switch_didFall(GCSwitch *s) = 0;
};

class GCSwitch : GCRunLoop::Callback {
    GCSwitchDelegate *delegate = nullptr;
    InterruptIn pin;
    bool isOn = false;
    void riseCallback();
    void fallCallback();
    
public:
    GCSwitch(GCSwitchDelegate *delegate, PinName pin);
    void runLoopCallback(void *arg);
    INLINE bool read() {
        return pin;
    }
    INLINE operator bool() {
        return read();
    }
};

#endif
