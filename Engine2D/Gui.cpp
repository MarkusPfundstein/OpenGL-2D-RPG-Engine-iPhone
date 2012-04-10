//
//  Gui.cpp
//  OpenGL
//
//  Created by Markus Pfundstein on 9/11/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>

#include "Gui.h"
#include "Constants.h"
#include "Player.h"
#include "NPC.h"
#include "Texture2D.h"
#include "Object.h"
#include "BattleEngine.h"

Gui::Gui(const Texture2D& spriteSet)
{
    this->guiSprite = new Sprite2D();
    
    this->guiSprite->loadData(spriteSet, NUMBER_SPRITES_GUISET_X, NUMBER_SPRITES_GUISET_Y, 1, (char*)"gui");
    
    this->guiSprite->setX(0.0f);
    this->guiSprite->setY(SCREEN_HEIGHT_IPHONE-this->guiSprite->getRect().height);
    
    this->interactedNPC = NULL;
    this->interactedObject = NULL;
    //printf("Gui loaded\n");
}

Gui::~Gui()
{
    Texture2D::delete_texture(this->guiSprite->getTexture());
    this->guiSprite->delete_buffers();
    delete this->guiSprite, this->guiSprite = NULL;
}

void Gui::draw(int currentGameState)
{
    //setMatrixToGUIMode();
    
    if (currentGameState == WORLD_MAP)
    {
        this->guiSprite->draw(0);
        
        glTranslatef(SCREEN_WIDTH_IPHONE-128, 0.0f, 0.0f);
        
        this->guiSprite->draw(1);
    }
}

void Gui::handle_player_movement(const Point2D& at_position, Player &_char)
{
    // if player is still animating we will NOT allow any movement
    if (_char.getIsAnimating() == false)
    {
        float width = this->guiSprite->getRect().width;
        float height = this->guiSprite->getRect().height;
        float offset = SCREEN_HEIGHT_IPHONE - height;
        
        if (at_position.x <=
            width && at_position.y >= offset)
        {
            if (at_position.x <= width*2/3 && at_position.x >= width*1/3 &&
                at_position.y <= SCREEN_HEIGHT_IPHONE-height*2/3)
            {
                _char.setViewDirection(UP);
                _char.setMoving(true);
                
            }
            else if (at_position.x <= width*2/3 && at_position.x >= width*1/3 &&
                     at_position.y >= SCREEN_HEIGHT_IPHONE-height*1/3)
            {
                _char.setViewDirection(DOWN);
                _char.setMoving(true);
                
            }
            else if (at_position.x <= width*1/3 &&
                     at_position.y >= SCREEN_HEIGHT_IPHONE-height*2/3 && at_position.y <= SCREEN_HEIGHT_IPHONE-height*1/3)
            {
                _char.setViewDirection(LEFT);
                _char.setMoving(true);
                
            }
            else if (at_position.x >= width*2/3 &&
                     at_position.y >= SCREEN_HEIGHT_IPHONE-height*2/3 && at_position.y <= SCREEN_HEIGHT_IPHONE-height*1/3)
            {
                _char.setViewDirection(RIGHT);
                _char.setMoving(true);
                
            }
        }
    }
}

int Gui::handle_player_action(const Point2D &at_position, WorldMap &worldmap)
{
    Rect2D actionButton;
    
    actionButton.pos.x = 387.0f;
    actionButton.pos.y = 228.0f;
    actionButton.width = 61.0f;
    actionButton.height = 61.0f;
    
    Point2D tempPoint = at_position;
    
    if (isPointInRect(&tempPoint, &actionButton))
    {
        int i;
        for (i = 0; i < worldmap.getNPCcounter(); i++)
        {
            if (worldmap.getNPC(i).getIfInPlayerFocus())
            {
                //printf("Player interaction with %s\n", worldmap.getNPC(i).getName());
                this->interactedNPC = &worldmap.getNPC(i);
                return ACTION_NPC;
            }
        }
        for (i = 0; i < worldmap.getObjectCounter(); i++)
        {
            if (worldmap.getObject(i).getIfInPlayerFocus()) {            
                //printf("Player interaction with %s\n", worldmap.getObject(i).getName());
                this->interactedObject = &worldmap.getObject(i);
                return ACTION_OBJECT;
            }
        }
    }
    
    return NO_ACTION;
}


NPC* Gui::getInteractedNPC()
{
    return this->interactedNPC;
}

Object* Gui::getInteractedObject()
{
    return this->interactedObject;
}

void Gui::freeInteractedNPC()
{
    this->interactedNPC = NULL;
}

void Gui::freeInteractedObject()
{
    this->interactedObject = NULL;
}