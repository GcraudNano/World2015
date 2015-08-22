#include "GCKicker.h"

GCKicker::GCKicker(PinName _pin) : pin(_pin)
{
}

void GCKicker::kick()
{
    if (readyToKick) {
        pin = 1;
        unkickTimer.start(kickTime);
        readyToKick = false;
    }
}

void GCKicker::unkick(void const *arg)
{
    GCKicker *self = reinterpret_cast<GCKicker *>(const_cast<void *>(arg));
    self->pin = 0;
    self->validateTimer.start(kickInterval);
}

void GCKicker::validate(const void *arg)
{
    bool *ready = reinterpret_cast<bool *>(const_cast<void *>(arg));
    *ready = true;
}
