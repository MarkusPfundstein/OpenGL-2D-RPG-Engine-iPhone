//
//  AnimatedSprite.cpp
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "AnimatedSprite.h"
#include "NPC.h"
#include "Helpers.h"
#include "GameEngine.h"
#include "BattleEngine.h"

AnimatedSprite::AnimatedSprite():Sprite2D()
{
    this->animationState = 0;
    this->animationCounter = 0;
    this->isInAnimationState = false;
    this->isAnimating = false;
    this->isMoving = false;
    Point2D zero = {0.0f, 0.0f};
    this->setOnTile(zero);
    this->engineUsed = NULL;
    this->battleEngineUsed = NULL;
    this->checksForCollision = true;
    this->battlefuncpIsSet = false;
    this->funcpIsSet = false;
    this->resetAnimationState = false;
    this->oldAnimationState = 0;
    this->setViewDirection(UP);
}

AnimatedSprite::~AnimatedSprite()
{
    if (this->engineUsed != NULL)
        this->engineUsed = NULL;
    
    if (this->battleEngineUsed != NULL)
        this->battleEngineUsed = NULL;
}

void AnimatedSprite::updateAnimationState()
{
    if (this->isAnimating)
    {
        animationCounter++;
        if (this->animationState != this->endAnimationState)
        {
            if (this->animationCounter >= this->animationSpeed)
            {
                if (this->endAnimationState>this->animationState)
                    this->animationState++;
                else
                {
                    this->animationState--;
                    if (this->animationState<=0)
                        this->animationState =0;
                }
                this->animationCounter = 0;
            }
        }
        else
        {
            if (this->animationCounter >= this->animationSpeed)
            {
                this->animationCounter = 0;
                this->isAnimating = false;
                if (this->funcpIsSet)
                {
                    this->funcp();
                    this->funcpIsSet = false;
                }
                else if (this->battlefuncpIsSet)
                {
                    (this->battleEngineUsed->*battlefuncp)();
                    this->battlefuncpIsSet = false;
                }
                
                if (resetAnimationState)
                    this->animationState = oldAnimationState;
            }
        }
    }
}

void AnimatedSprite::drawAnimated()
{
    // when there is a running animation in the pipeline we use this function
    if (this->isAnimating || this->isInAnimationState)
        this->draw(this->getAnimationState());
    else
    {
    
        glBindTexture(GL_TEXTURE_2D, this->texture);
        
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glVertexPointer(3, GL_FLOAT, 0, (char*)NULL);
        
        if (!isMoving)
        {
            //this->animationCounter = 0;
            switch(this->viewDirection)
            {
                case UP:
                    this->draw_to_scene(0);
                    break;
                case RIGHT:
                    this->draw_to_scene(this->numSpritesX+1);
                    break;
                case DOWN:
                    this->draw_to_scene(this->numSpritesX*2+1);
                    break;
                case LEFT:
                    this->draw_to_scene(this->numSpritesX*3+1);
                    break;
            }
        }
        else
        {
            this->animationCounter++;
            switch(this->viewDirection)
            {
                case UP:
                    if (this->animationCounter < 8)
                        this->draw_to_scene(0);
                    else if (this->animationCounter >= 8 && this->animationCounter < 16)
                        this->draw_to_scene(1);
                    else if (this->animationCounter >= 16 && this->animationCounter < 24)
                        this->draw_to_scene(2);
                    break;
                case RIGHT:
                    if (this->animationCounter < 8)
                        this->draw_to_scene(this->numSpritesX);
                    else if (this->animationCounter >= 8 && this->animationCounter < 16)
                        this->draw_to_scene(this->numSpritesX+1);
                    else if (this->animationCounter >= 16 && this->animationCounter < 24)
                        this->draw_to_scene(this->numSpritesX+2);
                    break;
                case DOWN:
                    if (this->animationCounter < 8)
                        this->draw_to_scene(this->numSpritesX*2);
                    else if (this->animationCounter >= 8 && this->animationCounter < 16)
                        this->draw_to_scene(this->numSpritesX*2+1);
                    else if (this->animationCounter >= 16 && this->animationCounter < 24)
                        this->draw_to_scene(this->numSpritesX*2+2);
                    break;
                case LEFT:
                    if (this->animationCounter < 8)
                        this->draw_to_scene(this->numSpritesX*3);
                    else if (this->animationCounter >= 8 && this->animationCounter < 16)
                        this->draw_to_scene(this->numSpritesX*3+1);
                    else if (this->animationCounter >= 16 && this->animationCounter < 24)
                        this->draw_to_scene(this->numSpritesX*3+2);
                    break;
            }
            if (this->animationCounter==23)
                this->animationCounter=0;
        }
    }
}

bool AnimatedSprite::collisionWithObject(Rect2D &a, Rect2D &b, u_int8_t &collision, int heightOfObject)
{
    bool alreadyhit = 0;
    bool colup = 0;
    bool colleft = 0;
    bool colright = 0;
    bool coldown = 0;
    
    float offsetY;
    if (heightOfObject > 1)
        offsetY = b.pos.y+b.height/heightOfObject;
    else
        offsetY = b.pos.y;
    
    if (a.pos.x + a.width < b.pos.x)
        collision = 0;
    else if (a.pos.x > b.pos.x + b.width)
        collision = 0;
    else if (a.pos.y + a.height < offsetY)
        collision = 0;
    else if (a.pos.y + a.height/2 > b.pos.y + b.height)
        collision = 0;
    else
    {
        // COLLISION
        
        if (a.pos.y + a.height > b.pos.y && a.pos.y < b.pos.y)
        {
            coldown = true;
        }
        if (a.pos.x < b.pos.x + b.width &&
            a.pos.x + a.width > b.pos.x + b.width)
        {
            colleft = true;
        }
        if (a.pos.x + a.width > b.pos.x &&
            a.pos.x < b.pos.x)
        {
            colright = true;
        }
        if (a.pos.y < b.pos.y+b.height && a.pos.y + a.height > b.pos.y + b.height)
        {
            colup = true;
        }
        
        alreadyhit = true;
        
    }
    
    collision = (coldown<<3)|(colleft<<2)|(colright<<1)|colup;
    
    return alreadyhit;
}

void AnimatedSprite::collisionWithTile(WorldMap &current_map, u_int8_t &collision)
{
    int m,n,i2,j2;
    
    Rect2D tempRect;
    
    tempRect.pos.y = this->rect.pos.y+this->rect.height/2;
    tempRect.pos.x = this->rect.pos.x;
    tempRect.width = this->rect.width;
    tempRect.height = this->rect.height/2;
    
    // starting points
    m = tempRect.pos.x / TILE_SIZE;
    n = tempRect.pos.y / TILE_SIZE;
    
    // end points
    i2 = ((tempRect.width+tempRect.pos.x)/ TILE_SIZE);
    j2 = ((tempRect.pos.y+tempRect.height)/TILE_SIZE);
    
    bool colup = false;
    bool coldown = false;
    bool colleft = false;
    bool colright = false;
    bool tileWithFlagAlreadyHit=false;
    for (int i=m-2; i < i2+2; i++)
    {
        for (int j=n-1; j < j2+2; j++)
        {
            if (!(current_map.getTileAt(i, j).data>>24    
                 &0x1) || (current_map.getTileAt(i, j).flags&0xff)!=0)      // tile is collision tile or special tile
            {
                
                if (tempRect.pos.x + tempRect.width-CHARACTER_COLL_OFFSETX+3 < current_map.getTileAt(i, j).x)
                    collision = 0;
                else if (tempRect.pos.x+CHARACTER_COLL_OFFSETX+3 > current_map.getTileAt(i, j).x + TILE_SIZE)
                    collision = 0;
                else if (tempRect.pos.y + tempRect.height-CHARACTER_COLL_OFFSETY < current_map.getTileAt(i, j).y)
                    collision = 0
                    ;
                else if (tempRect.pos.y+CHARACTER_COLL_OFFSETY > current_map.getTileAt(i, j).y + TILE_SIZE)
                    collision = 0;
                else
                {
                    
                    // COLLISION
                    // check for flags
                    if ((current_map.getTileAt(i, j).flags&0xff)==TILE_FLAG_TRANSPORTTILE && !tileWithFlagAlreadyHit &&this->isPlayer)
                    {   
                        tileWithFlagAlreadyHit = true;
                        Point2D destination;
                        destination.x = current_map.getTileAt(i, j).flags>>8&0xff;
                        destination.y = current_map.getTileAt(i, j).flags>>16&0xff;
                        current_map.suicide(destination, current_map.getTileAt(i, j)._string);
                    }
                    else if ((current_map.getTileAt(i, j).flags&0xff)==TILE_FLAG_EVENTTILE && !tileWithFlagAlreadyHit &&this->isPlayer)
                    {
                        tileWithFlagAlreadyHit = true;
                        
                        this->checksForCollision = false;
                        
                        engineUsed->startDialog(ACTION_EVENTTILE, current_map.getTileAt(i, j)._string);
                    }
                    
                    //check for collision tile collision tile:
                    if (!(current_map.getTileAt(i, j).data>>24    
                         &0x1))
                    {
                        if (tempRect.pos.y + tempRect.height > current_map.getTileAt(i, j).y && tempRect.pos.y < current_map.getTileAt(i, j).y)
                        {
                            coldown = true;
                        }
                        if (tempRect.pos.x < current_map.getTileAt(i, j).x + TILE_SIZE &&
                            tempRect.pos.x + tempRect.width > current_map.getTileAt(i, j).x + TILE_SIZE)
                        {
                            colleft = true;
                        }
                        if (tempRect.pos.x + tempRect.width > current_map.getTileAt(i, j).x &&
                            tempRect.pos.x < current_map.getTileAt(i, j).x)
                        {
                            colright = true;
                        }
                        if (tempRect.pos.y < current_map.getTileAt(i, j).y+TILE_SIZE && tempRect.pos.y + tempRect.height > current_map.getTileAt(i, j).y + TILE_SIZE)
                        {
                            colup = true;
                        }
                    }
                }
            }
        }
    }
    // write all values in collision
    collision = (coldown<<3)|(colleft<<2)|(colright<<1)|colup;
}

void AnimatedSprite::setOnTile(Point2D point)
{
    if (!this->isObject)
    {
        point.x = point.x*TILE_SIZE;
        point.y = (point.y-1)*TILE_SIZE;
    }
    else
    {
        point.x = point.x*TILE_SIZE;
        point.y = point.y*TILE_SIZE;
    }
    
    this->setPos(point);
}

void AnimatedSprite::setMoving(bool val)
{
    this->isMoving = val;
}

void AnimatedSprite::setViewDirection(ViewDirection direction)
{
    this->viewDirection = direction;
}

float AnimatedSprite::getMovingSpeed()const
{
    return this->movingSpeed;
}

bool AnimatedSprite::getMoving() const
{
    return this->isMoving;
}

bool AnimatedSprite::getIsObject() const
{
    return this->isObject;
}

ViewDirection AnimatedSprite::getViewDirection() const
{
    return this->viewDirection;
}

int AnimatedSprite::getAnimationState() const
{
    return this->animationState;
}

void AnimatedSprite::setAnimationState(int val)
{
    this->animationState = val;
}

bool AnimatedSprite::getIsAnimating() const
{
    return this->isAnimating;
}
void AnimatedSprite::setIsAnimating(bool val)
{
    this->isAnimating = val;
}

void AnimatedSprite::setAnimationSpeed(int val)
{
    this->animationSpeed = val;
}

void AnimatedSprite::setEndAnimationState(int val)
{
    this->endAnimationState = val;
    //printf("%s end animation state set to: %d\n", this->name, this->endAnimationState);
}

void AnimatedSprite::pointToEngine(GameEngine &_engine)
{
    this->engineUsed = &_engine;
}

void AnimatedSprite::setChecksForCollison(bool val)
{
    this->checksForCollision = val;
}

void AnimatedSprite::setisInAnimationState(bool val)
{
    this->isInAnimationState = val;
}

void AnimatedSprite::setEndAnimationFunction(void (BattleEngine::*_pfunc)(), BattleEngine &b)
{
    this->battlefuncp = _pfunc;
    this->battleEngineUsed = &b;
    this->battlefuncpIsSet = 1;
}
void AnimatedSprite::setEndAnimationFunction(void (*_pfunc)())
{
    this->funcpIsSet = 1;
    this->funcp = _pfunc;
}

void AnimatedSprite::resetAfterEndAnimationStateIsReached(bool b)
{
    this->oldAnimationState = this->animationState;
    this->resetAnimationState = b;
}