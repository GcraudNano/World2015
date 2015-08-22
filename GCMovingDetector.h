#ifndef __GCMovingDetector__
#define __GCMovingDetector__

#include "GCRobot.h"
#include "GCSharedObjects.h"

class GCMovingDetectorDelegate;

class GCMovingDetector {
private:
    struct BallInformation {
        GcraudNano::GCBallDegrees degrees;
        GcraudNano::GCBallDistance distance;
        GcraudNano::GCBallDegrees tiltValue;
    };
    static constexpr int mailSize = 10;
    Mail<BallInformation, mailSize> mail;
    unsigned char threadStack[GcraudNano::defaultStackSize * 2];
    Thread thread = Thread(&threadCallback, this, osPriorityAboveNormal, GcraudNano::defaultStackSize * 2, threadStack);
    static void __attribute__((noreturn)) threadCallback(void const *arg);
    GcraudNano::MoveInformation *shouldGoStraightToGoal(const BallInformation *infomation);
    GcraudNano::GCBallDegrees absoluteDegrees(const BallInformation *degrees);
    GcraudNano::MoveInformation *decideAround(BallInformation *information);
    GcraudNano::GCBallDegrees actualDegrees(BallInformation *information);
    void slowDownFix(GcraudNano::MoveInformation *information);
    void detect(BallInformation *information);
    
public:
    GCMovingDetectorDelegate *delegate = nullptr;
    bool isKeeper = false;
    
    GCMovingDetector(GCMovingDetectorDelegate *delegate, bool isKeeper);
    void reloadBallInformation(
        GcraudNano::GCBallDegrees degrees,
        GcraudNano::GCBallDistance distance,
        GcraudNano::GCBallDegrees tiltValue
    );
    void moveAgainstLines(GcraudNano::GCLineDegrees degrees, GcraudNano::GCBallDegrees tiltValue);
};

class GCMovingDetectorDelegate {
public:
//    virtual void movingDetector_irInformation(
//        GCMovingDetector *detector,
//        GcraudNano::GCBallDegrees *degrees,
//        GcraudNano::GCBallDistance *distance
//    ) {}
    virtual void movingDetector_movingDegreesDetected(
        GCMovingDetector *detector,
        GcraudNano::GCBallDegrees degrees,
        GcraudNano::MovingSpeed speed
    ) = 0;
    virtual bool movingDetector_rightIsFarther(GCMovingDetector *detector) = 0;
    virtual bool movingDetector_isRightClose(GCMovingDetector *detector) = 0;
    virtual bool movingDetector_isLeftClose(GCMovingDetector *detector) = 0;
    virtual bool movingDetector_isBackClose(GCMovingDetector *detector) = 0;
    virtual bool movingDetector_isBackFar(GCMovingDetector *detector) = 0;
    virtual GcraudNano::RobotLocation movingDetector_whereAmI(GCMovingDetector *detector) = 0;
};


#endif
