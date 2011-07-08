

#import "WelcomeViewController.h"

@implementation WelcomeViewController

- (IBAction)startFlashcodeDetection {
    [self presentModalViewController:[[FaceDetectorTestController alloc] init] animated:YES];
//	[self presentModalViewController:[[MyAVController alloc] init] animated:YES];
}

- (void)dealloc {
    [super dealloc];
}

@end
