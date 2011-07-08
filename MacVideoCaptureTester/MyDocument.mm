//
//  MyDocument.m
//  StillMotion
//

/*
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by 
 Apple Inc. ("Apple") in consideration of your agreement to the
 following terms, and your use, installation, modification or
 redistribution of this Apple software constitutes acceptance of these
 terms.  If you do not agree with these terms, please do not use,
 install, modify or redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software. 
 Neither the name, trademarks, service marks or logos of Apple Inc. 
 may be used to endorse or promote products derived from the Apple
 Software without specific prior written permission from Apple.  Except
 as expressly stated in this notice, no other rights or licenses, express
 or implied, are granted by Apple herein, including but not limited to
 any patent rights that may be infringed by your derivative works or by
 other works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2007 Apple Inc. All Rights Reserved.
 
 */

#import "MyDocument.h"
#import "FaceDetector.h"

@implementation MyDocument

@synthesize frameRateLabel = mTextField;
@synthesize output = mImageView;
@synthesize date = mDate;


- (NSString *)windowNibName
{
    return @"MyDocument";
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{
    fd.init();
    frameCounter = 0;
    busy = false;
    self.date = [[NSDate alloc] init];
    NSError *error = nil;
    
    [super windowControllerDidLoadNib:aController];
    
    //[[aController window] setDelegate:self];
        
    if (!mCaptureSession) {
        // Set up a capture session that outputs raw frames
        BOOL success;
        
        mCaptureSession = [[QTCaptureSession alloc] init];
        
        // Find a video device
        QTCaptureDevice *device = [QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeVideo];
        success = [device open:&error];
        if (!success) {
            [[NSAlert alertWithError:error] runModal];
            return;
        }
        
        // Add a device input for that device to the capture session
        mCaptureDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:device];
        success = [mCaptureSession addInput:mCaptureDeviceInput error:&error];
        if (!success) {
            [[NSAlert alertWithError:error] runModal];
            return;
        }
        
        // Add a decompressed video output that returns raw frames to the session
        mCaptureDecompressedVideoOutput = [[QTCaptureDecompressedVideoOutput alloc] init];
        [mCaptureDecompressedVideoOutput setDelegate:self];
        success = [mCaptureSession addOutput:mCaptureDecompressedVideoOutput error:&error];
        if (!success) {
            [[NSAlert alertWithError:error] runModal];
            return;
        }
        
        // Preview the video from the session in the document window
        [mCaptureView setCaptureSession:mCaptureSession];
        
        startTime = [NSDate date];
        
        // Start the session
        [mCaptureSession startRunning];
    }
}

- (void)windowWillClose:(NSNotification *)notification
{
    [mCaptureSession stopRunning];
    
    QTCaptureDevice *device = [mCaptureDeviceInput device];
    if ([device isOpen])
        [device close];    
}

- (void)dealloc
{    
    [mCaptureSession release];
    [mCaptureDeviceInput release];
    [mCaptureDecompressedVideoOutput release];
    
    if (startTime != nil)
        [startTime release];
    [super dealloc];
}

// This delegate method is called whenever the QTCaptureDecompressedVideoOutput receives a frame
- (void)captureOutput:(QTCaptureOutput *)captureOutput didOutputVideoFrame:(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:(QTCaptureConnection *)connection
{
    // Store the latest frame
	// This must be done in a @synchronized block because this delegate method is not called on the main thread
    CVImageBufferRef imageBufferToRelease;
    
    CVBufferRetain(videoFrame);
    
    @synchronized (self) {
        imageBufferToRelease = mCurrentImageBuffer;
        mCurrentImageBuffer = videoFrame;
    }
    
    CVBufferRelease(imageBufferToRelease);
}

NSImage* nsImageFromCGImageRef(CGImageRef image)
{
    NSRect imageRect = NSMakeRect(0.0, 0.0, 0.0, 0.0);
    CGContextRef imageContext = nil;
    NSImage* newImage = nil;
    // Get the image dimensions.
    imageRect.size.height = CGImageGetHeight(image);
    imageRect.size.width = CGImageGetWidth(image);
    // Create a new image to receive the Quartz image data.
	if(imageRect.size.height ==0 ||imageRect.size.width ==0)
	{
		NSLog(@"nsImageFromCGImageRef should not be here!!!,CGImageRef is %@",image);
	}
    newImage = [[[NSImage alloc] initWithSize:imageRect.size] autorelease];
    [newImage lockFocus];
    // Get the Quartz context and draw.
    imageContext = (CGContextRef)[[NSGraphicsContext currentContext]
								  graphicsPort];
    CGContextDrawImage(imageContext, *(CGRect*)&imageRect, image);
    [newImage unlockFocus];
    return newImage;
}

CGImageRef CGImageRefCopyFromNSImage(NSImage* image)
{
    NSData * imageData = [image TIFFRepresentation];	
    CGImageRef imageRef = nil;	
    if(imageData)		
    {		
        CGImageSourceRef imageSource = 	CGImageSourceCreateWithData((CFDataRef)imageData,  NULL);	 //need release	
        imageRef = CGImageSourceCreateImageAtIndex( imageSource, 0, NULL);   //need release by caller
		
		CFRelease(imageSource);
    }
	
    return imageRef;	
}

-(NSImage*) GrayImageFromNSImage: (NSImage*)image
{
    if(nil == image)
        return nil;
    BOOL enableAlaph = NO;
    
    CGColorSpaceRef colorspace = NULL;
    CGContextRef context = NULL;
    
    colorspace = CGColorSpaceCreateWithName(enableAlaph ?
                                            kCGColorSpaceGenericRGB : kCGColorSpaceGenericGray);  //need release
    //colorspace = CGColorSpaceCreateDeviceGray();
    
    unsigned char *bitmapData;
    int bitmapByteCount;
    int bitmapBytesPerRow;
    CGImageRef inImage = CGImageRefCopyFromNSImage(image);  //need release
    
    size_t pixelsWide = CGImageGetWidth(inImage);
    size_t pixelsHigh = CGImageGetHeight(inImage);
    
    bitmapBytesPerRow = pixelsWide * (enableAlaph ? 4 : 1);
    bitmapByteCount = bitmapBytesPerRow * pixelsHigh;
    //LOGDEBUG("bitmapdata is %d", bitmapByteCount);
    bitmapData = (unsigned char *)malloc(bitmapByteCount);  //need free
    context = CGBitmapContextCreate(bitmapData,
                                    pixelsWide,
                                    pixelsHigh,
                                    8,
                                    bitmapBytesPerRow,
                                    colorspace,
                                    (enableAlaph ?
                                     kCGImageAlphaPremultipliedLast : kCGImageAlphaNone));  //need release
    
    
    CGRect rect = {{0, 0}, {pixelsWide, pixelsHigh}};
    CGContextDrawImage(context, rect, inImage);
    CGImageRelease(inImage);
    if(enableAlaph)
    {
        int i,j;
        for(i = 0; i < bitmapBytesPerRow; i += 4)
            for(j = 0; j < pixelsHigh; j++)
            {
//                int grey = (int)(bitmapData[j * bitmapBytesPerRow + i]*0.299
//                                 + bitmapData[j * bitmapBytesPerRow + i+1]*0.587 + bitmapData[j *
//                                                                                              bitmapBytesPerRow + i+2]*0.114);
                int grey = (int)((bitmapData[j*bitmapBytesPerRow + i] + bitmapData[j*bitmapBytesPerRow + i+1] +
                                 bitmapData[j*bitmapBytesPerRow + i+2] + bitmapData[j*bitmapBytesPerRow + i+3])/4);
                
                bitmapData[j * bitmapBytesPerRow + i] = bitmapData[j *
                                                                   bitmapBytesPerRow + i + 1] = bitmapData[j * bitmapBytesPerRow + i + 2] =
                grey;
            }
    }    

    _rect r = fd.detect(bitmapData, pixelsWide, pixelsHigh);
    
    NSLog(@"%d, %d, %d, %d", r.x1, r.y1, r.x2, r.y2);
    CGRect face_rect = CGContextConvertRectToDeviceSpace(context, CGRectMake(r.x1, r.y1, r.x2-r.x1, r.y2-r.y1));
    CGContextSetLineWidth(context, 5);
    CGContextStrokeRect(context, face_rect);

    CGImageRef test = CGBitmapContextCreateImage(context) ;  //need release
    free(bitmapData);
    //LOGDEBUG("test is %s", test == nil ? "nil" : "full");
    
    NSImage * img = nsImageFromCGImageRef(test);
    
    CGColorSpaceRelease(colorspace);
    CGContextRelease(context);
    CGImageRelease(test);
    return img;
}

- (IBAction)addFrame:(id)sender
{
    if (frameCounter == 0)
        startTime = [[NSDate alloc] init];
    [[self frameRateLabel] setStringValue: [NSString stringWithFormat:@"fps=%.3f", (++frameCounter)/([[[[NSDate alloc] init] autorelease] timeIntervalSinceDate:startTime])]];
//    [[self frameRateLabel] setStringValue: [NSString stringWithFormat:@"fps=%.3f", [[NSDate date] timeIntervalSinceDate: startTime]]];
    // Get the most recent frame
	// This must be done in a @synchronized block because the delegate method that sets the most recent frame is not called on the main thread
    CVImageBufferRef imageBuffer;
    
    @synchronized (self) {
        imageBuffer = CVBufferRetain(mCurrentImageBuffer);
    }
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
    /*Lock the image buffer*/
    CVPixelBufferLockBaseAddress(imageBuffer,0); 

    NSCIImageRep *imageRep = [NSCIImageRep imageRepWithCIImage:[CIImage imageWithCVImageBuffer:imageBuffer]];
    NSImage *image = [[[NSImage alloc] initWithSize:[imageRep size]] autorelease];
    [image addRepresentation:imageRep];
    [[self output] performSelectorOnMainThread:@selector(setImage:) withObject:[self GrayImageFromNSImage:image] waitUntilDone:true];
    
	/*We unlock the  image buffer*/
	CVPixelBufferUnlockBaseAddress(imageBuffer,0);
	
	[pool drain];
    [self performSelectorOnMainThread:@selector(addFrame:) withObject:nil waitUntilDone:false];
}

@end
