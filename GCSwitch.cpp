#include "GCSwitch.h"

GCSwitch::GCSwitch(GCSwitchDelegate *_delegate, PinName _pin) : delegate(_delegate), pin(_pin)
{
    MBED_ASSERT(delegate != nullptr);
    
    if (pin) {
        GCRunLoop::mainLoop.dispatch_async(this, reinterpret_cast<void *>(1));
    } else {
        GCRunLoop::mainLoop.dispatch_async(this, reinterpret_cast<void *>(0));
    }
    pin.rise(this, &GCSwitch::riseCallback);
    pin.fall(this, &GCSwitch::fallCallback);
}

void GCSwitch::riseCallback()
{
    GCRunLoop::mainLoop.dispatch_async(this, reinterpret_cast<void *>(1));
}

void GCSwitch::fallCallback()
{
    GCRunLoop::mainLoop.dispatch_async(this, reinterpret_cast<void *>(0));
}

void GCSwitch::runLoopCallback(void *arg)
{
    if (reinterpret_cast<intptr_t>(arg)) {
        delegate->switch_didRise(this);
    } else {
        delegate->switch_didFall(this);
    }
}
