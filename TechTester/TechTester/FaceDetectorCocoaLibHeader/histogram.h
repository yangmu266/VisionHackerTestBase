#pragma once

#include "image.h"

///////////////////////////////////////////////////////////////////////
// HISTOGRAM: grayscale or color histogram (evenly spaced bins)
///////////////////////////////////////////////////////////////////////

class HISTOGRAM 
{
public: 
    enum enumHistMode
    {
        GRAY_HISTOGRAM, 
        COLOR3_HISTOGRAM, 
    }; 

private: 
    enumHistMode m_HistMode; 
    int m_nShift1, m_nShift2, m_nShift3; 
    int m_nNumBins; 
    float *m_pfProb; 
    float m_fSum; 

public:
    // nBits1,2,3 are the number of bits to represent each color component 
    HISTOGRAM(enumHistMode mode, int nBits1, int nBits2=0, int nBits3=0); 
    ~HISTOGRAM(); 

public:
    void ResetToUniform(); 
    void ResetToZero(); 
    void Normalize(); 

    void Copy(HISTOGRAM *pHist); 
    void Add(HISTOGRAM *pHist); 
    void AddConst(float fConst); 
    void Sub(HISTOGRAM *pHist);     // the one who call this function will ensure after substraction all probability remains positive
    void Mix(HISTOGRAM *pHist, float alpha); 

    float CalcBhatCoef(HISTOGRAM *pHist); 

    inline enumHistMode GetHistMode() { return m_HistMode; }; 
    inline float GetProb(BYTE *pVal) { return m_pfProb[FindBin(pVal)]; }; 
    inline float GetProbC(BYTE *pVal) { return m_pfProb[FindBinC(pVal)]; }; 
    inline float GetProb(int id) { return m_pfProb[id]; };
    inline float *GetProbPtr() { return m_pfProb; }; 
    inline float GetProbSum() { return m_fSum; }; 
    inline int GetNumBins1() { return 1<<(8-m_nShift1); }; 
    inline int GetNumBins2() { return 1<<(8-m_nShift2); }; 
    inline int GetNumBins3() { return 1<<(8-m_nShift3); }; 
    inline int GetNumBins() { return m_nNumBins; }; 

    // high-level functions 
    // cumulate histogram of a region 
    HRESULT CumulateROIHist(IMAGE *pImg, IRECT &roi); 
    HRESULT CumulateROIHist(IMAGE *pImg, IRECT &roi, float *pfKernel); 
    HRESULT CumulateROIHist(IMAGE *pImg, IMAGE *pMask, IRECT &roi, float *pfKernel); 
    HRESULT CumulateROIHist(IMAGEC *pImg, IRECT &roi); 
    HRESULT CumulateROIHist(IMAGEC *pImg, IRECT &roi, float *pfKernel); 
    HRESULT CumulateROIHist(IMAGEC *pImg, IMAGE *pMask, IRECT &roi, float *pfKernel); 

    HRESULT SkinColorFilterBGR();    // for filtering a histogram with skin color (remove non-skin colors) 
    HRESULT SkinColorFilterRGB();
    HRESULT CreateSkinColorMaskBGR(); // for skin color masking 
    HRESULT CreateSkinColorMaskRGB();

public: 

    inline int FindBin(BYTE *pData) 
    {
        return (pData[0]>>m_nShift1);
    };

    inline int FindBinC(BYTE *pData) 
    {
        return (((pData[0]>>m_nShift1)<<(16-m_nShift2-m_nShift3)) |
                ((pData[1]>>m_nShift2)<<(8-m_nShift3)) |
                 (pData[2]>>m_nShift3));
    };
}; 
