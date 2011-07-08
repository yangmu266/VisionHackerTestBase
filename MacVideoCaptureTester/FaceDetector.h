//
//  FaceDetector.h
//  MyAVController
//
//  Created by Yang Mu on 11-4-23.
//  Copyright 2011年 Tsinghua University. All rights reserved.
//
#ifndef __FACE_DETECTOR__
#define __FACE_DETECTOR__

struct _rect {
    int x1, x2, y1, y2;
};

#define DEFAULT_WIDTH 192
#define DEFAULT_HEIGHT 144


class FaceDetector {
public:
    int init();
    int clear();
    void release(); 
    _rect detect(unsigned char* data, int width, int height);
};
  
#endif