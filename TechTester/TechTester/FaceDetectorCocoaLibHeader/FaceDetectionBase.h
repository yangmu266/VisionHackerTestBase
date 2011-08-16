#pragma once

#include <vector>
#include "winTypes.h"

class FaceRect
{
public:
    int left, top;
    int right, bottom;
    int m_cat; 
    float m_fPoseScore;

    HRESULT Save(FILE *fp);
    HRESULT Load(FILE *fp);
};

class FaceDetectionBase
{
public:
    FaceDetectionBase();
    virtual ~FaceDetectionBase() = 0;
    
    virtual HRESULT Detect(const unsigned char *image, int w, int h, 
                           std::vector<FaceRect> &faces, int isIpad = 0, int stride = 0) = 0;
};