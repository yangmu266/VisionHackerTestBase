//
//  OpenCVDemo0ViewController.h
//  OpenCVDemo0
//
//  Created by Tang Wenbin on 4/24/11.
//  Copyright 2011 Tsinghua University. All rights reserved.
//




#import <UIKit/UIKit.h>


@interface OpenCVFaceDetector : NSObject {
//    CvHaarClassifierCascade* cascade;
	void* cascade;
}

-(void) loadModel;
-(void) releaseModel;
-(CGRect) faceDetect: (UIImage*) srcImage: (int) width :(int) height: (int) lowQuality;

@end
