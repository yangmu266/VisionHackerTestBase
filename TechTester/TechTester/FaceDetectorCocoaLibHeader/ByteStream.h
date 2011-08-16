
#include "winTypes.h"
#include <exception>
#include <string.h>

class ByteStream
{
public:
    template<typename T>
    static inline T ReadSingleValue(const BYTE* buffer, int iBufLen, int& iStreamPos)
    {
        if(iBufLen < 0 || iStreamPos < 0 || buffer == 0L)
        {
            throw std::string("invalid buffer or position");
        }

        if(iBufLen - iStreamPos < sizeof(T))
        {
            throw std::string("buffer overflow!");
        }
        else
        {
            T val = *(T*)(&buffer[iStreamPos]);
            iStreamPos += sizeof(T);
            return val;
        }
    }


    template<typename T>
    static inline void ReadArray(const BYTE* buffer, int iBufLen, int& iStreamPos,
        T* target, int count)
    {
        if(iBufLen < 0 || iStreamPos < 0 || buffer == 0L || target == 0L || count < 0)
        {
            throw std::string("invalid buffer or position");
        }

        if((iBufLen - iStreamPos) < (int)(sizeof(T) * count))
        {
            throw std::string("buffer overflow!");
        }
        else
        {
            int nTotalBytes = sizeof(T) * count;
            memcpy((void*)&target[0], (void*)&buffer[iStreamPos], nTotalBytes);
            iStreamPos += nTotalBytes;
        }
    }
};
