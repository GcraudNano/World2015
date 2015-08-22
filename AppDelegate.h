#ifndef __AppDelegate__
#define __AppDelegate__

#include "GCRobot.h"
#include "GCSharedObjects.h"
#include "GCRunLoop.h"
#include "GCMotor.h"
#include "GCMovingDetector.h"
#include "GCThreeMotors.h"
#include "GCCompassSensor.h"
#include "GC16IRSensors.h"
#include "GCSwitch.h"
#include "GCThreeSonicSensors.h"
#include "GCKicker.h"
#include "GCYLineSensors.h"

class AppDelegate : GCMovingDetectorDelegate, GCThreeMotorsDelegate, GCCompassSensorDelegate, GC16IRSensorsDelegate, GCSwitchDelegate, GCYLineSensorsDelegate {
    GCMotor rightMotor = GCMotor(PB_15, PB_4, PB_10);
    GCMotor leftMotor  = GCMotor(PB_13, PA_8, PB_1);
    GCMotor backMotor  = GCMotor(PB_3, PB_5, PB_14);
    GCSwitch redSwitch = GCSwitch(this, PC_8);
    GCSwitch greenSwitch = GCSwitch(this, PC_5);
    GCKicker kicker = GCKicker(PC_9);
    GCSwitch kickerSensor = GCSwitch(this, PC_4);
    GCMovingDetector movingDetector = GCMovingDetector(this, false);
    GCThreeMotors threeMotors = GCThreeMotors(this);
    GCCompassSensor compassSensor = GCCompassSensor(NC, PA_10, this);
    GCThreeSonicSensors threeSonicSensors = GCThreeSonicSensors(PA_15, PB_7, PC_13);
    
    GC16IRSensors irSensors = GC16IRSensors(
        this,
        PA_4,
        PA_1,
        PA_0,
        PB_0,
        PC_2,
        PC_0,
        PC_3,
        PC_1,
        PA_5,
        PB_9,
        PB_8,
        PA_6,
        PA_7,
        PA_9,
        PC_7,
        PB_6
    );
    
    GCYLineSensors lineSensors = GCYLineSensors(PA_11, PB_12, PA_12, this);
    
    bool isRedSwitchOn = false;
    GcraudNano::GCBallDegrees tiltValue = GcraudNano::GCBallDegrees::D0;
    
    void redSwitchDidRise();
    void redSwitchDidFall();
    
    static constexpr int compassDisablingTime = 100;
    bool compassEnabled = true;
    bool readyToKick = false;
    bool tiltOffsetHasSet = false;
    RtosTimer compassEnablingTimer = RtosTimer(compassEnabler, osTimerOnce, &compassEnabled);
    static void compassEnabler(void const *arg);
    static constexpr int escapeTime = 350;
    RtosTimer escapeTimer = RtosTimer(escapedFromLine, osTimerOnce, this);
    static void escapedFromLine(void const *arg);
    bool isEscaping = false;
    
    GcraudNano::GCLineDegrees lastLineDegrees = GcraudNano::GCLineDegreesNone;
    
public:
    AppDelegate();
    
    void movingDetector_movingDegreesDetected(
        GCMovingDetector *detector,
        GcraudNano::GCBallDegrees degrees,
        GcraudNano::MovingSpeed speed
    );
    bool movingDetector_rightIsFarther(GCMovingDetector *detector);
    bool movingDetector_isRightClose(GCMovingDetector *detector);
    bool movingDetector_isLeftClose(GCMovingDetector *detector);
    bool movingDetector_isBackClose(GCMovingDetector *detector);
    bool movingDetector_isBackFar(GCMovingDetector *detector);
    GcraudNano::RobotLocation movingDetector_whereAmI(GCMovingDetector *detector);
    
    void threeMotors_runRightMotor(GCThreeMotors *motors, float power);
    void threeMotors_holdRightMotor(GCThreeMotors *motors);
    void threeMotors_freeRightMotor(GCThreeMotors *motors);
    void threeMotors_runLeftMotor(GCThreeMotors *motors, float power);
    void threeMotors_holdLeftMotor(GCThreeMotors *motors);
    void threeMotors_freeLeftMotor(GCThreeMotors *motors);
    void threeMotors_runBackMotor(GCThreeMotors *motors, float power);
    void threeMotors_holdBackMotor(GCThreeMotors *motors);
    void threeMotors_freeBackMotor(GCThreeMotors *motors);
    
    void compassSensor_valuesAvailable(GCCompassSensor *sensor, GcraudNano::GCBallDegrees tiltValue, float fixValue);
    
    void irSensors_ballDetected(GC16IRSensors *sensors, GcraudNano::GCBallDegrees degrees, GcraudNano::GCBallDistance distance);
    
    void switch_didRise(GCSwitch *s);
    void switch_didFall(GCSwitch *s);
    
    void lineSensors_lineDetected(GCYLineSensors *sensors, GcraudNano::GCLineDegrees degrees);
};

#endif
