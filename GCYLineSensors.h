#ifndef __GCYLineSensors__
#define __GCYLineSensors__

#include "GCRobot.h"
#include "GCRunLoop.h"
#include "GCSharedObjects.h"

class GCYLineSensorsDelegate;

class GCYLineSensors : GCRunLoop::Callback {    

private:
    InterruptIn leftInt;
    DigitalIn   backInt;
    InterruptIn rightInt;
    
    void leftDidRise();
    void leftDidFall();
    void backDidRise();
    void backDidFall();
    void rightDidRise();
    void rightDidFall();
    
    GcraudNano::GCLineDegrees currentDegrees = GcraudNano::GCLineDegreesNone;
    
    Thread thread;
    unsigned char threadStack[GcraudNano::defaultStackSize];
    static void __attribute__((noreturn)) threadCallback(void const *arg);
    void __attribute__((noreturn)) observe();
    
public:
    GCYLineSensors(PinName upperLeft, PinName back, PinName upperRight, GCYLineSensorsDelegate *delegate);
    GCYLineSensorsDelegate *delegate = nullptr;
    INLINE GcraudNano::GCLineDegrees read() {
        return currentDegrees;
    }
    void runLoopCallback(void *arg);
};

class GCYLineSensorsDelegate {
public:
    virtual void lineSensors_lineDetected(GCYLineSensors *sensors, GcraudNano::GCLineDegrees degrees) = 0;
};

#endif
