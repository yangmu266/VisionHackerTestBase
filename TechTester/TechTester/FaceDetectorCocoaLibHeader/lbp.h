
#pragma once

#include "winTypes.h"

#include <stdlib.h>

namespace ILabs
{

#ifndef SAFE_ARRAYDELETE
#define SAFE_ARRAYDELETE(p)        { if (p != NULL) {delete [] p; p = NULL;} }
#endif

#define LBP_DIMENSION   59

typedef unsigned char BYTE;

class Lbp
{
    public:
        Lbp(int nCellX, int nCellY);
        ~Lbp();
        void GetLBPRepresentation(__in_ecount(width*height) float* patch, __in_ecount(LBP_VEC_SIZE) float* featPtr, int width, int height);
    
    private:

        int m_nCellX;
        int m_nCellY;
        int m_nLBPLen;

        BYTE    m_lbpLUT[256];
        BYTE*   m_imgLBP;
        int     m_nPixelNum;
    };

}