//
//  Script.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 9/26/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include <fstream>
using namespace std;
#include "Script.h"
#include "ScriptDef.h"
#include "Helpers.h"
#include "Texture2D.h"
#include "NPC.h"
#include "Dialog.h"
#include "Object.h"
#include "Constants.h"
#include "GameEngine.h"
#include "Sprite2D.h"
#include "Item.h"
#include "Inventory.h"

Script::Script()
{
    this->dialog = NULL;

    this->globalValues = (int8_t*)malloc(MAX_GLOBAL_VARIABLES*sizeof(int8_t));
    this->globalBools = (bool*)malloc(MAX_GLOBAL_VARIABLES*sizeof(bool));
    
    for (int i = 0; i < MAX_GLOBAL_VARIABLES; i++)
    {
        this->globalValues[i] = 0;
        this->globalBools[i] = false;
    }
}

Script::~Script()
{
    delete this->dialog, this->dialog = NULL;
    free(this->globalValues), this->globalValues = NULL;
    free(this->globalBools), this->globalBools = NULL;
}

void Script::parseNewWorldScript(const char *filename, WorldMap &worldmap)
{
    const char* path = getPathFromIPhoneFile(filename);
    
    //printf("Open ScriptFile: %s\n", filename);
    
    ifstream inputFile;
    inputFile.open(path);
    
    inputFile.seekg(0, ios::beg);
    
    char buffer[40];
    memset(buffer, 0, 40);
    
    inputFile >> buffer;
    //printf("%s\n", buffer);
    
    while (strcmp(buffer, _END_OF_SCRIPT)!=0)
    {
        inputFile >> buffer;
        
        if (strcmp(buffer, _SCRIPT_NEW)==0)
        {
            //printf("SCRIPT: NEW spotted\n");
            inputFile >> buffer;
            if (strcmp(buffer, _SCRIPT_TRANSPORTTILE)==0)
            {
                //printf("SCRIPT: TRANSPORTTILE spotted\n");
                // allocate new temporary transporttile
                TempTileWithFlags *temp = new TempTileWithFlags;

                
                // read data into memory
                // point where the tile is going to be
                inputFile >> buffer;
                temp->data = atoi(buffer);
                inputFile >> buffer;
                temp->data |= atoi(buffer)<<8;
                // destination tile in new map
                inputFile >> buffer;
                temp->data |= atoi(buffer)<<16;
                inputFile >> buffer;
                temp->data |= atoi(buffer)<<24;
                // new map name
                inputFile >> buffer;
                temp->_string = new char[strlen(buffer)+1];
                memset(temp->_string, 0, strlen(buffer)+1);
                strcpy(temp->_string, buffer);
                
                //printf("%d, %d, %d, %d, %s\n", temp->data&0xff, temp->data>>8&0xff, temp->data>>16&0xff, temp->data>>24&0xff, temp->_string);
                
                // write info into worldmap file 
                worldmap.addTileFlagAt(temp->data&0xff, temp->data>>8&0xff, TILE_FLAG_TRANSPORTTILE, temp->data>>16&0xff, temp->data>>24&0xff, temp->_string);
                
                delete [] temp->_string, temp->_string = NULL;
                delete temp, temp = NULL;
            }
            else if (strcmp(buffer, _SCRIPT_EVENTTILE)==0)
            {
                //printf("SCRIPT: EVENTTILE spotted\n");
                
                TempTileWithFlags *temp = new TempTileWithFlags;
                
                // read out location
                inputFile >> buffer;
                temp->data = atoi(buffer);
                inputFile >> buffer;
                temp->data |= atoi(buffer)<<8;
                
                // read out string
                inputFile >> buffer;
                temp->_string = new char[strlen(buffer)+1];
                memset(temp->_string, 0, strlen(buffer)+1);
                strcpy(temp->_string, buffer);
                
                //printf("%d, %d, %s\n", temp->data&0xff, temp->data>>8&0xff, temp->_string);
                
                worldmap.addTileFlagAt(temp->data&0xff, temp->data>>8&0xff, TILE_FLAG_EVENTTILE, 0, 0, temp->_string);
                
                delete [] temp->_string, temp->_string = NULL;
                delete temp, temp = NULL;
            }
            else if (strcmp(buffer, _SCRIPT_NPC)==0)
            {
                //printf("SCRIPT: NPC spotted\n");
                
                // create all temp stuff we need for creating NPCS
                Texture2D *tempTex = NULL;
                NPC *tempNPCs = NULL;
                Point2D position;
                position.x = 0.0f;
                position.y = 0.0f;
                int counter = 0;
                int ki_flag = 0;
                char *name;
                
                // read how much NPCS we will crate
                inputFile>>buffer;
                counter = atoi(buffer);
                //printf("SCRIPT: will create %d npcs\n", counter);
                
                // create tempNPCs Array
                tempNPCs = new NPC[counter];
                
                // read 'counter' times the loop
                for (int i = 0; i < counter; i++)
                {
                    // texture first
                    inputFile>>buffer;
                    if (tempTex == NULL)
                        tempTex = new Texture2D(buffer);
                    
                    // position
                    inputFile>>buffer;
                    position.x = atoi(buffer);
                    inputFile>>buffer;
                    position.y = atoi(buffer);
                    
                    // KI-flag
                    inputFile>>buffer;
                    if (strcmp(buffer, _KIFLAG_STAND_DOWN)==0)
                        ki_flag = NPC_STAND_DOWN;
                    else if (strcmp(buffer, _KIFLAG_STAND_LEFT)==0)
                        ki_flag = NPC_STAND_LEFT;
                    else if (strcmp(buffer, _KIFLAG_STAND_RIGHT)==0)
                        ki_flag = NPC_STAND_RIGHT;
                    else if (strcmp(buffer, _KIFLAG_STAND_UP)==0)
                        ki_flag = NPC_STAND_UP;
                    else if (strcmp(buffer, _KIFLAG_WALKAROUND)==0)
                        ki_flag = NPC_WALKAROUND;
                    else
                        ki_flag = NPC_STAND_DOWN;
                    
                    // Name
                    inputFile>>buffer;
                    name = new char[strlen(buffer)];
                    strcpy(name, buffer);
                    
                    // now as we loaded everything we fill it in the temp NPC at position i and set on the tile
                    tempNPCs[i].create(*tempTex, name, ki_flag);
                    tempNPCs[i].setOnTile(position);
                    
                    // delete temp texture and char array name
                    delete [] name, name = NULL;
                    delete tempTex, tempTex = NULL;
                }
                
                // now as we looped through it we put all the NPCS into the world
                worldmap.addNPCS(counter, tempNPCs);
                           
                ////printf("-----DELETE TEMP DATA-----\n");
                // and delete their memory
                if (tempNPCs != NULL)
                    delete [] tempNPCs, tempNPCs = NULL;
            }
            else if (strcmp(buffer, _SCRIPT_OBJECT)==0)
            {
                //printf("OBJECT SPOTTED\n");
                
                // create all temp stuff we need to create objects
                Texture2D *tempTex = NULL;
                Object *tempObjects = NULL;
                Point2D position;
                position.x = 0.0f;
                position.y = 0.0f;
                int counter = 0;
                int height = 0;
                char *name;
                
                // read next line (texture)
                inputFile>>buffer;
                
                counter = atoi(buffer);
                
                //printf("SCRIPT: will create %d Objects\n", counter);
                
                tempObjects = new Object[counter];
                
                // loop through all the objects
                for (int i = 0; i < counter; i++)
                {
                    inputFile>>buffer;
                    // first, load in texture
                    if (tempTex == NULL)
                        tempTex = new Texture2D(buffer);
                    
                    // position
                    inputFile>>buffer;
                    position.x = atoi(buffer);
                    inputFile>>buffer;
                    position.y = atoi(buffer);
                    
                    // isSquare
                    inputFile>>buffer;
                    height = atoi(buffer);
                    
                    // name
                    inputFile>>buffer;
                    name = new char[strlen(buffer)];
                    strcpy(name, buffer);
                    
                    // now we have all information, we fill the new objects
                    tempObjects[i].create(*tempTex, name, height);
                    tempObjects[i].setOnTile(position);
                    
                    delete [] name, name = NULL;
                    delete [] tempTex, tempTex = NULL;
                    
                }
                
                worldmap.addObjects(counter, tempObjects);
                
                // delete temp memory
                if (tempObjects != NULL)
                    delete [] tempObjects, tempObjects = NULL;
            }
            else if (strcmp(buffer, _SCRIPT_BATTLE)==0)
            {
                //printf("Battle spotted: %s\n", buffer);
                
                int numberBattles;
                int possibility;
                // read out how much
                inputFile>>buffer;
                numberBattles = atoi(buffer);
                if (numberBattles > MAX_BATTLES_PER_MAP)
                    numberBattles = MAX_BATTLES_PER_MAP;
                
                // read out the possibility of battles to occur
                inputFile>>buffer;
                possibility = atoi(buffer);
                
                // write everything in worldmap file
                worldmap.setPossibility(possibility);
                worldmap.setNumberDifferentBattles(numberBattles);
                worldmap.setHasBattle(true);
                
                for (int i = 0; i < numberBattles; i++)
                {
                    inputFile>>buffer;
                    worldmap.setBattleListItem(i, buffer);
                }
            }
            else 
            {
                //printf("Unknown command spotted, nested in NEW\n");
            }
        }

        else if (strcmp(buffer, _END_OF_SCRIPT)==0)
        {
            //printf("End of Script\n");
        }
        else 
        {
            //printf("Unknown command\n");
        }
    }
    
    inputFile.close();
}

bool Script::loadDialog(Font &withFont, Player &withPlayer, GameEngine &withEngine)
{
    this->dialog = new Dialog(withFont, *this, withPlayer, withEngine);
    
    return true;
}

Dialog& Script::getDialog()
{
    return *this->dialog;
}

void Script::changeGlobalValue(int value, int8_t toValue)
{
    //printf("SCRIPT: [Change Value #%d to %hhd]\n", value, toValue);
    this->globalValues[value] = toValue;
}

void Script::changeGlobalBool(int value, bool toValue)
{
    //printf("SCRIPT: [Change Switch #%d to %d]\n", value, toValue);
    this->globalBools[value] = toValue;
}

bool Script::compareBoolAt(int position, bool withValue)
{
    if (this->globalBools[position] == withValue)
        return true;
    else
        return false;
}

bool Script::compareValueAt(int position, int8_t withValue)
{
    if (this->globalValues[position] == withValue)
        return true;
    else
        return false;
}

void Script::parseItemFile(const char *filename, Inventory &inv)
{
    const char* path = getPathFromIPhoneFile(filename);
    
    ifstream inputFile;
    
    inputFile.open(path);
    
    if (!inputFile.is_open())
    {
        inputFile.close();
        return;
    }
    
    char buffer[400];
    memset(buffer, 0, 400);
    
    // first we parse the name of the spriteset, open it and create the spriteset
    inputFile>>buffer;
    char _name[40];
    memset(_name, 0, 40);
    strcpy(_name, buffer);
    Texture2D *t = new Texture2D(_name);
    
    // than we parse the x amount of sprites and the y amount of sprites
    int nx, ny;
    inputFile>>buffer;
    nx = atoi(buffer);
    inputFile>>buffer;
    ny = atoi(buffer);

    inv.getItemSet().loadData(*t, 4,2,1, _name);
    delete t, t = NULL;
    
    
    // than we parse the number of items we create
    inputFile>>buffer;
    int n = atoi(buffer);
    inv.setItemsUsed(n);
    
    // after that we create the items
    for (int i = 0; i < n; i++)
    {
        // name of the script file
        inputFile>>buffer;
        inv.createItemAtPosition(i);
        inv.getItemsAtPosition(i).create(buffer, i);
        inv.getItemsAtPosition(i).pointToSpriteset(inv.getItemSet());
    }
    
    // that was it
    // than we close
    inputFile.close();
}