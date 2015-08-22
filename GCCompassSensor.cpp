#include "GCCompassSensor.h"
#include "GCRunLoop.h"

const int GCCompassSensor::deadZone = 5;
const double GCCompassSensor::fixNum = 1.4;

void GCCompassSensor::fixCallerCallback(const void *arg)
{
    GCCompassSensor *self = reinterpret_cast<GCCompassSensor *>(const_cast<void *>(arg));
    self->setFixValues();
}

GCCompassSensor::GCCompassSensor(PinName tx, PinName rx, GCCompassSensorDelegate *_delegate) : serial(new Serial(tx, rx)), delegate(_delegate)
{
    MBED_ASSERT(delegate != nullptr);
    
    movingValues[numOfMovingValues] = endOfMovingValues;
    movingValuesPoint = movingValues;
    this->init();
    
    int16_t value = readRawValue();
    do {
        *movingValuesPoint = value;
    } while (*++movingValuesPoint != endOfMovingValues);
    movingValuesPoint = movingValues;
    movingAvarage = value;
    lastGetTime = 0;
    timer.start();
    lastValueForAccel = static_cast<double>(value) / 10.0;
    serial->baud(115200);
    serial->attach(this, &GCCompassSensor::serialCallback);
    fixCaller.start(15);
}

void GCCompassSensor::serialCallback()
{
    serialSplitter.addByte(serial->getc());
}

void GCCompassSensor::init()
{
#ifdef __MBED__
    
#endif
}

int16_t GCCompassSensor::readRawValue()
{
    int16_t ret = 0;
    
#ifdef __MBED__
    
    return -serialSplitter.bytes() + 18000;
    
#else
    
    static int16_t val = -1710;
    if (val == -1790) {
        val = 1810;
    }
    val -= 10;
    ret = val;
    
#endif
    
    return ret;
}

float GCCompassSensor::readValue()
{
    if (*++movingValuesPoint == endOfMovingValues) {
        movingValuesPoint = movingValues;
    }
    int16_t ret = readRawValue();
    if (movingAvarage > 9000 && ret < -9000) {
        movingAvarage += -*movingValuesPoint / numOfMovingValues + (ret + 36000) / numOfMovingValues;
        if (movingAvarage > 18000) {
            movingAvarage -= 36000;
        }
    } else if (movingAvarage < -9000 && ret > 9000) {
        movingAvarage += -*movingValuesPoint / numOfMovingValues + (ret - 36000) / numOfMovingValues;
        if (movingAvarage < -17999) {
            movingAvarage += 36000;
        }
    } else if (movingAvarage > 9000 && *movingValuesPoint < -9000) {
        movingAvarage += -(*movingValuesPoint + 36000) / numOfMovingValues + ret / numOfMovingValues;
        if (movingAvarage > 18000) {
            movingAvarage -= 36000;
        }
    } else if (movingAvarage < -900 && *movingValuesPoint > 9000) {
        movingAvarage += -(*movingValuesPoint - 36000) / numOfMovingValues + ret / numOfMovingValues;
        if (movingAvarage < -17999) {
            movingAvarage += 36000;
        }
    } else {
        movingAvarage += -*movingValuesPoint / numOfMovingValues + ret / numOfMovingValues;
    }
    *movingValuesPoint = ret;
    float value = static_cast<double>(movingAvarage) / 100.0;
    if (value > 180) {
        return value - 360;
    } else if (value < -180) {
        return value + 360;
    }
    
    return value;
}

#define ENABLE_PID

void GCCompassSensor::setFixValues()
{
    using namespace GcraudNano;
    
#ifdef ENABLE_PID
    auto now = timer.read_ms();
#endif
    
    float compassValue = readValue() - tiltOffset;
    if (compassValue > 180) {
        compassValue -= 360;
    } else if (compassValue < -180) {
        compassValue += 360;
    }
    
#ifdef ENABLE_PID
    double gap = compassValue - lastValueForAccel;
    if (gap > 300) {
        gap -= 360;
    } else if (gap < -300) {
        gap += 360;
    }
    
    integrationValue += compassValue;
    if (integrationValue > integrationLimit) {
        integrationValue = integrationLimit;
    } else if (integrationValue < -integrationLimit) {
        integrationValue = -integrationLimit;
    }
    
    double pidValue;
    pidValue = propotionGain * compassValue + (now == lastGetTime ? 0 : derivationGain * (gap * 1000 /*[ms/s]*/ / (now - lastGetTime))) + integrationGain * integrationValue;
    lastGetTime = now;
    if (compassValue > 180) {
        lastValueForAccel = compassValue - 360;
    } else if (compassValue < -180) {
        lastValueForAccel = compassValue + 360;
    } else {
        lastValueForAccel = compassValue;
    }
#endif
    
    if (compassValue > deadZone) {
        /* turn right */
#ifndef ENABLE_PID
        int val = static_cast<int>(fixNum * sqrtf(value) + 0.5);
        rightFixValueDelegate(val);
        backFixValueDelegate(val);
        leftFixValueDelegate(-val);
#else
        GCBallDegrees ret = static_cast<GCBallDegrees>(degToInt(GCBallDegrees::D0) + static_cast<int>(-compassValue / 11.25 + 0.5));
        delegate->compassSensor_valuesAvailable(this, degToInt(ret) == -1 ? GCBallDegrees::D_16875 : ret, pidValue);
#endif
    } else if (compassValue < -deadZone) {
        /* turn left */
#ifndef ENABLE_PID
        int val = static_cast<int>(fixNum * sqrtf(-value) + 0.5);
#endif
        delegate->compassSensor_valuesAvailable(this, static_cast<GCBallDegrees>(degToInt(GCBallDegrees::D0) + static_cast<int>(-compassValue / 11.25 - 0.5)), pidValue);
    } else {
        integrationValue = 0;
        delegate->compassSensor_valuesAvailable(this, GCBallDegrees::D0, 0);
    }
}

void GCCompassSensor::calibration()
{
#ifdef __MBED__
    
#endif
}

void GCCompassSensor::enterSleep()
{
#ifdef __MBED__
    

    
#endif
}

void GCCompassSensor::exitSleep()
{
#ifdef __MBED__
    
#endif
}

void GCCompassSensor::setTiltOffset(float offset)
{
    tiltOffset = offset;
    hasTiltOffsetVariable = true;
}

void GCCompassSensor::clearTiltOffset()
{
    tiltOffset = 0;
    hasTiltOffsetVariable = false;
}

bool GCCompassSensor::hasTiltOffset()
{
    return hasTiltOffsetVariable;
}

void GCCompassSensor::resetIntegrationValue()
{
    integrationValue = 0;
}
