#ifndef __GCCompassSensor__
#define __GCCompassSensor__

#include "GCRobot.h"
#include "GCSharedObjects.h"
#include "MultiBytesSplitter.h"
#include <functional>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"

class GCCompassSensor;

class GCCompassSensorDelegate {
public:
    virtual void compassSensor_valuesAvailable(GCCompassSensor *sensor, GcraudNano::GCBallDegrees tiltValue, float fixValue) = 0;
};

class GCCompassSensor {
    static const int deadZone;
    static const double fixNum;
    void init();
    static constexpr float propotionGain  = 0.0058; // 20
    static constexpr float integrationGain = 0.0001;
    float integrationValue = 0;
    static constexpr float integrationLimit = 1000;
    static constexpr float derivationGain = 0.000825; // 0.0
    float lastValueForAccel = 0;
    int lastGetTime = 0;
    static constexpr int numOfMovingValues = 5;
    static constexpr int16_t endOfMovingValues = INT16_MAX;
    int16_t movingAvarage = 0;
    int16_t movingValues[numOfMovingValues + 1];
    int16_t *movingValuesPoint = nullptr;
    static constexpr int offsetStartAddress = 50;
    float tiltOffset = 0;
    bool hasTiltOffsetVariable = false;
    Timer timer = Timer();
    Serial *serial;
    MultiBytesSplitter<uint16_t> serialSplitter = MultiBytesSplitter<uint16_t>();
    void serialCallback();
    RtosTimer fixCaller = RtosTimer(fixCallerCallback, osTimerPeriodic, this);
    static void fixCallerCallback(const void *arg);
    
public:
    GCCompassSensor(PinName tx, PinName rx, GCCompassSensorDelegate *delegate);
    GCCompassSensorDelegate *delegate;
    int16_t readRawValue();
    float readValue();
    void setFixValues();
    void calibration();
    void enterSleep();
    void exitSleep();
    void setTiltOffset(float offset);
    void clearTiltOffset();
    bool hasTiltOffset();
    void resetIntegrationValue();
};

#pragma GCC diagnostic pop

#endif
