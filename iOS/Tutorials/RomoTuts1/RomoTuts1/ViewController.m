//
//  ViewController.m
//  RomoTuts1
//
//  Created by Phu Nguyen on 4/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

- (void)gyroscopeCallback:(CMGyroData *)gyroData;
@end

@implementation ViewController
@synthesize accelXLabel, accelYLabel, accelZLabel, startStopButton;

- (IBAction)runRomo:(id)sender
{
    //Go full forward for 1s
    [romo  moveLeftMotor:192 andRight:192];
    
    //Start gyroscope
    if(!motionManager) {
        motionManager = [[CMMotionManager alloc] init];
        
        if([motionManager isGyroAvailable]) {
            [motionManager setGyroUpdateInterval:1.0/5.0f];
            [motionManager startGyroUpdatesToQueue:[NSOperationQueue mainQueue] withHandler:^(CMGyroData *gyroData, NSError *error)
             {
                 [self gyroscopeCallback:gyroData];
             }];
        }
        //Add altitude detection
        if(motionManager.deviceMotionAvailable) {
            motionManager.deviceMotionUpdateInterval = 1.0/5.0f;
            [motionManager startDeviceMotionUpdates];
        }
        
    }
    detectMotion = YES;
    
    
}

#define degrees(x) (180.0 * x / M_PI)

- (void)gyroscopeCallback:(CMGyroData *)gyroData
{
    
    motion = [motionManager deviceMotion];
    //Set a filter to detect when you want the reverse and turn move queue to run
    
    NSLog(@"pitch: %.4f roll: %.4f yaw: %.4f", degrees(motion.attitude.pitch),degrees(motion.attitude.roll), degrees(motion.attitude.yaw));
    
    if (fabsf(degrees(motion.attitude.pitch)) < 79) {
        [romo stopMotors:RMOutputDrive];
        //This is how you execute a move queue
        [romo setMoveQueue:reverseTurnMoveQueue];
        [romo startMoveQueue];
        detectMotion = NO;
    }
    
    
}

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration
{
    accelXLabel.text = [NSString stringWithFormat:@"x: %f", acceleration.x];
    accelYLabel.text = [NSString stringWithFormat:@"y: %f", acceleration.y];
    accelZLabel.text = [NSString stringWithFormat:@"z: %f", acceleration.z];
}

/* RomoMovement Delegate */

- (void)didBeginMoveQueueStep:(NSInteger)stepNumber
{
    
}
- (void)didFinishMoveQueue:(NSInteger)completedSteps inMoveQueue:(NSArray *)moveQueue
{
    detectMotion = YES;
    [romo moveLeftMotor:192 andRight:192];
}
- (void)didFinishMoveQueueStep:(NSInteger) stepNumber
{
    
}
- (void)didFinishMoving
{
    
}


- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.    
    romo = [[RomoMovement alloc] init];
    [romo setDelegate:self];
    
    //Set up movequeue
    NSDictionary *moveBackStep = [NSDictionary dictionaryWithObjectsAndKeys:
                                  [NSNumber numberWithInt:0],@"mode",
                                  [NSNumber numberWithFloat:1.2],@"time",
                                  [NSNumber numberWithInt:64],@"left",
                                  [NSNumber numberWithInt:64],@"right", nil];
    
    NSDictionary *moveLeftStep = [NSDictionary dictionaryWithObjectsAndKeys:
                                  [NSNumber numberWithInt:0],@"mode",
                                  [NSNumber numberWithFloat:1.5],@"time",
                                  [NSNumber numberWithInt:216],@"left",
                                  [NSNumber numberWithInt:32],@"right", nil];
    
    reverseTurnMoveQueue = [[NSArray alloc] initWithObjects:moveBackStep,moveLeftStep, nil];
    
    
    //Activate the accelerometer
    accelerometer = [UIAccelerometer sharedAccelerometer];
    [accelerometer setDelegate:self];
    [accelerometer setUpdateInterval:1.0f/5.0f];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

@end
