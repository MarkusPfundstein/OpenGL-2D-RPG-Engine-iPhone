//
//  Monster.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 10/15/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "Monster.h"
#include "AnimatedSprite.h"
#include "BattleEngine.h"
#include "Player.h"
#include "BattleHelpers.h"

Monster::Monster()
{
    this->limitpoints = 0;
    this->level = 0;
    this->finishedWithTurn = 0;
    this->isAlive = true;
    this->target = NULL;
    this->Ki_set = false;
}

void Monster::handleKI(BattleEngine &b)
{
    bool goOn = true;
    static int thinking_time = 0;
    bool changeKIset = true;
    int wait = WAIT_TIME;
    if (!changeKIset)
        wait = 0;
    if (!Ki_set && ++thinking_time>wait)
    {
        thinking_time = 0;
        printf("handle KI from %s with KI Flag: %d\n", this->getSprite().getName(), this->KI_Flag);
        
        switch(this->KI_Flag)
        {
            case KI_AGGRO:  // monsters attacks randomly one of the characters
            {
                // if action points are lower than we can do for an attack
                if (this->actionpoints < AP_ATTACK)
                {
                    printf("%s has not enough action points for MELEE\n", this->getSprite().getName());
                    
                    // we look if we have enough to defend 
                    if (this->hitpoints < this->max_hitpoints/2 && this->actionpoints >= AP_DEFENSE)
                        this->Defend();
                
                    goOn = false;
                }
                
                // if we have enough points we always attack !! (KI AGGRO)
                else
                {
                    // get a random character
                    int randomChoice = rand()%b.getPlayerUsed().getNCharsInGroup();
                    this->target = &b.getPlayerUsed().getCharacterAtPosition(randomChoice);
                    // check if target is living
                    if (!this->target->getIsAlive())
                    {
                        printf("Target dead next target\n");
                        // no, so we have to do the whole procedure from the beginning. That means we DONT want to set Ki_set to true.
                        changeKIset = false;
                    }
                    // attack
                    else
                        this->attackTarget(MELEE);

                }
            } break;
            case KI_DEFENSE:
            {
                this->finishedWithTurn = 1;
            } break;
                
            case STANDARD_FLAG:
                break;
        }
        
        if (changeKIset)
            this->Ki_set = true;
    }

    if (!goOn)
        this->finishedWithTurn = true;

}

void Monster::attackTarget(enum ATTACK_TYPE withType)
{
    printf("%s attacks Character #%s\n", this->getSprite().getName(), target->getSprite().getName());

    switch (withType)
    {
        case MELEE:
        {
            // deduct action points
            this->actionpoints-=AP_ATTACK;
            
            // motivate enemy to do the attack
            this->setOldPos(this->getSprite().getRect().pos);
            Rect2D t = rectMake(this->getSprite().getRect().pos.x+this->getSprite().getRect().width, this->getSprite().getRect().pos.y, this->getSprite().getRect().width, this->getSprite().getRect().height);
            this->setTargetRect(t);
            this->setAttackType(MELEE);
            this->isAttacking = true;
            this->isMoving = 1;
        } break;
        case NO_ATTACK:
            break;
    }
}

void Monster::updateMonster(BattleEngine &b)
{    
    int result = this->updatePosition(this->attackType, this->targetRect, this->isAttacking, 0, 0);
    // position reached
    if (result == 1)
    {
        // indivate the battle engine to draw damage
        b.setLastvalueChanged(-(giveDamageTo(*target, *this, this->attackType)));
        Point2D p = this->target->getSprite().getRect().pos;
        b.setpointOfValueChanged(p);
        b.setvalueChanged(1);
        
        // indicate the target to play an animation
        this->target->getSprite().setAnimationState(ANI_STATE_HURT);
        this->target->getSprite().setIsAnimating(1);
        this->target->getSprite().setAnimationSpeed(15);
        this->target->getSprite().setEndAnimationState(ANI_STATE_STANDING);
        
        // we are not attacking anymore
        this->isAttacking = false;
        this->targetRect = rectMake(this->oldPos.x, this->oldPos.y, this->getSprite().getRect().width, this->getSprite().getRect().height);
        this->setOldPos(this->getSprite().getRect().pos);
    }
    // origin reached again
    else if (result == 0)
    {
        this->isMoving = false;
        this->target = NULL;
        this->attackType = NO_ATTACK;
        this->Ki_set = false;
    }
    else
    {
        // do nothing
    }
}

void Monster::setKI_Flag(enum KI_FLAG val)
{
    this->KI_Flag = val;
}

enum KI_FLAG Monster::getKI_Flag() const
{
    return this->KI_Flag;
}

bool Monster::getFinishedWithTurn() const
{
    return this->finishedWithTurn;
}

void Monster::setFinishedWithTurn(bool val)
{
    this->finishedWithTurn = val;
}

void Monster::setKi_set(bool val)
{
    this->Ki_set = val;
}


