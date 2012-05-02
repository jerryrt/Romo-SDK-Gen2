//
//  ViewController.h
//  RomoTuts1
//
//  Created by Phu Nguyen on 4/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "RomoMovement.h"
#import <CoreMotion/CoreMotion.h>

@interface ViewController : UIViewController
<RomoMovementDelegate, UIAccelerometerDelegate>
{
    RomoMovement *romo;
    
    CMMotionManager *motionManager;
    CMDeviceMotion  *motion;
    
    BOOL detectMotion;
    
    NSArray *reverseTurnMoveQueue;
    
    UIAccelerometer *accelerometer;
}

@property (nonatomic, strong) IBOutlet UIButton *startStopButton;
@property (nonatomic, strong) IBOutlet UILabel  *accelXLabel;
@property (nonatomic, strong) IBOutlet UILabel  *accelYLabel;
@property (nonatomic, strong) IBOutlet UILabel  *accelZLabel;

- (IBAction)runRomo:(id)sender;

@end
