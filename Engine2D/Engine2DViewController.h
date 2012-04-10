//
//  Engine2DViewController.h
//  Engine2D
//
//  Created by Markus Pfundstein on 9/13/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#import <UIKit/UIKit.h>

#import <OpenGLES/EAGL.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#import "Helpers.h"

@interface Engine2DViewController : UIViewController {
    EAGLContext *context;
    GLuint program;
    
    BOOL animating;
    NSInteger animationFrameInterval;
    CADisplayLink *displayLink;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;
@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) CADisplayLink *displayLink;

- (Point2D) landScapeTouchPoint:(CGPoint) touch_pos;
- (void)reSizeGLScene;
- (void)initEngine;
- (void)deallocEngine;
- (void)startAnimation;
- (void)stopAnimation;

@end
