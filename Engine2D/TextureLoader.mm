//
//  Texture2D.m
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#import "TextureLoader.h"

@implementation TextureLoader

@synthesize texture;
@synthesize width;
@synthesize height;

- (id)initWithFilename: (NSString*)filename
{
    NSString *seperator = @".";
    
    NSArray *split = [filename componentsSeparatedByString:seperator];
    
    self = [super init];
    if (self) {
    
        [self loadTextureFromPath:[split objectAtIndex:0] ofType:[split objectAtIndex:1]];
    }
    
    return self;
}

-(void)loadTextureFromPath:(NSString*)texPath ofType:(NSString*)type
{
    NSString *path = [[NSBundle mainBundle] pathForResource:texPath ofType:type];
    NSData *texData = [[NSData alloc] initWithContentsOfFile:path];
    UIImage *image = [[UIImage alloc] initWithData:texData];

    width = CGImageGetWidth(image.CGImage);
    height = CGImageGetHeight(image.CGImage);    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    void *imageData = malloc( height * width * 4 );
    CGContextRef context = CGBitmapContextCreate( 
                                                 imageData, width, height, 8, 4 * width, colorSpace,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
    CGColorSpaceRelease( colorSpace );
    CGContextClearRect( context, CGRectMake( 0, 0, width, height ) );
    CGContextTranslateCTM( context, 0, height - height );
    CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), image.CGImage );
    
    // create opengl texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);   
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, imageData);
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // clea up
    CGContextRelease(context);
    free(imageData);
    [image release];
    [texData release];
}

-(void)dealloc
{
    [super dealloc];
}


@end
