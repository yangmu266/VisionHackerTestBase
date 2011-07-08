#import "MyAVController.h"


@implementation MyAVController

@synthesize captureSession = _captureSession;
@synthesize imageView = _imageView;
@synthesize customLayer = _customLayer;
@synthesize prevLayer = _prevLayer;

#pragma mark -
#pragma mark Initialization
- (id)init {
	self = [super init];
	if (self) {
		/*We initialize some variables (they might be not initialized depending on what is commented or not)*/
		self.imageView = nil;
		self.prevLayer = nil;
		self.customLayer = nil;
	}
	return self;
}

- (void)viewDidLoad {
	/*We intialize the capture*/
	[self initCapture];
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
	self.customLayer = [CALayer layer];
	self.customLayer.frame = self.view.bounds;
	self.customLayer.transform = CATransform3DRotate(CATransform3DIdentity, M_PI/2.0f, 0, 0, 1);
	self.customLayer.contentsGravity = kCAGravityResizeAspectFill;
	[self.view.layer addSublayer:self.customLayer];
	/*We add the imageView*/
	self.imageView = [[UIImageView alloc] init];
	self.imageView.frame = CGRectMake(0, 0, 320, 240);
	 [self.view addSubview:self.imageView];
	/*We add the preview layer*/
	self.prevLayer = [AVCaptureVideoPreviewLayer layerWithSession: self.captureSession];
	self.prevLayer.frame = CGRectMake(0, 240, 100, 100);
	self.prevLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
	[self.view.layer addSublayer: self.prevLayer];
	/*We start the capture*/
    frameCounter = 0;
	[self.captureSession startRunning];
	
}

- (UIImage *) convertToGreyscale:(UIImage *)i :(uint32_t*)rgbImage{    
    int kRed = 1;
    int kGreen = 2;
    int kBlue = 4;
    
    int colors = kGreen;
    int m_width = i.size.width;
    int m_height = i.size.height;
    
/*    uint32_t *rgbImage = (uint32_t *) malloc(m_width * m_height * sizeof(uint32_t));
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(rgbImage, m_width, m_height, 8, m_width * 4, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipLast);
    CGContextSetInterpolationQuality(context, kCGInterpolationHigh);
    CGContextSetShouldAntialias(context, NO);
    CGContextDrawImage(context, CGRectMake(0, 0, m_width, m_height), [i CGImage]);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);*/
    
    // now convert to grayscale
    uint8_t *m_imageData = (uint8_t *) malloc(m_width * m_height);
    for(int y = 0; y < m_height; y++) {
        for(int x = 0; x < m_width; x++) {
            uint32_t rgbPixel=rgbImage[y*m_width+x];
            uint32_t sum=0,count=0;
            if (colors & kRed) {sum += (rgbPixel>>24)&255; count++;}
            if (colors & kGreen) {sum += (rgbPixel>>16)&255; count++;}
            if (colors & kBlue) {sum += (rgbPixel>>8)&255; count++;}
            m_imageData[y*m_width+x]=sum/count;
        }
    }
//    free(rgbImage);
    
    // convert from a gray scale image back into a UIImage
    uint8_t *result = (uint8_t *) calloc(m_width * m_height *sizeof(uint32_t), 1);
    
    // process the image back to rgb
    for(int i = 0; i < m_height * m_width; i++) {
        result[i*4]=0;
        int val=m_imageData[i];
        result[i*4+1]=val;
        result[i*4+2]=val;
        result[i*4+3]=val;
    }
    
    // create a UIImage
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(result, m_width, m_height, 8, m_width * sizeof(uint32_t), colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipLast);
    CGImageRef image = CGBitmapContextCreateImage(context);
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    UIImage *resultUIImage = [UIImage imageWithCGImage:image];
    CGImageRelease(image);
    
    // make sure the data will be released by giving it to an autoreleased NSData
    [NSData dataWithBytesNoCopy:result length:m_width * m_height];
    
    return resultUIImage;
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
    
    
    
    
    
/*    
    int kRed = 1;
    int kGreen = 2;
    int kBlue = 4;
    
    int colors = kGreen;
  */  
    NSLog(@"width=%lu height=%lu", width, height);
    frameCounter++;
//    FILE* raw_byte_out;
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectoryPath = [paths objectAtIndex:0];
    NSString* path;
    if (frameCounter < 5)
        path = [documentsDirectoryPath stringByAppendingPathComponent: [NSString stringWithFormat:@"some%d.txt", frameCounter]];     
    else
        path = [documentsDirectoryPath stringByAppendingPathComponent:@"_some.txt"];     
    NSLog(@"path=%@", path);
    
//    NSString * path = [[NSBundle mainBundle] pathForResource:  @"some" ofType: @"txt"];
    FILE *raw_byte_out = fopen([path cStringUsingEncoding:1],"w");
//    if (f == NULL) NSLog([path stringByAppendingString:@" not found"]);    
//    if (frameCounter == 1)
//        raw_byte_out = fopen("frame.txt", "w");
//    else
//        raw_byte_out = fopen("frame2.txt", "w");
    // now convert to grayscaleoen
    uint8_t *m_imageData = (uint8_t *) malloc(width * height);
    int line = 0;
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            uint32_t offset=y*width+x;
            uint32_t rgbPixel= baseAddress[offset];
//            uint32_t sum=0,count=0;
/*            if (colors & kRed) {sum += (rgbPixel>>24)&255; count++;}
            if (colors & kGreen) {sum += (rgbPixel>>16)&255; count++;}
            if (colors & kBlue) {sum += (rgbPixel>>8)&255; count++;}
            m_imageData[y*width+x]=sum/count;*/
            m_imageData[offset] = (((rgbPixel>>24)&255)+((rgbPixel>>16)&255)+((rgbPixel>>8)&255))/3;
            fprintf(raw_byte_out, "%u ", m_imageData[offset]);
        }
        fprintf(raw_byte_out, "\n");
        fflush(raw_byte_out);
        line++;
    }
    NSLog(@"#line=%d", line);
    fclose(raw_byte_out);
//    free(baseAddress);
    
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
    
    // create a UIImage
    CGColorSpaceRef newColorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(result, width, height, 8, width*sizeof(uint32_t), newColorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipLast);
    CGImageRef srcImage = CGBitmapContextCreateImage(context);
    CGContextRelease(context);
    CGColorSpaceRelease(newColorSpace);
    UIImage *grayImage = [UIImage imageWithCGImage:srcImage];
    CGImageRelease(srcImage);
    
    // make sure the data will be released by giving it to an autoreleased NSData
    [NSData dataWithBytesNoCopy:result length:width * height];
    free(m_imageData);
    
    
    CGImageRef myImage = srcImage;
    NSAutoreleasePool *newpool = [[NSAutoreleasePool alloc] init];
    UIImage *img = [UIImage imageWithCGImage:myImage];
    NSData *pngData = UIImagePNGRepresentation(img);
    if (frameCounter<5)
        path = [documentsDirectoryPath stringByAppendingPathComponent:[NSString stringWithFormat:@"some%d.png", frameCounter]];
    else
        path = [documentsDirectoryPath stringByAppendingPathComponent:@"_some.png"];
    [pngData writeToFile: path atomically:YES];
    [newpool drain];
    
    
    
    
    
    
    
    
    
    
    
    
    /*We display the result on the custom layer. All the display stuff must be done in the main thread because
	 UIKit is no thread safe, and as we are not in the main thread (remember we didn't use the main_queue)
	 we use performSelectorOnMainThread to call our CALayer and tell it to display the CGImage.*/
	[self.customLayer performSelectorOnMainThread:@selector(setContents:) withObject: (id) newImage waitUntilDone:YES];
	
	/*We display the result on the image view (We need to change the orientation of the image so that the video is displayed correctly).
	 Same thing as for the CALayer we are not in the main thread so ...*/
//	UIImage *image= [UIImage imageWithCGImage:newImage scale:1.0 orientation:UIImageOrientationRight];
//    image = [self convertToGreyscale:image:baseAddress];
	
	/*We relase the CGImageRef*/
	CGImageRelease(newImage);
	
	[self.imageView performSelectorOnMainThread:@selector(setImage:) withObject:grayImage waitUntilDone:YES];
	
	/*We unlock the  image buffer*/
	CVPixelBufferUnlockBaseAddress(imageBuffer,0);
	
	[pool drain];
} 

#pragma mark -
#pragma mark Memory management

- (void)viewDidUnload {
	self.imageView = nil;
	self.customLayer = nil;
	self.prevLayer = nil;
}

- (void)dealloc {
	[self.captureSession release];
    [super dealloc];
}


@end