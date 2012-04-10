//
//  Characters.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 10/15/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include <math.h>
#include "Characters.h"
#include "Texture2D.h"
#include "AnimatedSprite.h"

Characters::Characters()
{
    this->weapon = NULL;
    this->attackType = NO_ATTACK;
    this->isMoving = false;
    this->isDefending = false;
}

Characters::~Characters()
{
    this->sprite->delete_buffers();
    Texture2D::delete_texture(this->sprite->getTexture());
    delete this->sprite, this->sprite = NULL;
    
    if (this->weapon != NULL)
    {
        this->weapon->delete_buffers();
        Texture2D::delete_texture(this->weapon->getTexture());
        delete this->weapon, this->weapon = NULL;
    }
}



void Characters::create(const Texture2D &spriteSet, int nx, int ny,  char* name)
{
    this->sprite = new AnimatedSprite();
    this->sprite->loadData(spriteSet, nx, ny, 1, name);
    this->getSprite().setisInAnimationState(1);

}

void Characters::createWeapon(const Texture2D &spriteSet, int nx, int ny)
{
    this->weapon = new AnimatedSprite();
    this->weapon->loadData(spriteSet, nx, ny, 1, (char*)"Weapon");
}

int Characters::updatePosition(enum ATTACK_TYPE last_type, Rect2D &targetRect, bool attacking, bool jump, bool rightSide)
{
    float V = 5.00f;
    
    // if the character is moving
    if (this->isMoving)
    {
        // we save the position
        Point2D pa = this->getSprite().getRect().pos;
        
        // if we do a melee attack 
        if (last_type == MELEE)
        {
            // we check if we attacking => moving TOWARDS the enemy
            if (attacking)
            {
                // adjust gets true if the sprite would jump OVER y = 0
                bool adjust = false;
                
                // if target is far away, we double the velocity
                if (targetRect.pos.x+targetRect.width < 180.0f && rightSide)
                    V = V*2;
                // calculate jump
                
                double x1,x2;
                double y1,y2;
                
                // start point
                x1 = this->oldPos.x;
                y1 = this->oldPos.y;
                
                
                // attack is with a jump
                if (jump)
                {
                    // target point
                    double x3 = targetRect.pos.x+targetRect.width/2;
                    double y3 = targetRect.pos.y+targetRect.height;
                    
                    // middle point
                    x2 = (x1+x3)/2;
                    y2 = targetRect.pos.y-targetRect.height;
                    if (y2<0)
                    {
                        adjust = true;
                        y2=0;
                        y3-=this->getSprite().getRect().height/2;
                    }
                    
                    double a,b,c;
                    
                    a = (x1*(y2-y3)+x2*(y3-y1)+x3*(y1-y2))/((x1-x2)*(x1-x3)*(x3-x2));	
                    b = (x1*x1*(y2-y3)+x2*x2*(y3-y1)+x3*x3*(y1-y2))/((x1-x2)*(x1-x3)*(x2-x3));
                    c = (x1*x1*(x2*y3-x3*y2)+x1*(x3*x3*y2-x2*x2*y3)+x2*x3*y1*(x2-x3))/((x1-x2)*(x1-x3)*(x2-x3));
                    
                    if (rightSide)
                        pa.x -= V;
                    else
                        pa.x += V;
                    pa.y = a*pa.x*pa.x + b*pa.x + c;
                    
                    this->setNewPositionWithWeapon(pa);
                    
                    Point2D pp = targetRect.pos;
                    pp.x += targetRect.width/2;
                    pp.y += targetRect.height;
                    
                    Rect2D test1 = this->getSprite().getRect();
                    Rect2D test2 = targetRect;
                    if (isRectInRect(&test1, &test2))
                        return 1;
                    
                    if (adjust)
                    {
                        float dist = distPoints(&pa, &pp);
                        if (dist<this->getSprite().getRect().height+10)
                            return 1; // we arrived
                    }
                }
                // attack without jump
                else
                {
                    x2 = targetRect.pos.x;
                    y2 = targetRect.pos.y; 
                    
                    if (rightSide)
                        pa.x -= V;
                    else
                        pa.x += V;
                    
                    pa.y = (((y2-y1)/(x2-x1))*pa.x) + (((x2*y1)-(x1*y2))/(x2-x1));
                    
                    this->setNewPositionWithWeapon(pa);
                    
                    if (this->getSprite().getRect().pos.x > targetRect.pos.x && !rightSide)
                        return 1;
                    else if (this->getSprite().getRect().pos.x < targetRect.pos.x && rightSide)
                        return 1;
                }
            }
            else // we move back to our old position
            {
                if (oldPos.x < 180.0f && rightSide)
                    V = V*2;
                
                double x1 = this->oldPos.x;
                double y1 = this->oldPos.y;
                
                double x2 = targetRect.pos.x;
                double y2 = targetRect.pos.y;
                
                if (rightSide)
                    pa.x += V;
                else
                    pa.x -= V;
                
                pa.y = (((y2-y1)/(x2-x1))*pa.x) + (((x2*y1)-(x1*y2))/(x2-x1));
                
                this->setNewPositionWithWeapon(pa);
                
                if (this->getSprite().getRect().pos.x == targetRect.pos.x &&
                    this->getSprite().getRect().pos.y == targetRect.pos.y)
                    return 0;
                //return 0;
            }
        }            
    }
    
    return -1;  // -1 means we dont do anything
}

void Characters::setNewPositionWithWeapon(Point2D &n)
{
    this->getSprite().setPos(n);
    if (this->weapon != NULL)
    {
        Point2D weaponpos = n;
        weaponpos.y += this->getSprite().getRect().height/2;
        this->getWeaponSprite().setPos(weaponpos);
    }
}

void Characters::Defend()
{
    printf("%s defends\n", this->getSprite().getName());

    this->actionpoints-=AP_DEFENSE;
    this->isDefending = true;
}

AnimatedSprite& Characters::getSprite() const
{
    return *this->sprite;  
}

AnimatedSprite& Characters::getWeaponSprite() const
{
    return *this->weapon;
}

int Characters::getHitpoints() const
{
    return this->hitpoints;
}

int Characters::getManapoints() const
{
    return this->manapoints;
}

int Characters::getActionpoints() const
{
    return this->actionpoints;
}

u_int8_t Characters::getLevel() const
{
    return this->level;
}

int Characters::getLimitpoints() const
{
    return this->limitpoints;
}

void Characters::setHitpoints(int val)
{
    if (val>=this->max_hitpoints)
        val = this->max_hitpoints;
    this->hitpoints = val;
}

void Characters::setManapoints(int val)
{
    if (val>=this->max_manapoints)
        val = this->max_manapoints;
    this->manapoints = val;
}

void Characters::setActionPoints(int val)
{
    this->actionpoints = val;
}

void Characters::setLevel(u_int8_t val)
{
    this->level = val;
}

void Characters::setLimitPoints(int val)
{
    this->limitpoints = val;
}

bool Characters::getIsAlive() const
{
    return this->isAlive;
}

void Characters::setIsAlive(bool val)
{
    this->isAlive = val;
}

void Characters::setAttackStrength(int val)
{
    this->attackStrength = val;
}

void Characters::setDefense(int val)
{
    this->defense = val;
}

int Characters::getAttackStrength() const
{
    return this->attackStrength;
}

int Characters::getDefense() const
{
    return this->defense;
}

void Characters::setIsMoving(bool val)
{
    this->isMoving = val;
}

bool Characters::getIsMoving() const
{
    return this->isMoving;
}


void Characters::setOldPos(Point2D o)
{
    this->oldPos = o;
}

Point2D Characters::getOldPos() const
{
    return this->oldPos;
}

void Characters::setAttackType(enum ATTACK_TYPE t)
{
    this->attackType = t;
}

enum ATTACK_TYPE Characters::getAttackType() const
{
    return this->attackType;
}

void Characters::setTargetRect(Rect2D r)
{
    this->targetRect = r;
}

Rect2D Characters::getTargetRect() const
{
    return this->targetRect;
}

void Characters::setIsAttacking(bool val)
{
    this->isAttacking = val;
}

bool Characters::getIsAttacking() const
{
    return this->isAttacking;
}

int Characters::getMaxHitpoints() const
{
    return this->max_hitpoints;
}

int Characters::getMaxManapoints() const
{
    return this->max_manapoints;
}

bool Characters::getIsGettingAttacked() const
{
    return this->isGettingAttacked;
}

void Characters::setIsGettingAttacked(bool b)
{
    this->isGettingAttacked = b;
}

int Characters::getMaxLimitpoints() const
{
    return this->max_limitpoints;
}

void Characters::setMaxHitpoints(int val)
{
    this->max_hitpoints = val;
}

void Characters::setMaxManapoints(int val)
{
    this->max_manapoints = val;
}

void Characters::setMaxLimitpoints(int val)
{
    this->max_limitpoints = val;
}

bool Characters::getIsDefending() const
{
    return this->isDefending;
}

void Characters::setIsDefending(bool b)
{
    this->isDefending = b;
}