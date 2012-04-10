//
//  Engine2DAppDelegate.h
//  Engine2D
//
//  Created by Markus Pfundstein on 9/13/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#import <UIKit/UIKit.h>

@class Engine2DViewController;

@interface Engine2DAppDelegate : NSObject <UIApplicationDelegate>

@property (nonatomic, retain) IBOutlet UIWindow *window;

@property (nonatomic, retain) IBOutlet Engine2DViewController *viewController;

@end
