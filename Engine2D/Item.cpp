//
//  Item.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 10/22/11.
//  Copyright (c) 2011 The Saints. All rights reserved.
//

#include <iostream>

#include "Item.h"
#include "Helpers.h"
#include "Texture2D.h"
#include "Sprite2D.h"
#include "AnimatedSprite.h"
#include "Characters.h"
#include "BattleEngine.h"

Item::Item()
{
    this->sprite = NULL;
    this->collected = false;
    this->numberCollected = 0;
}

Item::~Item()
{
    this->sprite = NULL;
    this->target = NULL;
    this->engine = NULL;
}

bool Item::create(const char *filename, int tex)
{
    memset(this->script, 0, 40);
    strcpy(script, filename);
    
    this->tex_id = tex;
    
    this->engine = NULL;
    this->target = NULL;
    
    return 1;
}

void Item::pointToTargetAndEngine(Characters &c, BattleEngine &b)
{
    this->engine = &b;
    this->target = &c;
}

void Item::freeTargetAndEngine()
{
    this->engine = NULL;
    this->target = NULL;
}

bool Item::use()
{
    this->parseItemFile();
    
    this->numberCollected -= 1;
    if (this->numberCollected <= 0)
    {
        this->collected = false;
        return 0;
    }
    
    return 1;
}

bool Item::parseItemFile()
{
    const char* path = getPathFromIPhoneFile(this->script);

    ifstream inputFile;

    inputFile.open(path);
    
    if (!inputFile.is_open())
        return 0;
    
    char buffer[400];
    memset(buffer, 0, 400);
    
    // first we parse the target
    inputFile>>buffer;
    if (strcmp(buffer, _ITEM_SCRIPT_TARGET)==0)
    {
        // we know now that the item will affect the target
        
        // now we check how it will increase or decrease s.th.
        inputFile>>buffer;
        bool increase = 0;
        bool decrease = 0;

        if (strcmp(buffer, "+")==0)
            increase = 1;
        else if (strcmp(buffer, "-")==0)
            decrease = 1;
        
        char attribut[40];
        memset(attribut,0,40);
        // now we check what attribute it will change
        inputFile>>buffer;
        strcpy(attribut, buffer);
        
        // now we check how much
        inputFile>>buffer;
        int factor = atoi(buffer);
        
        if (target->getIsAlive())
        {
            // now we execute
            if (strcmp(attribut, _ITEM_SCRIPT_HEALTH)==0)
            {
                int hp = target->getHitpoints();
                if (increase)
                    hp+=factor;
                if (decrease)
                    hp-=factor;
                target->setHitpoints(hp);

            }
            else if (strcmp(attribut, _ITEM_SCRIPT_MANA)==0)
            {
                int mp = target->getManapoints();
                if (increase)
                    mp+=factor;
                if (decrease)
                    mp-=factor;
                target->setManapoints(mp);
            }
        }
        else
            factor = 0;
        // indicate the battle engine to draw the change in value
        Point2D p = target->getSprite().getRect().pos;
        if (decrease)
            factor = -factor;
        engine->setLastvalueChanged(factor);
        engine->setpointOfValueChanged(p);
        engine->setvalueChanged(true);
    }
    
    // than we close
    inputFile.close();
    
    return 1;
}

void Item::pointToSpriteset(Sprite2D &set)
{
    this->sprite = NULL;
    this->sprite = &set;
}

void Item::setCollected(bool b)
{
    this->collected = b;
}

void Item::setNumberCollected(u_int8_t n)
{
    this->numberCollected = n;
    if (this->numberCollected>0)
        this->collected = true;
}

bool Item::getCollected() const
{
    return this->collected;
}

u_int8_t Item::getNumberCollected() const
{
    return this->numberCollected;
}

u_int8_t Item::getTexId() const
{
    return this->tex_id;
}


