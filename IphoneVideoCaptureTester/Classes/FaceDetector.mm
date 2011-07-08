//
//  FaceDetector.cpp
//  MyAVController
//
//  Created by Yang Mu on 11-4-23.
//  Copyright 2011年 Tsinghua University. All rights reserved.
//

#include "FaceDetector.h"
#include "vector"
#include <exception>
#import <UIKit/UIKit.h>

int FaceDetector::init() {

	return 0;
}

int FaceDetector::clear() {
    return 0;
}

void FaceDetector::release() {
}

_rect FaceDetector::detect(unsigned char* data, int width, int height) {
    _rect r;
	r.x1 = r.y1 = 50;
	r.x2 = r.y2 = 100;

	return r;
}
