#ifndef __GCSharedObjects__
#define __GCSharedObjects__

#include "GCRobot.h"

namespace GcraudNano {
    enum class GCBallDegrees {
        D_16875,
        D_15750,
        D_14625,
        D_13500,
        D_12375,
        D_11250,
        D_10125,
        D_9000,
        D_7875,
        D_6750,
        D_5625,
        D_4500,
        D_3375,
        D_2250,
        D_1125,
        D0,
        D1125,
        D2250,
        D3375,
        D4500,
        D5625,
        D6750,
        D7875,
        D9000,
        D10125,
        D11250,
        D12375,
        D13500,
        D14625,
        D15750,
        D16875,
        D18000,
        D12000,
        D_12000,
        D6000,
        D_6000,
        DNone = -1,
    };
    
    INLINE constexpr int degToInt(GCBallDegrees deg) {
        return static_cast<int>(deg);
    }
    
    INLINE constexpr int degToActualInt(GCBallDegrees deg) {
        return deg == GCBallDegrees::D12000 ? 12000 : deg == GCBallDegrees::D_12000 ? -12000 : deg == GCBallDegrees::D6000 ? 6000 : deg == GCBallDegrees::D_6000 ? -6000 : 1125 * static_cast<int>(deg) - 16875;
    }
    
    enum class GCIRSensorDegrees {
        D_16875 = degToInt(GCBallDegrees::D_16875),
        D_14625 = degToInt(GCBallDegrees::D_14625),
        D_12375 = degToInt(GCBallDegrees::D_12375),
        D_10125 = degToInt(GCBallDegrees::D_10125),
        D_7875  = degToInt(GCBallDegrees::D_7875),
        D_5625  = degToInt(GCBallDegrees::D_5625),
        D_3375  = degToInt(GCBallDegrees::D_3375),
        D_1125  = degToInt(GCBallDegrees::D_1125),
        D1125   = degToInt(GCBallDegrees::D1125),
        D3375   = degToInt(GCBallDegrees::D3375),
        D5625   = degToInt(GCBallDegrees::D5625),
        D7875   = degToInt(GCBallDegrees::D7875),
        D10125  = degToInt(GCBallDegrees::D10125),
        D12375  = degToInt(GCBallDegrees::D12375),
        D14625  = degToInt(GCBallDegrees::D14625),
        D16875  = degToInt(GCBallDegrees::D16875),
    };
    
    INLINE constexpr int degToInt(GCIRSensorDegrees deg) {
        return static_cast<int>(deg);
    }
    
    typedef int GCLineDegrees;
    constexpr GCLineDegrees GCLineDegreesNone = 0;
    constexpr GCLineDegrees GCLineDegrees45   = 1 << 0;
    constexpr GCLineDegrees GCLineDegrees_45  = 1 << 1;
    constexpr GCLineDegrees GCLineDegrees180  = 1 << 2;
    constexpr GCLineDegrees GCLineDegrees0    = GCLineDegrees45 | GCLineDegrees_45;
    constexpr GCLineDegrees GCLineDegrees135  = GCLineDegrees45 | GCLineDegrees180;
    constexpr GCLineDegrees GCLineDegrees_135 = GCLineDegrees_45 | GCLineDegrees180;
    
    enum class GCBallDistance {
        Far,
        Middle,
        Near,
    };
    
    enum class MovingSpeed {
        LineSlow,
        Slow,
        Middle,
        Fast,
    };
    struct MoveInformation {
        GcraudNano::GCBallDegrees direction;
        MovingSpeed speed;
    };
    
    enum class RobotLocation {
        CloseEnoughToPutOut,
        AlmostMiddle,
        TooCloseToOpponentsGoal,
    };
    
#if defined(LOGGING) || defined(__clang__)
    extern Serial pc;
#endif
    
#ifdef WORDS_STACK_SIZE
    constexpr uint32_t defaultStackSize = DEFAULT_STACK_SIZE;
#else
#   ifndef __clang__
#       error unreachable
#   endif
    constexpr uint32_t defaultStackSize = 2048; // on Xcode
#endif
}

#endif
