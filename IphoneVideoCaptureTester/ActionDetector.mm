//
//  ActionDetector.m
//  MyAVController
//
//  Created by Yang Mu on 11-5-1.
//  Copyright 2011年 Tsinghua University. All rights reserved.
//

#import "ActionDetector.h"


@implementation ActionDetector

@synthesize xlist=_xlist;
@synthesize ylist=_ylist;

-(id)initWithFaceRect: (int)width height:(int)height {
	self = [super init];
	if (self) {
		self.xlist = [NSMutableArray arrayWithCapacity: 100];
		self.ylist = [NSMutableArray arrayWithCapacity: 100];
		frameCounter = 0;
		actionFinishTime = 0;
		lastActionType = 0;
		faceRectWidth = width;
		faceRectHeight = height;
	}
	return self;
}

-(kHeadAction) add: (float)x y: (float)y {
	NSLog(@"add: x=%.3f, y=%.3f, frameCounter=%d, actionFinishTime=%d", x, y, frameCounter, actionFinishTime);
	//	float delta_x = 0, delta_y = 0;
	[self.xlist insertObject: [[NSNumber alloc] initWithFloat:x] atIndex:0];
	[self.ylist insertObject: [[NSNumber alloc] initWithFloat:y] atIndex:0];
	frameCounter++;
	if (frameCounter > ACTION_DURATION*2) {
		[self.xlist removeLastObject];
		[self.ylist removeLastObject];
	}
	if ((frameCounter < ACTION_DURATION+1) || (frameCounter - actionFinishTime < ACTION_DURATION/2)) {
		return kHeadActionNone;
	}
	
	kHeadAction result = kHeadActionNone;
	
	// shake
    float max_x, min_x, lastIndex = 0;
    max_x = min_x = [(NSNumber*)[self.xlist objectAtIndex:0] floatValue];
    for (int i=MIN(ACTION_DURATION-1, frameCounter-actionFinishTime); i>=0; i--) {
        float pos = [(NSNumber*)[self.xlist objectAtIndex:i] floatValue];
		if (pos > max_x) {
			lastIndex = i;
			max_x = MAX(pos, max_x);
		}
		if (pos < min_x) {
			lastIndex = i;
			min_x = MIN(pos, min_x);
		}
    }
    if (max_x - min_x > faceRectWidth/2) {
		NSLog(@"shake, max_x=%.3f, min_x=%.3f, base=%.3f", max_x, min_x, faceRectWidth/2);
        actionFinishTime = frameCounter-lastIndex;
		result = kHeadActionShake;
    }
	
    // nod
    float max_y, min_y;
	lastIndex = 0;
    max_y = min_y = [(NSNumber*)[self.ylist objectAtIndex:0] floatValue];
    for (int i=MIN(ACTION_DURATION-1, frameCounter-actionFinishTime); i>=0; i--) {
        float pos = [(NSNumber*)[self.ylist objectAtIndex:i] floatValue];
		if (pos > max_y) {
			lastIndex = i;
			max_y = MAX(pos, max_y);
		}
		if (pos < min_y) {
			lastIndex = i;
			min_y = MIN(pos, min_y);
		}
    }
    if (max_y - min_y > faceRectHeight/3) {
		if (((result != kHeadActionNone) && ((max_y-min_y)*1.5 > max_x - min_x)) || (result==kHeadActionNone)) {
			NSLog(@"nod, max_y=%.3f, min_y=%.3f, base=%.3f", max_y, min_y, faceRectHeight/3);
			actionFinishTime = frameCounter-lastIndex;
			result = kHeadActionNod;
		}
    }
	/*
	 // pattern left->right->left
	 sgn = 1;
	 delta_x = 0;
	 for (int i=0 ;i<ACTION_DURATION; i++) {
	 NSLog(@"%d: %.3f %.3f", i, [(NSNumber*)[self.xlist objectAtIndex:i] floatValue], [(NSNumber*)[self.xlist objectAtIndex:i+1] floatValue]);
	 float delta = [(NSNumber*)[self.xlist objectAtIndex:i] floatValue] - [(NSNumber*)[self.xlist objectAtIndex:i+1] floatValue];
	 int sign = (int)(delta / delta);
	 if (sign*sgn < 0) {
	 if (delta_x < faceRectWidth/2) {
	 NSLog(@"delta=%.3f, sgn=%d, x2=%.3f", delta, sign, delta_x);
	 isNod = false;
	 break;
	 }
	 sgn = sign;
	 if (sgn > 0)
	 break;
	 delta_x = 0;
	 }
	 delta_x += (delta>0)?delta:-delta;
	 }
	 if ((isNod) && (lastActionType != 1)) {
	 lastActionType = 1;
	 return kHeadActionNod;
	 }
	 
	 // pattern right->left->right
	 sgn = -1;
	 delta_x = 0;
	 for (int i=0 ;i<ACTION_DURATION; i++) {
	 float delta = [(NSNumber*)[self.xlist objectAtIndex:i] floatValue] - [(NSNumber*)[self.xlist objectAtIndex:i+1] floatValue];
	 int sign = (int)(delta / delta);
	 if (sign*sgn < 0) {
	 if (delta_x < faceRectWidth/2) {
	 NSLog(@"delta=%.3f, sgn=%d, x2=%.3f", delta, sign, delta_x);
	 isNod = false;
	 break;
	 }
	 sgn = sign;
	 if (sgn < 0)
	 break;
	 delta_x = 0;
	 }
	 delta_x += (delta>0)?delta:-delta;
	 }
	 if ((isNod) && (lastActionType != 2)) {
	 lastActionType = 2;
	 return kHeadActionNod;
	 }*/
	
	//	NSLog(@"total_delta_x=%.3f, delta_x=%.3f", delta_x, ([(NSNumber*)[self.xlist objectAtIndex:0] floatValue] - [(NSNumber*)[self.xlist objectAtIndex:ACTION_DURATION] floatValue]));
	//	if ((delta_x > faceRectWidth)&&
	//		(delta_x > NOD_ACTION_DELTA_RATE*abs([(NSNumber*)[self.xlist objectAtIndex:0] floatValue] - [(NSNumber*)[self.xlist objectAtIndex:ACTION_DURATION] floatValue]))) {
	//		actionFinishTime = frameCounter;
	//		return kHeadActionShake;		
	//	}
	
	// shake
	/*	for (int i=0 ;i<ACTION_DURATION; i++) {
	 float delta = [(NSNumber*)[self.ylist objectAtIndex:i] floatValue] - [(NSNumber*)[self.ylist objectAtIndex:i+1] floatValue];
	 delta_y += (delta>0)?delta:-delta;
	 }
	 NSLog(@"total_delta_y=%.3f, delta_y=%.3f", delta_y, ([(NSNumber*)[self.ylist objectAtIndex:0] floatValue] - [(NSNumber*)[self.ylist objectAtIndex:ACTION_DURATION] floatValue]));
	 if ((delta_y > faceRectHeight)&&
	 (delta_y > SHAKE_ACTION_DELTA_RATE*abs([(NSNumber*)[self.ylist objectAtIndex:0] floatValue] - [(NSNumber*)[self.ylist objectAtIndex:ACTION_DURATION] floatValue]))) {
	 actionFinishTime = frameCounter;
	 return kHeadActionShake;		
	 }*/
	
	return result;
}

@end
