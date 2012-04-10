//
//  Script.h
//  Engine2D
//
//  Created by Markus Pfundstein on 9/26/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Script_h
#define Engine2D_Script_h

#include "WorldMap.h"

class Dialog;
class Font;
class WorldMap;
class Player;
class GameEngine;
class Inventory;
class Script
{
    Dialog *dialog;
    
    // global Variables the script engine uses
    int8_t *globalValues;
    bool *globalBools;
    
public:
    Script();
    ~Script();
    
    // parses script when new map gets loaded
    void parseNewWorldScript(const char* filename, WorldMap &worldmap);
    
    // this function reads out the itemfile and allocates enough memory for the Items
    void parseItemFile(const char* filename, Inventory &i);
    
    // dialog handler
    bool loadDialog(Font &withFont, Player &withPlayer, GameEngine &withEngine);
    Dialog& getDialog();
    
    // functions to change the value of global variables
    void changeGlobalValue(int value, int8_t toValue);
    void changeGlobalBool(int value, bool toValue);
    
    bool compareValueAt(int position, int8_t withValue);
    bool compareBoolAt(int position, bool withValue);
};

#endif
