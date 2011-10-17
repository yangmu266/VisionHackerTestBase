//
//  FaceDetectorTestController.m
//  MyAVController
//
//  Created by Yang Mu on 11-4-23.
//  Copyright 2011年 Tsinghua University. All rights reserved.
//


#include <iostream>
#include <cmath>

#import "FaceDetectorTestController.h"


@implementation FaceDetectorTestController

@synthesize captureSession = _captureSession;
@synthesize imageView = _imageView;
@synthesize frameCounterLabel = _frameCounterLabel;
//@synthesize opcvfd;
@synthesize opencvSwitchButton;
@synthesize qualitySwitchButton;
@synthesize sampleButton;

@synthesize centerLabel;
@synthesize curveImageView;

@synthesize fd;
//@synthesize ad;
@synthesize factor;
@synthesize lowQuality;
@synthesize useOpenCV;

@synthesize frameCounter;
@synthesize nodCounter;
@synthesize shakeCounter;

@synthesize sampleCounter;
@synthesize sampleFolderName;
@synthesize sampleRecorderFrameCounter;
@synthesize sampleAcceleration;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // local test
        
        self.imageView = [[UIImageView alloc] init];
        self.imageView.frame = CGRectMake(0, 0, 144, 192);
        [self.view addSubview:self.imageView];
		lowQuality = 0;
		useOpenCV = 0;
		sampleRecorderFrameCounter = 0;
//		opcvfd = [[OpenCVFaceDetector alloc] init];
//		ad = [[ActionDetector alloc] initWithFaceRect:192/3 height:144/3];
//		[opcvfd loadModel];
		frameCounter = 0;
		sampleCounter = 0;
		nodCounter = shakeCounter = 0;
    }
    return self;
}

- (void)dealloc
{
	fd.release();
//	[opcvfd releaseModel];
	
	if (factor)
	{
		free(factor);
		factor = NULL;
	}
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
	
	fd.init();

//	faceBounds.x1=faceBounds.x2=faceBounds.y1=faceBounds.y2=0;
//	faceBounds.x2=faceBounds.y2=100;
//	[NSTimer scheduledTimerWithTimeInterval:0.03 target:self selector:@selector(draw) userInfo:nil repeats:YES];
	
#ifdef LOCAL_TEST
	[self localTest];
#endif
	
#ifndef LOCAL_TEST
    [self initCapture];
#endif
	UIAccelerometer *acc = [UIAccelerometer sharedAccelerometer];
    acc.delegate = self;
    acc.updateInterval = 1.0f / 60.0f;
	
	// Rotate Label
	
	CGAffineTransform transform = self.centerLabel.transform;
	transform = CGAffineTransformRotate(transform, - M_PI / 2.0f);
	self.centerLabel.transform = transform;
	 
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)initCapture {
	/*We setup the input*/
    NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
	AVCaptureDeviceInput *captureInput = [AVCaptureDeviceInput 
										  deviceInputWithDevice:[devices lastObject]
										  error:nil];
    
	/*We setupt the output*/
	AVCaptureVideoDataOutput *captureOutput = [[AVCaptureVideoDataOutput alloc] init];
	/*While a frame is processes in -captureOutput:didOutputSampleBuffer:fromConnection: delegate methods no other frames are added in the queue.
	 If you don't want this behaviour set the property to NO */
	captureOutput.alwaysDiscardsLateVideoFrames = YES; 
	/*We specify a minimum duration for each frame (play with this settings to avoid having too many frames waiting
	 in the queue because it can cause memory issues). It is similar to the inverse of the maximum framerate.
	 In this example we set a min frame duration of 1/10 seconds so a maximum framerate of 10fps. We say that
	 we are not able to process more than 10 frames per second.*/
	//captureOutput.minFrameDuration = CMTimeMake(1, 10);
	
	/*We create a serial queue to handle the processing of our frames*/
	dispatch_queue_t queue;
	queue = dispatch_queue_create("cameraQueue", NULL);
	[captureOutput setSampleBufferDelegate:self queue:queue];
	dispatch_release(queue);
	// Set the video output to store frame in BGRA (It is supposed to be faster)
	NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey; 
	NSNumber* value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA]; 
	NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:value forKey:key]; 
	[captureOutput setVideoSettings:videoSettings]; 
	/*And we create a capture session*/
	self.captureSession = [[AVCaptureSession alloc] init];
	/*We add input and output*/
    [self.captureSession setSessionPreset:AVCaptureSessionPresetLow];
	[self.captureSession addInput:captureInput];
	[self.captureSession addOutput:captureOutput];
	/*We add the Custom Layer (We need to change the orientation of the layer so that the video is displayed correctly)*/
	/*We add the imageView*/
	self.imageView = [[UIImageView alloc] init];
	self.imageView.frame = CGRectMake(0, 0, 144, 192);
    [self.view addSubview:self.imageView];
	/*We start the capture*/
	[self.captureSession startRunning];
	
}

#pragma mark -
#pragma mark AVCaptureSession delegate
- (void)captureOutput:(AVCaptureOutput *)captureOutput 
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer 
	   fromConnection:(AVCaptureConnection *)connection 
{ 
	/*We create an autorelease pool because as we are not in the main_queue our code is
	 not executed in the main thread. So we have to create an autorelease pool for the thread we are in*/
	
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer); 
    /*Lock the image buffer*/
    CVPixelBufferLockBaseAddress(imageBuffer,0); 
    /*Get information about the image*/
    uint32_t *baseAddress = (uint32_t *)CVPixelBufferGetBaseAddress(imageBuffer);
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    
    /*Create a CGImageRef from the CVImageBufferRef*/
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB(); 
    CGContextRef newContext = CGBitmapContextCreate(baseAddress, width, height, 8, bytesPerRow, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
    CGImageRef newImage = CGBitmapContextCreateImage(newContext); 
	
    /*We release some components*/
    CGContextRelease(newContext); 
    CGColorSpaceRelease(colorSpace);
    
    
    
    
    // now convert to grayscaleoen
    uint8_t *m_imageData = (uint8_t *) malloc(width * height);
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            uint32_t offset=y*width+x;
            uint32_t rgbPixel= baseAddress[offset/*(height-y-1)*width+x*/];
            m_imageData[offset] = (((rgbPixel>>24)&255)+((rgbPixel>>16)&255)+((rgbPixel>>8)&255))/3;
        }
    }
	
	
	if ((lowQuality == 1)&&(!useOpenCV)) {
		uint8_t* newData = (uint8_t*)malloc(width*height/4);
		for (int y=0; y<height/2; y++) 
			for (int x=0; x<width/2; x++) {
				int sum = 0;
				int div = 0;
				for (int i=0; i<3; i++) 
					for (int j=0; j<3; j++) {
						if ((y*2+i-1>=0)&& (y*2+i-1<height)&&(x*2+j-1>=0)&&(x*2+j-1<width)) {
							sum += ((int)m_imageData[(y*2+i-1)*width+x*2+j-1])*factor[i*3+j];
							div += factor[i*3+j];
						}
					}
				sum /= div;
				newData[y*width/2+x] = (uint8_t)m_imageData[y*width*2+x*2];
//				newData[x*height/2+y] = (m_imageData[y*2*width+x*2]+m_imageData[y*2*width+x*2+1]+
//									 m_imageData[(y*2+1)*width+x*2]+m_imageData[(y*2+1)*width+x*2+1])/4;
			}
//		int temp = width; width = height; height = temp;
		width/=2;height/=2;
		free(m_imageData);
		m_imageData = newData;
	} else {
//		uint8_t* newData = (uint8_t*)malloc(width*height);
//		for (int y=0; y<height; y++) 
//			for (int x=0; x<width; x++) {
//				newData[x*height+y] = m_imageData[y*width+x];
//			}
//		int temp = width; width = height; height = temp;
//		free(m_imageData);
//		m_imageData = newData;	
	}
    // convert from a gray scale image back into a UIImage
    uint8_t *result = (uint8_t *) calloc(width * height *sizeof(uint32_t), 1);
	
    // process the image back to rgb
    for(int i = 0; i < height * width; i++) {
        result[i*4]=0;
        int val=m_imageData[i];
        result[i*4+1]=val;
        result[i*4+2]=val;
        result[i*4+3]=val;
    }
	
	CGColorSpaceRef newColorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(result, width, height, 8, width*sizeof(uint32_t), newColorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipLast);
    
	std::vector<struct FDElement> elements = fd.detect(m_imageData, width, height);
//	_rect r;
	for (int i=0; i<elements.size(); i++) {
		switch (elements[i].type) {
            case kFDTypeLine:
                CGContextBeginPath (context);
                CGContextMoveToPoint(context, elements[i].x1, elements[i].y1);
                CGContextAddLineToPoint(context, elements[i].x2, elements[i].y2);
                CGContextStrokePath(context);
                break;
            case kFDTypePoint:
                CGContextBeginPath (context);
                CGContextAddArc(context, elements[i].x1, elements[i].y1, 3, 0, -0.01, 0);
                CGContextStrokePath(context);
                break;
			case kFDTypeRect:
				CGRect face_rect = CGContextConvertRectToDeviceSpace(context, CGRectMake(elements[i].x1, elements[i].y1, elements[i].x2-elements[i].x1, elements[i].y2-elements[i].y1));
				CGContextStrokeRect(context, face_rect);
				break;
		}
	}
    
    // create a UIImage
    CGImageRef srcImage = CGBitmapContextCreateImage(context);
    CGContextRelease(context);
    CGColorSpaceRelease(newColorSpace);
    UIImage *grayImage = [UIImage imageWithCGImage:srcImage scale:1.0 orientation:UIImageOrientationLeftMirrored];
    CGImageRelease(srcImage);
	
    // make sure the data will be released by giving it to an autoreleased NSData
	NSData *resData=[NSData dataWithBytesNoCopy:m_imageData length:width*height];
	free(result);
//	[NSData dataWithBytesNoCopy:result length:width * height];
	    
	/*We relase the CGImageRef*/
	CGImageRelease(newImage);
	[self.imageView performSelectorOnMainThread:@selector(setImage:) withObject:grayImage waitUntilDone:YES];
//	[self.frameCounterLabel performSelectorOnMainThread : @ selector(setText : ) withObject:[NSString stringWithFormat:@"frameCount=%d, isOpenCV=%d, isLowQuality=%d", frameCounter++, useOpenCV, lowQuality] waitUntilDone:YES];
	
	/*We unlock the  image buffer*/
	CVPixelBufferUnlockBaseAddress(imageBuffer,0);
	
	[pool drain];
} 

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
	@synchronized(self) {
		if (sampleAcceleration)
			[sampleAcceleration release];
		sampleAcceleration = [acceleration retain];
	}
}

@end
