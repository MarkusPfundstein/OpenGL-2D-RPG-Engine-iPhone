//
//  Dialog.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 10/1/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>

#include "Dialog.h"
#include "Font.h"
#include "Helpers.h"
#include "Script.h"
#include "Player.h"
#include "Camera.h"
#include "AnimatedSprite.h"
#include "Object.h"
#include "GameEngine.h"
#include "WorldMap.h"
#include "NPC.h"

Dialog::Dialog(Font &withFont, Script &parent, Player &withPlayer, GameEngine &withEngine) : fontUsed(withFont), scriptUsed(parent), playerUsed(withPlayer), engineUsed(withEngine)
{
    this->inputMode = NO_INPUT;

    Texture2D *texDialog = new Texture2D((char*)"dialogs.png");
    this->sprites = new Sprite2D();
    this->sprites->loadData(*texDialog, 2, 2, 1, (char*)"dialog GUI");
    this->sprites->setX(0.0f);
    this->sprites->setY(SCREEN_HEIGHT_IPHONE-this->sprites->getRect().height);
    delete texDialog, texDialog = NULL;
    
    this->objectUsed = NULL;
    this->npcUsed = NULL;
    
    for (int i = 0; i < 4; i++)
    {
        this->rectsGui[i].width = this->sprites->getRect().width;
        this->rectsGui[i].height = this->sprites->getRect().height;
    }

}

Dialog::~Dialog()
{
    Texture2D::delete_texture(this->sprites->getTexture());
    this->sprites->delete_buffers();
    delete this->sprites, this->sprites = NULL;
}

bool Dialog::startDialog(const char *filename, const char* ext)
{
    char completeFileName[40];
    memset(completeFileName, 0, 40);
    
    strcat(completeFileName, filename);
    
    if (ext != NULL)
        strcat(completeFileName, ext);
    
    const char* path = getPathFromIPhoneFile(completeFileName);
    
    this->inputFile.open(path);
    
    if (!this->inputFile.is_open())
    {
        //printf("No file with filename %s found\n", filename);
        this->inputFile.close();
        return false;
    }
    
    return true;
}

int Dialog::readNextLine(int input)
{
    int returnValue = 1;
    char buffer[400];
    memset(buffer, 0, 400);
    
    memset(this->currentString, 0, 400);
    
    do {
        
        if (input == 1)
        {
            // input was 1 before so we search the script for input1 method
            do 
            {
                inputFile>>buffer;
            } while (strcmp(buffer, _SCRIPT_INPUT1)!=0);
            
            this->inputMode = NO_INPUT;
            input = 0;
        }
        else if (input == 2)
        {
            do 
            {
                inputFile>>buffer;
            } while (strcmp(buffer, _SCRIPT_INPUT2)!=0);
            
            this->inputMode = NO_INPUT;
            input = 0;
        }
        else if (input == 3)
        {
            do 
            {
                inputFile>>buffer;
            } while (strcmp(buffer, _SCRIPT_INPUT3)!=0);
            
            this->inputMode = NO_INPUT;
            input = 0;
        }
        
        inputFile>>buffer;
        
        if (strcmp(buffer, _END_OF_DIALOG)==0)
        {
            returnValue = 0;
            break;
        }
        else if (strcmp(buffer, _SCRIPT_ENDIF)==0)
        {
            // do nothing
        }
        else if (strcmp(buffer, _SCRIPT_LOCKNPC)==0)
        {
            char *npcToLock;
            // read name of NPC 
            inputFile>>buffer;
            npcToLock = new char[strlen(buffer)+1];
            memset(npcToLock, 0, strlen(buffer)+1);
            strcpy(npcToLock, buffer);
            
            //printf("Lock NPC: %s\n", npcToLock);
            
            for (int i = 0; i < this->worldUsed->getNPCcounter(); i++) 
            {
                if (strcmp(npcToLock, this->worldUsed->getNPC(i).getName())==0)
                {
                    this->worldUsed->getNPC(i).setMoving(false);
                    this->worldUsed->getNPC(i).setLocked(true);
                }
                    
            }
            
            delete [] npcToLock, npcToLock = NULL;
            
        }
        else if (strcmp(buffer, _SCRIPT_UNLOCKNPC)==0)
        {
            
            char *npcToUnlock;
            // read name of NPC 
            inputFile>>buffer;
            npcToUnlock = new char[strlen(buffer)+1];
            memset(npcToUnlock, 0, strlen(buffer)+1);
            strcpy(npcToUnlock, buffer);
            
            //printf("Unlock NPC: %s\n", npcToUnlock);
            
            for (int i = 0; i < this->worldUsed->getNPCcounter(); i++) 
            {
                if (strcmp(npcToUnlock, this->worldUsed->getNPC(i).getName())==0)
                {
                    this->worldUsed->getNPC(i).setLocked(false);
                }
                
            }
            
            delete [] npcToUnlock, npcToUnlock = NULL;
        }
        else if (strcmp(buffer, _SCRIPT_SETANISTATE)==0)
        {
            int newState;
            char *whoShallAnimate;
            
            inputFile>>buffer;
            whoShallAnimate = new char[strlen(buffer)+1];
            memset(whoShallAnimate, 0, strlen(buffer)+1);
            strcpy(whoShallAnimate, buffer);
            
            inputFile>>buffer;
            newState = atoi(buffer);
            
            if (strcmp(whoShallAnimate, _SCRIPT_PLAYER)==0)
            {
                this->playerUsed.setAnimationState(newState);
                this->playerUsed.setisInAnimationState(true);
            }
            else if (this->objectUsed != NULL && strcmp(whoShallAnimate, _SCRIPT_THIS)==0)
            {
                this->objectUsed->setAnimationState(newState);
            }
            else if (this->npcUsed != NULL && strcmp(whoShallAnimate, _SCRIPT_THIS)==0)
            {
                this->npcUsed->setAnimationState(newState);
                this->npcUsed->setisInAnimationState(true);
            }
            else // we have to deal with an NPC or an object
            {
                int i;
                for (i = 0; i < worldUsed->getNPCcounter(); i++)
                {
                    if (strcmp(whoShallAnimate, worldUsed->getNPC(i).getName())==0)
                    {
                        worldUsed->getNPC(i).setisInAnimationState(true);
                        worldUsed->getNPC(i).setAnimationState(newState);
                    }
                }
                for (i = 0; i < worldUsed->getObjectCounter(); i++)
                {
                    if (strcmp(whoShallAnimate, worldUsed->getObject(i).getName())==0)
                    {
                        worldUsed->getObject(i).setAnimationState(newState);
                    }
                }
            }
            
            delete [] whoShallAnimate, whoShallAnimate = NULL;
        }
        else if (strcmp(buffer, _SCRIPT_RESETANISTATE)==0)
        {
            char *whoShallAnimate;
            inputFile>>buffer;
            whoShallAnimate = new char[strlen(buffer)+1];
            memset(whoShallAnimate, 0, strlen(buffer)+1);
            strcpy(whoShallAnimate, buffer);
            
            if (strcmp(whoShallAnimate, _SCRIPT_PLAYER)==0)
            {
                this->playerUsed.setisInAnimationState(false);
            }
            else if (this->objectUsed != NULL && strcmp(whoShallAnimate, _SCRIPT_THIS)==0)
            {
                this->objectUsed->setisInAnimationState(false);
            }
            else if (this->npcUsed != NULL && strcmp(whoShallAnimate, _SCRIPT_THIS)==0)
            {
                this->npcUsed->setisInAnimationState(false);
            }
            else // we have to deal with an NPC or an object
            {
                int i;
                for (i = 0; i < worldUsed->getNPCcounter(); i++)
                {
                    if (strcmp(whoShallAnimate, worldUsed->getNPC(i).getName())==0)
                    {
                        worldUsed->getNPC(i).setisInAnimationState(false);
                    }
                }
                for (i = 0; i < worldUsed->getObjectCounter(); i++)
                {
                    if (strcmp(whoShallAnimate, worldUsed->getObject(i).getName())==0)
                    {
                        worldUsed->getObject(i).setisInAnimationState(false);
                    }
                }
            }
            
            delete [] whoShallAnimate, whoShallAnimate = NULL;
        }
        else if (strcmp(buffer, _SCRIPT_PLAYANI)==0)
        {
            int startAni;
            int endAni;
            int aniSpeed;
            char *whoShallAnimate;
            
            // read who shall do it
            inputFile>>buffer;
            whoShallAnimate = new char[strlen(buffer)+1];
            memset(whoShallAnimate, 0, strlen(buffer)+1);
            strcpy(whoShallAnimate, buffer);
            
            // read start Animation
            inputFile>>buffer;
            startAni = atoi(buffer);
            // read end Animation
            inputFile>>buffer;
            endAni = atoi(buffer);
            // read speed of animation
            inputFile>>buffer;
            aniSpeed = atoi(buffer);
            
            if (this->objectUsed != NULL && strcmp(whoShallAnimate, _SCRIPT_THIS)==0)
            {
                this->objectUsed->setAnimationState(startAni);
                this->objectUsed->setEndAnimationState(endAni);
                this->objectUsed->setIsAnimating(true);
                this->objectUsed->setAnimationSpeed(aniSpeed);
            }
            else if (this->npcUsed != NULL && strcmp(whoShallAnimate, _SCRIPT_THIS)==0)
            {
                this->npcUsed->setAnimationState(startAni);
                this->npcUsed->setEndAnimationState(endAni);
                this->npcUsed->setIsAnimating(true);
                this->npcUsed->setAnimationSpeed(aniSpeed);
            }
            else if (strcmp(whoShallAnimate, _SCRIPT_PLAYER)==0)
            {
                this->playerUsed.setAnimationState(startAni);
                this->playerUsed.setEndAnimationState(endAni);
                this->playerUsed.setIsAnimating(true);
                this->playerUsed.setAnimationSpeed(aniSpeed);
            }
            else
            {
                int i;
                for (i = 0; i < worldUsed->getNPCcounter(); i++)
                {
                    if (strcmp(whoShallAnimate, worldUsed->getNPC(i).getName())==0)
                    {
                        worldUsed->getNPC(i).setAnimationState(startAni);
                        worldUsed->getNPC(i).setEndAnimationState(endAni);
                        worldUsed->getNPC(i).setIsAnimating(true);
                        worldUsed->getNPC(i).setAnimationSpeed(aniSpeed);
                    }
                }
                for (i = 0; i < worldUsed->getObjectCounter(); i++)
                {
                    if (strcmp(whoShallAnimate, worldUsed->getObject(i).getName())==0)
                    {
                        worldUsed->getObject(i).setAnimationState(startAni);
                        worldUsed->getObject(i).setEndAnimationState(endAni);
                        worldUsed->getObject(i).setIsAnimating(true);
                        worldUsed->getObject(i).setAnimationSpeed(aniSpeed);
                    }
                }
            }
            
            delete [] whoShallAnimate, whoShallAnimate = NULL;
        }
        else if (strcmp(buffer, _SCRIPT_PAUSE_ENGINE)==0)
        {
            // pause command 
            inputFile>>buffer;
            engineUsed.setPauseDuration(atoi(buffer));
            engineUsed.setPaused(true);
            
            returnValue = 2;
            break;
            
        }
        else if (strcmp(buffer, _SCRIPT_INPUTDIALOG)==0)
        {
            // script command to display input dialog
            // parse next object 
            inputFile>>buffer;
            this->inputMode = (INPUTMODE)atoi(buffer);
        }
        else if (strcmp(buffer, _SCRIPT_CHANGEVAL)==0)
        {
            // we got a script command to change a value
            
            // read out first value
            inputFile>>buffer;
            int value = atoi(buffer);
            
            // read out second value
            inputFile>>buffer;
            int8_t toValue = atoi(buffer);
            
            this->scriptUsed.changeGlobalValue(value,toValue);
        }
        else if (strcmp(buffer, _SCRIPT_CHANGEBOOL)==0)
        {
            // we got a script command to change a bool
            
            inputFile>>buffer;
            int value = atoi(buffer);
            inputFile>>buffer;
            bool toValue = atoi(buffer);
            
            this->scriptUsed.changeGlobalBool(value, toValue);
        }
        else if (strcmp(buffer, _SCRIPT_IF)==0)
        {
            int value;
            // we got a script IF
            inputFile>>buffer;
            
            // check if value or bool
            if (strcmp(buffer, _SCRIPT_GLOBALVAL)==0)
            {
                int8_t compareValue;
                
                inputFile>>buffer;
                value = atoi(buffer);
                inputFile>>buffer;
                compareValue = atoi(buffer);
                
                if (!this->scriptUsed.compareValueAt(value, compareValue))
                {
                    //printf("SCRIPT: [Value %d != %hhd, search for ENDIF]\n", value, compareValue);
                    do {
                        inputFile>>buffer;
                    } while (strcmp(buffer, _SCRIPT_ENDIF)!=0);
                }
                else
                {
                    //printf("SCRIPT: [Value %d == %hhd, go on with text]\n", value, compareValue);
                }
            }
            else if (strcmp(buffer, _SCRIPT_GLOBALBOOL)==0)
            {
                bool compareValue;
                inputFile>>buffer;
                value = atoi(buffer);
                inputFile>>buffer;
                compareValue = atoi(buffer);
                
                if (!this->scriptUsed.compareBoolAt(value, compareValue))
                {
                    //printf("SCRIPT: [Switch %d != %d, search for ENDIF]\n", value, compareValue);
                    do {
                        inputFile>>buffer;
                    } while (strcmp(buffer, _SCRIPT_ENDIF)!=0);
                }
                else
                {
                    //printf("SCRIPT: [Switch %d == %d, go on with text]\n", value, compareValue);
                }
            }
        }
        
        else if (strcmp(buffer, _END_OF_LINE) != 0)
        {
            strcat(this->currentString, buffer);
            strcat(this->currentString, " ");
        }
        
    } while (strcmp(buffer, _END_OF_LINE)!=0);
    
    //printf("%s\n", currentString);
    
    return returnValue;
}

void Dialog::clearDialog()
{
    this->inputFile.close();
    
    this->objectUsed = NULL;
    this->npcUsed = NULL;
}

int Dialog::handleInput(const Point2D &atposition, const Camera &camera)
{
    int goOn = -1;
    
    Point2D tempPoint = atposition;
    
    if (this->inputMode == NO_INPUT)
    {
       if (isPointInRect(&tempPoint, &this->rectsGui[0]))
       {
           goOn = this->readNextLine(NO_INPUT);
       }
    }
    else if (this->inputMode >= NO_INPUT)
    {
        tempPoint.x = atposition.x + camera.getRect().pos.x;
        tempPoint.y = atposition.y - camera.getRect().pos.y;
        
        if (isPointInRect(&tempPoint, &this->rectsGui[1]))
        {
            goOn = this->readNextLine(1);
        }
        else if (isPointInRect(&tempPoint, &this->rectsGui[2]))
        {
            goOn = this->readNextLine(2);
        }
        
        if (this->inputMode == INPUT_3_QUESTIONS)
        {
            if (isPointInRect(&tempPoint, &this->rectsGui[3]))
            {
                goOn = this->readNextLine(3);
            }
        }
    }
    
    return goOn;
}

void Dialog::show(Player &player)
{
    // draw 1,2,3 
    if (this->inputMode == INPUT_2_QUESTIONS)
    {
        this->rectsGui[1].pos.x = player.getRect().pos.x-60;
        this->rectsGui[1].pos.y = player.getRect().pos.y-40;
        
        this->rectsGui[2].pos.x = player.getRect().pos.x+30;
        this->rectsGui[2].pos.y = this->rectsGui[1].pos.y;
        
        this->sprites->setPos(rectsGui[1].pos);
        this->sprites->draw(1);
        this->sprites->setPos(rectsGui[2].pos);
        this->sprites->draw(2);
    }
    else if (this->inputMode == INPUT_3_QUESTIONS)
    {
        this->rectsGui[1].pos.x = player.getRect().pos.x-60;
        this->rectsGui[1].pos.y = player.getRect().pos.y-40;
        
        this->rectsGui[2].pos.x = player.getRect().pos.x-15;
        this->rectsGui[2].pos.y = player.getRect().pos.y-60;
        
        this->rectsGui[3].pos.x = player.getRect().pos.x+30;
        this->rectsGui[3].pos.y = this->rectsGui[1].pos.y;
        
        this->sprites->setPos(rectsGui[1].pos);
        this->sprites->draw(1);
        this->sprites->setPos(rectsGui[3].pos);
        this->sprites->draw(3);
        this->sprites->setPos(rectsGui[2].pos);
        this->sprites->draw(2);
    }
    
    // set matrix to GUI mode
    setMatrixToGUIMode();
    // draw the Next Icon
    
    this->rectsGui[0].pos.x = SCREEN_WIDTH_IPHONE-90;
    this->rectsGui[0].pos.y = SCREEN_HEIGHT_IPHONE-this->sprites->getRect().height-32;
    this->sprites->setPos(this->rectsGui[0].pos);
    
    if (this->inputMode == NO_INPUT)
        this->sprites->draw(0);
    
    if (this->currentString != NULL)
        this->fontUsed.draw_string(10.0f, 220.0f, 1.0f, 10.0f, 1.0f, 1.0f, 1.0f, 1.0f, this->currentString);
}

void Dialog::pointToObject(Object &other)
{
    this->objectUsed = &other;
}

void Dialog::pointToNPC(NPC &other)
{
    this->npcUsed = &other;
}

void Dialog::setWorldUsed(WorldMap &newWorldUsed)
{
    this->worldUsed = NULL;
    this->worldUsed = &newWorldUsed;
}
