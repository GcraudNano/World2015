#ifndef __GCRunLoop__
#define __GCRunLoop__

#include "GCRobot.h"
#include <functional>

class GCRunLoop {
public:
    class Callback {
    public:
        virtual void runLoopCallback(void *arg) = 0;
    };
    
private:
    class FuncWrapper {
        Callback *obj;
        void *arg;
    public:
        FuncWrapper(Callback *_obj, void *_arg) : obj(_obj), arg(_arg) {
            
        }
        void operator () (void) {
            obj->runLoopCallback(arg);
        }
    };

    typedef FuncWrapper runloop_block_t;
    
    static constexpr int queueSize = 20;
    GCRunLoop();
    void operator = (GCRunLoop &r) {
        /* so you cannot assign anything to GCRunLoop::mainLoop */
    }
    Mail<runloop_block_t, queueSize> queue;
    bool isRunning = false;
    
public:
    static GCRunLoop mainLoop;
    void __attribute__((noreturn)) run();
    void dispatch_async(Callback *sender, void *arg);
//    template <typename T>
//    void dispatch_sync(T* tptr, void (T::*mptr)(void), Thread *thread);
};

#endif
