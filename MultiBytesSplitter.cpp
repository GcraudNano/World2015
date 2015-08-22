#include "MultiBytesSplitter.h"
#include <limits>

template <typename T>
MultiBytesSplitter<T>::MultiBytesSplitter()
{
    do {
        *valuePoint = 0;
    } while (++valuePoint != valuePointLimit);
    valuePoint = valuePointLimit;
}

template <typename T>
void MultiBytesSplitter<T>::addByte(uint8_t byte)
{
    if (byte == std::numeric_limits<uint8_t>::max()) {
        if (splitterSeen) {
            *valuePoint = byte;
            if (valuePoint-- == value) {
                valuePoint = valuePointLimit;
            }
            splitterSeen = false;
        } else {
            splitterSeen = true;
        }
    } else {
        if (splitterSeen) {
            valuePoint = valuePointLimit;
            splitterSeen = false;
        }
        *valuePoint = byte;
        if (valuePoint-- == value) {
            valuePoint = valuePointLimit;
        }
    }
}

template <typename T>
T MultiBytesSplitter<T>::bytes()
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wcast-align"
    return *reinterpret_cast<T *>(value);
#pragma GCC diagnostic pop
}

template class MultiBytesSplitter<int16_t>;
template class MultiBytesSplitter<uint16_t>;
template class MultiBytesSplitter<int32_t>;
template class MultiBytesSplitter<uint32_t>;
template class MultiBytesSplitter<int64_t>;
template class MultiBytesSplitter<uint64_t>;
