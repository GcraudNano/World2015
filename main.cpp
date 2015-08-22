#include "GCRobot.h"
#include "AppDelegate.h"
#include "GCRunLoop.h"
#include "GCSharedObjects.h"

int main(int argc, const char * argv[])
{
#if defined(LOGGING) || defined(__clang__)
    GcraudNano::pc.baud(230400);
#endif
    AppDelegate delegate;
    GCRunLoop::mainLoop.run();
    (void)delegate;
    
    return 0;
}
