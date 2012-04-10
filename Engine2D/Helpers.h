//
//  Helpers.h
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef OpenGL_Helpers_h
#define OpenGL_Helpers_h

#define PI 3.14159265

typedef struct {
    float x;
    float y;
} Point2D;

typedef struct {
    int x;
    int y;
} Point2Di;

typedef struct {
    Point2D pos;
    float width;
    float height;
} Rect2D;

typedef struct {
    int data;           // contains all the standart data like tex id for layer1-3 and if tile is walkable
    int flags;
    float x,y;
    char *_string;
} TILE;

extern const char* getPathFromIPhoneFile(const char *filename);
extern float distVect(Rect2D *ra, Rect2D *rb);
extern float distPoints(Point2D *a, Point2D *b);
extern void setMatrixToGUIMode();
extern bool isPointInRect(Point2D *a, Rect2D *b);
extern Point2D getMidpointOf(Rect2D *a);
extern Rect2D rectMake(float x, float y, float w, float h);
extern Point2D pointMake(float x, float y);
extern bool isRectInRect(Rect2D *a, Rect2D *b);

#endif
