#ifndef __GC16IRSensors__
#define __GC16IRSensors__

#include "GCRobot.h"
#include "GCSharedObjects.h"
#include "GCRunLoop.h"
#include <stdarg.h>

class GC16IRSensors;

class GC16IRSensorsDelegate {
public:
    virtual void irSensors_ballDetected(
        GC16IRSensors *sensors,
        GcraudNano::GCBallDegrees degrees,
        GcraudNano::GCBallDistance distance
    ) = 0;
};

class GC16IRSensors : GCRunLoop::Callback {
    static constexpr int numOfSensors = 16;
    static constexpr int farBase = 3;
    static constexpr int middleBase = 5;
    static constexpr int readingTimeAsMicroSeconds = 833 * 2;
    static constexpr int readingTimeAsMilliSeconds = readingTimeAsMicroSeconds / 1000;
    DigitalIn *pins[numOfSensors];
    DigitalIn **pinsPoint = pins;
    DigitalIn **pinsLimit = pins + numOfSensors;
    Thread *readingThread = nullptr;
    unsigned char threadStack[GcraudNano::defaultStackSize];
    
    static void __attribute__((noreturn)) threadCallback(const void *arg);
    void readValues();
    
public:
    GC16IRSensors(GC16IRSensorsDelegate *delegate, PinName pins, ...);
    GC16IRSensorsDelegate *delegate;
    void runLoopCallback(void *arg);
};

#endif
