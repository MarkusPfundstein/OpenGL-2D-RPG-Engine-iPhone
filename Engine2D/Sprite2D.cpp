//
//  Quad2D.cpp
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "Sprite2D.h"

Sprite2D::Sprite2D()
{

}

// Sprite2D class functions

Sprite2D::~Sprite2D()
{
    //printf("Sprite with name: %s deleted\n", this->name);
}

void Sprite2D::deleteMem()
{
    Texture2D::delete_texture(this->texture);
    this->delete_buffers();
}

void Sprite2D::loadData(const Texture2D& spriteSet, int _numSpritesX, int _numSpritesY, int _height, char* _name)
{
    this->numSpritesX = _numSpritesX;
    this->numSpritesY = _numSpritesY;
    this->height = _height;
    
    this->scalar = 1.0f;
    this->angle = 0.0f;
    
    strcpy(this->name, _name);
    
    this->rect.pos.x = 0;
    this->rect.pos.y = 0;
    this->rect.width = spriteSet.width / numSpritesX;
    this->rect.height = spriteSet.height / numSpritesY;
    
    const GLfloat vertices[] = {
        this->rect.width, 0, 0,
        0, 0, 0,
        0, this->rect.height, 0, 
        0, this->rect.height, 0,
        this->rect.width, this->rect.height, 0,
        this->rect.width, 0, 0,
        
    };
    
    glGenBuffers(1, &this->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    
    this->texture = spriteSet.texture;
    this->tbo = new GLuint[numSpritesX*numSpritesY];
    
    // create texture buffers, 1 for each animationstep
    Point2D *point = new Point2D[6];
    Rect2D offset;
    GLfloat *texcoords = new GLfloat[12];
    
    int counter = 0;
    int nSpriteCol = 0;
    int nSpriteRow = 0;
    
    for (int i = 0; i < numSpritesY; i++)
    {
        for (int j = 0; j < numSpritesX; j++)
        {
            offset.pos.x = (float)nSpriteRow/numSpritesX;                           
            offset.pos.y = (float)nSpriteCol/numSpritesY;
            offset.width = (float)1/numSpritesX;              
            offset.height = (float)1/numSpritesY;             
            
            point[0].x = offset.pos.x + offset.width;
            point[0].y = offset.pos.y;
            point[1].x = offset.pos.x;
            point[1].y = offset.pos.y;
            point[2].x = offset.pos.x;
            point[2].y = offset.pos.y + offset.height;
            point[3].x = point[2].x;
            point[3].y = point[2].y;
            point[4].x = point[1].x + offset.width;
            point[4].y = point[1].y + offset.height;
            point[5].x = point[0].x;
            point[5].y = point[0].y;
            
            for (int k = 0, l = 0; l < 6; k += 2, l++) 
            {
                texcoords[k] = point[l].x;
                texcoords[k+1] = point[l].y;
            }
            
            glGenBuffers(1, &this->tbo[counter]);
            glBindBuffer(GL_ARRAY_BUFFER, this->tbo[counter]);
            glBufferData(GL_ARRAY_BUFFER, 48, texcoords, GL_STATIC_DRAW);
            
            nSpriteRow++;
            counter++;
        }
        nSpriteCol++;
    }
    
    delete[] point;
    delete[] texcoords;
    
    //printf("Object created with name: %s\n", this->name);
}

void Sprite2D::draw(GLuint tex_id)
{
    glBindTexture(GL_TEXTURE_2D, this->texture);
    
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glVertexPointer(3, GL_FLOAT, 0, (char*)NULL);
    
    this->draw_to_scene(tex_id);
}

void Sprite2D::draw_to_scene(GLuint texture_id)
{
    glPushMatrix();
    
    glTranslatef(this->rect.pos.x, this->rect.pos.y, 0.0f);
    glRotatef(this->angle, 0, 0, 1);
    glScalef(this->scalar, this->scalar, 0.0f);
    
    glBindBuffer(GL_ARRAY_BUFFER, this->tbo[texture_id]);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)NULL);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glPopMatrix();
}

Rect2D Sprite2D::getRect() const
{
    return this->rect;
}

void Sprite2D::setPos(const Point2D &newpos)
{
    this->rect.pos.x = newpos.x;
    this->rect.pos.y = newpos.y;
}

void Sprite2D::setX(float _x)
{
    this->rect.pos.x = _x;
}

void Sprite2D::setY(float _y)
{
    this->rect.pos.y = _y;
}

void Sprite2D::scale(float k)
{
    this->scalar = k;
}

void Sprite2D::rotate(float _angle)
{
    this->angle = _angle;
}

GLuint& Sprite2D::getTexture()
{
    return this->texture;
}

GLuint Sprite2D::getVBO()
{
    return this->vbo;
}
GLuint* Sprite2D::getTBO()
{
    return this->tbo;
}

int Sprite2D::getNumSpritesX()
{
    return this->numSpritesX;
}

int Sprite2D::getNumSpritesY()
{
    return this->numSpritesY;
}

char* Sprite2D::getName()
{
    return this->name;
}

void Sprite2D::setName(char *newName)
{
    memset(this->name, 0, 30);
    strcpy(this->name, newName);
}

void Sprite2D::delete_buffers()
{
    glDeleteBuffers(1, &this->vbo);
    
    int i;
    
    for (i = 0; i < numSpritesX*numSpritesY; i++)
    {
        glDeleteBuffers(1, &this->tbo[i]);
    }
    
    //if (numSpritesX*numSpritesY > 1)
        delete [] this->tbo;
}

int Sprite2D::getHeight() const 
{
    return this->height;
}
