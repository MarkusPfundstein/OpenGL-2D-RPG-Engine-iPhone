//
//  Monster.h
//  Engine2D
//
//  Created by Markus Pfundstein on 10/15/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Monster_h
#define Engine2D_Monster_h

#include "Characters.h"
#include "BattleHelpers.h"

class Characters;
class BattleEngine;
class Monster : public Characters
{
private:
    KI_FLAG KI_Flag;
    
    bool finishedWithTurn;
    bool Ki_set;
    
    // the chosen target for the next action
    Characters *target;
    
    // KI functions
    void attackTarget(enum ATTACK_TYPE withType);
    
public:
    Monster();
    
    void updateMonster(BattleEngine &b);
    void setKI_Flag(enum KI_FLAG val);
    
    enum KI_FLAG getKI_Flag() const;
    
    void handleKI(BattleEngine &b);
    
    void setFinishedWithTurn(bool val);
    bool getFinishedWithTurn() const;
    
    void setKi_set(bool val);
};


#endif
