//
//  Object.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 10/3/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "Object.h"
#include "Texture2D.h"
#include "Helpers.h"
#include "Player.h"

Object::Object() : AnimatedSprite()
{
    this->animationCounter=0;
    this->isMoving = false;
    this->viewDirection = DOWN;
    this->isPlayer = false;
    this->inPlayerFocus = false;
    this->isObject = true;
    this->movingSpeed = 1.0f;
}

void Object::createCopyOf(const Object& other)
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
    this->animationCounter = other.animationCounter;
    this->isMoving = other.isMoving;
    this->movingSpeed = other.movingSpeed;
    this->isPlayer = other.isPlayer;
    this->inPlayerFocus = other.inPlayerFocus;
    this->isObject = other.isObject;
    this->height = other.height;
    strcpy(this->name, other.name);
    
    //printf("Object with Name %s copied\n", this->name);
}

void Object::create(const Texture2D &spriteSet, char* name, int _height)
{
    this->loadData(spriteSet, NUMBER_SPRITES_OBJECTS_X, NUMBER_SPRITES_OBJECTS_Y, _height, name);
}

void Object::update(const AnimatedSprite &player)
{
    this->updateAnimationState();
    
    if (this->inPlayerFocus)
    {
        Rect2D tempA = player.getRect();
        
        // loose focus on player if he walks away
        if (distVect(&tempA, &this->rect)>=32)
        {
            this->setInPlayerFocus(false);
        }
    }
    

}

bool Object::getIfInPlayerFocus() const
{
    return this->inPlayerFocus;
}

void Object::setInPlayerFocus(bool val)
{
    this->inPlayerFocus = val;
    //printf("Object %s PlayerFocus %d\n", this->name, this->inPlayerFocus);
}




