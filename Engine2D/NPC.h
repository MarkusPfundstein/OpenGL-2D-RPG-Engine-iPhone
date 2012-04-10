//
//  NPC.h
//  Engine2D
//
//  Created by Markus Pfundstein on 9/27/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_NPC_h
#define Engine2D_NPC_h

#include "AnimatedSprite.h"

class NPC : public AnimatedSprite
{
private:
    int flags;              // 0-7 KI Flag  
    int timerPlayerAttention;
    int timerForWalkaroundScript;
    
    bool locked;
    bool inPlayerFocus;
    
    void move(AnimatedSprite &_char, WorldMap &current_map);
public:
    NPC();
    
    void createCopyOf(const NPC& other);
    void create(const Texture2D &spriteSet, char* name, int _flags);
    void update(AnimatedSprite &_char, WorldMap &current_map);
    void setLocked(bool val);
    void setInPlayerFocus(bool val);
    
    int getFlags();
    bool getIfInPlayerFocus() const;
};



#endif
