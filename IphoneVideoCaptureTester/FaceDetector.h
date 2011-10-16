//
//  FaceDetector.h
//  MyAVController
//
//  Created by Yang Mu on 11-4-23.
//  Copyright 2011年 Tsinghua University. All rights reserved.
//
#ifndef __FACE_DETECTOR__
#define __FACE_DETECTOR__

#include <opencv2/opencv.hpp>
#include "vector"

// PIXELS_WIDTH/HEIGHT定义视频流图像宽/高
#define PIXELS_WIDTH 320
#define PIXELS_HEIGHT 240

// 输出图像标识类型，(圆点，直线，矩形），标识于输出图像之上
enum {
    kFDTypePoint = 0,
    kFDTypeLine,
    kFDTypeRect
};

struct FDElement {
    // 当type=kFDTypePoint
    //      绘制圆点
    //      x1, y1: 点坐标
    //
    // 当type=kFDTypeLine
    //      绘制直线
    //      x1, y1: 起点坐标
    //      x2, y2: 终点坐标
    //
    // 当type=kFDTypeRect
    //      绘制矩形
    //      x1, y1: 左下坐标（较小值）
    //      x2, y2: 右上坐标（较大值）
    int type;
    
    int x1, y1;
    int x2, y2;
};

class FaceDetector {
public:
    // 初始化函数，在最初调用
    int init();
    // 当线程暂停时调用
    int clear();
    // 稀构函数，程序结束时调用
    void release(); 
    //    _rect detect(unsigned char* data, int width, int height);
    
    // 传入灰度图*data，宽度为width，高度height。每个像素占1 byte空间。
    // 根据传入图像，返回一个标识数组，在输出图像上标识对应位置。
    // 标识类型目前支持：点，线，矩形，详见struct FDElement
    std::vector<struct FDElement> detect(unsigned char* data, int width, int height);
};

#endif