//
//  FaceDetectorTestController.h
//  MyAVController
//
//  Created by Yang Mu on 11-4-23.
//  Copyright 2011年 Tsinghua University. All rights reserved.
//

#include "FaceDetector.h"


#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

//#import "OpenCVFaceDetector.h"
//#include "ActionDetector.h"
#include <vector>
using std::vector;

//#define LOCAL_TEST 1

@interface FaceDetectorTestController : UIViewController<AVCaptureVideoDataOutputSampleBufferDelegate, UIAccelerometerDelegate> {
	AVCaptureSession *_captureSession;
	UIImageView *_imageView;
	UILabel *_frameCounterLabel;
	
	CGPoint currentPosition;
	
	vector<CGFloat> xHistory;
	vector<CGFloat> yHistory;
}

@property (nonatomic) FaceDetector fd;
//@property (nonatomic, retain) ActionDetector *ad;
@property int* factor;
@property int lowQuality;
@property int useOpenCV;
@property int frameCounter;
@property int nodCounter, shakeCounter;

@property (nonatomic, retain) UIImageView *imageView;
@property (nonatomic, retain) AVCaptureSession *captureSession;
@property (nonatomic, retain) IBOutlet UILabel *frameCounterLabel;
@property (nonatomic, retain) IBOutlet UIButton *qualitySwitchButton;
@property (nonatomic, retain) IBOutlet UIButton *opencvSwitchButton;

@property (nonatomic, retain) IBOutlet UILabel *centerLabel;
@property (nonatomic, retain) IBOutlet UIImageView *curveImageView;

//-(void)localTest;
-(void)initCapture;
//-(IBAction)qualitySwitchButtonDown;
//-(IBAction)opencvSwitchButtonDown;
//@property (nonatomic, retain) OpenCVFaceDetector *opcvfd;

@property int sampleCounter;
@property (nonatomic, retain) NSString *sampleFolderName;
@property int sampleRecorderFrameCounter;
@property (nonatomic, retain) IBOutlet UIButton *sampleButton;
@property (nonatomic, retain) UIAcceleration *sampleAcceleration;
//-(IBAction)sampleButtonDown;

@end
