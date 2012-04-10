//
//  BattleEngine.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 10/15/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <fstream>
using namespace std;
#include "BattleEngine.h"
#include "Gui.h"
#include "Texture2D.h"
#include "AnimatedSprite.h"
#include "Constants.h"
#include "Helpers.h"
#include "Player.h"
#include "Characters.h"
#include "Monster.h"
#include "Font.h"
#include "BattleHelpers.h"
#include "GameEngine.h"
#include "Inventory.h"

BattleEngine::BattleEngine(Font &_f, GameEngine &_e, Inventory &_i) : fontUsed(_f), engineUsed(_e), inventoryUsed(_i)
{
    this->createBuffers();
    this->playerUsed = NULL;
    
    for (int i = 0; i < MAX_MONSTER_PER_BATTLE; i++)
        monsters[i] = NULL;
    
    Texture2D *battleTex = new Texture2D((char*)"battleGUI.png");
    
    this->gui = new Sprite2D();
    this->gui->loadData(*battleTex, 4, 4, 1, (char*)"battle GUI");
    
    delete battleTex;
    
    for (int i = 0; i < MENU_ITEMS_BATTLE; i++)
    {
        this->menuItems[i].isActive = true;
        this->menuItems[i].texId = i;
        this->menuItems[i].menuCode = (MENU_CODES)i;
    }
}

BattleEngine::~BattleEngine()
{
    // delete all memory allocated for background texture
    glDeleteBuffers(1, &this->backgroundTBO);
    glDeleteBuffers(1, &this->backgroundVBO);
    
    Texture2D::delete_texture(this->gui->getTexture());
    this->gui->delete_buffers();
    delete this->gui;
}

void BattleEngine::clearMemory()
{
    Texture2D::delete_texture(this->backgroundTexId);
    
    this->charInFocus = NULL;
    this->monsterInFocus = NULL;
    this->playerUsed = NULL;
    
    for (int i = 0; i < this->monstersInBattle; i++)
        delete this->monsters[i], monsters[i] = NULL;
    
    delete [] itemSelector, itemSelector = NULL;
}

bool BattleEngine::initializeBattle(Player &_player, const char* battleScript)
{    
    bool noError = true;
    this->playerUsed = &_player;
    
    // parse Battlescript
    this->parseBattleScript(battleScript);
    
    float xOffset = 10;
    float yOffset = 60;
    Point2D position = {350,80};
    
    // initialize position for the characters
    for (int i = 0; i < this->playerUsed->getNCharsInGroup(); i++)
    {
        Characters *c = &this->playerUsed->getCharacterAtPosition(i);
        c->getSprite().setPos(position);
        c->setIsMoving(false);
        c->setActionPoints(10);
        c->setIsAttacking(false);
        Point2D weaponpos = c->getSprite().getRect().pos;
        weaponpos.y += c->getSprite().getRect().height/2;
        c->getWeaponSprite().setPos(weaponpos);
        
        position.x += xOffset;
        position.y += yOffset;
        c = NULL;
    }
    
    this->reallocItems = false;
    this->allocMemForItems();
    
    bool playerStarts = true;
    if (playerStarts)
        this->playerTurn = true;
    else
        this->playerTurn = false;
    
    this->currentBattleState = WAIT_FOR_INPUT;
    
    this->charInFocus = NULL;
    this->monsterInFocus = NULL;
    this->setTimerBack();
    this->isPaused = false;
    this->valueChanged = 0;
    this->itemCurrentlyUsed = NULL;
    
    return noError;
}

void BattleEngine::allocMemForItems()
{
    if (reallocItems)
    {
        delete [] itemSelector, itemSelector = NULL;
        reallocItems = false;
    }
    
    float startX = 32;
    Rect2D r = {startX,62,inventoryUsed.getItemSet().getRect().width,inventoryUsed.getItemSet().getRect().height};
    int offsetX = r.width + r.width/2;
    int offsetY = r.height + r.height/2;
    int n = this->itemsInBattle = inventoryUsed.getItemsUsed();
    this->itemSelector = new ItemSelector[n];
    this->itemsDisplayed = 0;       // <= only used to disable the item button when count is 0
    // go through all the items the character has in inventory and create a rect for them
    for (int i = 0; i < n; i++)
    {
        Item *t = &inventoryUsed.getItemsAtPosition(i);
        if (t->getCollected())
        {
            itemSelector[i].rect = r;
            itemSelector[i].selected = false;
            r.pos.x += offsetX;
            itemsDisplayed++;
            if (itemsDisplayed%3==0)
            {
                r.pos.y += offsetY;
                r.pos.x = startX;
            }
        }
        t = NULL;
    }
}

int BattleEngine::update()
{
    int goOn = 1;
    int i;
    static int monstersDead = 0;
    static int playersDead = 0;
    
    if (this->playerTurn)
        this->handlePlayers();
    
    if (!this->playerTurn)
        this->handleMonster();
    
    
    // update characters    
    for (int i = 0; i < this->getPlayerUsed().getNCharsInGroup(); i++)
    {   
        Characters *c = &this->getPlayerUsed().getCharacterAtPosition(i);
        // if char is alive
        if (c->getIsAlive())
        {
            // we check if his hitpoints are under 0
            if (c->getHitpoints() <= 0 )
            {
                c->setIsAlive(false);
                c->setHitpoints(0);
                playersDead++;
            }
            // we check if he is defending
            if (c->getIsDefending() && !c->getSprite().getIsAnimating())
            {
                // and NOT moving (attacking)
                if (!c->getIsMoving())
                    c->getSprite().setAnimationState(ANI_STATE_DEF);
            }
            // alive+not moving = stand
            // check if he is animating right now
            else if (!c->getSprite().getIsAnimating())
                c->getSprite().setAnimationState(ANI_STATE_STANDING);
            
            // update position of spirte
            c->getSprite().updateAnimationState();
            
            // of weapon
            c->getWeaponSprite().updateAnimationState();
        }
        else
        {
            // player is dead 
           c->getSprite().setAnimationState(ANI_STATE_DEAD);
        }
        
        c = NULL;
    }
    
    for (i = 0; i < this->monstersInBattle; i++)
    {
        // check monster if its still alive and increment the dead counter respectively
        if (this->monsters[i]->getHitpoints()<=0 && this->monsters[i]->getIsAlive())
        {
            this->monsters[i]->setHitpoints(0);
            this->monsters[i]->setIsAlive(false);
            monstersDead++;
        }
        else
        {
            this->monsters[i]->getSprite().updateAnimationState();
        }
    }
    
    // end battle if monsters dead equals the number of monsters in battle
    if (monstersDead==this->monstersInBattle)
    {
        goOn = -1; 
        monstersDead = 0;
    }
    
    // same goes for characters
    if (playersDead==this->playerUsed->getNCharsInGroup())
    {
        goOn = 0;
        playersDead = 0;
    }
    
    if (timerIsOn)
        timer++;
    
    if (timer > this->timerSpeed)
    {
        this->setTimerBack();
        this->isPaused = false;
    }

    return goOn;
}

void BattleEngine::handlePlayers()
{
    // handle animations
    if (this->charInFocus != NULL)
    {
        if (this->currentBattleState == WAIT_FOR_INPUT)
        {
            // set all menu items ALWAYS active again
            for (int i = 0; i < MENU_ITEMS_BATTLE-1; i++)
                menuItems[i].isActive = 1;
            
            // and than check which one shall not be active
            if (this->charInFocus->getActionpoints() < AP_ATTACK)
                menuItems[ATTACK].isActive = false;
            if (this->charInFocus->getActionpoints() < AP_DEFENSE || this->charInFocus->getIsDefending())
                menuItems[DEFEND].isActive = false;
            if (this->charInFocus->getActionpoints() < AP_MAGIC)
                menuItems[MAGIC].isActive = false;
            if (this->charInFocus->getActionpoints() < AP_ITEM || this->itemsDisplayed <= 0)
                menuItems[ITEM].isActive = false;
        }
        // we have selected a player AND a monster AND are not waiting
        if (this->monsterInFocus != NULL && !this->isPaused)
        {
            Rect2D t = this->charInFocus->getTargetRect();
            // we update players position
            // result = 1 means that the first location is reached , so the player is moving TOWARDS the monster
            int result = this->charInFocus->updatePosition(this->charInFocus->getAttackType(), t, this->charInFocus->getIsAttacking(), 1, 1);
            if (result==1)
            {
                // this is the moment when the player is at the enemy 
                this->isPaused = true;
                this->charInFocus->setIsAttacking(false);
                this->charInFocus->setTargetRect(rectMake(this->charInFocus->getOldPos().x, this->charInFocus->getOldPos().y, this->charInFocus->getSprite().getRect().width, this->charInFocus->getSprite().getRect().height));
                this->charInFocus->setOldPos(this->charInFocus->getSprite().getRect().pos);
                
                // let weapon animate
                this->charInFocus->getWeaponSprite().setIsAnimating(true);
                this->charInFocus->getWeaponSprite().setEndAnimationState(3);
                this->charInFocus->getWeaponSprite().setAnimationSpeed(5);   
                this->charInFocus->getWeaponSprite().resetAfterEndAnimationStateIsReached(true);
                // when the animation stops we call damagePlayerToMonster which will give the damage and animate the monster hurt
                this->charInFocus->getWeaponSprite().setEndAnimationFunction(&BattleEngine::damagePlayerToMonster, *this);
                
                // we let the engine wait for 60 frames
                this->startTimer(60);
                
            }
            // player reached his original position
            else if (result == 0)
            {
                this->monsterInFocus->getSprite().setAnimationState(0);
                this->charInFocus->setIsMoving(false);
                this->monsterInFocus = NULL;
            }
        }
    }
}

void BattleEngine::handleMonster()
{
    static int monstersWhoDidSth = 0;
    
    // handle monster which turn it is
    if (monsters[monstersWhoDidSth]->getIsAlive())
    {
        if ( !monsters[monstersWhoDidSth]->getFinishedWithTurn())
        {
            this->monsters[monstersWhoDidSth]->handleKI(*this);
            monsters[monstersWhoDidSth]->updateMonster(*this);

        }
        else
            monstersWhoDidSth++;
    }
    else
        monstersWhoDidSth++;
    
    // end battle if condition = true 
    if (monstersWhoDidSth == this->monstersInBattle)
    {
        monstersWhoDidSth = 0;
        this->resetTurn();
    }
}

void BattleEngine::resetTurn()
{
    if (this->playerTurn)
    {
        this->playerTurn = false;
        
        for (int i = 0; i < this->playerUsed->getNCharsInGroup(); i++)
        {
            // increase player action points
            int ap = this->playerUsed->getCharacterAtPosition(i).getActionpoints();
            ap += AP_GAIN_PER_TURN;
            this->playerUsed->getCharacterAtPosition(i).setActionPoints(ap);
        }
        
        for (int i = 0; i < this->monstersInBattle; i++)
        {
            this->monsters[i]->setIsDefending(false);
        }
        
        return;
    }
    else if (!this->playerTurn)
    {
        this->playerTurn = true;

        for (int i = 0; i < this->monstersInBattle; i++)
        {
            if (this->monsters[i]->getIsAlive())
            {
                this->monsters[i]->setFinishedWithTurn(false);
                this->monsters[i]->setKi_set(false);
                // increase monster action points
                int ap = this->monsters[i]->getActionpoints();
                ap += AP_GAIN_PER_TURN;
                this->monsters[i]->setActionPoints(ap);
            }
        }
        
        for (int i = 0; i <this->playerUsed->getNCharsInGroup(); i++)
        {
            this->playerUsed->getCharacterAtPosition(i).setIsDefending(false);
        }
        
        return;
    }
}

void BattleEngine::handleInput(const Point2D &at_position)
{    
    if (this->playerTurn && !this->isPaused)
    {
        Point2D point = at_position;
        if (this->currentBattleState == WAIT_FOR_INPUT)
        {
            if (this->charInFocus == NULL)
            {
                // check if player ends turn
                if (isPointInRect(&point, &menuItems[END_TURN].rect))
                {
                    this->resetTurn();
                }
                else
                {
                    // check if player selects a char
                    Rect2D characterRect;

                    for(int i = 0; i < this->getPlayerUsed().getNCharsInGroup(); i++)
                    {
                        characterRect = this->getPlayerUsed().getCharacterAtPosition(i).getSprite().getRect();
                        if (isPointInRect(&point, &characterRect) && this->getPlayerUsed().getCharacterAtPosition(i).getIsAlive() && this->charInFocus == NULL)
                        {
                            this->charInFocus = &this->getPlayerUsed().getCharacterAtPosition(i);
                        }                    
                    }
                }
            }
            // player selected a char aready
            else
            {
                if (!this->charInFocus->getIsMoving())
                {
                    // check for the menu items
                    for (int i = 0; i < MENU_ITEMS_BATTLE-1; i++)
                    {                        
                        if (isPointInRect(&point, &menuItems[i].rect) && menuItems[i].isActive)
                        {
                            switch (menuItems[i].menuCode)
                            {
                                case ATTACK:
                                    this->currentBattleState = PLAYER_ATTACK;
                                    break;
                                case MAGIC:
                                    break;
                                case ITEM:
                                    this->currentBattleState = PLAYER_ITEM;
                                    break;
                                case DEFEND:
                                    this->charInFocus->Defend();
                                    this->charInFocus->getSprite().setAnimationState(ANI_STATE_DEF);
                                    break;
                                case CANCEL:                    
                                    this->freeCharInFocus();
                                    this->currentBattleState = WAIT_FOR_INPUT;
                                    break;
                                case END_TURN:
                                    break;
                            }
                        }
                    }
                }
            }
        }
        // we are not in Wait for Input
        else if (this->currentBattleState == PLAYER_ATTACK)
        {
            // cancel pressed
            if (isPointInRect(&point, &menuItems[CANCEL].rect))
            {
                this->currentBattleState = WAIT_FOR_INPUT;
            }
            // clicked somewhere else
            else
            {
                // clicks on monster
                for (int i = 0; i < this->monstersInBattle; i++)
                {
                    Rect2D monsterRect = this->monsters[i]->getSprite().getRect();
                    if (isPointInRect(&point, &monsterRect) && this->monsters[i]->getIsAlive())
                    {
                        if (this->currentBattleState == PLAYER_ATTACK && this->charInFocus->getActionpoints() >= AP_ATTACK)
                        {
                            // deduct action points needed for that action
                            int ap = this->charInFocus->getActionpoints();
                            ap -= AP_ATTACK;
                            this->charInFocus->setActionPoints(ap);
                            
                            // set the monster in focus
                            this->monsterInFocus = this->monsters[i];
                            
                            // set back to wait for input state
                            this->currentBattleState = WAIT_FOR_INPUT;
                            
                            // set moving state, attacktype, save old Position and set target Coordinates
                            this->charInFocus->setIsMoving(true);
                            this->charInFocus->setAttackType(MELEE);
                            this->charInFocus->setOldPos(this->charInFocus->getSprite().getRect().pos);
                            this->charInFocus->setIsAttacking(true);
                            this->charInFocus->setTargetRect(this->monsterInFocus->getSprite().getRect());
                            
                            this->charInFocus->getSprite().setAnimationState(ANI_STATE_STANDING);
                        }
                    }
                }
            }
        }
        else if (this->currentBattleState==PLAYER_ITEM || this->currentBattleState == ITEM_SELECTED)
        {
            if (this->currentBattleState == PLAYER_ITEM)
            {
                // cancel pressed
                if (isPointInRect(&point, &menuItems[CANCEL].rect))
                {
                    this->currentBattleState = WAIT_FOR_INPUT;
                }
                else
                {
                    for (int i = 0; i < this->itemsInBattle; i++)
                    {
                        if (isPointInRect(&point, &itemSelector[i].rect))
                        {
                            if (inventoryUsed.getItemsAtPosition(i).getCollected())
                            {
                                this->currentBattleState = ITEM_SELECTED;
                                itemSelector[i].selected = true;
                            }
                        }
                        else
                            itemSelector[i].selected = false;
                    }
                }
            }
            else if (this->currentBattleState == ITEM_SELECTED)
            {
                // cancel pressed
                if (isPointInRect(&point, &menuItems[CANCEL].rect))
                {
                    this->currentBattleState = PLAYER_ITEM;
                    for (int i = 0; i < this->itemsInBattle; i++)
                        itemSelector[i].selected = false;
                }
                else
                {
                    Characters* temp = NULL;
                    // clicks on monster
                    for (int i = 0; i < this->monstersInBattle; i++)
                    {
                        Rect2D monsterRect = this->monsters[i]->getSprite().getRect();
                        if (isPointInRect(&point, &monsterRect) && this->monsters[i]->getIsAlive())
                        {
                            temp = static_cast<Characters*>(this->monsters[i]);
                        }
                    }
                    // clicks on characters
                    for (int i = 0; i < this->playerUsed->getNCharsInGroup(); i++)
                    {
                        Rect2D t = this->playerUsed->getCharacterAtPosition(i).getSprite().getRect();
                        if (isPointInRect(&point, &t))
                            temp = &this->playerUsed->getCharacterAtPosition(i);
                    }
                    
                    // temp must be not NULL if player chose a target
                    if (temp != NULL)
                    {
                        for (int i = 0; i < itemsInBattle; i++)
                        {
                            if (itemSelector[i].selected)
                            {
                                Item *t = &inventoryUsed.getItemsAtPosition(i);
                                if (t->getNumberCollected()>0)
                                {
                                    int ap = this->charInFocus->getActionpoints();
                                    ap -= AP_ITEM;
                                    this->charInFocus->setActionPoints(ap);
                                    // NECESSARY!!!!
                                    t->pointToTargetAndEngine(*temp, *this);
                                    // adjust the pointer to the right adress
                                    this->itemCurrentlyUsed = t;
                                    
                                    // initialize animation
                                    // reset has to be called before setAnimationState to reset to 0 
                                    this->charInFocus->getSprite().resetAfterEndAnimationStateIsReached(true);
                                    this->charInFocus->getSprite().setIsAnimating(1);
                                    this->charInFocus->getSprite().setAnimationSpeed(30);
                                    this->charInFocus->getSprite().setAnimationState(5);
                                    this->charInFocus->getSprite().setEndAnimationState(6);
                                    // call the useItem function after the animation ends
                                    this->charInFocus->getSprite().setEndAnimationFunction(&BattleEngine::useItem, *this);
                                    
                                    this->startTimer(30);
                                    this->isPaused = true;
                                    
                                    this->currentBattleState = WAIT_FOR_INPUT;
                                }
                                t = NULL;
                            }
                        }
                    }
                    temp = NULL;
                }
            }
        }
    }
    // not player turn, no input
    else
        return;
}

void BattleEngine::damagePlayerToMonster()
{
    if (this->charInFocus != NULL && this->monsterInFocus != NULL)
    {
        this->lastvalueChanged = -giveDamageTo(*this->monsterInFocus, *this->charInFocus, this->charInFocus->getAttackType());
        this->valueChanged = true;
        this->pointOfValueChanged = this->monsterInFocus->getSprite().getRect().pos;
        this->monsterInFocus->getSprite().setIsAnimating(true);
        this->monsterInFocus->getSprite().setAnimationSpeed(2);
        this->monsterInFocus->getSprite().setEndAnimationState(1);
        this->monsterInFocus->getSprite().resetAfterEndAnimationStateIsReached(1);
    }
}

void BattleEngine::useItem()
{
    if (!itemCurrentlyUsed->use())
    {
        this->reallocItems = true;
        // an item count reached 0, so we have to realloc everything
        allocMemForItems();
    }
    itemCurrentlyUsed->freeTargetAndEngine();
    itemCurrentlyUsed = NULL;
}

void BattleEngine::draw()
{
    // first we draw the background
    glBindBuffer(GL_ARRAY_BUFFER, this->backgroundVBO);
    glVertexPointer(3, GL_FLOAT, 0, (char*)NULL);
    glBindTexture(GL_TEXTURE_2D, this->backgroundTexId);
    glBindBuffer(GL_ARRAY_BUFFER, this->backgroundTBO);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)NULL);
    
    glDisable(GL_BLEND);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);    
    
    glEnable(GL_BLEND);
    
    int i;

    // than the monsters
    for (i = 0; i < this->monstersInBattle; i++)
    {
        if (this->monsters[i]->getIsAlive())
            this->monsters[i]->getSprite().drawAnimated();
    }
    
    // than we draw the four characters 
    for (i = 0; i < this->playerUsed->getNCharsInGroup(); i++)
    {
        this->playerUsed->getCharacterAtPosition(i).getSprite().drawAnimated();
        // and their weapons
        this->playerUsed->getCharacterAtPosition(i).getWeaponSprite().drawAnimated();
    }
    
    // than we draw the damage
    this->drawChangeInValue();

    if (this->charInFocus != NULL)
    {
        if (!this->charInFocus->getIsMoving())
        {
            Rect2D tempRect = this->charInFocus->getSprite().getRect();

            if (this->currentBattleState == WAIT_FOR_INPUT)
            {
                Point2D charaMidpoint = getMidpointOf(&tempRect);
                this->drawMenuAround(charaMidpoint);
            }
            else if (this->currentBattleState == PLAYER_ATTACK)
            {
                // draw cancel menu to the right of the selected char
                this->gui->setPos(menuItems[CANCEL].rect.pos);
                this->gui->draw(CANCEL);
                this->fontUsed.draw_string(180, 30, 1.0, 10, 0.8f, 0, 0.2f, 0.8f, "ATTACK: #CHOOSE TARGET");
            }
            else if (this->currentBattleState == PLAYER_ITEM)
            {
                Sprite2D *itemSet = &this->inventoryUsed.getItemSet();
                // draw item menu as a grid 
                for (int i = 0; i < this->inventoryUsed.getItemsUsed(); i++)
                {
                    Item *item = &this->inventoryUsed.getItemsAtPosition(i);
                    
                    if (item->getCollected())
                    {
                        itemSet->setPos(this->itemSelector[i].rect.pos);
                        itemSet->draw(item->getTexId());
                        Point2D p = this->itemSelector[i].rect.pos;
                        p.x += itemSet->getRect().width-20;
                        p.y += itemSet->getRect().height-10;
                        this->fontUsed.draw_string(p.x, p.y, 1, 10, 1.0f, 1.0f, 1.0f, 1.0f, "%02d", item->getNumberCollected());
                    }
                    
                    item = NULL;
                }
                
                this->gui->setPos(menuItems[CANCEL].rect.pos);
                this->gui->draw(CANCEL);
                
                itemSet = NULL;
            }
            else if (this->currentBattleState == ITEM_SELECTED)
            {
                this->gui->setPos(menuItems[CANCEL].rect.pos);
                this->gui->draw(CANCEL);
                this->fontUsed.draw_string(180, 30, 1.0, 10, 0.8f, 0, 0.2f, 0.8f, "ITEM: #CHOOSE TARGET");
            }
                
            this->fontUsed.draw_string(0, 0, 1.0, 10, 0.8f, 0, 0.2f, 0.8f, "%s HP:%d/%d MP:%d/%d LP:%d AP:%d", this->charInFocus->getSprite().getName(), this->charInFocus->getHitpoints(), this->charInFocus->getMaxHitpoints(), this->charInFocus->getManapoints(), this->charInFocus->getMaxManapoints(),this->charInFocus->getLimitpoints(), this->charInFocus->getActionpoints());
        }
    }
    else
    {        
        // Menu Item for ending a turn
        if (this->playerTurn)
        {
            menuItems[END_TURN].rect = rectMake(400, 250, 64, 64);
            this->gui->setPos(menuItems[END_TURN].rect.pos);
            this->gui->draw(END_TURN);
        }
    }

    if (SHOW_MONSTER_INFO)
    {
        float offset = 0;
        for (int i = 0; i < this->monstersInBattle; i++)
        {
            this->fontUsed.draw_string(10, 220+offset, 1.0, 10, 0.8f, 0, 0.2f, 0.8f, "Monster:%d HP:%d/%d MP:%d/%d AP:%d", i, this->monsters[i]->getHitpoints(), this->monsters[i]->getMaxHitpoints(), this->monsters[i]->getManapoints(), this->monsters[i]->getManapoints(),this->monsters[i]->getActionpoints());
            offset += 20;
        }
    }
}

void BattleEngine::drawChangeInValue()
{
    static int Timer = 0;
    static float alpha = 1.0f;
    if (valueChanged)
    {
        Timer++;
        // draw damage at pointOfValueChanged
        
        float r;
        float g;
        float b;
        
        if (this->lastvalueChanged >= 0)
        {
            r = 0.0f;
            g = 1.0f;
            b = 0.0f;
        }
        else if (this->lastvalueChanged == 0)
        {
            r = 0.0f;
            g = 0.0f;
            b = 0.0f;
        }
        else
        {
            r = 1.0f;
            g = 0.0f;
            b = 0.0f;
        }
        
        
        this->fontUsed.draw_string(pointOfValueChanged.x, pointOfValueChanged.y, 1.0, 10, r, g, b, alpha, "%d", this->lastvalueChanged);
        
        // fancy animation
        pointOfValueChanged.y--;
        alpha -= 1/WAIT_TIME;
        
        if (Timer>WAIT_TIME)
        {
            Timer=0;
            valueChanged = 0;
            alpha = 1.0f;
        }
    }
}

void BattleEngine::drawMenuAround(Point2D &_p)
{
    Point2D t;
    static const float radius = 62.0f;
    
    int i;
    float angle = 306.0f;
    float offset_angle = 360/(MENU_ITEMS_BATTLE-1);
    for (i = 0; i < MENU_ITEMS_BATTLE-1; i++)
    {
        t.x = _p.x + cosf(angle*PI/180.0f) * radius;
        t.y = _p.y + sinf(angle*PI/180.0f) * radius;
        
        t.x -= this->gui->getRect().width/2;
        t.y -= this->gui->getRect().height/2;
        
        this->gui->setPos(t);
        
        if (!this->menuItems[i].isActive)
            glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
        
        this->gui->draw(menuItems[i].texId);

        if (!this->menuItems[i].isActive)
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        
        angle -= offset_angle;
        
        this->menuItems[i].rect = rectMake(t.x, t.y, 64, 64);
    }
}

void BattleEngine::startTimer(float time)
{
    this->timerIsOn = true;
    this->timerSpeed = time;
}

void BattleEngine::setTimerBack()
{
    this->timerIsOn = false;
    this->timer = 0;
}

void BattleEngine::freeCharInFocus()
{
    this->charInFocus = NULL;
}

void BattleEngine::parseBattleScript(const char *filename)
{
    const char* path = getPathFromIPhoneFile(filename);
    
    char buffer[40];
    memset(buffer, 0, 40);
    
    ifstream inputFile;
    inputFile.open(path);
    
    // parsing.
    //first we parse the texture to load
    inputFile>>buffer;
    
    // load texture for background
    Texture2D *backTex = new Texture2D(buffer);
    this->backgroundTexId = backTex->texture;
    delete backTex;
    
    // than we load all the enemys
    // first we parse the number of enemys in this battle
    inputFile>>buffer;
    this->monstersInBattle = atoi(buffer);
    
    Point2D monsterCoordinate;
    // than we parse through all the monsters
    for (int i = 0; i < this->monstersInBattle; i++)
    {
        this->monsters[i] = new Monster();
        // first we parse the texture the monster will use
        inputFile>>buffer;
        
        Texture2D *tempTex = new Texture2D(buffer);
        this->monsters[i]->create(*tempTex, 2, 1, buffer);
        delete tempTex, tempTex = NULL;
        
        // than the coordinates
        inputFile>>buffer;
        monsterCoordinate.x = atoi(buffer);
        inputFile>>buffer;
        monsterCoordinate.y = atoi(buffer);
        
        this->monsters[i]->getSprite().setPos(monsterCoordinate);
        
        // than the KI Flag
        inputFile>>buffer;
        KI_FLAG flagToSet = this->setFlagForMonster(buffer);
        this->monsters[i]->setKI_Flag(flagToSet);
        
        // than the hitpoints
        inputFile>>buffer;
        this->monsters[i]->setMaxHitpoints(atoi(buffer));
        this->monsters[i]->setHitpoints(atoi(buffer));

        
        // than the manapoints
        inputFile>>buffer;
        this->monsters[i]->setMaxManapoints(atoi(buffer));
        this->monsters[i]->setManapoints(atoi(buffer));

        
        // than the action points
        inputFile>>buffer;
        this->monsters[i]->setActionPoints(atoi(buffer));
        
        // attack
        inputFile>>buffer;
        this->monsters[i]->setAttackStrength(atoi(buffer));
        
        // defense
        inputFile>>buffer;
        this->monsters[i]->setDefense(atoi(buffer));
        
        
        char monsterName[40];
        memset(monsterName, 0, 40);
        sprintf(monsterName, "Monster: %d", i);
        this->monsters[i]->getSprite().setName(monsterName);
        
    }
    
    inputFile.close();
}

enum KI_FLAG BattleEngine::setFlagForMonster(const char *buffer)
{
    if (strcmp(buffer, _KI_SCRIPT_KI_AGGRO)==0)
        return KI_AGGRO;
    else if (strcmp(buffer, _KI_SCRIPT_KI_DEFENSE)==0)
        return KI_DEFENSE;
    else
        return STANDARD_FLAG;
}

void BattleEngine::createBuffers()
{
    const GLfloat vertices[] = {
        SCREEN_WIDTH_IPHONE, 0, 0,
        0, 0, 0,
        0, SCREEN_HEIGHT_IPHONE, 0, 
        0, SCREEN_HEIGHT_IPHONE, 0,
        SCREEN_WIDTH_IPHONE, SCREEN_HEIGHT_IPHONE, 0,
        SCREEN_WIDTH_IPHONE, 0, 0,
    };
    
    glGenBuffers(1, &this->backgroundVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->backgroundVBO);
    glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    
    const GLfloat texcoords[] = {
        1, 0,
        0, 0,
        0, 1,
        0, 1,
        1, 1, 
        1, 0
    };
    
    glGenBuffers(1, &this->backgroundTBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->backgroundTBO);
    glBufferData(GL_ARRAY_BUFFER, 48, texcoords, GL_STATIC_DRAW);
}

void BattleEngine::freeMonsterInFocus()
{
    this->monsterInFocus = NULL;
}

Player& BattleEngine::getPlayerUsed() const
{
    return *this->playerUsed;
}

bool BattleEngine::getPlayerTurn() const
{
    return this->playerTurn;
}



void BattleEngine::setLastvalueChanged(int val)
{
    this->lastvalueChanged = val;
}

void BattleEngine::setpointOfValueChanged(Point2D &p)
{
    this->pointOfValueChanged = p;
}

void BattleEngine::setvalueChanged(bool val)
{
    this->valueChanged = val;
}
