//
//  GameEngine.h
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef OpenGL_GameEngine_h
#define OpenGL_GameEngine_h

#include <OpenGLES/ES1/gl.h>
#include "Helpers.h"
#include "Sprite2D.h"
#include "Texture2D.h"
#include "Player.h"
#include "WorldMap.h"
#include "Gui.h"
#include "Camera.h"
#include "Font.h"
#include "Dialog.h"
#include "Script.h"
#include "BattleEngine.h"
#include "Item.h"
#include "ItemDef.h"
#include "Inventory.h"

class GameEngine
{
private:
    bool newMapLoaded;
    bool isPaused;
    bool goOnWithScript;
    
    int pauseTimer;
    int pauseDuration;
    
    GameStates currentGameState;
    
    WorldMap *current_map;
    Gui *gui;
    Font *font_silon;
    Script *script;
    Player *player;
    Camera *camera;
    BattleEngine *battleEngine;
    
    Characters *charactersForBattle;
    
    Inventory *inventory;

    void adjust_scene();    
    bool loadNewMap(const char* filename);
    
public:
    GameEngine();
    ~GameEngine();
    
    bool initGameEngine();
    
    void update_objects();
    void draw_scene(int fps);
    void handle_input(const Point2D& at_position);
    void input_stop();
    void startDialog(ACTION_CODES actionCode, const char* optionalString);
    void setPaused(bool val);
    void setPauseDuration(int val);
    void updateWaiting();
    void updateDialog(int goOn);
    bool getIfPaused() const;

};

#endif
