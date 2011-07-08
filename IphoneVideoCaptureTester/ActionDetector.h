//
//  ActionDetector.h
//  MyAVController
//
//  Created by Yang Mu on 11-5-1.
//  Copyright 2011年 Tsinghua University. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "FaceDetector.h"

enum kHeadAction{
	kHeadActionNone = 2,
	kHeadActionShake = 1,
	kHeadActionNod = 0
};

#define ACTION_DURATION 18
#define NOD_ACTION_DELTA_RATE 5
#define SHAKE_ACTION_DELTA_RATE 5

@interface ActionDetector : NSObject {
    NSMutableArray *_xlist;
	NSMutableArray *_ylist;

	int frameCounter;
	int actionFinishTime;
	int lastActionType;
	float faceRectHeight, faceRectWidth;
}
 
-(id)initWithFaceRect: (int)width height:(int)height;
-(kHeadAction) add: (float)x y: (float)y; 

@property (nonatomic, retain) NSMutableArray* xlist;
@property (nonatomic, retain) NSMutableArray* ylist;

@end
