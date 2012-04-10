//
//  OpenGLViewController.m
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "Engine2DViewController.h"
#import "EAGLView.h"
#import "Constants.h"
#import "GameEngine.h"

// C++ classes
GameEngine *engine;

// Implementation of class
@implementation Engine2DViewController

@synthesize animating;
@synthesize context;
@synthesize displayLink;

#pragma mark -
#pragma mark Input methods

-(void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch* touch = [[touches allObjects] objectAtIndex:0];
    
    engine->handle_input([self landScapeTouchPoint:[touch locationInView:self.view]]);
    
}

-(void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    engine->input_stop();
} 

-(void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    //
    // THIS DOES SCREW UP THE COLLISION DETECTION IF PLAYER MOVES FAST OVER THE GUI ARROW
    //
    
    UITouch* touch = [[touches allObjects] objectAtIndex:0];
    
    engine->handle_input([self landScapeTouchPoint:[touch locationInView:self.view]]);


}

-(Point2D) landScapeTouchPoint:(CGPoint) touch_pos
{
    Point2D point;
    point.x = touch_pos.y;
    point.y = SCREEN_HEIGHT_IPHONE - touch_pos.x;
    
    return point;
}

#pragma mark -
#pragma mark General Drawing Routines
static int m_nFps; // current FPS
static CFTimeInterval lastFrameStartTime; 
static int m_nAverageFps; // the average FPS over 15 frames
static int m_nAverageFpsCounter;
static int m_nAverageFpsSum;


- (void)gameLoop
{
    CFTimeInterval thisFrameStartTime = CFAbsoluteTimeGetCurrent();
    float deltaTimeInSeconds = thisFrameStartTime - lastFrameStartTime;
    m_nFps = (deltaTimeInSeconds == 0) ? 0: 1 / (deltaTimeInSeconds);
    
    m_nAverageFpsCounter++;
    m_nAverageFpsSum+=m_nFps;
    if (m_nAverageFpsCounter >= 15) // calculate average FPS over 15 frames
    {
        m_nAverageFps = m_nAverageFpsSum/m_nAverageFpsCounter;
        m_nAverageFpsCounter = 0;
        m_nAverageFpsSum = 0;
    }
    
    lastFrameStartTime = thisFrameStartTime;

    
    engine->update_objects();
    
    [(EAGLView *)self.view setFramebuffer];
    
    glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    
    engine->draw_scene((float)m_nFps);
    
    [(EAGLView *)self.view presentFramebuffer];
    

}
// fps calculation



- (void)reSizeGLScene
{
    
    glDisable(GL_DITHER);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    //glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

#pragma mark
#pragma mark CPP Objects Load/Unload

- (void)initEngine
{
    [self reSizeGLScene];
    
    engine = new GameEngine();
    if (!engine->initGameEngine())
    {
        NSLog(@"Error init Game Engine\n");
    }
}

- (void)deallocEngine
{
    delete engine;
}


- (void)awakeFromNib
{
    EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
    
    if (!aContext)
        NSLog(@"Failed to create ES context");
    else if (![EAGLContext setCurrentContext:aContext])
        NSLog(@"Failed to set ES context current");
    
	self.context = aContext;
	[aContext release];
	
    [(EAGLView *)self.view setContext:context];
    [(EAGLView *)self.view setFramebuffer];
    
    
    animating = FALSE;
    animationFrameInterval = 1;
    self.displayLink = nil;
    
    [self initEngine];
}

- (void)dealloc
{
    NSLog(@"deAlloc");
    
    [self deallocEngine];
    
    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
    
    [context release];
    
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc. that aren't in use.
}

- (void)viewWillAppear:(BOOL)animated
{
    [self startAnimation];
    
    [self reSizeGLScene];
    
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopAnimation];
    
    [super viewWillDisappear:animated];
}

- (void)viewDidUnload
{
	[super viewDidUnload];
	
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }

    // Tear down context.
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];
	self.context = nil;	
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
    /*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
    if (frameInterval >= 1) {
        animationFrameInterval = frameInterval;
        
        if (animating) {
            [self stopAnimation];
            [self startAnimation];
        }
    }
}

- (void)startAnimation
{
    if (!animating) {
        CADisplayLink *aDisplayLink = [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(gameLoop)];
        [aDisplayLink setFrameInterval:animationFrameInterval];
        [aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.displayLink = aDisplayLink;
        
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if (animating) {
        [self.displayLink invalidate];
        self.displayLink = nil;
        animating = FALSE;
    }
}

@end
