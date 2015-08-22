#ifndef __GCRobot__
#define __GCRobot__

#pragma GCC system_header

#define __STDC_LIMIT_MACROS
#include "mbed.h"
#undef __STDC_LIMIT_MACROS

#include "mbed-rtos/rtos/rtos.h"

#define degrees(d) ((d) * M_PI / 180)

#define INLINE inline __attribute__((always_inline))

#define synchronized(mutex)                                 \
    {                                                       \
        class __Mutex_Finallizer__ {                        \
            Mutex *m = nullptr;                             \
            bool unlocked = false;                          \
        public:                                             \
            __Mutex_Finallizer__(Mutex *_m) : m(_m) {       \
                if (! __get_IPSR()) m->lock();              \
            }                                               \
            void unlock() {                                 \
                if (unlocked) {                             \
                    if (! __get_IPSR()) m->unlock();        \
                    unlocked = true;                        \
                }                                           \
            }                                               \
            ~__Mutex_Finallizer__() {                       \
                unlock();                                   \
            }                                               \
        };                                                  \
        __Mutex_Finallizer__ __finalizer__(&(mutex));       \

#define endSynchronized                                     \
        __finalizer__.unlock();                             \
    }

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define radians(deg) ((deg)*DEG_TO_RAD)
#define abs(v) ((v) >= 0 ? (v) : (-v))

static INLINE constexpr double safeRadians(int degrees)
{
    return degrees > 90 && degrees <= 180 ? radians(180 - degrees) :
    degrees > 180 && degrees <= 270 ? radians(180 - degrees) :
    degrees > 270 ? safeRadians(degrees - 360) :
    degrees < -90 ? safeRadians(degrees + 360) :
    radians(degrees);
}

#endif
