//
//  Inventory.h
//  Engine2D
//
//  Created by Markus Pfundstein on 10/22/11.
//  Copyright (c) 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Inventory_h
#define Engine2D_Inventory_h

#include "ItemDef.h"

class Sprite2D;
class Item;
class Script;
class Inventory
{
private:
    Item *items[MAX_NUMBER_OF_ITEMS];
    u_int8_t itemsUsed;
    
    Sprite2D *itemSpriteset;
    
public:
    Inventory();
    ~Inventory();
    
    bool load(Script &s);
    
    
    Sprite2D& getItemSet() const;
    Item & getItemsAtPosition(int i) const;
    void createItemAtPosition(int i);
    void setItemsUsed(u_int8_t n);
    u_int8_t getItemsUsed() const;
};

#endif
