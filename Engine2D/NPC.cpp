//
//  NPC.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 9/27/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "NPC.h"
#include "ScriptDef.h"
#include "Helpers.h"
#include "Object.h"
#include "Constants.h"

NPC::NPC() : AnimatedSprite()
{
    this->animationCounter=0;
    this->isMoving = false;
    this->isPlayer = false;
    this->inPlayerFocus = false;
    this->locked = false;
    this->isObject = false;
    this->movingSpeed = 1.0f;
    this->height = 2;
}

void NPC::createCopyOf(const NPC& other)
{
    this->numSpritesX = other.numSpritesX;
    this->numSpritesY = other.numSpritesY;
    this->tbo = other.tbo;
    this->vbo = other.vbo;
    this->texture = other.texture;
    this->rect = other.rect;
    this->viewDirection = other.viewDirection;
    this->scalar = other.scalar;
    this->angle = other.angle;
    this->flags = other.flags;
    this->animationCounter = other.animationCounter;
    this->isMoving = other.isMoving;
    this->movingSpeed = other.movingSpeed;
    this->isPlayer = other.isPlayer;
    this->inPlayerFocus = other.inPlayerFocus;
    this->isObject = other.isObject;
    this->height = other.height;
    strcpy(this->name, other.name);
    
    //printf("NPC with Name %s copied\n", this->name);
}

// for creation without the Constructor 
void NPC::create(const Texture2D &spriteSet, char *name, int _flags)
{
    this->flags = _flags;
    if (flags < 4)
        this->viewDirection = (ViewDirection)_flags;   // flag 0-4 = viewdirection
    else 
        this->viewDirection = DOWN;                    // otherwise they are invisible when they not mov
    
    this->loadData(spriteSet, 4, 4, 2, name);
}

int NPC::getFlags()
{
    return this->flags;
}

void NPC::update(AnimatedSprite &_char, WorldMap &current_map)
{
    this->updateAnimationState();

    if (!this->locked)
    {
        if (this->isInAnimationState)
            this->isInAnimationState = false;
        
        if (this->inPlayerFocus)
        {
            this->timerPlayerAttention++;
            this->isMoving = false;
            
            // loose focus on player after no reaction
            if (this->timerPlayerAttention >= 180)
            {
                this->setInPlayerFocus(false);
                this->timerPlayerAttention = 0;
            }
            
            Rect2D tempA = _char.getRect();
            
            // loose focus on player if he walks away
            if (distVect(&tempA, &this->rect)>=32)
            {
                this->setInPlayerFocus(false);
            }
                    
        }
        // we update only if NPC is not in player focus
        else
        {
            switch (this->flags)
            {
                case NPC_STAND_UP:
                {
                    if (this->viewDirection != UP)
                        this->viewDirection = UP;
                    break;
                }
                case NPC_STAND_DOWN:
                {
                    if (this->viewDirection != DOWN)
                        this->viewDirection = DOWN;
                    break;
                }
                case NPC_STAND_LEFT:
                {
                    if (this->viewDirection != LEFT)
                        this->viewDirection = LEFT;
                    break;
                }
                case NPC_STAND_RIGHT:
                {
                    if (this->viewDirection != RIGHT)
                        this->viewDirection = RIGHT;
                    break;
                }
                case NPC_WALKAROUND:
                {
                    this->timerForWalkaroundScript++;
                    if (this->isMoving == false)
                    {
                        int newDir = rand()%4;
                        this->viewDirection = (ViewDirection) newDir;
                        this->isMoving = true;
                    }
                    
                    if (this->timerForWalkaroundScript > 200)
                    {
                        int newDir = rand()%4;
                        this->viewDirection = (ViewDirection) newDir;

                        
                        this->timerForWalkaroundScript = 0;
                    }
                    
                    if (this->isMoving)
                        this->move(_char, current_map);
                    
                    break;
                }
            }
        }
    }
}

void NPC::move(AnimatedSprite &_char, WorldMap &current_map)
{
    if (this->rect.pos.x < 0)
    {
        this->rect.pos.x = 0;
        this->isMoving = false;
    }
    else if (this->rect.pos.y < 0-this->rect.height/2)
    {
        this->rect.pos.y = 0-this->rect.height/2;
        this->isMoving = false;
    }
    else if (this->rect.pos.x+this->rect.width > current_map.get_world_width())
    {
        this->rect.pos.x = current_map.get_world_width()-this->rect.width;
        this->isMoving = false;
    }
    else if (this->rect.pos.y+this->rect.height > current_map.get_world_height())
    {
        this->rect.pos.y = current_map.get_world_height()-this->rect.height;
        this->isMoving = false;
    }
    else 
    {
        Point2D temp;
        temp.x = rect.pos.x;
        temp.y = rect.pos.y;
        
        switch (this->viewDirection)
        {
            case UP:
            {
                this->rect.pos.y-=this->movingSpeed;
                break;
            }
            case LEFT:
            {
                this->rect.pos.x-=this->movingSpeed;
                break;
            }
            case RIGHT:
            {
                this->rect.pos.x+=this->movingSpeed;
                break;
            }
            case DOWN:
            {
                this->rect.pos.y+=this->movingSpeed;
                break;
            }
        }
        
        bool alreadyhit = false;
        u_int8_t collision=0;
        this->collisionWithTile(current_map, collision);
        Rect2D r1 = this->getRect();
        if (collision == 0)
        {

            Rect2D r2 = _char.getRect();
            int i;
            this->collisionWithObject(r1, r2, collision, _char.getHeight());
            if (collision == 0)
            {
                for (i = 0; i < current_map.getNPCcounter(); i++)
                {
                    if (&current_map.getNPC(i) != this)
                    {
                        if (!alreadyhit )
                        {
                            r2 = current_map.getNPC(i).getRect();
                            alreadyhit = this->collisionWithObject(r1, r2, collision, current_map.getNPC(i).getHeight());
                        }
                    }
                }
            }
            if (collision==0)
            {
                for (i = 0; i < current_map.getObjectCounter(); i++)
                {
                    if (!alreadyhit)
                    {
                        Rect2D r2 = current_map.getObject(i).getRect();
                        alreadyhit = this->collisionWithObject(r1, r2, collision, current_map.getObject(i).getHeight());
                    }
                }
            }
        }
        
        if (this->viewDirection==DOWN && collision>>3&0x1)
        {
            this->rect.pos.y = temp.y-COL_OFFSET_DOWN;
            this->isMoving=false;
        }
        if (this->viewDirection==LEFT && collision>>2&0x1)
        {
            
            this->rect.pos.x = temp.x+COL_OFFSET_LEFT;
            this->isMoving=false;
        }
        if (this->viewDirection==RIGHT && collision>>1&0x1)
        {
            
            this->rect.pos.x = temp.x-COL_OFFSET_RIGHT;
            this->isMoving=false;
        }
        if (this->viewDirection==UP && collision&0x1)
        {
            
            this->rect.pos.y = temp.y+COL_OFFSET_UP;
            this->isMoving=false;
        }
    }
}

bool NPC::getIfInPlayerFocus() const
{
    return this->inPlayerFocus;
}

void NPC::setInPlayerFocus(bool val)
{
    this->timerPlayerAttention = 0;
    this->inPlayerFocus = val;
}

void NPC::setLocked(bool val)
{
    this->locked = val;
}