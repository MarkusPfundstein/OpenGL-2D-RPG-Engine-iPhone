//
//  GameEngine.cpp
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
using namespace std;
#include "GameEngine.h"
#include "Constants.h"
#include "Script.h"
#include "NPC.h"
#include "Object.h"
#include "Characters.h"


GameEngine::GameEngine()
{    


}

GameEngine::~GameEngine()
{
    delete this->gui;
    delete this->camera;
    delete this->script;
    delete this->font_silon;
    delete this->player;
    delete this->current_map;
    delete this->battleEngine;
    delete this->inventory;

    
}

bool GameEngine::initGameEngine()
{
    //printf("Initialize Game Engine\n"    
    this->script = NULL;
    this->player = NULL;
    this->gui = NULL;
    this->font_silon = NULL;
    this->inventory = NULL;
    
    this->isPaused = false;
    this->goOnWithScript = false;
    this->currentGameState = WORLD_MAP;     
       
    // initialize cam
    this->camera = new Camera();
    
    Texture2D *texFontSilom = new Texture2D((char*)"font_silom.png");
    this->font_silon = new Font(*texFontSilom, (char*)"Font Silom");
    delete texFontSilom, texFontSilom = NULL;
    
    Texture2D *tileset_gui;
    
    tileset_gui = new Texture2D((char*)"mainGUI.png");
    this->gui = new Gui(*tileset_gui);
    
    delete tileset_gui, tileset_gui = NULL;
    
    // initialize player object which will walk around on the worldmap
    this->player = new Player();
    this->player->pointToEngine(*this);
    
    // initialize script engine
    this->script = new Script();
    this->script->loadDialog(*this->font_silon, *this->player, *this);
    
    // initialize items
    this->inventory = new Inventory();
    this->inventory->load(*this->script);
    
    // initialize battleEngine
    this->battleEngine = new BattleEngine(*this->font_silon, *this, *this->inventory);
    
    /* ========================================== */
    // for BATTLE TEST purposes
    // 4 characters
    Texture2D *tempSet = new Texture2D((char*)"female1bat.png");
    Texture2D *weaponSet = new Texture2D((char*)"melee2.png");
    
    this->player->setNCharsInGroup(3);
    
    charactersForBattle = new Characters[this->player->getNCharsInGroup()];
    char charName[40];
    memset(charName, 0, 40);
    for (int i = 0; i < this->player->getNCharsInGroup(); i++)
    {
        sprintf(charName, "Character:%d", i);
        charactersForBattle[i].create(*tempSet, 4, 2, charName);
        charactersForBattle[i].createWeapon(*weaponSet, 4, 1);
        this->player->setCharacter(charactersForBattle[i], i);
        Characters *c = &this->player->getCharacterAtPosition(i);
        c->setLevel(10);
        c->setMaxHitpoints(300);
        c->setHitpoints(100);
        c->setMaxManapoints(100);
        c->setManapoints(30);
        c->setActionPoints(10);
        c->setLimitPoints(0);
        c->setMaxLimitpoints(0);
        c->setIsAlive(true);
        c->setAttackStrength(20);
        c->setDefense(10);
        c=NULL;
    }

    delete tempSet, tempSet = NULL;
    delete weaponSet, weaponSet = NULL;
    
    // ITEMS
    for (int i = 0; i < this->inventory->getItemsUsed(); i++)
    {
        // we create a couple items of each and set them to collected so that the player can use them
        this->inventory->getItemsAtPosition(i).setNumberCollected(3);
    }
    
    // END OF BATTLE TEST PURPOSES
    /* ========================================== */
    
    // load first world
    this->current_map = NULL;

    if (!this->loadNewMap((char*)"forest1.txt"))
    {
        return false;
    }
    
    return true;
}

void GameEngine::update_objects()
{
    // is paused means that no the script doesnt get updated ... the graphics and objects are still getting updated...
    if (this->isPaused)
        updateWaiting();
    
    if (this->currentGameState == WORLD_MAP || this->currentGameState == DIALOG)
    {
        if (this->newMapLoaded)
        {
            if (!this->current_map->getIsActive())
            {
                //printf("map is not active, load new one\n");
                this->newMapLoaded = false;
                this->loadNewMap(this->current_map->getNewWorld());
            }
            
            if (this->goOnWithScript && !this->isPaused)
            {
                // read next line of the script with entry point 0
                int goOn = this->script->getDialog().readNextLine(0);
                this->updateDialog(goOn);
            }
            
            this->player->update_position(*this->current_map);
            this->camera->update(*this->player, *this->current_map);
            
            // check for encounter
            if (this->current_map->updateWorld(*this->player) == CALLBACK_BATTLE && this->currentGameState != DIALOG)
          {
            int whichBattle =2;//= rand()%this->current_map->getNumberDifferentBattles();
                    this->battleEngine->initializeBattle(*this->player, this->current_map->getBattleListItem(whichBattle));
                    this->currentGameState = BATTLE;
          }
        }
    }
    else if (this->currentGameState == BATTLE)
    {
        int result = this->battleEngine->update();
        if (result!=1)
        {                
            this->battleEngine->clearMemory();
            if (result == -1)
            {
                this->currentGameState = WORLD_MAP;
            }
            else
            {
                this->currentGameState = WORLD_MAP;
            }
        }
    }
}

void GameEngine::draw_scene(int fps)
{
    if (this->newMapLoaded)
    {
        
        if (this->currentGameState != BATTLE)
        {
            // adjust matrix to draw the drawing scene
            this->adjust_scene();
            
            this->current_map->draw(this->camera->getRect());
            
            if (this->currentGameState == DIALOG)
            {
                if (!this->isPaused)
                    this->script->getDialog().show(*this->player);
            }
        }
        else    // we are in battle Mode
        {
            this->battleEngine->draw();
        }
        // to enable gui drawing 
        setMatrixToGUIMode();
        
        // if we have a battle we draw the battle
        
        if (SHOW_PLAYER_POS && this->currentGameState != BATTLE)
        {
            this->font_silon->draw_string(0, 0, 1.0, 10, 0.8f, 0, 0.2f, 0.8f, "Player X:%1.f, Y:%1.f", this->player->getRect().pos.x/TILE_SIZE, (this->player->getRect().pos.y/TILE_SIZE)+1);
        }
        
        if (SHOW_FPS)
        {
            this->font_silon->draw_string(0.0, 300.0, 1.0, 10, 0.8f, 0.2f, 0.5f, 1.0f, "FPS: %d", fps);
        }
        
        if (!this->isPaused)
            this->gui->draw(this->currentGameState);
    }
}

void GameEngine::handle_input(const Point2D &at_position)
{
    if (!this->isPaused)
    {
        if (this->currentGameState == WORLD_MAP)
        {
            this->gui->handle_player_movement(at_position, *this->player);
            ACTION_CODES actionCode = (ACTION_CODES)this->gui->handle_player_action(at_position, *this->current_map);
            
            this->startDialog(actionCode, NULL);
        }
        else if (this->currentGameState == DIALOG)
        {
            int goOn = this->script->getDialog().handleInput(at_position, *this->camera);
            this->updateDialog(goOn);
        }
        else if (this->currentGameState == BATTLE)
        {
            this->battleEngine->handleInput(at_position);
        }
    }
}

void GameEngine::startDialog(ACTION_CODES actionCode, const char* optionalString)
{
    if (actionCode == ACTION_NPC)
    {
        // open dialog according to NPC name
        if (this->script->getDialog().startDialog(this->gui->getInteractedNPC()->getName(), ".dlg"))
        {
            this->currentGameState = DIALOG;
            this->gui->getInteractedNPC()->setLocked(true);
            this->script->getDialog().pointToNPC(*this->gui->getInteractedNPC());
            int goOn = this->script->getDialog().readNextLine(0);
            this->updateDialog(goOn);
        }
    }
    else if (actionCode == ACTION_OBJECT)
    {
        // if the object has a dialog script
        if (this->script->getDialog().startDialog(this->gui->getInteractedObject()->getName(), ".dlg"))
        {
            this->currentGameState = DIALOG;
            this->script->getDialog().pointToObject(*this->gui->getInteractedObject());
            int goOn = this->script->getDialog().readNextLine(0);
            this->updateDialog(goOn);
        }
    }
    else if (actionCode == ACTION_EVENTTILE)
    {
        // if there is a event tile we open the event script
        if (this->script->getDialog().startDialog(optionalString, NULL))
        {
            this->currentGameState = DIALOG;
            int goOn = this->script->getDialog().readNextLine(0);
            if (goOn != 0)
                this->player->setMoving(false);
            this->updateDialog(goOn);
        }
    }
}

void GameEngine::updateDialog(int goOn)
{
    if (goOn == -1)
    {
        // do nothing, this happens when the player clicks outside an input rect
    }
    else if (goOn == 1)
        this->goOnWithScript = false;
    else if (goOn == 2)
    {
        // after wait time script will go on automatically
        this->goOnWithScript = true;
    }
    else if (goOn == 0)
    {
        if (this->gui->getInteractedNPC() != NULL) // we call this function only if we have a npc, objects cant be locked
            this->gui->getInteractedNPC()->setLocked(false);
        this->script->getDialog().clearDialog();
        this->currentGameState = WORLD_MAP;
        this->goOnWithScript = false;
        this->player->setChecksForCollison(true);
    }
}

void GameEngine::input_stop()
{
    this->player->setMoving(false);
}

bool GameEngine::loadNewMap(const char* filename)
{    
    char fileToLoad[40];
    memset(fileToLoad, 0, 40);
    strcpy(fileToLoad, filename);
    
    Point2D playerStart;
    
    if (this->current_map != NULL)
    {
        playerStart = this->current_map->getNewDestination();
        delete this->current_map;
        this->current_map = NULL;
    }
    else
    {
        playerStart.x = NEW_GAME_START_X;
        playerStart.y = NEW_GAME_START_Y;
    }
    
    //printf("GameEngine: loadNewMap!!!\n");
    // load new map
    this->current_map = new WorldMap();
    
    if (!this->current_map->initWorld(fileToLoad, *this->script))
    {
        //printf("ERROR: Initializing world\n");
        return false; 
    }
    
    this->script->getDialog().setWorldUsed(*this->current_map);
    
    // set player graphic
    this->player->setOnTile(playerStart);
    
    this->current_map->createDisplayList(*this->player);
    
    this->newMapLoaded = true;
    
    return true;
}

void GameEngine::adjust_scene()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    //rotated due to landscape mode
    glOrthof(this->camera->getRect().pos.y, this->camera->getRect().pos.y+this->camera->getRect().height, (this->camera->getRect().pos.x+this->camera->getRect().width), this->camera->getRect().pos.x, -1.0f, 1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90.0f, 0, 0, 1);
    glTranslatef(0.0f, -SCREEN_HEIGHT_IPHONE, 0.0f);
}

void GameEngine::setPaused(bool val)
{
    //printf("Game Engine state to wait: %d with Duration: %d\n", val, this->pauseDuration);
    this->pauseTimer = 0;
    this->isPaused = val;
}

void GameEngine::setPauseDuration(int val)
{
    this->pauseDuration = val;
}

void GameEngine::updateWaiting()
{
    if (this->pauseTimer < this->pauseDuration)
    {
        this->pauseTimer++;        
    }
    else 
    {
        this->pauseTimer = 0;
        this->isPaused = false;
    }
}

bool GameEngine::getIfPaused() const
{
    return this->isPaused;
}

