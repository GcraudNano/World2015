#ifndef __GCThreeMotors__
#define __GCThreeMotors__

#include "GCRobot.h"
#include "GCSharedObjects.h"

class GCThreeMotorsDelegate;

class GCThreeMotors {
public:
    static constexpr unsigned long factorial(unsigned long x)
    {
        return x > 1 ? x * factorial(x - 1) : 1;
    }
    static constexpr double myPow(double x, unsigned int y)
    {
        return y == 0 ? 1
        :
        y > 1 ? x * myPow(x, y - 1) : x;
    }
    static constexpr double mysin(double x, unsigned int i = 7)
    {
        return i == 0 ? x
        :
        (i % 2 ? -1 : 1) * myPow(x, 2 * i + 1) / factorial(2 * i + 1) + mysin(x, i - 1);
    }
    
private:
    
    static constexpr float fastRightPower   = 0.7;
    static constexpr float maxRightPower    = 0.65;
    static constexpr float slowRightPower   = 0.6;
    static constexpr float lineRightPower   = 0;
    
    static constexpr float fastLeftPower    = 0.7;
    static constexpr float maxLeftPower     = 0.65;
    static constexpr float slowLeftPower    = 0.6;
    static constexpr float lineLeftPower    = 0;
    
    static constexpr float fastBackPower    = 0.7;
    static constexpr float maxBackPower     = 0.65;
    static constexpr float slowBackPower    = 0.6;
    static constexpr float lineBackPower    = 0;
    
    static constexpr float decideFastRightPower(float degrees) {
        return fastRightPower * mysin(safeRadians(degrees + 60));
    }
    static constexpr float decideMiddleRightPower(float degrees) {
        return maxRightPower * mysin(safeRadians(degrees + 60));
    }
    static constexpr float decideSlowRightPower(float degrees) {
        return slowRightPower * mysin(safeRadians(degrees + 60));
    }
    static constexpr float decideLineRightPower(float degrees) {
        return lineRightPower * mysin(safeRadians(degrees + 60));
    }
    
    static constexpr float decideFastLeftPower(float degrees) {
        return fastLeftPower * mysin(safeRadians(60 - degrees));
    }
    static constexpr float decideMiddleLeftPower(float degrees) {
        return maxLeftPower * mysin(safeRadians(60 - degrees));
    }
    static constexpr float decideSlowLeftPower(float degrees) {
        return slowLeftPower * mysin(safeRadians(60 - degrees));
    }
    static constexpr float decideLineLeftPower(float degrees) {
        return lineLeftPower * mysin(safeRadians(60 - degrees));
    }
    
    static constexpr float decideFastBackPower(float degrees) {
        return fastBackPower * mysin(safeRadians(degrees - 180));
    }
    static constexpr float decideMiddleBackPower(float degrees) {
        return maxBackPower * mysin(safeRadians(degrees - 180));
    }
    static constexpr float decideSlowBackPower(float degrees) {
        return slowBackPower * mysin(safeRadians(degrees - 180));
    }
    static constexpr float decideLineBackPower(float degrees) {
        return lineBackPower * mysin(safeRadians(degrees - 180));
    }
    
public:
    GCThreeMotors(GCThreeMotorsDelegate *delegate);
    
    GCThreeMotorsDelegate *delegate = nullptr;
    void decideMotorPower(GcraudNano::MoveInformation moveInfo);
};

class GCThreeMotorsDelegate {
public:
    virtual void threeMotors_runRightMotor(GCThreeMotors *motors, float power) = 0;
    virtual void threeMotors_holdRightMotor(GCThreeMotors *motors) = 0;
    virtual void threeMotors_freeRightMotor(GCThreeMotors *motors) = 0;
    virtual void threeMotors_runLeftMotor(GCThreeMotors *motors, float power) = 0;
    virtual void threeMotors_holdLeftMotor(GCThreeMotors *motors) = 0;
    virtual void threeMotors_freeLeftMotor(GCThreeMotors *motors) = 0;
    virtual void threeMotors_runBackMotor(GCThreeMotors *motors, float power) = 0;
    virtual void threeMotors_holdBackMotor(GCThreeMotors *motors) = 0;
    virtual void threeMotors_freeBackMotor(GCThreeMotors *motors) = 0;
};

#endif
