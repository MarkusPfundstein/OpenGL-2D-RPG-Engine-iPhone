//
//  BattleHelpers.h
//  Engine2D
//
//  Created by Markus Pfundstein on 10/16/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_BattleHelpers_h
#define Engine2D_BattleHelpers_h

#include "Helpers.h"

#define MENU_ITEMS_BATTLE   6


#define AP_ATTACK    7
#define AP_DEFENSE   4
#define AP_MAGIC     9
#define AP_ITEM      5
#define AP_GAIN_PER_TURN    10

#define ANI_STATE_STANDING 0
#define ANI_STATE_HURT     1
#define ANI_STATE_DEAD     2
#define ANI_STATE_DEF      3
#define ANI_STATE_WEAK     4

#define MAX_MONSTER_PER_BATTLE  5

#define _KI_SCRIPT_KI_AGGRO   "KI_AGGRO"
#define _KI_SCRIPT_KI_DEFENSE "KI_DEFENSE"

#define WAIT_TIME           30

enum MENU_CODES {
    ATTACK = 0,
    MAGIC,
    ITEM,
    DEFEND,
    CANCEL,
    END_TURN
};

enum BATTLE_STATE {
    WAIT_FOR_INPUT = 0,
    PLAYER_ATTACK,
    PLAYER_MAGIC,
    PLAYER_ITEM,
    PLAYER_SPECIAL,
    ITEM_SELECTED
};

enum KI_FLAG
{
    STANDARD_FLAG = 0,
    KI_AGGRO = 10,
    KI_DEFENSE = 20
};

enum ATTACK_TYPE
{
    NO_ATTACK = -1,
    MELEE = 0,  
};

struct MenuItem {
    bool isActive;
    int texId;
    MENU_CODES menuCode;
    Rect2D rect;
};

struct ItemSelector {
    Rect2D rect;
    bool selected;
};

class Characters;
extern int giveDamageTo(Characters &target, Characters &from, enum ATTACK_TYPE type);


#endif
