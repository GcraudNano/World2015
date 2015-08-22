#include "AppDelegate.h"
#include "GCRunLoop.h"

using namespace GcraudNano;

void AppDelegate::movingDetector_movingDegreesDetected(GCMovingDetector *detector, GCBallDegrees degrees, MovingSpeed speed)
{
    if (compassEnabled && isRedSwitchOn && ! isEscaping) {
        
        switch (lineSensors.read()) {
            case GCLineDegrees0:
                if (GCBallDegrees::D_4500 <= degrees && degrees <= GCBallDegrees::D4500) {
                    return;
                }
                break;
                
            case GCLineDegrees45:
                if (GCBallDegrees::D_9000 <= degrees && degrees <= GCBallDegrees::D18000) {
                    return;
                }
                break;
                
            case GCLineDegrees_45:
                if (degrees <= GCBallDegrees::D9000) {
                    return;
                }
                break;
                
            case GCLineDegrees135:
                if (degrees <= GCBallDegrees::D0 || GCBallDegrees::D9000 <= degrees) {
                    return;
                }
                break;
                
            case GCLineDegrees_135:
                if (degrees <= GCBallDegrees::D_9000 || GCBallDegrees::D0 <= degrees) {
                    return;
                }
                break;
                
            case GCLineDegrees180:
                if (degrees <= GCBallDegrees::D_4500 || GCBallDegrees::D4500 <= degrees) {
                    return;
                }
                break;
                
            default:
                error("%s(%d) %s Unreachable code with degrees %d", __FILE__, __LINE__, __func__, lineSensors.read());
                break;
                
            case GCLineDegreesNone:
                
                switch (lastLineDegrees) {
                    case GCLineDegrees0:
                        if (GCBallDegrees::D_2250 <= degrees && degrees <= GCBallDegrees::D2250) {
                            return;
                        } else {
                            lastLineDegrees = GCLineDegreesNone;
                        }
                        break;
                        
                    case GCLineDegrees45:
                        if (GCBallDegrees::D4500 <= degrees && degrees <= GCBallDegrees::D13500) {
                            return;
                        } else {
                            lastLineDegrees = GCLineDegreesNone;
                        }
                        break;
                        
                    case GCLineDegrees_45:
                        if (GCBallDegrees::D_13500 <= degrees && degrees <= GCBallDegrees::D_4500) {
                            return;
                        } else {
                            lastLineDegrees = GCLineDegreesNone;
                        }
                        break;
                        
                    case GCLineDegrees135:
                        if (GCBallDegrees::D11250 <= degrees && degrees <= GCBallDegrees::D15750) {
                            return;
                        } else {
                            lastLineDegrees = GCLineDegreesNone;
                        }
                        break;
                        
                    case GCLineDegrees_135:
                        if (GCBallDegrees::D_15750 <= degrees && degrees <= GCBallDegrees::D_11250) {
                            return;
                        } else {
                            lastLineDegrees = GCLineDegreesNone;
                        }
                        break;
                        
                    case GCLineDegrees180:
                        if (degrees <= GCBallDegrees::D_13500 || GCBallDegrees::D13500 <= degrees) {
                            return;
                        } else {
                            lastLineDegrees = GCLineDegreesNone;
                        }
                        break;
                        
                    case GCLineDegreesNone:
                        break;
                        
                    default:
                        error("%s(%d) %s Unreachable code with degrees %d", __FILE__, __LINE__, __func__, lastLineDegrees);
                        break;
                }
                
                break;
        }
        
        if (degrees == GCBallDegrees::DNone) {
            rightMotor.free();
            leftMotor.free();
            backMotor.free();
        } else {
            if (tiltOffsetHasSet && GCBallDegrees::D_2250 <= degrees && degrees <= GCBallDegrees::D2250) {
                compassSensor.setTiltOffset(0);
                tiltOffsetHasSet = false;
            }
            MoveInformation info;
            info.direction = degrees;
            info.speed = speed;
            threeMotors.decideMotorPower(info);
        }
        
    }
}

bool AppDelegate::movingDetector_rightIsFarther(GCMovingDetector *detector)
{
    return threeSonicSensors.isRightFarther();
}

bool AppDelegate::movingDetector_isRightClose(GCMovingDetector *detector)
{
    return threeSonicSensors.isRightClose();
}

bool AppDelegate::movingDetector_isLeftClose(GCMovingDetector *detector)
{
    return threeSonicSensors.isLeftClose();
}

bool AppDelegate::movingDetector_isBackClose(GCMovingDetector *detector)
{
    return threeSonicSensors.isBackClose();
}

bool AppDelegate::movingDetector_isBackFar(GCMovingDetector *detector)
{
    return threeSonicSensors.isBackFar();
}

RobotLocation AppDelegate::movingDetector_whereAmI(GCMovingDetector *detector)
{
    return threeSonicSensors.whereAmI();
}

void AppDelegate::threeMotors_runRightMotor(GCThreeMotors *motors, float power)
{
    rightMotor.forward(power);
}

void AppDelegate::threeMotors_holdRightMotor(GCThreeMotors *motors)
{
    rightMotor.hold();
}

void AppDelegate::threeMotors_freeRightMotor(GCThreeMotors *motors)
{
    rightMotor.free();
}

void AppDelegate::threeMotors_runLeftMotor(GCThreeMotors *motors, float power)
{
    leftMotor.forward(power);
}

void AppDelegate::threeMotors_holdLeftMotor(GCThreeMotors *motors)
{
    leftMotor.hold();
}

void AppDelegate::threeMotors_freeLeftMotor(GCThreeMotors *motors)
{
    leftMotor.free();
}

void AppDelegate::threeMotors_runBackMotor(GCThreeMotors *motors, float power)
{
    backMotor.forward(power);
}

void AppDelegate::threeMotors_holdBackMotor(GCThreeMotors *motors)
{
    backMotor.hold();
}

void AppDelegate::threeMotors_freeBackMotor(GCThreeMotors *motors)
{
    backMotor.free();
}

void AppDelegate::compassSensor_valuesAvailable(GCCompassSensor *sensor, GcraudNano::GCBallDegrees tiltValue, float fixValue)
{
    if (compassEnabled) {
        rightMotor.fixPower = fixValue;
        leftMotor.fixPower  = -fixValue;
        backMotor.fixPower  = fixValue;
    }
    this->tiltValue = tiltValue;
    if (! isRedSwitchOn) {
        sensor->resetIntegrationValue();
    } else if (tiltOffsetHasSet && kickerSensor && tiltValue == GCBallDegrees::D0) {
        kicker.kick();
        compassEnabled = false;
        compassEnablingTimer.start(compassDisablingTime);
    }
}

void AppDelegate::irSensors_ballDetected(GC16IRSensors *sensors, GCBallDegrees degrees, GCBallDistance distance)
{
    if (degrees == GCBallDegrees::DNone) {
        if (isRedSwitchOn && compassEnabled && ! lineSensors.read()) {
            switch (threeSonicSensors.detectBackState()) {
                case GCThreeSonicSensors::GoBackState::StraightBack:
                {
                    MoveInformation info;
                    info.direction = GCBallDegrees::D18000;
                    info.speed = MovingSpeed::Middle;
                    threeMotors.decideMotorPower(info);
                }
                    break;
                    
                case GCThreeSonicSensors::GoBackState::RightBack:
                {
                    MoveInformation info;
                    info.direction = GCBallDegrees::D_12000;
                    info.speed = MovingSpeed::Middle;
                    threeMotors.decideMotorPower(info);
                }
                    break;
                    
                case GCThreeSonicSensors::GoBackState::LeftBack:
                {
                    MoveInformation info;
                    info.direction = GCBallDegrees::D12000;
                    info.speed = MovingSpeed::Middle;
                    threeMotors.decideMotorPower(info);
                }
                    break;
                    
                case GCThreeSonicSensors::GoBackState::DoNotBack:
                    rightMotor.free();
                    leftMotor.free();
                    backMotor.free();
                    break;
                    
                default:
                    error("%s(%d) %s Unreachable code", __FILE__, __LINE__, __func__);
                    break;
            }
        }
    } else {
        readyToKick = GCBallDegrees::D_2250 <= degrees && degrees <= GCBallDegrees::D2250 && distance == GCBallDistance::Near;
        movingDetector.reloadBallInformation(degrees, distance, tiltValue);
    }
}

void AppDelegate::switch_didRise(GCSwitch *s)
{
    if (s == &redSwitch) {
        isRedSwitchOn = true;
        threeSonicSensors.validateAll();
        /* allow motors to fix */
        rightMotor.free();
        leftMotor.free();
        backMotor.free();
    } else if (s == &greenSwitch) {
        movingDetector.isKeeper = true;
    } else if (s == &kickerSensor && compassEnabled) {
        if (isRedSwitchOn) {
            if (! readyToKick) return;
            rightMotor.fixPower = 0;
            leftMotor.fixPower = 0;
            backMotor.fixPower = 0;
            MoveInformation info;
            info.direction = GCBallDegrees::D0;
            info.speed = MovingSpeed::Middle;
            threeMotors.decideMotorPower(info);
            compassSensor.setTiltOffset(threeSonicSensors.detectDegreesToGoal());
            tiltOffsetHasSet = true;
            kicker.kick();
        } else {
            kicker.kick();
        }
        compassEnabled = false;
        compassEnablingTimer.start(compassDisablingTime);
    }
}

void AppDelegate::switch_didFall(GCSwitch *s)
{
    if (s == &redSwitch) {
        isRedSwitchOn = false;
        rightMotor.freeImmediately();
        leftMotor.freeImmediately();
        backMotor.freeImmediately();
        lastLineDegrees = GCLineDegreesNone;
    } else if (s == &greenSwitch) {
        movingDetector.isKeeper = false;
    }
}

void AppDelegate::compassEnabler(void const *arg)
{
    bool *enabled = reinterpret_cast<bool *>(const_cast<void *>(arg));
    *enabled = true;
}

void AppDelegate::escapedFromLine(const void *arg)
{
    AppDelegate *self = reinterpret_cast<AppDelegate *>(const_cast<void *>(arg));
    if (self->lastLineDegrees & GCLineDegrees180 && self->lineSensors.read()) {
        self->escapeTimer.start(escapeTime);
    } else {
        self->rightMotor.hold();
        self->leftMotor.hold();
        self->backMotor.hold();
        self->isEscaping = false;
    }
}

void AppDelegate::lineSensors_lineDetected(GCYLineSensors *sensors, GcraudNano::GCLineDegrees degrees)
{
    static MoveInformation info = {GCBallDegrees::D0, MovingSpeed::Fast};
    info.speed = MovingSpeed::Fast;
    
    switch (degrees) {
        case GCLineDegrees0:
            if (threeSonicSensors.isLeftClose() && ! threeSonicSensors.isRightClose()) {
                info.direction = GCBallDegrees::D_12000;
            } else if (threeSonicSensors.isRightClose() && ! threeSonicSensors.isLeftClose()) {
                info.direction = GCBallDegrees::D12000;
            } else {
                info.direction = GCBallDegrees::D18000;
            }
            info.speed = MovingSpeed::Middle;
            threeMotors.decideMotorPower(info);
            lastLineDegrees = degrees;
            break;

        case GCLineDegrees45:
            if (lastLineDegrees == GCLineDegrees180 || threeSonicSensors.isBackClose()) {
                info.direction = GCBallDegrees::D_6000;
            } else {
                info.direction = GCBallDegrees::D_12000;
            }
            threeMotors.decideMotorPower(info);
            lastLineDegrees = degrees;
            break;

        case GCLineDegrees_45:
            if (lastLineDegrees == GCLineDegrees180 || threeSonicSensors.isBackClose()) {
                info.direction = GCBallDegrees::D6000;
            } else {
                info.direction = GCBallDegrees::D12000;
            }
            threeMotors.decideMotorPower(info);
            lastLineDegrees = degrees;
            break;
            
        case GCLineDegrees135:
            info.direction = GCBallDegrees::D_6000;
            info.speed = MovingSpeed::Slow;
            threeMotors.decideMotorPower(info);
            lastLineDegrees = degrees;
            break;
            
        case GCLineDegrees_135:
            info.direction = GCBallDegrees::D6000;
            info.speed = MovingSpeed::Slow;
            threeMotors.decideMotorPower(info);
            lastLineDegrees = degrees;
            break;
            
        case GCLineDegrees180:
            info.direction = GCBallDegrees::D0;
            info.speed = MovingSpeed::Slow;
            threeMotors.decideMotorPower(info);
            lastLineDegrees = degrees;
            break;
            
        case GCLineDegreesNone:
            return;
            
        case GCLineDegrees45 | GCLineDegrees_45 | GCLineDegrees180:
            rightMotor.hold();
            leftMotor.hold();
            backMotor.hold();
            return;
            
        default:
            error("%s(%d) %s Unreachable code with degrees %d", __FILE__, __LINE__, __func__, degrees);
            return;
    }
    
    isEscaping = true;
    escapeTimer.start(escapeTime);
}

AppDelegate::AppDelegate()
{
}
