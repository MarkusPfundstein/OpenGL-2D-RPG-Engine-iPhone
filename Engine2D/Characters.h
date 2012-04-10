//
//  Characters.h
//  Engine2D
//
//  Created by Markus Pfundstein on 10/15/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Characters_h
#define Engine2D_Characters_h

#include "Helpers.h"
#include "BattleHelpers.h"

class AnimatedSprite;
class Texture2D;
class Characters
{
protected:    
    
    // stats
    u_int8_t level;
    
    int hitpoints;
    int max_hitpoints;
    int manapoints;
    int max_manapoints;
    int actionpoints;
    int limitpoints;
    int max_limitpoints;
    
    int attackStrength;
    int defense;
    
    // conditions during battle
    bool isDefending;
    
    AnimatedSprite *sprite;
    AnimatedSprite *weapon;
    
    bool isAlive;
    
    // dont confuse this with the isMoving from the AnimatedSprite class. This one is ONLY for battle Characters!!!!
    bool isMoving;
    bool isAttacking;
    bool isGettingAttacked;
    
    Point2D oldPos;
    
    ATTACK_TYPE attackType;
    Rect2D targetRect;
    
    void setNewPositionWithWeapon(Point2D &n);
    
    
public:
    Characters();
    virtual ~Characters();
    
    void create(const Texture2D &spriteSet, int nx, int ny, char* name);
    void createWeapon(const Texture2D &spriteSet, int nx, int ny);
    int updatePosition(enum ATTACK_TYPE last_type, Rect2D &targetRect, bool attacking, bool jump, bool rightSide);
    
    
    void Defend();
    
    // getter, setter
    AnimatedSprite& getSprite() const;
    AnimatedSprite& getWeaponSprite() const;
    int getHitpoints() const;
    int getMaxHitpoints() const;
    int getManapoints() const;
    int getMaxManapoints() const;
    int getActionpoints() const;
    int getLimitpoints() const;
    int getMaxLimitpoints() const;
    u_int8_t getLevel() const;
    bool getIsAlive() const;
    int getAttackStrength() const;
    int getDefense() const;
    bool getIsMoving() const;
    Point2D getOldPos() const;
    enum ATTACK_TYPE getAttackType() const;
    Rect2D getTargetRect() const;
    bool getIsAttacking() const;
    bool getIsDefending() const;
    bool getIsGettingAttacked() const;
    
    void setTargetRect(Rect2D r);
    void setHitpoints(int val);
    void setMaxHitpoints(int val);
    void setManapoints(int val);
    void setMaxManapoints(int val);
    void setActionPoints(int val);
    void setLimitPoints(int val);
    void setMaxLimitpoints(int val);
    void setLevel(u_int8_t val);
    void setIsAlive(bool val);
    void setDefense(int val);
    void setAttackStrength(int val);
    void setIsMoving(bool val);
    void setOldPos(Point2D o);
    void setAttackType(enum ATTACK_TYPE t);
    void setIsAttacking(bool val);
    void setIsDefending(bool b);
    void setIsGettingAttacked(bool b);
};

#endif
