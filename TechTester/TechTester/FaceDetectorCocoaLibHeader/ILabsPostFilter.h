
#pragma once

#include "lbp.h"
#include "FaceDetectorLbpPostFilter.h"

namespace ILabs
{

#define N_CELL_X (4)
#define N_CELL_Y (4)
#define CATEGORIES_NUM (15)
#define LBP_VEC_SIZE ((N_CELL_X)*(N_CELL_Y)*(LBP_DIMENSION))

#define AUTO_PASS_SCORE (1000)


class ILabsPostFilter
{
public:
    ILabsPostFilter(FILE *fpModel);
    ~ILabsPostFilter();
    bool PreClassify(int cat, float vjScore) const;
    float Classify(__in_ecount(width*height) float* imvec, int width, int height, int cat, float vjScore) const;

private:
    void Load(FILE *fpModel);
    void Load(__in_ecount(nBufferLength) const BYTE* pbBuffer, const int nBufferLength, int& iStreamPos);
    void Release();

private:
    float m_fastPassTh[CATEGORIES_NUM];   // If vjScore is above this threshold, return AUTO_PASS_SCORE without running the classifier

    Lbp* m_lbpGenerator;
    Poly2Classifier** m_classifiers;

};


}