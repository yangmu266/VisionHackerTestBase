//
//  FaceDetector.cpp
//  MyAVController
//
//  Created by Yang Mu on 11-4-23.
//  Copyright 2011年 Tsinghua University. All rights reserved.
//

#include "FaceDetector.h"
#include "vector"


int FaceDetector::init() {
    // sample 
    
	initialized = true;
	HRESULT hr = E_FAIL;
	
    m_faceDetection = new(std::nothrow) FaceDetectionSoftCascade();
    if(m_faceDetection == 0) 
		return -1;
	
	NSString *datPath = [[NSBundle mainBundle] pathForResource:@"detector_pruned_rot" ofType:@"dat"];
    NSString *binPath = [[NSBundle mainBundle] pathForResource:@"dummyFile" ofType:@"bin"];
    //	NSString *binPath = [[NSBundle mainBundle] pathForResource:@"ilabs_postfilter_14jun10a" ofType:@"bin"];
    hr = ((FaceDetectionSoftCascade*)m_faceDetection)->Init([datPath cStringUsingEncoding:NSUTF8StringEncoding], [binPath cStringUsingEncoding:NSUTF8StringEncoding], true);
	if(hr != S_OK){
		throw "Detection init failed!";
	}
    //    NSLog(@"%@", datPath);
	
	return 0;
}

int FaceDetector::clear() {
    return 0;
}

void FaceDetector::release() {
    // sample 
    
    initialized = false;
	if (m_faceDetection != NULL) {
		((FaceDetectionSoftCascade*)m_faceDetection)->Release();
		delete m_faceDetection;
		m_faceDetection = NULL;
	}
}

std::vector<struct FDElement> FaceDetector::detect(unsigned char* data, int width, int height) {
    // sample 
    //    NSLog(@"%d, %d", width, height);
	std::vector<FaceRect> rects;
    
    std::vector<struct FDElement> result;
    
    HRESULT ret = m_faceDetection->Detect(data, width, height, rects, 0, 0); 
	if (ret != 0) {
		throw "Detection failed!";
	}
	int size=rects.size();
    
    if (size>0) {
        struct FDElement rect;
        rect.type = kFDTypeRect;
        rect.x1 = rects[0].left;
        rect.y1 = rects[0].top;
        rect.x2 = rects[0].right;
        rect.y2 = rects[0].bottom;
        result.push_back(rect);
    }
    /*
    struct FDElement line;
    line.type = kFDTypeLine;
    line.x1 = line.y1 = 1;
    line.x2 = line.y2 = 150;
    
    struct FDElement point;
    point.type = kFDTypePoint;
    point.x1 = 50;
    point.y1 = 80;
    
    result.push_back(line);
    result.push_back(point);
    */
    return result;
}


//////////////////////////////////////////////////////////////////////////////////////////
// Private
//////////////////////////////////////////////////////////////////////////////////////////

FaceDetector::FaceDetector() {
    initialized = false;
	m_faceDetection = NULL;
}

FaceDetector::~FaceDetector() {
    release();
}
