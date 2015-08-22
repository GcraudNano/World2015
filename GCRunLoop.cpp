#include "GCRunLoop.h"

GCRunLoop GCRunLoop::mainLoop = GCRunLoop();

GCRunLoop::GCRunLoop() : queue(Mail<runloop_block_t, queueSize>())
{
}

void GCRunLoop::run()
{
    if (isRunning) {
        printf("%s(%d) %s called twice\n", __FILE__, __LINE__, __func__);
        abort();
    }
    isRunning = true;
    while (1) {
        osEvent event = queue.get();
        auto func = reinterpret_cast<runloop_block_t *>(event.value.p);
        (*func)();
        queue.free(func);
    }
}

void GCRunLoop::dispatch_async(Callback *sender, void *arg)
{
    MBED_ASSERT(sender != nullptr);
    runloop_block_t *f = queue.alloc();
    *f = FuncWrapper(sender, arg);
    queue.put(f);
}

//void GCRunLoop::dispatch_sync(T *tptr, void (T::*mptr)(), Thread *thread)
//{
//    dispatch_async([=]{
//        while (thread->get_state() == Thread::Running) ;
//        func();
//        thread->signal_set(42);
//    });
//    thread->signal_wait(42);
//}
