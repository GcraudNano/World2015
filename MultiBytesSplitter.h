#ifndef __MultiBytesSplitter__
#define __MultiBytesSplitter__

#include <inttypes.h>

template <typename T>
class MultiBytesSplitter {
    uint8_t value[sizeof(T)];
    uint8_t *valuePoint = value;
    uint8_t *valuePointLimit = valuePoint + sizeof(T) - 1;
    bool splitterSeen = false;
    
public:
    MultiBytesSplitter();
    void addByte(uint8_t byte);
    T bytes();
};

#endif
