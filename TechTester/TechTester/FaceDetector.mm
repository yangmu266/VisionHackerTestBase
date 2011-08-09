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

	return 0;
}

int FaceDetector::clear() {
    return 0;
}

void FaceDetector::release() {
}

std::vector<struct FDElement> FaceDetector::detect(unsigned char* data, int width, int height) {
    std::vector<struct FDElement> result;
    struct FDElement line;
    line.type = kFDTypeLine;
    line.x1 = line.y1 = 1;
    line.x2 = line.y2 = 150;
    
    struct FDElement point;
    point.type = kFDTypePoint;
    point.x1 = 50;
    point.y1 = 80;
    
    struct FDElement rect;
    rect.type = kFDTypeRect;
    rect.x1 = 50;
    rect.y1 = 30;
    rect.x2 = 60;
    rect.y2 = 90;

    result.push_back(rect);
    result.push_back(line);
    result.push_back(point);
    
    return result;
}
