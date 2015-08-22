#include "GCThreeSonicSensors.h"
#include "GCRunLoop.h"

using namespace GcraudNano;

void GCThreeSonicSensors::runLoopCallback(void *arg)
{
    float newLeftValue  = -1;
    float newBackValue  = -1;
    float newRightValue = -1;
    
    timer.reset();
    
    leftSonic.output();
    backSonic.output();
    rightSonic.output();
    
    leftSonic  = 1;
    backSonic  = 1;
    rightSonic = 1;
    
    wait_us(interval);
    
    leftSonic  = 0;
    backSonic  = 0;
    rightSonic = 0;
    
    leftSonic.input();
    backSonic.input();
    rightSonic.input();
    
    timer.start();
    while (! (leftSonic | backSonic | rightSonic)) {
        if (timer.read_us() > timeout) {
            goto finalize;
        }
    }
    
    timer.reset();
    timer.start();
    while (timer.read_us() <= timeout) {
        if (! leftSonic) {
            newLeftValue = microSecondsToCentiMeter(timer.read_us());
            while (timer.read_us() <= timeout) {
                if (! backSonic) {
                    newBackValue = microSecondsToCentiMeter(timer.read_us());
                    while (timer.read_us() <= timeout) {
                        if (! rightSonic) {
                            newRightValue = microSecondsToCentiMeter(timer.read_us());
                            goto finalize;
                        }
                    }
                    goto finalize;
                } else if (! rightSonic) {
                    newRightValue = microSecondsToCentiMeter(timer.read_us());
                    while (timer.read_us() <= timeout) {
                        if (! backSonic) {
                            newBackValue = microSecondsToCentiMeter(timer.read_us());
                            goto finalize;
                        }
                    }
                    newBackValue = microSecondsToCentiMeter(interval);
                    goto finalize;
                }
            }
            goto finalize;
        } else if (! backSonic) {
            newBackValue = microSecondsToCentiMeter(timer.read_us());
            while (timer.read_us() <= timeout) {
                if (! leftSonic) {
                    newLeftValue = microSecondsToCentiMeter(timer.read_us());
                    while (timer.read_us()) {
                        if (! rightSonic) {
                            newRightValue = microSecondsToCentiMeter(timer.read_us());
                            goto finalize;
                        }
                    }
                    newRightValue = microSecondsToCentiMeter(timer.read_us());
                    goto finalize;
                } else if (! rightSonic) {
                    newRightValue = microSecondsToCentiMeter(timer.read_us());
                    while (timer.read_us() <= timeout) {
                        if (! leftSonic) {
                            newLeftValue = microSecondsToCentiMeter(timer.read_us());
                            goto finalize;
                        }
                    }
                    newLeftValue = microSecondsToCentiMeter(interval);
                    goto finalize;
                }
            }
            goto finalize;
        } else if (! rightSonic) {
            newRightValue = microSecondsToCentiMeter(timer.read_us());
            while (timer.read_us() <= timeout) {
                if (! leftSonic) {
                    newLeftValue = microSecondsToCentiMeter(timer.read_us());
                    while (timer.read_us() <= timeout) {
                        if (! backSonic) {
                            newBackValue = microSecondsToCentiMeter(timer.read_us());
                            goto finalize;
                        }
                    }
                    goto finalize;
                } else if (! backSonic) {
                    newBackValue = microSecondsToCentiMeter(timer.read_us());
                    while (timer.read_us() <= timeout) {
                        if (! leftSonic) {
                            newLeftValue = microSecondsToCentiMeter(timer.read_us());
                            goto finalize;
                        }
                    }
                    goto finalize;
                }
            }
            goto finalize;
        }
    }
    
finalize:
    timer.stop();
    
    if ((newRightValue + newLeftValue) >= stageWidth) {
        isRightValid = true;
        isLeftValid  = true;
    } else {
        int rightChange = newRightValue - rightValue;
        if (abs(rightChange) >= rapidChange) {
            isRightValid = rightChange > 0 ? true : false;
        }
        int leftChange = newLeftValue - leftValue;
        if (abs(leftChange) >= rapidChange) {
            isLeftValid = leftChange > 0 ? true : false;
        }
    }
    int backChange = newBackValue - backValue;
    if (abs(backChange) >= rapidChange) {
        isBackValid = backChange > 0 ? true : false;
    }
    
    rightValue = newRightValue;
    leftValue  = newLeftValue;
    backValue  = newBackValue;
    
    GCRunLoop::mainLoop.dispatch_async(this, nullptr);
}

GCThreeSonicSensors::GCThreeSonicSensors(PinName left, PinName back, PinName right) : leftSonic(left), backSonic(back), rightSonic(right)
{
    GCRunLoop::mainLoop.dispatch_async(this, nullptr);
}

bool GCThreeSonicSensors::isLeftClose()
{
    if (isLeftValid) {
        return leftValue <= closeToWall;
    } else if (isRightValid) {
        return farFromWallLR <= rightValue;
    }
    return false;
}

bool GCThreeSonicSensors::isLeftFar()
{
    if (isLeftValid) {
        return farFromWallLR <= leftValue;
    } else if (isRightValid) {
        return rightValue <= closeToWall;
    }
    return false;
}

bool GCThreeSonicSensors::isLeftTooClose()
{
    return leftValue <= tooCloseToWall;
}

bool GCThreeSonicSensors::isBackClose()
{
    return isBackValid && backValue <= closeToWall;
}

bool GCThreeSonicSensors::isBackFar()
{
    return isBackValid && backValue >= farFromWallB;
}

bool GCThreeSonicSensors::isBackTooClose()
{
    return backValue <= tooCloseToWall;
}

bool GCThreeSonicSensors::isRightClose()
{
    if (isRightValid) {
        return rightValue <= closeToWall;
    } else if (isLeftValid) {
        return farFromWallLR <= leftValue;
    }
    return false;
}

bool GCThreeSonicSensors::isRightFar()
{
    if (isRightValid) {
        return farFromWallLR <= rightValue;
    } else if (isLeftValid) {
        return leftValue <= closeToWall;
    }
    return false;
}

bool GCThreeSonicSensors::isRightTooClose()
{
    return rightValue <= tooCloseToWall;
}

bool GCThreeSonicSensors::isRightFarther()
{
    if (isRightValid) {
        if (isLeftValid) {
            return rightValue >= leftValue;
        } else {
            return rightValue >= halfStageWidth;
        }
    }
    return isLeftValid && leftValue <= halfStageWidth;
}

RobotLocation GCThreeSonicSensors::whereAmI()
{
    if (backValue <= 30) {
        return RobotLocation::CloseEnoughToPutOut;
    } else if (backValue <= 80) {
        return RobotLocation::AlmostMiddle;
    } else {
        return RobotLocation::TooCloseToOpponentsGoal;
    }
}

GCThreeSonicSensors::GoBackState GCThreeSonicSensors::detectBackState()
{
    if (backValue >= shouldGoBackDistance) {
        if (isRightValid || isLeftValid) {
            if (isLeftClose()) {
                return GoBackState::RightBack;
            } else if (isRightClose()) {
                return GoBackState::LeftBack;
            } else {
                return GoBackState::StraightBack;
            }
        } else {
            return GoBackState::StraightBack;
        }
    }
    return GoBackState::DoNotBack;
}

float GCThreeSonicSensors::detectDegreesToGoal()
{
    constexpr long goalToWall = 30;
    if (isBackValid) {
        if (isRightValid) {
            if (isLeftValid) {
                int gyap = rightValue - leftValue;
                if (gyap > 0) {
                    return -degrees(atan2(gyap, stageHeight - goalToWall - backValue));
                } else {
                    return degrees(atan2(-gyap, stageHeight - goalToWall - backValue));
                }
            } else {
                int gyap = halfStageWidth - rightValue;
                if (gyap > 0) {
                    return degrees(atan2(gyap, stageHeight - goalToWall - backValue));
                } else {
                    return -degrees(atan2(-gyap, stageHeight - goalToWall - backValue));
                }
            }
        } else if (isLeftValid) {
            int gyap = halfStageWidth - leftValue;
            if (gyap > 0) {
                return -degrees(atan2(gyap, stageHeight - goalToWall - backValue));
            } else {
                return degrees(atan2(-gyap, stageHeight - goalToWall - backValue));
            }
        }
    }
    return 0;
}
