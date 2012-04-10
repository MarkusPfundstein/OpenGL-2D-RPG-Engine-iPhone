//
//  Item.h
//  Engine2D
//
//  Created by Markus Pfundstein on 10/22/11.
//  Copyright (c) 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Item_h
#define Engine2D_Item_h
#include <fstream>
using namespace std;
#include "ItemDef.h"

class Sprite2D;
class Characters;
class BattleEngine;
class Item
{
private:
    bool collected;
    u_int8_t numberCollected;
    
    // this is the script
    char script[40];
    
    u_int8_t tex_id;
    Sprite2D *sprite;
    
    Characters *target;
    BattleEngine *engine;
    
    bool parseItemFile();
public:
    Item();
    ~Item();
    
    bool create(const char *filename, int tex);
    void pointToSpriteset(Sprite2D &set);
    
    void setCollected(bool b);
    void setNumberCollected(u_int8_t n);
    
    bool getCollected() const;
    u_int8_t getNumberCollected() const;
    u_int8_t getTexId() const;
    
    // returns 0 if the item count reaches 0
    void pointToTargetAndEngine(Characters &c, BattleEngine &b);
    void freeTargetAndEngine();
    bool use();
};

#endif
