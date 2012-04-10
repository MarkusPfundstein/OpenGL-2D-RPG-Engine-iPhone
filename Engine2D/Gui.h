//
//  Gui.h
//  OpenGL
//
//  Created by Markus Pfundstein on 9/11/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef OpenGL_Gui_h
#define OpenGL_Gui_h

#include "Helpers.h"

class Texture2D;
class Player;
class Sprite2D;
class WorldMap;
class NPC;
class Object;
class Gui
{    
protected:
    Sprite2D *guiSprite;
    NPC* interactedNPC;
    Object* interactedObject;
    
public:
    Gui(const Texture2D& spriteset);
    ~Gui();
    
    void draw(int currentGameState);
    
    void handle_player_movement(const Point2D& at_position, Player& _char);
    int handle_player_action(const Point2D& at_position, WorldMap& worldmap);
    
    void freeInteractedNPC();
    void freeInteractedObject();
    
    NPC* getInteractedNPC();
    Object* getInteractedObject();
};

#endif
