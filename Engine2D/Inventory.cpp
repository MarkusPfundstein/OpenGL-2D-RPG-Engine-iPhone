//
//  Inventory.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 10/22/11.
//  Copyright (c) 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "Inventory.h"
#include "Sprite2D.h"
#include "Item.h"
#include "Texture2D.h"
#include "Script.h"

Inventory::Inventory()
{
    
}

Inventory::~Inventory()
{
    for (int i = 0; i < this->itemsUsed; i++)
        delete this->items[i], this->items[i] = NULL;
    
    this->itemSpriteset->deleteMem();
    delete this->itemSpriteset;
}

bool Inventory::load(Script &s)
{
    this->itemSpriteset = new Sprite2D();
    s.parseItemFile((char*)"items.txt", *this);
    
    return 1;
}

Item& Inventory::getItemsAtPosition(int i) const
{
    return *this->items[i];
}

Sprite2D& Inventory::getItemSet() const
{
    return *this->itemSpriteset;
}

void Inventory::setItemsUsed(u_int8_t n)
{
    this->itemsUsed = n;
}

void Inventory::createItemAtPosition(int i)
{
    this->items[i] = new Item();
}


u_int8_t Inventory::getItemsUsed() const
{
    return this->itemsUsed;
}