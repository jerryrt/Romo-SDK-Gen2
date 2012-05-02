//
//  ListVC.m
//  RomoSampleApp
//
//  Created by Phu Nguyen on 3/22/12.
//  Copyright (c) 2012 Romotive, Inc. All rights reserved.
//

#import "ListVC.h"

@interface ListVC ()

- (NSString *)stringForAuxValue:(int)auxValue;
- (BOOL)isIOS5;
@end

@implementation ListVC
@synthesize tblCommands, btnRun, btnEdit;

/*****************************
 Button Events
 *****************************/

- (IBAction)btnAddTap:(id)sender
{
    //Present Modal Dialog for the Cmd Options
    [self presentModalViewController:cmdOptionsVC animated:YES];
    [cmdOptionsVC loadNewOptions];

}
- (IBAction)btnRunTap:(id)sender
{
    if (moveQueue.count == 0) {
        return;
    }

    if (!isInQueue) {

        [romo setMoveQueue:[NSArray arrayWithArray:moveQueue]];
        [romo startMoveQueue];
        [btnRun setTitle:@"Stop"];
        
        if([self isIOS5]) {
            [btnRun setTintColor:[UIColor redColor]];
        }
        isInQueue = YES;
        
    } else {
        [romo stopMoveQueue];

        isInQueue = NO;
    }
}   
- (IBAction)btnEditTap:(id)sender
{
    if(tblCommands.editing) {
        [tblCommands setEditing:NO animated:YES];
        [tblCommands reloadData];
        [btnEdit setTitle:@"Edit"];
		[btnEdit setStyle:UIBarButtonItemStyleBordered];
        [btnRun setEnabled:YES];  
    } else {
        [tblCommands setEditing:YES animated:YES];
		[tblCommands reloadData];
		[btnEdit setTitle:@"Done"];
		[btnEdit setStyle:UIBarButtonItemStyleDone];
        [btnRun setEnabled:NO];  
    }
}
/*****************************
 CmdOptions Delegate
 *****************************/
- (void)didSaveWithOptions:(NSDictionary *)options forIndex:(NSInteger)index
{
    if (index < 0) {
        [moveQueue addObject:options];
    } else {
        [moveQueue removeObjectAtIndex:index];
        [moveQueue insertObject:options atIndex:index];
    }
    
    [tblCommands reloadData];
}

/*****************************
 RomoMovement Delegate
 *****************************/

- (void)didBeginMoveQueueStep:(NSInteger)stepNumber
{
    
    [tblCommands selectRowAtIndexPath:[NSIndexPath indexPathForRow:stepNumber inSection:0] animated:YES scrollPosition:UITableViewScrollPositionNone];
}
- (void)didFinishMoveQueue:(NSInteger)completedSteps inMoveQueue:(NSArray *)moveQueue
{
    [romo stopAllMotors];
    if([self isIOS5]) {
        [btnRun setTintColor:[UIColor colorWithRed:91.0f/255.0f green:174.0f/255.0f blue:254.0f/255.0f alpha:1.0]];
    }
    [btnRun setTitle:@"Run"];
    isInQueue = NO;
    
}
- (void)didFinishMoveQueueStep:(NSInteger) stepNumber
{
    [tblCommands deselectRowAtIndexPath:[NSIndexPath indexPathForRow:stepNumber inSection:0] animated:YES];
}
- (void)didFinishMoving
{

}
/*****************************
 UITableDelegate & DataSource
 *****************************/

- (void) tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    
    if ([romo isRunning]) {
        [romo stopAllMotors];
        [romo stopMoveQueue];
    }
    
    if ([[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"mode"] intValue] == 0) {
        [romo moveRomoforTime:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"time"] floatValue] 
                     withLeft:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"left"] floatValue] 
                     andRight:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"right"] floatValue]];
    } else {
        
        [romo moveAuxforTime:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"time"] floatValue]
                    withAux1:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"aux1"] intValue]  
                        aux2:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"aux2"] intValue]   
                        aux3:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"aux3"] intValue] ];
    }
   
    
}

-(void) tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath
{
    //Open up cmdOptions
    [self presentModalViewController:cmdOptionsVC animated:YES];
    [cmdOptionsVC loadExistingOptions:[moveQueue objectAtIndex:indexPath.row] atIndex:indexPath.row];
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return YES if you want the specified item to be editable.
    return YES;
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(!tblCommands.editing || !indexPath) return UITableViewCellEditingStyleNone;
    return UITableViewCellEditingStyleDelete;
}


- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath 
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        [moveQueue removeObjectAtIndex:indexPath.row];
        [tblCommands reloadData];
        
    }    
}
//Can reorder items
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath {
    return YES;
}

- (void) tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)sourceIndexPath toIndexPath:(NSIndexPath *)destinationIndexPath
{
    NSDictionary * item = [moveQueue objectAtIndex:sourceIndexPath.row];
    [moveQueue removeObjectAtIndex:sourceIndexPath.row];
    
    if(destinationIndexPath.row < [moveQueue count]) {
        [moveQueue insertObject:item atIndex:destinationIndexPath.row];
    } else {
        [moveQueue insertObject:item atIndex:[moveQueue count]];
    }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath 
{
    static NSString * CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) 
    {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:CellIdentifier];
    }
    
    if ([[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"mode"] intValue] == 0) {
        cell.textLabel.text = [NSString stringWithFormat:@"Left:%i Right:%i",
                               [[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"left"] intValue],
                               [[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"right"]intValue] ];
    } else {
        
        cell.textLabel.text = [NSString stringWithFormat:@"Aux1:%@ Aux2:%@, Aux3:%@",
                               [self stringForAuxValue:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"aux1"] intValue]],
                               [self stringForAuxValue:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"aux2"] intValue]],
                               [self stringForAuxValue:[[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"aux3"] intValue]] ];
    }
    
    
    cell.detailTextLabel.text = [NSString stringWithFormat:@"%.1fs",
                                 [[[moveQueue objectAtIndex:indexPath.row] objectForKey:@"time"] floatValue]];
    cell.accessoryType = UITableViewCellAccessoryDetailDisclosureButton;
    
    return cell;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section 
{
    return [moveQueue count];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

/*****************************
 View Events
 *****************************/
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        self.title = NSLocalizedString(@"List", @"List");
        self.tabBarItem.image = [UIImage imageNamed:@"RSTabsGears"];
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    romo = [[RomoMovement alloc] init];
    
    [romo setDelegate:self];
    
    //Load moveQueue
    moveQueue = [[NSMutableArray alloc] init];
    
    if (cmdOptionsVC == nil) {
        cmdOptionsVC = [[CmdOptionsVC alloc] init];
        cmdOptionsVC.delegate = self;
    }
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)viewDidAppear:(BOOL)animated
{
    //[romo moveLeftMotor:128 andRight:128];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait || interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown);
}

/*****************************
 Helper functions
 *****************************/
- (BOOL)isIOS5
{
    return ([[[UIDevice currentDevice] systemVersion] floatValue] >= 5.0);
}

- (NSString *)stringForAuxValue:(int)auxValue
{
    switch (auxValue) {
        case 0: {
            return @"R";
        }   break;
        case 1: {
            return @"S";
        }   break; 
        case 2: {
            return @"F";
        }   break;
    }
    return @"";
}


@end
