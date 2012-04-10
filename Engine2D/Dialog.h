//
//  Dialog.h
//  Engine2D
//
//  Created by Markus Pfundstein on 10/1/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Dialog_h
#define Engine2D_Dialog_h

#include <fstream>
using namespace std;
#include "Helpers.h"
#include "ScriptDef.h"

class Sprite2D;
class Font;
class Script;
class Player;
class NPC;
class Camera;
class WorldMap;
class Object;
class GameEngine;
class Dialog
{
    // all the pointers the script can and will influence.. & are not allowed to be changed in the code. * can point to different objects of course
    Object *objectUsed;
    NPC *npcUsed;
    Player &playerUsed;
    Script &scriptUsed;
    Font& fontUsed;
    WorldMap* worldUsed;
    GameEngine& engineUsed;
    
    // the char that will get printen to the screen
    char currentString[400];
    
    // the scriptfile
    ifstream inputFile;
    
    INPUTMODE inputMode;
    
    // graphical interfaace
    Sprite2D *sprites;
    Rect2D rectsGui[4];

public:
    Dialog(Font& withFont, Script &parent, Player &withPlayer, GameEngine &withEngine);
    ~Dialog();
    
    bool startDialog(const char* filename, const char* ext);
    int readNextLine(int input);    // gives false back if it was the last line in dialog
    void show(Player& player);      // draws the dialog to the screen, player determines where the inputDialog options appear   
    void clearDialog();             // clears memory and sets pointers to NULL
    
    int handleInput(const Point2D &atposition, const Camera &camera);   // handle player input
    
    void pointToObject(Object &other);
    void pointToNPC(NPC &other);
    void setWorldUsed(WorldMap &newWorldUsed);
};

#endif
