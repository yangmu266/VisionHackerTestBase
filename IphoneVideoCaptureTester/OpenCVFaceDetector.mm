//
//  OpenCVDemo0ViewController.m
//  OpenCVDemo0
//
//  Created by Tang Wenbin on 4/24/11.
//  Copyright 2011 Tsinghua University. All rights reserved.
//

#import <opencv2/imgproc/imgproc_c.h>
#import <opencv2/objdetect/objdetect.hpp>

#import "OpenCVFaceDetector.h"

@implementation OpenCVFaceDetector


#pragma mark -
#pragma mark OpenCV Support Methods

// NOTE you SHOULD cvReleaseImage() for the return value when end of the code.
- (IplImage *)CreateIplImageFromUIImage:(UIImage *)image {
	CGImageRef imageRef = image.CGImage;
    
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	IplImage *iplimage = cvCreateImage(cvSize(image.size.width, image.size.height), IPL_DEPTH_8U, 4);
	CGContextRef contextRef = CGBitmapContextCreate(iplimage->imageData, iplimage->width, iplimage->height,
													iplimage->depth, iplimage->widthStep,
													colorSpace, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrderDefault);
	CGContextDrawImage(contextRef, CGRectMake(0, 0, image.size.width, image.size.height), imageRef);
	CGContextRelease(contextRef);
	CGColorSpaceRelease(colorSpace);
    
	IplImage *ret = cvCreateImage(cvGetSize(iplimage), IPL_DEPTH_8U, 3);
	cvCvtColor(iplimage, ret, CV_RGBA2BGR);
	cvReleaseImage(&iplimage);
    
	return ret;
}

// NOTE You should convert color mode as RGB before passing to this function
- (UIImage *)UIImageFromIplImage:(IplImage *)image {
	NSLog(@"IplImage (%d, %d) %d bits by %d channels, %d bytes/row %s", image->width, image->height, image->depth, image->nChannels, image->widthStep, image->channelSeq);
    
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	NSData *data = [NSData dataWithBytes:image->imageData length:image->imageSize];
	CGDataProviderRef provider = CGDataProviderCreateWithCFData((CFDataRef)data);
	CGImageRef imageRef = CGImageCreate(image->width, image->height,
										image->depth, image->depth * image->nChannels, image->widthStep,
										colorSpace, kCGImageAlphaNone|kCGBitmapByteOrderDefault,
										provider, NULL, false, kCGRenderingIntentDefault);
	UIImage *ret = [UIImage imageWithCGImage:imageRef];
	CGImageRelease(imageRef);
	CGDataProviderRelease(provider);
	CGColorSpaceRelease(colorSpace);
	return ret;
}

-(void) loadModel
{
    if (!cascade)
    {
        NSString *path = [[NSBundle mainBundle] pathForResource:@"haarcascade_frontalface_default" ofType:@"xml"];
        cascade = (CvHaarClassifierCascade*)cvLoad([path cStringUsingEncoding:NSASCIIStringEncoding], NULL, NULL, NULL);
    }
}

-(void) releaseModel
{
//    cvReleaseHaarClassifierCascade ( &((CvHaarClassifierCascade*)cascade) ) ;
	cvReleaseHaarClassifierCascade((CvHaarClassifierCascade**) &cascade ) ;
    cascade = nil;
}

-(CGRect) faceDetect: (UIImage*)srcImage :(int) width : (int) height :(int)lowQuality
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
	if(srcImage) {
		cvSetErrMode(CV_ErrModeParent);
        
		IplImage *image = [self CreateIplImageFromUIImage:srcImage];
		
		int scale = 2;
		if (lowQuality==0)
			scale = 1;
		
		// Scaling down
		IplImage *small_image = cvCreateImage(cvSize(image->width/scale,image->height/scale), IPL_DEPTH_8U, 3);
		
		if (lowQuality==1)
			cvPyrDown(image, small_image, CV_GAUSSIAN_5x5);
		else 
			small_image = cvCloneImage(image);
		// Load XML
		
		
		CvMemStorage* storage = cvCreateMemStorage(0);
		
		// Detect faces and draw rectangle on them
		CvSeq* faces = cvHaarDetectObjects(small_image,  (CvHaarClassifierCascade*)cascade, storage, 1.2f, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(0,0), cvSize(20, 20));
		cvReleaseImage(&small_image);
		
		// Draw results on the iamge
		for(int i = 0; i < faces->total; i++) {
			NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
			
			// Calc the rect of faces
			CvRect cvrect = *(CvRect*)cvGetSeqElem(faces, i);
			CGRect face_rect = CGRectMake(cvrect.x * scale, cvrect.y * scale, cvrect.width * scale, cvrect.height * scale);
			return face_rect;
			
			[pool release];
		}
		
		cvReleaseMemStorage(&storage);
		
        
	}
    
	[pool release];
    return CGRectMake(0, 0, 0, 0);
}

- (void)dealloc
{
    [self releaseModel];
    [super dealloc];
}

@end
