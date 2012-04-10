//
//  Helpers.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 9/16/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <OpenGLES/ES1/gl.h>
#include "Constants.h"
#include "Helpers.h"


// returns c string of a file located on mainBundle of the App
const char* getPathFromIPhoneFile(const char *filename)
{
    NSString *NSFilename = [NSString stringWithCString:filename encoding:[NSString defaultCStringEncoding]];
    
    NSString *seperator = @".";
    
    NSArray *split = [NSFilename componentsSeparatedByString:seperator];
    
    NSString *NSPath = [[NSBundle mainBundle] pathForResource:[split objectAtIndex:0] ofType:[split objectAtIndex:1]];
    
    return [NSPath UTF8String];
}

float distVect(Rect2D *ra, Rect2D *rb)
{
    Point2D a;
    a.x = (ra->pos.x + ra->width)/2;
    a.y = (ra->pos.y + ra->height)/2;
    Point2D b;
    b.x = (rb->pos.x + ra->width)/2;
    b.y = (rb->pos.y + ra->height)/2;
    
    float deltaX;
    float deltaY;
    
    if (a.x > b.x)
        deltaX = a.x - b.x;
    else
        deltaX = b.x - a.x;
    
    if (a.y > b.y)
        deltaY = a.y - b.y;
    else
        deltaY = b.y - a.y;
    
    float result = sqrtf(deltaX*deltaX + deltaY*deltaY);

    
    return result;
}

float distPoints(Point2D *a, Point2D *b)
{
    float deltaX;
    float deltaY;
    
    if (a->x > b->x)
        deltaX = a->x - b->x;
    else
        deltaX = b->x - a->x;
    
    if (a->y > b->y)
        deltaY = a->y - b->y;
    else
        deltaY = b->y - a->y;
    
    float result = sqrtf(deltaX*deltaX + deltaY*deltaY);
    
    
    return result;
}

bool isRectInRect(Rect2D *a, Rect2D *b)
{
    bool r = false;
        
    if (a->pos.x + a->width < b->pos.x)
        r = 0;
    else if (a->pos.x > b->pos.x + b->width)
        r = 0;
    else if (a->pos.y + a->height < b->pos.y)
        r = 0;
    else if (a->pos.y + a->height > b->pos.y + b->height)
        r = 0;
    else
        r = 1;
        
    return r;
}


void setMatrixToGUIMode()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrthof(0, SCREEN_HEIGHT_IPHONE, SCREEN_WIDTH_IPHONE, 0, -1.0f, 1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90.0f, 0, 0, 1);
    glTranslatef(0.0f, -SCREEN_HEIGHT_IPHONE, 0.0f);
}

bool isPointInRect(Point2D *a, Rect2D *b)
{
    if (a->x >= b->pos.x &&
        a->y >= b->pos.y &&
        a->x < b->pos.x + b->width &&
        a->y < b->pos.y + b->height)
    {
        return true;
    }
    
    return false;
}

Point2D getMidpointOf(Rect2D *a)
{
    Point2D p;
    
    p.x = a->pos.x + a->width/2;
    p.y = a->pos.y + a->height/2;
    
    return p;
}

Rect2D rectMake(float x, float y, float w, float h)
{
    Rect2D r;
    
    r.pos.x = x;
    r.pos.y = y;
    r.width = w;
    r.height = h;
    
    return r;
}

Point2D pointMake(float x, float y)
{
    Point2D p;
    
    p.x = x;
    p.y = y;
    
    return p;
}
