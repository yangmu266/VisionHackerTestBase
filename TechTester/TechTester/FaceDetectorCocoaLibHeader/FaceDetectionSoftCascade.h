#pragma once

#include "FaceDetectionBase.h"
#include "winTypes.h"
#include <vector>

#include "Multidetector.h"
using namespace MutilFaceDetector;


class FaceDetectionSoftCascade : public FaceDetectionBase
{
public:
    FaceDetectionSoftCascade();
    ~FaceDetectionSoftCascade();

    HRESULT Init(const char *classifierFile, const char *svmPostFilterFile, bool skipBelowThresh);

    HRESULT Detect(const unsigned char *bytes, int w, int h, std::vector<FaceRect> &faces, int stride = 0, int isIpad = 0);
	
	void Release();

private:
    IN_IMAGE m_iimage;
	I_IMAGE m_iimageM;
    DETECTOR *m_detector;
    bool m_skipBelowThresh;
	
	//self-added
	FaceRect m_currRect;
	float m_currAveVal_LT;
	float m_currAveVal_RT;
	
	//self-added
	unsigned char *bytesA;
	unsigned char *bytesM;
	bool m_bFirst;
};
