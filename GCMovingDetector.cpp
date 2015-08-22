#include "GCMovingDetector.h"
#include "GCRunLoop.h"

using namespace GcraudNano;

GCMovingDetector::GCMovingDetector(GCMovingDetectorDelegate *_d, bool _isKeeper) : mail(Mail<BallInformation, mailSize>()), delegate(_d), isKeeper(_isKeeper)
{
    MBED_ASSERT(delegate != nullptr);
}

void GCMovingDetector::threadCallback(const void *arg)
{
    GCMovingDetector *self = reinterpret_cast<GCMovingDetector *>(const_cast<void *>(arg));
    while (1) {
        osEvent event = self->mail.get();
        BallInformation *info = reinterpret_cast<BallInformation *>(event.value.p);
        self->detect(info);
        self->thread.yield();
    }
}

MoveInformation *GCMovingDetector::shouldGoStraightToGoal(const GCMovingDetector::BallInformation *information)
{
    static MoveInformation info;
    info.speed = MovingSpeed::Fast;
    if (! isKeeper && information->distance == GCBallDistance::Far) {
        GCBallDegrees degrees = information->degrees;
        if (degrees <= GCBallDegrees::D_6750 || GCBallDegrees::D6750 <= degrees) {
            if (! delegate->movingDetector_isBackClose(this)) {
                if (degrees > GCBallDegrees::D0) {
                    if (degrees <= GCBallDegrees::D14625) {
                        info.direction = degrees >= GCBallDegrees::D12375 ? GCBallDegrees::D18000 : GCBallDegrees::D12000;
                    } else {
                        info.direction = delegate->movingDetector_rightIsFarther(this) ? GCBallDegrees::D_13500 : GCBallDegrees::D13500;
                    }
                } else {
                    if (GCBallDegrees::D_14625 <= degrees) {
                        info.direction = degrees <= GCBallDegrees::D_12375 ? GCBallDegrees::D18000 : GCBallDegrees::D_12000;
                    } else {
                        info.direction = delegate->movingDetector_rightIsFarther(this) ? GCBallDegrees::D_13500 : GCBallDegrees::D13500;
                    }
                }
//                if (information->tiltValue != GCBallDegrees::D0) {
//                    info.speed = MovingSpeed::Middle;
//                }
                return &info;
            }
        } else {
            info.direction = information->degrees;
//            if (information->tiltValue != GCBallDegrees::D0) {
//                info.speed = MovingSpeed::Middle;
//            }
            return &info;
        }
    }
    return nullptr;
}

GCBallDegrees GCMovingDetector::absoluteDegrees(const BallInformation *information)
{
    GCBallDegrees degrees = information->degrees;
    GCBallDegrees tiltValue = information->tiltValue;
    degrees = static_cast<GCBallDegrees>(degToInt(degrees) + degToInt(tiltValue) - degToInt(GCBallDegrees::D0));
    if (degrees > GCBallDegrees::D18000) {
        degrees = static_cast<GCBallDegrees>(degToInt(degrees) - ((degToInt(GCBallDegrees::D18000) - degToInt(GCBallDegrees::D0))) * 2);
    } else if (degrees < GCBallDegrees::D_16875) {
        degrees = static_cast<GCBallDegrees>(degToInt(degrees) + ((degToInt(GCBallDegrees::D18000) - degToInt(GCBallDegrees::D0))) * 2);
    }
    return degrees;
}

MoveInformation *GCMovingDetector::decideAround(BallInformation *information)
{
#ifdef __MBED__
    #define repoatError()                                                           \
    printf(                                                                         \
        "%s(%d) %s Unreachable code with degrees %d, tiltValue %d, isKeeper %d\n",  \
        __FILE__,                                                                   \
        __LINE__,                                                                   \
        __func__,                                                                   \
        degToInt(information->degrees),                                             \
        degToInt(information->tiltValue),                                           \
        isKeeper                                                                    \
    );                                                                              \
    abort()
#else
    #define repoatError abort();
#endif
    
    static MoveInformation moveInfo;
    
    switch (information->distance) {
        case GCBallDistance::Far:
            moveInfo.speed = MovingSpeed::Fast;
            break;
            
        case GCBallDistance::Middle:
            moveInfo.speed = MovingSpeed::Middle;
            break;
            
        case GCBallDistance::Near:
            moveInfo.speed = MovingSpeed::Slow;
            break;
            
        default:
            repoatError();
            break;
    }
    
    switch (information->degrees) {
        case GCBallDegrees::D0:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.direction = GCBallDegrees::D18000;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        switch (information->distance) {
                            case GCBallDistance::Far:
                                moveInfo.direction = GCBallDegrees::DNone;
                                break;
                                
                            case GCBallDistance::Middle:
                                moveInfo.direction = GCBallDegrees::DNone;
                                break;
                                
                            case GCBallDistance::Near:
                                moveInfo.speed = MovingSpeed::Slow;
                                moveInfo.direction = GCBallDegrees::D18000;
                                break;
                                
                            default:
                                repoatError();
                                break;
                        }
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        switch (information->distance) {
                            case GCBallDistance::Far:
                                moveInfo.direction = GCBallDegrees::DNone;
                                break;
                                
                            case GCBallDistance::Middle:
                                moveInfo.speed = MovingSpeed::Slow;
                                moveInfo.direction = GCBallDegrees::D0;
                                break;
                                
                            case GCBallDistance::Near:
                                moveInfo.direction = GCBallDegrees::D0;
                                break;
                                
                            default:
                                repoatError();
                                break;
                        }
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = GCBallDegrees::D0;
            }
            break;
            
        case GCBallDegrees::D1125:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D16875;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D2250;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
//                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D0 : GCBallDegrees::D3375;
                moveInfo.direction = GCBallDegrees::D2250;
            }
            break;
            
        case GCBallDegrees::D2250:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D15750;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D3375;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
//                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D1125 : GCBallDegrees::D4500;
                moveInfo.direction = GCBallDegrees::D3375;
            }
            break;
            
        case GCBallDegrees::D3375:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D14625;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D4500;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D4500 : GCBallDegrees::D9000;
            }
            break;
            
        case GCBallDegrees::D4500:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D13500;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D5625;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D13500 : GCBallDegrees::D9000;
            }
            break;
            
        case GCBallDegrees::D5625:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D12375;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D6750;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D14625 : GCBallDegrees::D9000;
            }
            break;
            
        case GCBallDegrees::D6750:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        moveInfo.direction = information->distance == GCBallDistance::Near ? GCBallDegrees::D13500 : GCBallDegrees::D11250;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        /* No Effect */
                        /* moveInfo.direction = GCBallDegrees::D6750; */
                        moveInfo.speed = MovingSpeed::Fast;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D6750 :
                information->distance == GCBallDistance::Near ? GCBallDegrees::D15750 : GCBallDegrees::D11250;
            }
            break;
            
        case GCBallDegrees::D7875:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        moveInfo.direction = information->distance == GCBallDistance::Near ? GCBallDegrees::D14625 : GCBallDegrees::D12375;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        /* No Effect */
                        /* moveInfo.direction = GCBallDegrees::D7875; */
                        moveInfo.speed = MovingSpeed::Fast;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D7875 :
                information->distance == GCBallDistance::Near ? GCBallDegrees::D16875 : GCBallDegrees::D12375;
            }
            break;
            
        case GCBallDegrees::D9000:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        if (information->distance == GCBallDistance::Near) {
                            moveInfo.speed = delegate->movingDetector_whereAmI(this) == RobotLocation::CloseEnoughToPutOut ? MovingSpeed::Middle : MovingSpeed::Fast;
                            moveInfo.direction = GCBallDegrees::D18000;
                        } else {
                            moveInfo.direction = GCBallDegrees::D13500;
                        }
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        /* No Effect */
                        /* moveInfo.direction = GCBallDegrees::D9000; */
                        moveInfo.speed = MovingSpeed::Fast;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D9000 :
                information->distance == GCBallDistance::Near ? moveInfo.speed = MovingSpeed::Fast, GCBallDegrees::D18000 : GCBallDegrees::D13500;
            }
            break;
            
        case GCBallDegrees::D10125:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        if (information->distance == GCBallDistance::Near) {
                            moveInfo.speed = MovingSpeed::Fast;
                            moveInfo.direction = GCBallDegrees::D18000;
                        } else {
                            moveInfo.direction = GCBallDegrees::D14625;
                        }
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.direction = GCBallDegrees::D18000;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D10125 :
                information->distance == GCBallDistance::Near ? moveInfo.speed = MovingSpeed::Fast, GCBallDegrees::D18000 : GCBallDegrees::D14625;
            }
            
            break;
            
        case GCBallDegrees::D11250:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        if (information->distance == GCBallDistance::Near) {
                            moveInfo.speed = delegate->movingDetector_whereAmI(this) == RobotLocation::CloseEnoughToPutOut ? MovingSpeed::Middle : MovingSpeed::Fast;
                            moveInfo.direction = GCBallDegrees::D18000;
                        } else {
                            moveInfo.direction = GCBallDegrees::D15750;
                        }
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.direction = GCBallDegrees::D18000;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D10125 :
                information->distance == GCBallDistance::Near ? moveInfo.speed = MovingSpeed::Fast, GCBallDegrees::D18000 : GCBallDegrees::D15750;
            }
            break;
            
        case GCBallDegrees::D_1125:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_16875;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D_2250;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
//                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D0 : GCBallDegrees::D_3375;
                moveInfo.direction = GCBallDegrees::D_2250;
            }
            break;
            
        case GCBallDegrees::D_2250:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_15750;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D_3375;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
//                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D_1125 : GCBallDegrees::D_4500;
                moveInfo.direction = GCBallDegrees::D_3375;
            }
            break;
            
        case GCBallDegrees::D_3375:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_14625;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D_4500;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D_4500 : GCBallDegrees::D_9000;
            }
            break;
            
        case GCBallDegrees::D_4500:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_13500;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D_5625;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D_13500 : GCBallDegrees::D_9000;
            }
            break;
            
        case GCBallDegrees::D_5625:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_12375;
                        break;
                        
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = GCBallDegrees::D_9000;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.speed = MovingSpeed::Middle;
                        moveInfo.direction = GCBallDegrees::D_6750;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = information->distance == GCBallDistance::Near && ! delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D_14625 : GCBallDegrees::D_9000;
            }
            break;
            
        case GCBallDegrees::D_6750:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        moveInfo.speed = MovingSpeed::Slow;
                        moveInfo.direction = information->distance == GCBallDistance::Near ? GCBallDegrees::D_13500 : GCBallDegrees::D_11250;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        /* No Effect */
                        /* moveInfo.direction = GCBallDegrees::D_6750; */
                        moveInfo.speed = MovingSpeed::Fast;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D_6750 :
                information->distance == GCBallDistance::Near ? GCBallDegrees::D_15750: GCBallDegrees::D_11250;
            }
            break;
            
        case GCBallDegrees::D_7875:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        moveInfo.direction = information->distance == GCBallDistance::Near ? GCBallDegrees::D_14625 : GCBallDegrees::D_12375;
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        /* No Effect */
                        /* moveInfo.direction = GCBallDegrees::D_7875; */
                        moveInfo.speed = MovingSpeed::Fast;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D_7875 :
                information->distance == GCBallDistance::Near ? GCBallDegrees::D_16875 : GCBallDegrees::D_12375;
            }
            break;
            
        case GCBallDegrees::D_9000:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        if (information->distance == GCBallDistance::Near) {
                            moveInfo.speed = delegate->movingDetector_whereAmI(this) == RobotLocation::CloseEnoughToPutOut ? MovingSpeed::Middle : MovingSpeed::Fast;
                            moveInfo.direction = GCBallDegrees::D18000;
                        } else {
                            moveInfo.direction = GCBallDegrees::D_13500;
                        }
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        /* No Effect */
                        /* moveInfo.direction = GCBallDegrees::D_9000; */
                        moveInfo.speed = MovingSpeed::Fast;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D_9000 :
                information->distance == GCBallDistance::Near ? moveInfo.speed = MovingSpeed::Middle, GCBallDegrees::D18000 : GCBallDegrees::D_13500;
            }
            break;
            
        case GCBallDegrees::D_10125:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        if (information->distance == GCBallDistance::Near) {
                            moveInfo.speed = delegate->movingDetector_whereAmI(this) == RobotLocation::CloseEnoughToPutOut ? MovingSpeed::Middle : MovingSpeed::Fast;
                            moveInfo.direction = GCBallDegrees::D18000;
                        } else {
                            moveInfo.direction = GCBallDegrees::D_14625;
                        }
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.direction = GCBallDegrees::D18000;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D_10125 :
                information->distance == GCBallDistance::Near ? moveInfo.speed = MovingSpeed::Middle, GCBallDegrees::D18000 : GCBallDegrees::D_14625;
            }
            break;
            
        case GCBallDegrees::D_11250:
            if (isKeeper) {
                switch (delegate->movingDetector_whereAmI(this)) {
                    case RobotLocation::TooCloseToOpponentsGoal:
                    case RobotLocation::AlmostMiddle:
                        if (information->distance == GCBallDistance::Near) {
                            moveInfo.speed = delegate->movingDetector_whereAmI(this) == RobotLocation::CloseEnoughToPutOut ? MovingSpeed::Middle : MovingSpeed::Fast;
                            moveInfo.direction = GCBallDegrees::D18000;
                        } else {
                            moveInfo.direction = GCBallDegrees::D_15750;
                        }
                        break;
                        
                    case RobotLocation::CloseEnoughToPutOut:
                        moveInfo.direction = GCBallDegrees::D18000;
                        break;
                        
                    default:
                        repoatError();
                        break;
                }
            } else {
                moveInfo.direction = delegate->movingDetector_isBackClose(this) ? GCBallDegrees::D_10125 :
                information->distance == GCBallDistance::Near ? moveInfo.speed = MovingSpeed::Middle, GCBallDegrees::D18000 : GCBallDegrees::D_15750;
            }
            break;
            
        case GCBallDegrees::D_12375:
        case GCBallDegrees::D_13500:
            if (isKeeper) {
                if (delegate->movingDetector_whereAmI(this) == RobotLocation::CloseEnoughToPutOut) {
                    moveInfo.speed = MovingSpeed::Fast;
                    moveInfo.direction = GCBallDegrees::D18000;
                }
            } else {
                if (information->distance == GCBallDistance::Near) {
                    moveInfo.speed = MovingSpeed::Middle;
                } else {
                    moveInfo.speed = MovingSpeed::Fast;
                }
                if (delegate->movingDetector_isBackClose(this)) {
                    moveInfo.direction = GCBallDegrees::D_10125;
                } else {
                    moveInfo.direction = GCBallDegrees::D18000;
                }
            }
            break;
            
        case GCBallDegrees::D12375:
        case GCBallDegrees::D13500:
            if (isKeeper) {
                if (information->distance == GCBallDistance::Near) moveInfo.speed = MovingSpeed::Middle;
                moveInfo.direction = GCBallDegrees::D18000;
            } else {
                if (information->distance == GCBallDistance::Near) {
                    moveInfo.speed = MovingSpeed::Middle;
                } else {
                    moveInfo.speed = MovingSpeed::Fast;
                }
                if (delegate->movingDetector_isBackClose(this)) {
                    moveInfo.direction = GCBallDegrees::D10125;
                } else {
                    moveInfo.direction = GCBallDegrees::D18000;
                }
            }
            break;
            
        case GCBallDegrees::D14625:
        case GCBallDegrees::D15750:
        case GCBallDegrees::D16875:
            if (isKeeper) {
                if (information->distance == GCBallDistance::Near) {
                    moveInfo.speed = MovingSpeed::Middle;
                    if (delegate->movingDetector_isLeftClose(this)) {
                        moveInfo.direction = GCBallDegrees::D_9000;
                    } else if (delegate->movingDetector_isRightClose(this)) {
                        moveInfo.direction = GCBallDegrees::D9000;
                    } else {
                        moveInfo.direction = GCBallDegrees::D_12000;
                    }
                } else {
                    moveInfo.direction = delegate->movingDetector_isRightClose(this) ? GCBallDegrees::D18000 : GCBallDegrees::D_13500;
                }
            } else {
                if (delegate->movingDetector_isBackClose(this)) {
                    moveInfo.direction = GCBallDegrees::DNone;
                } else if (information->distance == GCBallDistance::Near) {
                    moveInfo.speed = MovingSpeed::Middle;
                    if (delegate->movingDetector_isLeftClose(this)) {
                        moveInfo.direction = GCBallDegrees::D_9000;
                    } else if (delegate->movingDetector_isRightClose(this)) {
                        moveInfo.direction = GCBallDegrees::D9000;
                    } else {
                        moveInfo.direction = GCBallDegrees::D_12000;
                    }
                } else {
                    moveInfo.direction = delegate->movingDetector_isRightClose(this) ? moveInfo.speed = MovingSpeed::Fast, GCBallDegrees::D18000 : GCBallDegrees::D_13500;
                }
            }
            break;
            
        case GCBallDegrees::D_14625:
        case GCBallDegrees::D_15750:
        case GCBallDegrees::D_16875:
            if (isKeeper) {
                if (information->distance == GCBallDistance::Near) {
                    moveInfo.speed = MovingSpeed::Middle;
                    if (delegate->movingDetector_isRightClose(this)) {
                        moveInfo.direction = GCBallDegrees::D9000;
                    } else if (delegate->movingDetector_isLeftClose(this)) {
                        moveInfo.direction = GCBallDegrees::D_9000;
                    } else {
                        moveInfo.direction = GCBallDegrees::D12000;
                    }
                } else {
                    moveInfo.direction = delegate->movingDetector_isLeftClose(this) ? GCBallDegrees::D18000 : GCBallDegrees::D13500;
                }
            } else {
                if (delegate->movingDetector_isBackClose(this)) {
                    moveInfo.direction = GCBallDegrees::DNone;
                } else if (information->distance == GCBallDistance::Near) {
                    moveInfo.speed = MovingSpeed::Middle;
                    if (delegate->movingDetector_isRightClose(this)) {
                        moveInfo.direction = GCBallDegrees::D9000;
                    } else if (delegate->movingDetector_isLeftClose(this)) {
                        moveInfo.direction = GCBallDegrees::D_9000;
                    } else {
                        moveInfo.direction = GCBallDegrees::D12000;
                    }
                } else {
                    moveInfo.direction = delegate->movingDetector_isLeftClose(this) ? moveInfo.speed = MovingSpeed::Fast, GCBallDegrees::D18000 : GCBallDegrees::D13500;
                }
            }
            break;
            
        case GCBallDegrees::D18000:
            if (isKeeper) {
                if (information->distance == GCBallDistance::Near) moveInfo.speed = MovingSpeed::Middle;
                if (delegate->movingDetector_rightIsFarther(this)) {
                    moveInfo.direction = GCBallDegrees::D_13500;
                } else {
                    moveInfo.direction = GCBallDegrees::D13500;
                }
            } else {
                if (delegate->movingDetector_isBackClose(this)) {
                    moveInfo.direction = GCBallDegrees::DNone;
                } else if (information->distance == GCBallDistance::Near) moveInfo.speed = MovingSpeed::Middle;
                if (delegate->movingDetector_rightIsFarther(this)) {
                    moveInfo.direction = GCBallDegrees::D_13500;
                } else {
                    moveInfo.direction = GCBallDegrees::D13500;
                }
            }
            break;
            
        default:
            repoatError();
            break;
    }
    
    return &moveInfo;
}

#undef reportError

void GCMovingDetector::slowDownFix(MoveInformation *information)
{
    if (information->speed <= MovingSpeed::LineSlow) {
        return;
    }
    
    switch (information->direction) {
        case GCBallDegrees::D0:
        case GCBallDegrees::D1125:
        case GCBallDegrees::D_1125:
        case GCBallDegrees::D2250:
        case GCBallDegrees::D_2250:
            if (delegate->movingDetector_isBackFar(this)) {
                information->speed = MovingSpeed::LineSlow;
            }
            break;
            
        case GCBallDegrees::D3375:
        case GCBallDegrees::D4500:
        case GCBallDegrees::D5625:
            if (delegate->movingDetector_isBackFar(this) || delegate->movingDetector_isLeftClose(this)) {
                information->speed = MovingSpeed::LineSlow;
            }
            break;
            
        case GCBallDegrees::D_3375:
        case GCBallDegrees::D_4500:
        case GCBallDegrees::D_5625:
            if (delegate->movingDetector_isBackFar(this) || delegate->movingDetector_isRightClose(this)) {
                information->speed = MovingSpeed::LineSlow;
            }
            break;
            
        case GCBallDegrees::D6750:
        case GCBallDegrees::D7875:
        case GCBallDegrees::D9000:
        case GCBallDegrees::D10125:
        case GCBallDegrees::D11250:
            if (delegate->movingDetector_isLeftClose(this)) {
                information->speed = MovingSpeed::LineSlow;
            }
            break;
            
        case GCBallDegrees::D_6750:
        case GCBallDegrees::D_7875:
        case GCBallDegrees::D_9000:
        case GCBallDegrees::D_10125:
        case GCBallDegrees::D_11250:
            if (delegate->movingDetector_isRightClose(this)) {
                information->speed = MovingSpeed::LineSlow;
            }
            break;
            
        case GCBallDegrees::D12000:
        case GCBallDegrees::D12375:
        case GCBallDegrees::D13500:
        case GCBallDegrees::D14625:
            if (delegate->movingDetector_isBackClose(this) || delegate->movingDetector_isLeftClose(this)) {
                information->speed = MovingSpeed::LineSlow;
            }
            break;
            
        case GCBallDegrees::D_12000:
        case GCBallDegrees::D_12375:
        case GCBallDegrees::D_13500:
        case GCBallDegrees::D_14625:
            if (delegate->movingDetector_isBackClose(this) || delegate->movingDetector_isRightClose(this)) {
                information->speed = MovingSpeed::LineSlow;
            }
            break;
            
        case GCBallDegrees::D15750:
        case GCBallDegrees::D16875:
        case GCBallDegrees::D_15750:
        case GCBallDegrees::D_16875:
        case GCBallDegrees::D18000:
            if (delegate->movingDetector_isBackClose(this)) {
                information->speed = MovingSpeed::LineSlow;
            }
            break;
            
        case GCBallDegrees::DNone:
            break;
            
        default:
            error(
                  "%s(%d) %s Unreachable code with direction %d, speed %d, isKeeper %d",
                  __FILE__, __LINE__, __func__, information->direction, information->speed, isKeeper);
            break;
    }
}

GCBallDegrees GCMovingDetector::actualDegrees(BallInformation *information)
{
    GCBallDegrees degrees = information->degrees;
    GCBallDegrees tiltValue = information->tiltValue;
    GCBallDistance distance = information->distance;
    if (tiltValue == GCBallDegrees::D0) {
        if (degrees == GCBallDegrees::D12000) {
            degrees = GCBallDegrees::D12375;
        } else if (degrees == GCBallDegrees::D_12000) {
            degrees = GCBallDegrees::D_12375;
        }
    }
    if (degrees != GCBallDegrees::DNone && distance != GCBallDistance::Far) {
        degrees = static_cast<GCBallDegrees>(degToInt(degrees) - (degToInt(tiltValue) - degToInt(GCBallDegrees::D0)));
        if (degrees > GCBallDegrees::D18000) {
            degrees = static_cast<GCBallDegrees>(degToInt(degrees) - (degToInt(GCBallDegrees::D18000) - degToInt(GCBallDegrees::D0)) * 2);
        } else if (degrees < GCBallDegrees::D_16875) {
            degrees = static_cast<GCBallDegrees>(degToInt(degrees) + (degToInt(GCBallDegrees::D18000) - degToInt(GCBallDegrees::D0)) * 2);
        }
    }
    return degrees;
}

void GCMovingDetector::detect(GCMovingDetector::BallInformation *information)
{
    class MailFreeer {
        Mail<BallInformation, mailSize> *mail;
        BallInformation *info;
        
    public:
        MailFreeer(Mail<BallInformation, mailSize> *_m, BallInformation *_i) : mail(_m), info(_i) {}
        ~MailFreeer() { mail->free(info); }
    };
    
    MailFreeer freeer = MailFreeer(&mail, information);
    
    if (information->degrees == GCBallDegrees::DNone) {
        return;
    }
    
    MoveInformation *moveInfo;
    if ((moveInfo = shouldGoStraightToGoal(information))) {
        moveInfo->direction = actualDegrees(information);
        slowDownFix(moveInfo);
//        GCRunLoop::mainLoop.dispatch_async([=]{
            delegate->movingDetector_movingDegreesDetected(this, moveInfo->direction, moveInfo->speed);
//        });
    } else {
        information->degrees = absoluteDegrees(information);
        moveInfo = decideAround(information);
        information->degrees = moveInfo->direction;
        moveInfo->direction = actualDegrees(information);
        slowDownFix(moveInfo);
//        GCRunLoop::mainLoop.dispatch_async([=]{
            delegate->movingDetector_movingDegreesDetected(this, moveInfo->direction, moveInfo->speed);
//        });
    }
    
    (void)freeer;
}

void GCMovingDetector::reloadBallInformation(GCBallDegrees degrees, GCBallDistance distance, GCBallDegrees tiltValue)
{
    BallInformation *info = mail.alloc();
    info->degrees = degrees;
    info->distance = distance;
    info->tiltValue = tiltValue;
    mail.put(info);
}

void GCMovingDetector::moveAgainstLines(GcraudNano::GCLineDegrees degrees, GcraudNano::GCBallDegrees tiltValue)
{
    MBED_ASSERT(degrees != GCLineDegreesNone);
    GCBallDegrees lineDegrees = GCBallDegrees::DNone;
    
    switch (degrees) {
        case GCLineDegrees0:
            lineDegrees = GCBallDegrees::D0;
            break;
            
        case GCLineDegrees45:
            lineDegrees = GCBallDegrees::D4500;
            
        case GCLineDegrees_45:
            lineDegrees = GCBallDegrees::D_4500;
            break;
            
        case GCLineDegrees135:
            lineDegrees = GCBallDegrees::D13500;
            break;
            
        case GCLineDegrees_135:
            lineDegrees = GCBallDegrees::D_13500;
            break;
            
        case GCLineDegrees180:
            lineDegrees = GCBallDegrees::D18000;
            break;
            
        default:
            error("%s(%d) %s Unreachable code with degrees %d", __FILE__, __LINE__, __func__, degrees);
            break;
    }
    
    BallInformation information = {lineDegrees, GCBallDistance::Far, tiltValue};
    lineDegrees = absoluteDegrees(&information);
    /* Now we have the absolute line degrees */
    
    /* Next, we should reverse the degrees */
    lineDegrees = static_cast<GCBallDegrees>(degToInt(lineDegrees) + degToInt(GCBallDegrees::D0));
    if (GCBallDegrees::D18000 < lineDegrees) {
        lineDegrees = static_cast<GCBallDegrees>(degToInt(lineDegrees) - degToInt(GCBallDegrees::D18000));
    }
    
    delegate->movingDetector_movingDegreesDetected(this, lineDegrees, MovingSpeed::Fast);
}
