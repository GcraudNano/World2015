#ifndef __GCThreeSonicSensors__
#define __GCThreeSonicSensors__

#include "GCRobot.h"
#include "GCSharedObjects.h"
#include "GCRunLoop.h"

class GCThreeSonicSensors : GCRunLoop::Callback {
public:
    enum class GoBackState {
        DoNotBack = 0,
        RightBack,
        LeftBack,
        StraightBack,
    };
    
private:
    static constexpr int interval       = 10;
    static constexpr int timeout        = 100000;
    static constexpr int closeToWall    = 35;
    static constexpr int farFromWallLR  = 87;
    static constexpr int farFromWallB   = 130;
    static constexpr int stageWidth     = 115;
    static constexpr int stageHeight    = 183;
    static constexpr int rapidChange    = 20;
    static constexpr int shouldGoBackDistance = 60;
    static constexpr int halfStageWidth = stageWidth / 2;
    static constexpr int tooCloseToWall = 10;
    
    DigitalInOut leftSonic;
    DigitalInOut backSonic;
    DigitalInOut rightSonic;
    float leftValue  = 0;
    float backValue  = 0;
    float rightValue = 0;
    
    bool isLeftValid  = false;
    bool isBackValid  = false;
    bool isRightValid = false;
    
    Timer timer = Timer();
    
    static constexpr INLINE float microSecondsToCentiMeter(int us) {
        return us * 17 / 1000.0;
    }
    
public:
    GCThreeSonicSensors(PinName left, PinName back, PinName right);
    bool isLeftClose();
    bool isLeftFar();
    bool isLeftTooClose();
    bool isBackClose();
    bool isBackFar();
    bool isBackTooClose();
    bool isRightClose();
    bool isRightFar();
    bool isRightTooClose();
    bool isRightFarther();
    void validateAll() {
        isLeftValid  = true;
        isBackValid  = true;
        isRightValid = true;
    }
    GcraudNano::RobotLocation whereAmI();
    GoBackState detectBackState();
    float detectDegreesToGoal();
    void runLoopCallback(void __attribute__((unused)) *arg);
};

#endif
