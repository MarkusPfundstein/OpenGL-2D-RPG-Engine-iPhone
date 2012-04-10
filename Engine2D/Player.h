//
//  Player.h
//  Engine2D
//
//  Created by Markus Pfundstein on 9/25/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Player_h
#define Engine2D_Player_h

#include "AnimatedSprite.h"
#include "Characters.h"

class Characters;
class NPC;
class Player : public AnimatedSprite
{
private:
    u_int8_t nCharsInGroup;
    Characters *charactersInGroup[3];
    
public:
    Player();
    ~Player();
    
    void update_position(WorldMap &current_map);

    u_int8_t getNCharsInGroup() const;
    void setNCharsInGroup(u_int8_t val);
    
    void setCharacter(Characters &_newCharacter, u_int8_t atPosition);
    Characters& getCharacterAtPosition(u_int8_t position);
};

#endif
