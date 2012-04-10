//
//  BattleHelpers.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 10/16/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "Characters.h"
#include "BattleHelpers.h"

int calculateMeleeDamage(const Characters& attacker, const Characters& defender, bool critical);

int calculateMeleeDamage(const Characters& attacker, const Characters& defender, bool critical)
{
    int d;
    int criticMultiplier = 1;
    if (critical)
        criticMultiplier = 2;
    
    d = (attacker.getAttackStrength() - defender.getDefense()) * criticMultiplier;
    
    if (defender.getIsDefending())
        d-=d/2;
    
    return d;
}

int giveDamageTo(Characters &target, Characters &from, enum ATTACK_TYPE type)
{
    int d = 0;
    if (type == MELEE)
    {
        int hp = target.getHitpoints();
        d = calculateMeleeDamage(from, target, false);
    
        if (d < 0) d = 0;
        
        if (target.getIsDefending())
            target.setIsDefending(false);
        
        hp -= d;
        target.setHitpoints(hp); 
    }
    
    return d;
}