//
//  Quad2D.h
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef OpenGL_Quad2D_h
#define OpenGL_Quad2D_h

#include <OpenGLES/ES1/gl.h>
#include "Helpers.h"
#include "Texture2D.h"

class Sprite2D
{
protected:
    int numSpritesX;
    int numSpritesY;
    int height;
    float scalar;
    float angle;
    
    GLuint vbo;
    GLuint *tbo;
    GLuint texture;
    
    Rect2D rect;
    
    char name[30];
    

    
public:
    Sprite2D();
    Sprite2D(const Texture2D &spriteSet, int _numSpritesX, int _numSpritesY, int _height, char *_name);
    virtual ~Sprite2D();
    
    void loadData(const Texture2D &spriteSet, int _numSpritesX, int _numSpritesY, int _height, char* _name);
    
    void draw(GLuint tex_id);
    
    void draw_to_scene(GLuint texture_id);
    void scale(float k);
    void rotate(float _angle);
    void setPos(const Point2D &newpos);
    void setX(float _x);
    void setY(float _y);
    void setName(char* newName);
    
    Rect2D getRect() const;

    GLuint& getTexture();
    GLuint getVBO();
    GLuint* getTBO();
    int getNumSpritesX();
    int getNumSpritesY();
    
    char* getName();
    
    void delete_buffers();
    
    int getHeight() const;
    
    void deleteMem();

};

#endif
