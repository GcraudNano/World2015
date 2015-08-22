#include "GCThreeMotors.h"
#include "GCRunLoop.h"

using namespace GcraudNano;

GCThreeMotors::GCThreeMotors(GCThreeMotorsDelegate *_delegate) : delegate(_delegate)
{
    MBED_ASSERT(delegate != nullptr);
}

void GCThreeMotors::decideMotorPower(MoveInformation moveInfo)
{
#define makeCase(degrees, speed)                                                                \
    case degrees:                                                                               \
    {                                                                                           \
        constexpr auto rightPower = decide##speed##RightPower(degToActualInt(degrees) / 100.0); \
        constexpr auto leftPower  = decide##speed##LeftPower(degToActualInt(degrees) / 100.0);  \
        constexpr auto backPower  = decide##speed##BackPower(degToActualInt(degrees) / 100.0);  \
        delegate->threeMotors_runRightMotor(this, rightPower);                                  \
        delegate->threeMotors_runLeftMotor(this, leftPower);                                    \
        delegate->threeMotors_runBackMotor(this, backPower);                                    \
    }                                                                                           \
        break
    
    switch (moveInfo.speed) {
        case MovingSpeed::Fast:
            switch (moveInfo.direction) {
                makeCase(GCBallDegrees::D_16875, Fast);
                makeCase(GCBallDegrees::D_15750, Fast);
                makeCase(GCBallDegrees::D_14625, Fast);
                makeCase(GCBallDegrees::D_13500, Fast);
                makeCase(GCBallDegrees::D_12375, Fast);
                makeCase(GCBallDegrees::D_11250, Fast);
                makeCase(GCBallDegrees::D_10125, Fast);
                makeCase(GCBallDegrees::D_9000, Fast);
                makeCase(GCBallDegrees::D_7875, Fast);
                makeCase(GCBallDegrees::D_6750, Fast);
                makeCase(GCBallDegrees::D_5625, Fast);
                makeCase(GCBallDegrees::D_4500, Fast);
                makeCase(GCBallDegrees::D_3375, Fast);
                makeCase(GCBallDegrees::D_2250, Fast);
                makeCase(GCBallDegrees::D_1125, Fast);
//                makeCase(GCBallDegrees::D0, Fast);
                case GCBallDegrees::D0:
                    delegate->threeMotors_runRightMotor(this, 0.9);
                    delegate->threeMotors_runLeftMotor(this, 0.9);
                    delegate->threeMotors_holdBackMotor(this);
                    break;
                makeCase(GCBallDegrees::D1125, Fast);
                makeCase(GCBallDegrees::D2250, Fast);
                makeCase(GCBallDegrees::D3375, Fast);
                makeCase(GCBallDegrees::D4500, Fast);
                makeCase(GCBallDegrees::D5625, Fast);
                makeCase(GCBallDegrees::D6750, Fast);
                makeCase(GCBallDegrees::D7875, Fast);
                makeCase(GCBallDegrees::D9000, Fast);
                makeCase(GCBallDegrees::D10125, Fast);
                makeCase(GCBallDegrees::D11250, Fast);
                makeCase(GCBallDegrees::D12375, Fast);
                makeCase(GCBallDegrees::D13500, Fast);
                makeCase(GCBallDegrees::D14625, Fast);
                makeCase(GCBallDegrees::D15750, Fast);
                makeCase(GCBallDegrees::D16875, Fast);
                makeCase(GCBallDegrees::D18000, Fast);
                makeCase(GCBallDegrees::D12000, Fast);
                makeCase(GCBallDegrees::D_12000, Fast);
                makeCase(GCBallDegrees::D6000, Fast);
                makeCase(GCBallDegrees::D_6000, Fast);
                    
                default:
                    printf("%s(%d) %s Unreachable code with direction %d speed %d\n", __FILE__, __LINE__, __func__, moveInfo.direction, moveInfo.speed);
                    abort();
                    break;
            }
            break;
            
        case MovingSpeed::Middle:
            switch (moveInfo.direction) {
                makeCase(GCBallDegrees::D_16875, Middle);
                makeCase(GCBallDegrees::D_15750, Middle);
                makeCase(GCBallDegrees::D_14625, Middle);
                makeCase(GCBallDegrees::D_13500, Middle);
                makeCase(GCBallDegrees::D_12375, Middle);
                makeCase(GCBallDegrees::D_11250, Middle);
                makeCase(GCBallDegrees::D_10125, Middle);
                makeCase(GCBallDegrees::D_9000, Middle);
                makeCase(GCBallDegrees::D_7875, Middle);
                makeCase(GCBallDegrees::D_6750, Middle);
                makeCase(GCBallDegrees::D_5625, Middle);
                makeCase(GCBallDegrees::D_4500, Middle);
                makeCase(GCBallDegrees::D_3375, Middle);
                makeCase(GCBallDegrees::D_2250, Middle);
                makeCase(GCBallDegrees::D_1125, Middle);
//                makeCase(GCBallDegrees::D0, Middle);
                case GCBallDegrees::D0:
                    delegate->threeMotors_runRightMotor(this, 0.9);
                    delegate->threeMotors_runLeftMotor(this, 0.9);
                    delegate->threeMotors_holdBackMotor(this);
                    break;
                makeCase(GCBallDegrees::D1125, Middle);
                makeCase(GCBallDegrees::D2250, Middle);
                makeCase(GCBallDegrees::D3375, Middle);
                makeCase(GCBallDegrees::D4500, Middle);
                makeCase(GCBallDegrees::D5625, Middle);
                makeCase(GCBallDegrees::D6750, Middle);
                makeCase(GCBallDegrees::D7875, Middle);
                makeCase(GCBallDegrees::D9000, Middle);
                makeCase(GCBallDegrees::D10125, Middle);
                makeCase(GCBallDegrees::D11250, Middle);
                makeCase(GCBallDegrees::D12375, Middle);
                makeCase(GCBallDegrees::D13500, Middle);
                makeCase(GCBallDegrees::D14625, Middle);
                makeCase(GCBallDegrees::D15750, Middle);
                makeCase(GCBallDegrees::D16875, Middle);
                makeCase(GCBallDegrees::D18000, Middle);
                makeCase(GCBallDegrees::D12000, Middle);
                makeCase(GCBallDegrees::D_12000, Middle);
                makeCase(GCBallDegrees::D6000, Middle);
                makeCase(GCBallDegrees::D_6000, Middle);

                default:
                    printf("%s(%d) %s Unreachable code with direction %d speed %d\n", __FILE__, __LINE__, __func__, moveInfo.direction, moveInfo.speed);
                    abort();
                    break;
            }
            break;
            
        case MovingSpeed::Slow:
            switch (moveInfo.direction) {
                makeCase(GCBallDegrees::D_16875, Slow);
                makeCase(GCBallDegrees::D_15750, Slow);
                makeCase(GCBallDegrees::D_14625, Slow);
                makeCase(GCBallDegrees::D_13500, Slow);
                makeCase(GCBallDegrees::D_12375, Slow);
                makeCase(GCBallDegrees::D_11250, Slow);
                makeCase(GCBallDegrees::D_10125, Slow);
                makeCase(GCBallDegrees::D_9000, Slow);
                makeCase(GCBallDegrees::D_7875, Slow);
                makeCase(GCBallDegrees::D_6750, Slow);
                makeCase(GCBallDegrees::D_5625, Slow);
                makeCase(GCBallDegrees::D_4500, Slow);
                makeCase(GCBallDegrees::D_3375, Slow);
                makeCase(GCBallDegrees::D_2250, Slow);
                makeCase(GCBallDegrees::D_1125, Slow);
//                makeCase(GCBallDegrees::D0, Slow);
                case GCBallDegrees::D0:
                    delegate->threeMotors_runRightMotor(this, 0.9);
                    delegate->threeMotors_runLeftMotor(this, 0.9);
                    delegate->threeMotors_holdBackMotor(this);
                    break;
                makeCase(GCBallDegrees::D1125, Slow);
                makeCase(GCBallDegrees::D2250, Slow);
                makeCase(GCBallDegrees::D3375, Slow);
                makeCase(GCBallDegrees::D4500, Slow);
                makeCase(GCBallDegrees::D5625, Slow);
                makeCase(GCBallDegrees::D6750, Slow);
                makeCase(GCBallDegrees::D7875, Slow);
                makeCase(GCBallDegrees::D9000, Slow);
                makeCase(GCBallDegrees::D10125, Slow);
                makeCase(GCBallDegrees::D11250, Slow);
                makeCase(GCBallDegrees::D12375, Slow);
                makeCase(GCBallDegrees::D13500, Slow);
                makeCase(GCBallDegrees::D14625, Slow);
                makeCase(GCBallDegrees::D15750, Slow);
                makeCase(GCBallDegrees::D16875, Slow);
                makeCase(GCBallDegrees::D18000, Slow);
                makeCase(GCBallDegrees::D12000, Slow);
                makeCase(GCBallDegrees::D_12000, Slow);
                makeCase(GCBallDegrees::D6000, Slow);
                makeCase(GCBallDegrees::D_6000, Slow);
    
                default:
                    printf("%s(%d) %s Unreachable code with direction %d speed %d\n", __FILE__, __LINE__, __func__, moveInfo.direction, moveInfo.speed);
                    abort();
                    break;
            }
            break;
            
        case MovingSpeed::LineSlow:
            switch (moveInfo.direction) {
                makeCase(GCBallDegrees::D_16875, Line);
                makeCase(GCBallDegrees::D_15750, Line);
                makeCase(GCBallDegrees::D_14625, Line);
                makeCase(GCBallDegrees::D_13500, Line);
                makeCase(GCBallDegrees::D_12375, Line);
                makeCase(GCBallDegrees::D_11250, Line);
                makeCase(GCBallDegrees::D_10125, Line);
                makeCase(GCBallDegrees::D_9000, Line);
                makeCase(GCBallDegrees::D_7875, Line);
                makeCase(GCBallDegrees::D_6750, Line);
                makeCase(GCBallDegrees::D_5625, Line);
                makeCase(GCBallDegrees::D_4500, Line);
                makeCase(GCBallDegrees::D_3375, Line);
                makeCase(GCBallDegrees::D_2250, Line);
                makeCase(GCBallDegrees::D_1125, Line);
                makeCase(GCBallDegrees::D0, Line);
                makeCase(GCBallDegrees::D1125, Line);
                makeCase(GCBallDegrees::D2250, Line);
                makeCase(GCBallDegrees::D3375, Line);
                makeCase(GCBallDegrees::D4500, Line);
                makeCase(GCBallDegrees::D5625, Line);
                makeCase(GCBallDegrees::D6750, Line);
                makeCase(GCBallDegrees::D7875, Line);
                makeCase(GCBallDegrees::D9000, Line);
                makeCase(GCBallDegrees::D10125, Line);
                makeCase(GCBallDegrees::D11250, Line);
                makeCase(GCBallDegrees::D12375, Line);
                makeCase(GCBallDegrees::D13500, Line);
                makeCase(GCBallDegrees::D14625, Line);
                makeCase(GCBallDegrees::D15750, Line);
                makeCase(GCBallDegrees::D16875, Line);
                makeCase(GCBallDegrees::D18000, Line);
                makeCase(GCBallDegrees::D12000, Line);
                makeCase(GCBallDegrees::D_12000, Line);
                makeCase(GCBallDegrees::D6000, Line);
                makeCase(GCBallDegrees::D_6000, Line);
    
                default:
                    printf("%s(%d) %s Unreachable code with direction %d speed %d\n", __FILE__, __LINE__, __func__, moveInfo.direction, moveInfo.speed);
                    abort();
                    break;
            }
            break;
            
        default:
            error("%s(%d) %s Unreachable code with value %d\n", __FILE__, __LINE__, __func__, moveInfo.speed);
            abort();
            break;
    }
}
