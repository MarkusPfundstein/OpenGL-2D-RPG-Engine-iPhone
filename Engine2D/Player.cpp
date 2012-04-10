//
//  Player.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 9/25/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "NPC.h"
#include "Player.h"
#include "Object.h"
#include "Texture2D.h"
#include "Characters.h"

Player::Player() : AnimatedSprite()
{
    this->isPlayer = true;
    this->isObject = false;
    this->movingSpeed = 3.0f;
    this->isMoving = false;
    this->viewDirection = DOWN;
    
    Texture2D *charset1 = new Texture2D((char*)"female1.png");
    this->loadData(*charset1, NUMBER_SPRITES_CHARSET_X, NUMBER_SPRITES_CHARSET_Y, 2, (char*)"PLAYER");
    delete charset1;
}

Player::~Player()
{
    Texture2D::delete_texture(this->getTexture());
    this->delete_buffers();
}

u_int8_t Player::getNCharsInGroup() const
{
    return this->nCharsInGroup;
}

void Player::setCharacter(Characters &_newCharacter, u_int8_t atPosition)
{
    this->charactersInGroup[atPosition] = &_newCharacter;
}

Characters& Player::getCharacterAtPosition(u_int8_t position)
{
    return *this->charactersInGroup[position];
}

void Player::setNCharsInGroup(u_int8_t val)
{
    this->nCharsInGroup = val;
}

void Player::update_position(WorldMap &current_map)
{
    this->updateAnimationState();
        
    if (this->isMoving)
    {
        if (this->rect.pos.x < 0)
        {
            this->rect.pos.x = 0;
        }
        else if (this->rect.pos.y < 0-this->rect.height/2)
        {
            this->rect.pos.y = 0-this->rect.height/2;
        }
        else if (this->rect.pos.x+this->rect.width > current_map.get_world_width())
        {
            this->rect.pos.x = current_map.get_world_width()-this->rect.width;
        }
        else if (this->rect.pos.y+this->rect.height > current_map.get_world_height())
        {
            this->rect.pos.y = current_map.get_world_height()-this->rect.height;
        }
        else 
        {
            Point2D temp;
            temp.x = rect.pos.x;
            temp.y = rect.pos.y;
            
            if (this->viewDirection==DOWN)
            {
                this->rect.pos.y += this->movingSpeed;
            }
            else if(this->viewDirection==LEFT)
            {
                this->rect.pos.x -= this->movingSpeed;
            }
            else if(this->viewDirection==RIGHT)
            {
                this->rect.pos.x += this->movingSpeed;
            }
            else if(this->viewDirection==UP)
            {
                this->rect.pos.y -= this->movingSpeed;
            }
            
            if (this->checksForCollision)
            {
                u_int8_t collision=0;
                this->collisionWithTile(current_map, collision);
                
                if (collision == 0)
                {
                    bool alreadyhit = false;
                    int i;
                    Rect2D temp = this->getRect();
                    for (i = 0; i < current_map.getNPCcounter(); i++)
                    {
                        if (!alreadyhit )
                        {
                            Rect2D temp2 = current_map.getNPC(i).getRect();
                            alreadyhit = this->collisionWithObject(temp, temp2, collision, current_map.getNPC(i).getHeight());
                            if (alreadyhit)
                            {
                                
                                // now we can work with the particular npc
                                if (this->viewDirection == UP)
                                    current_map.getNPC(i).setViewDirection(DOWN);
                                else if (this->viewDirection == LEFT)
                                    current_map.getNPC(i).setViewDirection(RIGHT);
                                else if (this->viewDirection == RIGHT)
                                    current_map.getNPC(i).setViewDirection(LEFT);
                                else if (this->viewDirection == DOWN)
                                    current_map.getNPC(i).setViewDirection(UP);
                                
                                current_map.getNPC(i).setInPlayerFocus(true);
                            }
                        }
                    }
                    if (collision==0)
                    {
                        for (i = 0; i < current_map.getObjectCounter(); i++)
                        {
                            if (!alreadyhit)
                            {
                                Rect2D temp2 = current_map.getObject(i).getRect();
                                alreadyhit = this->collisionWithObject(temp, temp2, collision, current_map.getObject(i).getHeight());
                                if (alreadyhit)
                                {
                                    current_map.getObject(i).setInPlayerFocus(true);
                                }
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
    }
}
