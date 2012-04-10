//
//  AnimatedSprite.h
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef OpenGL_AnimatedSprite_h
#define OpenGL_AnimatedSprite_h

#include "WorldMap.h"
#include "Sprite2D.h"
#include "Constants.h"
#include "BattleHelpers.h"

class GameEngine;
class WorldMap;
class BattleEngine;
class AnimatedSprite : public Sprite2D
{
private:
    GameEngine *engineUsed; // has to get locked right after initializing the char who will trigger engine commands like starting a script
    BattleEngine *battleEngineUsed;
    
protected:
    // for animations
    int animationState;
    int endAnimationState;
    int animationSpeed;    
    int animationCounter;
    int oldAnimationState;
    
    ViewDirection viewDirection;
    
    void (BattleEngine::*battlefuncp)();
    bool battlefuncpIsSet;
    void (*funcp)();
    bool funcpIsSet;
    
    // how fast the sprite moves
    float movingSpeed;
    
    // some bools
    bool isMoving;
    bool isPlayer;
    bool isObject;              
    bool isAnimating;           // is the sprite currently being animated 
    bool isInAnimationState;    // is the sprite currently set to a specific state (this overwrites the original state without changing it)
    bool resetAnimationState;
    bool checksForCollision;    // does this sprite checks for collision?
    
public:
    AnimatedSprite();
    ~AnimatedSprite();
    
    void drawAnimated();        // draws the sprite
    
    // collision detection
    void collisionWithTile(WorldMap &current_map, u_int8_t &collision);
    bool collisionWithObject(Rect2D &a, Rect2D &b, u_int8_t &collision, int heightOfObject);
    
    void setViewDirection(ViewDirection direction);
    void setMoving(bool val);
    
    float getMovingSpeed() const;
    bool getMoving() const;
    bool getIsObject() const;
    ViewDirection getViewDirection() const;
    
    // ALL THE ANIMATION FUNCTIONS
    int getAnimationState() const;
    void setAnimationState(int val);
    bool getIsAnimating() const;
    // has to be set, otherwise the animation wont get played
    void setIsAnimating(bool val);
    // saves the current animation state and forces the engine to reset back that state after an animation ended
    void resetAfterEndAnimationStateIsReached(bool b);
    void setAnimationSpeed(int val);
    void setEndAnimationState(int val);
    // If you set this is true, the Setted Animation State will get drawn instead of the current ViewDirection until you set it back to false
    void setisInAnimationState(bool val);
    
    // set a function pointer to functions that get called when an animation ends
    // can take a function pointer to a C Style extern Function or a Battle Engine function.
    // If the second option is chosen we need to pass as well a pointer to the used Battle Engine
    void setEndAnimationFunction(void (BattleEngine::*_pfunc)(), BattleEngine &b);
    void setEndAnimationFunction(void (*_pfunc)());

    
    void updateAnimationState();
        
    void setOnTile(Point2D point);
    
    void pointToEngine(GameEngine &_engine);
    
    void setChecksForCollison(bool val);
    
};

#endif
