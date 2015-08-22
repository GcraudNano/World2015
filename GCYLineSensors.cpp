#include "GCYLineSensors.h"
using namespace GcraudNano;

GCYLineSensors::GCYLineSensors(PinName left, PinName back, PinName right, GCYLineSensorsDelegate *_delegate) : leftInt(left), backInt(back), rightInt(right),
thread(threadCallback, this, osPriorityNormal, GcraudNano::defaultStackSize, threadStack),
delegate(_delegate)
{
    MBED_ASSERT(delegate != nullptr);
    
    leftInt.rise(this, &GCYLineSensors::leftDidRise);
    leftInt.fall(this, &GCYLineSensors::leftDidFall);
    rightInt.rise(this, &GCYLineSensors::rightDidRise);
    rightInt.fall(this, &GCYLineSensors::rightDidFall);
}

void GCYLineSensors::threadCallback(const void *arg)
{
    GCYLineSensors *self = reinterpret_cast<GCYLineSensors *>(const_cast<void *>(arg));
    self->observe();
}

void GCYLineSensors::observe()
{
    int back = backInt;
//    int left = leftInt;
//    int right = rightInt;
    
    while (1) {
        if (back != backInt) {
            if (back) {
                backDidFall();
            } else {
                backDidRise();
            }
            back = ! back;
        }
        
//        if (left != leftInt) {
//            if (left) {
//                leftDidFall();
//            } else {
//                leftDidRise();
//            }
//            left = ! left;
//        }
//        if (right != rightInt) {
//            if (right) {
//                rightDidFall();
//            } else {
//                rightDidRise();
//            }
//            right = ! right;
//        }
        
        thread.yield();
    }
}

void GCYLineSensors::leftDidRise()
{
    currentDegrees |= GCLineDegrees45;
    GCRunLoop::mainLoop.dispatch_async(this, nullptr);
//    delegate->lineSensors_lineDetected(this, currentDegrees);
}

void GCYLineSensors::leftDidFall()
{
    currentDegrees &= ~(GCLineDegrees45);
    GCRunLoop::mainLoop.dispatch_async(this, nullptr);
//    delegate->lineSensors_lineDetected(this, currentDegrees);
}

void GCYLineSensors::backDidRise()
{
    currentDegrees |= GCLineDegrees180;
    GCRunLoop::mainLoop.dispatch_async(this, nullptr);
//    delegate->lineSensors_lineDetected(this, currentDegrees);
}

void GCYLineSensors::backDidFall()
{
    currentDegrees &= ~(GCLineDegrees180);
    GCRunLoop::mainLoop.dispatch_async(this, nullptr);
//    delegate->lineSensors_lineDetected(this, currentDegrees);
}

void GCYLineSensors::rightDidRise()
{
    currentDegrees |= GCLineDegrees_45;
    GCRunLoop::mainLoop.dispatch_async(this, nullptr);
//    delegate->lineSensors_lineDetected(this, currentDegrees);
}

void GCYLineSensors::rightDidFall()
{
    currentDegrees &= ~(GCLineDegrees_45);
    GCRunLoop::mainLoop.dispatch_async(this, nullptr);
//    delegate->lineSensors_lineDetected(this, currentDegrees);
}

void GCYLineSensors::runLoopCallback(void *arg)
{
    delegate->lineSensors_lineDetected(this, currentDegrees);
}
