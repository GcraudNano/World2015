#include "GC16IRSensors.h"
#include "GCRunLoop.h"

GC16IRSensors::GC16IRSensors(GC16IRSensorsDelegate *_delegate, PinName arg, ...) : delegate(_delegate)
{
    MBED_ASSERT(delegate != nullptr);
    
    PinName pin = arg;
    va_list list;
    va_start(list, arg);
    do {
        *pinsPoint = new DigitalIn(pin);
        pin = static_cast<PinName>(va_arg(list, int));
    } while (++pinsPoint != pinsLimit);
    va_end(list);
    pinsPoint = pins;
    readingThread = new Thread(threadCallback, this, osPriorityHigh, GcraudNano::defaultStackSize, threadStack);
}

void GC16IRSensors::threadCallback(const void *arg)
{
    GC16IRSensors *self = reinterpret_cast<GC16IRSensors *>(const_cast<void *>(arg));
    while (1) {
        self->readValues();
//        self->readingThread->yield();
    }
}

void GC16IRSensors::runLoopCallback(void *values)
{
    Timer timer;
    timer.start();
    int *valuesPoint = static_cast<int *>(values);
    while (timer.read_us() <= readingTimeAsMicroSeconds) {
        do {
            *valuesPoint += ! **pinsPoint;
        } while (valuesPoint++, ++pinsPoint != pinsLimit);
        pinsPoint = pins;
        valuesPoint = static_cast<int *>(values);
    }
    
    readingThread->signal_set(1);
}

void GC16IRSensors::readValues()
{
    using namespace GcraudNano;
    static GCIRSensorDegrees degrees[] = {
        GCIRSensorDegrees::D1125,
        GCIRSensorDegrees::D3375,
        GCIRSensorDegrees::D5625,
        GCIRSensorDegrees::D7875,
        GCIRSensorDegrees::D10125,
        GCIRSensorDegrees::D12375,
        GCIRSensorDegrees::D14625,
        GCIRSensorDegrees::D16875,
        GCIRSensorDegrees::D_16875,
        GCIRSensorDegrees::D_14625,
        GCIRSensorDegrees::D_12375,
        GCIRSensorDegrees::D_10125,
        GCIRSensorDegrees::D_7875,
        GCIRSensorDegrees::D_5625,
        GCIRSensorDegrees::D_3375,
        GCIRSensorDegrees::D_1125,
    };
    
    int values[numOfSensors] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    int *valuesPoint = values;
    
    GCRunLoop::mainLoop.dispatch_async(this, static_cast<void *>(values));
    readingThread->signal_wait(0, readingTimeAsMilliSeconds + 500);
    readingThread->signal_clr(1);
    
    int maxValue = 0;
    int maxDegrees1 = -1;
    int maxDegrees2 = -1;
    int numOfRespondedSensors = 0;
    GCIRSensorDegrees *degreesPoint = degrees;
    
    do {
        if (*valuesPoint) {
            numOfRespondedSensors++;
            if (*valuesPoint > maxValue) {
                maxValue = *valuesPoint;
                maxDegrees1 = degToInt(*degreesPoint);
                maxDegrees2 = -1;
            } else if (*valuesPoint == maxValue) {
                maxDegrees2 = degToInt(*degreesPoint);
            }
        }
    } while (degreesPoint++, valuesPoint++, ++pinsPoint != pinsLimit);
    
    pinsPoint = pins;
    
    if (maxValue) {
        GCBallDistance distance;
        if (numOfRespondedSensors <= farBase) {
            distance = GCBallDistance::Far;
        } else if (numOfRespondedSensors <= middleBase) {
            distance = GCBallDistance::Middle;
        } else {
            distance = GCBallDistance::Near;
        }
        if (maxDegrees2 == -1) {
            /* only one sensor has the max value */
            delegate->irSensors_ballDetected(this, static_cast<GCBallDegrees>(maxDegrees1), distance);
        } else {
            /* two sensors have the max values */
            if (maxDegrees1 == degToInt(GCIRSensorDegrees::D16875) &&
                maxDegrees2 == degToInt(GCIRSensorDegrees::D_16875)) {
                delegate->irSensors_ballDetected(this, GCBallDegrees::D18000, distance);
            } else {
                delegate->irSensors_ballDetected(
                    this,
                    static_cast<GCBallDegrees>((maxDegrees1 + maxDegrees2) / 2),
                    distance
                );
            }
        }
    } else {
        delegate->irSensors_ballDetected(this, GCBallDegrees::DNone, GCBallDistance::Far);
    }
}
