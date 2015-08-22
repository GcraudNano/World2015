#include "GCMotor.h"
#include "GCRunLoop.h"

#define ENABLE_SIN

void GCMotor::timerCallback(const void *arg)
{
    GCMotor *self = reinterpret_cast<GCMotor *>(const_cast<void *>(arg));
    self->step();
}

GCMotor::GCMotor(PinName dig1, PinName dig2, PinName ana) : digital1(dig1), digital2(dig2), analog(ana)
{
    for (int i = 0; i < numOfSteps; i++) {
        steps[i] = 0;
    }
    stepTimer.start(20);
    analog.period_us(10); // 20
    freeImmediately();
}

void GCMotor::forward(float power)
{
    shouldFree = false;
    shouldHold = false;
    forcedToStop = false;
#ifdef ENABLE_SIN
    synchronized(mutex) {
        stepsPoint = stepsTerminalPoint;
        float a = currentSpeed < power ? (power - currentSpeed) / 2 : (currentSpeed - power) / 2;
        float b = (currentSpeed + power) / 2;
        int step = 180 / numOfSteps;
        int x = 180 - step;
        int p = currentSpeed < power ? 90 : -90;
        *stepsPoint = power;
        while (stepsPoint-- != steps) {
            *stepsPoint = a * sin(radians(x - p)) + b;
            x -= step;
        }
//        currentSpeed = *++localStepsPoint;
        stepsPoint = steps;
    } endSynchronized
#else
    currentSpeed = power;
#endif
}

void GCMotor::step()
{
#ifdef ENABLE_SIN
    synchronized(mutex) {
        if (stepsPoint != stepsTerminalPoint) {
            currentSpeed = *++stepsPoint;
            setPower(currentSpeed);
        } else if (! forcedToStop) {
            /* to fix */
            if (shouldFree || shouldHold) {
                setPower(0);
            } else {
                setPower(currentSpeed);
            }
        }
    } endSynchronized
#else
    if (! forcedToStop) {
        setPower(currentSpeed);
    }
#endif
}

void GCMotor::hold()
{
#ifdef ENABLE_SIN
    synchronized(mutex) {
        shouldHold = true;
        shouldFree = false;
        forcedToStop = false;
        forward(0);
    } endSynchronized
#else
    holdImmediately();
    forcedToStop = false;
#endif
}

void GCMotor::free()
{
#ifdef ENABLE_SIN
    synchronized(mutex) {
        shouldHold = false;
        shouldFree = true;
        forcedToStop = false;
        forward(0);
    } endSynchronized
#else
    freeImmediately();
    forcedToStop = false;
#endif
}

void GCMotor::holdImmediately()
{
    synchronized(mutex) {
        forcedToStop = true;
        digital1 = 1;
        digital2 = 1;
        analog   = 0;
        currentSpeed = 0;
        stepsPoint = stepsTerminalPoint;
    } endSynchronized
}

void GCMotor::freeImmediately()
{
    synchronized(mutex) {
        forcedToStop = true;
        digital1 = 0;
        digital2 = 0;
        analog   = 0;
        currentSpeed = 0;
        stepsPoint = stepsTerminalPoint;
    } endSynchronized
}

void GCMotor::setPower(float power)
{
    synchronized(mutex) {
        power += fixPower;
        if (power >= 0) {
            digital1 = 0;
            digital2 = 1;
        } else {
            digital1 = 1;
            digital2 = 0;
        }
        
        analog = abs(power);
    } endSynchronized
}

float GCMotor::getCurrentSpeed()
{
    return currentSpeed;
}
