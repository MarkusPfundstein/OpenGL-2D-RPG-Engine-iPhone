//
//  BattleEngine.h
//  Engine2D
//
//  Created by Markus Pfundstein on 10/15/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_BattleEngine_h
#define Engine2D_BattleEngine_h

#include <OpenGLES/ES1/gl.h>
#include "BattleHelpers.h"
#include "Helpers.h"
#include "Constants.h"
#include "Monster.h"

class Gui;
class Sprite2D;
class Player;
class Font;
class GameEngine;
class Item;
class Inventory;
class BattleEngine
{
private:
    // STATIC STUFF that never gets changed
    GameEngine &engineUsed;
    Inventory &inventoryUsed;
    
    Sprite2D *gui;
    
    GLuint backgroundVBO;
    GLuint backgroundTBO;
    GLuint backgroundTexId;
    
    Player *playerUsed;
    Font &fontUsed;
    MenuItem menuItems[MENU_ITEMS_BATTLE];
    // END STATIC STUFF
    
    Characters *charInFocus;
    Monster *monsterInFocus;

    // represents the item buttons on the screen
    ItemSelector *itemSelector;
    
    // itemsInBattle determines how much items are in the battle (also 0 items)
    int itemsInBattle;
    // this is how much get displayed (all items - zero items)
    int itemsDisplayed;
    
    // timer stuff
    int timer;
    bool timerIsOn;
    bool isPaused;
    float timerSpeed;
    
    // is it players turn
    bool playerTurn;
    
    // do we have to reallocate the amount of items
    bool reallocItems;
    
    // to draw the damage or value changed in hp or anything on the screen
    int lastvalueChanged;
    bool valueChanged;
    Point2D pointOfValueChanged;
    
    // how many monsters are participating
    u_int8_t monstersInBattle;
    
    // array of pointers to this monsters
    Monster *monsters[MAX_MONSTER_PER_BATTLE];
    
    // the current battle state
    BATTLE_STATE currentBattleState;
    
    // a pointer to the item which is currently used
    Item *itemCurrentlyUsed;
    
    // draws the ring menu around the characters
    void drawMenuAround(Point2D &_p);
    
    // create background buffers
    void createBuffers();
    
    // parses the script when a battle starts
    void parseBattleScript(const char* filename);
    // sets KI FLag
    enum KI_FLAG setFlagForMonster(const char* buffer);
    
    // sets some pointer to NULL
    void freeCharInFocus();
    void freeMonsterInFocus();
    // starts the Timer and sets the time interval to time
    void startTimer(float time);
    void setTimerBack();
    
    // handle Turns
    void handlePlayers();
    void handleMonster();
    void resetTurn();
    
    // draws damage, or the value healed etc...
    void drawChangeInValue();
    
    // i needed to make extra functions for this because the endAnimationFunctionPointer doesnt take arguments. 
    void damagePlayerToMonster();
    void useItem();
    
    
    // allocates the memory for items which are available in the battle, gets also called when a items count reaches 0 (to reallocate memory)
    void allocMemForItems();

public:
    BattleEngine(Font &_f, GameEngine &_e, Inventory &_i);
    ~BattleEngine();
    
    // initializes a battle
    bool initializeBattle(Player &_player, const char* battleScript);
    // gets called after a battle ends to clear up memory
    void clearMemory();
    
    // draws the battle on the screen
    void draw();
    
    // updates all participants
    int update();
    
    // handles player Input
    void handleInput(const Point2D &at_position);
    
    // getter - setters for other functions
    bool getPlayerTurn() const;
    Player& getPlayerUsed() const;
    
    void setvalueChanged(bool val);
    void setpointOfValueChanged(Point2D &p);
    void setLastvalueChanged(int val);
};

#endif
