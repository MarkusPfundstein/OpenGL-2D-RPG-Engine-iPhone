//
//  WorldMap.cpp
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include <fstream>
using namespace std;
#include "Worldmap.h"
#include "Texture2D.h"
#include "NPC.h"
#include "DisplayList.h"
#include "Object.h"
#include "Script.h"


WorldMap::WorldMap()
{

}

bool WorldMap::initWorld(const char *filename, Script &_script)
{
    this->drawCollisionGrid = DRAW_COLLISION_GRID;
    this->isActive = true;
    memset(this->newWorld, 0, 30);
    
    this->npcs = NULL;
    this->npcs_counter = 0;
    
    this->objects = NULL;
    this->objects_counter = 0;
    
    this->displayList = NULL;
    
    this->parseLoadedFile(filename);
    
    char scriptName[strlen(filename)+8];
    memset(scriptName, 0, strlen(filename)+8);
    strcpy(scriptName, "script_");
    strcat(scriptName, filename);
    
    this->possibility = 0;
    this->numberDifferentBattles = 0;
    this->hasBattle = false;
    for (int i = 0; i < MAX_BATTLES_PER_MAP; i++)
    {
        memset(this->battleList[i], 0, 30);
    }
    
    _script.parseNewWorldScript(scriptName, *this);
    

    return true;
}

WorldMap::~WorldMap()
{
    glDeleteBuffers(1, &this->vbo);
    glDeleteTextures(1, &this->texture);
    glDeleteTextures(1, &this->texobjects);
    
    int i;
    
    for (i = 0; i < numSprites*numSprites; i++)
    {
        glDeleteBuffers(1, &this->tbo[i]);
    }
    
    delete [] this->tbo;
    
    for (i = 0; i < this->nRows; i++)
    {
        for (int j = 0; j < this->nCols; j++)
        {
            if (this->tiles[i][j]._string != NULL)
            {
                free(this->tiles[i][j]._string);
                this->tiles[i][j]._string = NULL;
            }
        }
    }
    
    for (i = 0; i < this->nRows; i++)
    {
        free(this->tiles[i]);
        this->tiles[i]=NULL;
    }
    free(this->tiles);
    this->tiles=NULL;
    

    for (i = 0; i < this->npcs_counter; i++)
    {
        Texture2D::delete_texture(this->npcs[i].getTexture());
        this->npcs[i].delete_buffers();
    }
    
    if (this->npcs != NULL)
    {
        delete [] this->npcs;
        this->npcs = NULL;
    }
    
    for (i = 0; i < this->objects_counter; i++)
    {
        Texture2D::delete_texture(this->objects[i].getTexture());
        this->objects[i].delete_buffers();
    }
    
    if (this->objects != NULL)
    {
        delete [] this->objects;
        this->objects = NULL;
    }
    
    if (this->displayList != NULL)
        delete this->displayList, this->displayList = NULL;
    
    //printf("Unloaded Map!\n");
}

void WorldMap::parseLoadedFile(const char *filename)
{
    const char* path = getPathFromIPhoneFile(filename);
    
    ifstream inputFile;
    inputFile.open(path);
    
    inputFile.seekg(0, ios::beg);
    char buffer[40];
    
    // variables we will use to store our values
    char textureToLoad[40];
    char texObjToLoad[40];
    int _rows;
    int _cols;
    
    //printf("Worldmap with Name: %s opened\n", filename);
    
    inputFile >> buffer;
    strcpy(textureToLoad, buffer);
    //printf("%s\n", textureToLoad);
    
    inputFile >> buffer;
    strcpy(texObjToLoad, buffer);
    //printf("%s\n", texObjToLoad);
    
    inputFile >> buffer;
    _rows = atoi(buffer);
    //printf("%d\n", _rows);
    
    inputFile >> buffer;
    _cols = atoi(buffer);
    //printf("%d\n", _cols);
    
    Texture2D *tileset = new Texture2D(textureToLoad);
    this->texture = tileset->texture;
    
    Texture2D *objects = new Texture2D(texObjToLoad);
    this->texobjects = objects->texture;
    
    delete tileset, tileset = NULL;
    delete objects, objects = NULL;
    
    this->nRows = _rows;
    this->nCols = _cols;
    
    int k;    

    // create 2d array for worldmap
    this->tiles = (TILE **) malloc(nRows *sizeof(TILE));
    
    for (k = 0; k < nRows; k++)
    {
        this->tiles[k] = (TILE *) malloc(nCols*sizeof(TILE));
    }
       
    float tempx;
    float tempy;
    
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            this->tiles[i][j]._string = NULL;
            this->tiles[i][j].flags = 0;
            
            tempx = (float)i*TILE_SIZE;
            tempy = (float)j*TILE_SIZE;
            
            this->tiles[i][j].x = tempx;
            this->tiles[i][j].y = tempy;
            
            this->tiles[i][j].data = 0;
            inputFile >> buffer;
            this->tiles[i][j].data |= atoi(buffer);       // layer1
            inputFile >> buffer;
            this->tiles[i][j].data |= atoi(buffer)<<8;    // layer2
            inputFile >> buffer;
            this->tiles[i][j].data |= atoi(buffer)<<16;   // layer3
            inputFile >> buffer;
            this->tiles[i][j].data |= atoi(buffer)<<24;   // is tile walkable
        }
    }
    
    inputFile.close();
    
    
    // world loaded, now we create vbos and tbos
    this->create_buffers();
}

void WorldMap::setNumberDifferentBattles(int n)
{
    this->numberDifferentBattles = n;
}

int WorldMap::getNumberDifferentBattles() const
{
    return this->numberDifferentBattles;
}

void WorldMap::setPossibility(u_int8_t val)
{
    this->possibility = val;
}

u_int8_t WorldMap::getPossibility() const
{
    return this->possibility;
}

void WorldMap::setBattleListItem(u_int8_t atPosition, const char *scriptName)
{
    if (atPosition >= this->numberDifferentBattles)
        atPosition = this->numberDifferentBattles;
    memset(this->battleList[atPosition], 0, 30);
    strcpy(this->battleList[atPosition], scriptName);
}

char* WorldMap::getBattleListItem(u_int8_t atPosition)
{
    return this->battleList[atPosition];
}

void WorldMap::setHasBattle(bool val)
{
    this->hasBattle = val;
}

bool WorldMap::getHasBattle() const
{
    return this->hasBattle;
}

void WorldMap::draw(const Rect2D& viewpoint)
{

    int m;
    int n;
    
    if (viewpoint.pos.x > viewpoint.width)
        m = (float)viewpoint.pos.x / TILE_SIZE;
    else
        m= 0;
        
    float temp = -viewpoint.pos.y;
    
    if (temp > viewpoint.height)
        n = (float)temp / TILE_SIZE;
    else
        n = 0;
    
    int i2 = ((viewpoint.pos.x + viewpoint.width) / TILE_SIZE)+1;
    int j2 = ((temp + viewpoint.height) / TILE_SIZE)+1;
    
    if (i2 > nRows)
        i2 = nRows;
    if (j2 > nCols)
        j2 = nCols;
    
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glVertexPointer(3, GL_FLOAT, 0, (char*)NULL);
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    
    for (int i = m; i < i2; i++)
    {
        for (int j = n; j < j2; j++)
        {
            glPushMatrix();
            
            // layer1
            glTranslatef(this->tiles[i][j].x, this->tiles[i][j].y, 0.0f);
            
            // EDITOR ONLY 
            if (this->drawCollisionGrid)
            {
                if (this->tiles[i][j].data>>24&0x1)
                {
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                }
                else
                {
                    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
                }
            }
            else
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            
            glBindBuffer(GL_ARRAY_BUFFER, this->tbo[this->tiles[i][j].data&0xff]);
            glTexCoordPointer(2, GL_FLOAT, 0, (void*)NULL);
            
            glDrawArrays(GL_TRIANGLES, 0, 6);    
            
            // layer2 
            if ((this->tiles[i][j].data>>8&0xff)!=255)
            {
                glEnable(GL_BLEND);
                glBindTexture(GL_TEXTURE_2D, this->texobjects);
                
                glBindBuffer(GL_ARRAY_BUFFER, this->tbo[this->tiles[i][j].data>>8&0xff]);
                glTexCoordPointer(2, GL_FLOAT, 0, (void*)NULL);
                
                glDrawArrays(GL_TRIANGLES, 0, 6);   
                
                glBindTexture(GL_TEXTURE_2D, this->texture);
                glDisable(GL_BLEND);
            }
            
            glPopMatrix();
        }
    }

    
    glEnable(GL_BLEND);

    if (this->drawCollisionGrid)
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    // draw all characters !!!!
    
    this->drawCharacters();
    
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glVertexPointer(3, GL_FLOAT, 0, (char*)NULL);
    
    glBindTexture(GL_TEXTURE_2D, this->texobjects);
    
    // draw layer 3
    for (int i = m; i < i2; i++)
    {
        for (int j = n; j < j2; j++)
        {
            if ((this->tiles[i][j].data>>16&0xff)!=255)
            {
                ////printf("draw!");
                glPushMatrix();
                                
                // layer1
                glTranslatef(this->tiles[i][j].x, this->tiles[i][j].y, 0.0f);
                
                glBindBuffer(GL_ARRAY_BUFFER, this->tbo[this->tiles[i][j].data>>16&0xff]);
                glTexCoordPointer(2, GL_FLOAT, 0, (void*)NULL);
                
                glDrawArrays(GL_TRIANGLES, 0, 6);    
                
                glPopMatrix();
            }
        }
    }
     
}

void WorldMap::drawCharacters()
{    
    // without display list 
    /*
    _char.draw();
    
    for (int i = 0; i < this->npcs_counter; i++)
        this->npcs[i].draw();
    */
    
    // with display list

    this->displayList = listsort(this->displayList, false);
    
    this->displayList->Display();
    
}

void WorldMap::createDisplayList(AnimatedSprite &_char)
{
    this->displayList = new DisplayList(&_char);
    DisplayList *tempList = NULL;
    
    int i;
    
    for (i = 0; i < this->npcs_counter; i++)
    {
        tempList = new DisplayList(&this->npcs[i]);
        this->displayList->Insert(tempList);
    }
    
    for (i = 0; i < this->objects_counter; i++)
    {
        tempList = new DisplayList(&this->objects[i]);
        this->displayList->Insert(tempList);
    }
    
    
}

void WorldMap::create_buffers()
{
    const GLfloat vertices[] = {
        TILE_SIZE, 0, 0,
        0, 0, 0,
        0, TILE_SIZE, 0, 
        0, TILE_SIZE, 0,
        TILE_SIZE, TILE_SIZE, 0,
        TILE_SIZE, 0, 0,
        
    };
    
    glGenBuffers(1, &this->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    
    // create texture buffers, 1 for each animationstep
    
    numSprites = NUMBER_SPRITES_TILESET;
    this->tbo = new GLuint[numSprites*numSprites];
    
    
    Point2D point[6];
    Rect2D offset;
    GLfloat texcoords[12];
    
    int counter = 0;
    int nSpriteCol = 0;
    int nSpriteRow = 0;
    
    for (int i = 0; i < numSprites; i++)
    {
        for (int j = 0; j < numSprites; j++)
        {
            offset.pos.x = (float)nSpriteCol/numSprites;                           
            offset.pos.y = (float)nSpriteRow/numSprites;
            offset.width = (float)1/numSprites;              
            offset.height = (float)1/numSprites;             
            
            point[0].x = offset.pos.x + offset.width;
            point[0].y = offset.pos.y;
            point[1].x = offset.pos.x;
            point[1].y = offset.pos.y;
            point[2].x = offset.pos.x;
            point[2].y = offset.pos.y + offset.height;
            point[3].x = point[2].x;
            point[3].y = point[2].y;
            point[4].x = point[1].x + offset.width;
            point[4].y = point[1].y + offset.height;
            point[5].x = point[0].x;
            point[5].y = point[0].y;
            
            for (int k = 0, l = 0; l < 6; k += 2, l++) 
            {
                texcoords[k] = point[l].x;
                texcoords[k+1] = point[l].y;
            }
            
            this->create_tbo(texcoords, counter);
            
            nSpriteCol++;
            counter++;
        }
        nSpriteRow++;
    }
}


void WorldMap::create_tbo(const GLfloat *texcoords, GLuint id)
{
    glGenBuffers(1, &this->tbo[id]);
    glBindBuffer(GL_ARRAY_BUFFER, this->tbo[id]);
    glBufferData(GL_ARRAY_BUFFER, 48, texcoords, GL_STATIC_DRAW);
}

float WorldMap::get_world_width() const
{
    return this->nRows*TILE_SIZE;
}

float WorldMap::get_world_height() const
{
    return this->nCols*TILE_SIZE;
}

TILE& WorldMap::getTileAt(int i, int j) const
{
    if (i < 0)
        i = 0;
    else if (i >= this->nRows)
        i = this->nRows-1;
    if (j < 0)
        j = 0;
    else if (j >= this->nCols)
        j = this->nCols-1;
    
    return this->tiles[i][j];
}

void WorldMap::addTileFlagAt(int i, int j, uint8_t flag, uint8_t flagprop1, uint8_t flagprop2, const char* optString)
{
    if (i < 0)
        i = 0;
    else if (i >= this->nRows)
        i = this->nRows-1;
    if (j < 0)
        j = 0;
    else if (j >= this->nCols)
        j = this->nCols-1;
    
    this->tiles[i][j].flags = flag;
    if (flagprop1 != 0)
        this->tiles[i][j].flags |= flagprop1<<8;
    
    if (flagprop2 != 0)
        this->tiles[i][j].flags |= flagprop2<<16;
    
    if (flag == TILE_FLAG_TRANSPORTTILE || flag == TILE_FLAG_EVENTTILE)
    {
        this->tiles[i][j]._string = (char*)malloc(strlen(optString));
        strcpy(this->tiles[i][j]._string, optString);
    }
    
    //printf("Tile[%d][%d], hasFlag: %d with fp1: %d, fp2: %d, optString: %s\n", i, j,  (this->tiles[i][j].flags&0xff), (this->tiles[i][j].flags>>8&0xff), (this->tiles[i][j].flags>>16&0xff), this->tiles[i][j]._string);
}

void WorldMap::addNPCS(u_int8_t counter, NPC *newNPCs)
{
    this->npcs_counter = counter;
    
    this->npcs = new NPC[this->npcs_counter];
    
    //printf("Add %d NPCs to World\n", this->npcs_counter);
    
    for (int i = 0; i < this->npcs_counter; i++)
    {
        this->npcs[i].createCopyOf(newNPCs[i]); // call copyconstructor of NPC class
    }
    
}

void WorldMap::addObjects(u_int8_t counter, Object *newObjects)
{
    this->objects_counter = counter;
    
    this->objects = new Object[this->objects_counter];
    
    //printf("Add %d Objects to World\n", this->objects_counter);
    
    for (int i = 0; i < this->objects_counter; i++)
    {
        this->objects[i].createCopyOf(newObjects[i]);
    }
}

void WorldMap::suicide(Point2D newDest, const char* newMap)
{
    strcpy(newWorld, newMap);
    this->newDestination = newDest;
    
    this->isActive = false;
    
    //printf("suicide: %s\n", newWorld);
}

enum WORLD_UPDATE_CALLBACK WorldMap::updateWorld(AnimatedSprite &_char)
{
    static int timer = 0;
    float BattleTime = 400;
    
    bool b = rand()%1;
    if (b)
        BattleTime += rand()%100;
    else
        BattleTime -= rand()%100;
        
    
    int i;
    for (i = 0; i < this->npcs_counter; i++)
    {
        this->npcs[i].update(_char, *this);
    }
    for (i = 0; i < this->objects_counter; i++)
    {
        this->objects[i].update(_char);
    }
    
    if (_char.getMoving() && this->hasBattle)
    {
        timer++;
        if (timer > BattleTime)
        {
            timer = 0;
            return CALLBACK_BATTLE;
        }
    }
    return CALLBACK_NO_BATTLE;
}

bool WorldMap::getIsActive() const
{
    return this->isActive;
}

char* WorldMap::getNewWorld() 
{
    return this->newWorld;
}

Point2D WorldMap::getNewDestination() const
{
    return this->newDestination;
}

u_int8_t WorldMap::getNPCcounter() const
{
    return this->npcs_counter;
}

u_int8_t WorldMap::getObjectCounter() const
{
    return this->objects_counter;
}

NPC& WorldMap::getNPC(int number) const
{    
    return this->npcs[number];
}

Object& WorldMap::getObject(int number) const
{
    return this->objects[number];
}