//
//  Object.h
//  Engine2D
//
//  Created by Markus Pfundstein on 10/3/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Object_h
#define Engine2D_Object_h

#include "AnimatedSprite.h"

class Texture2D;
class Object : public AnimatedSprite
{
private:
    bool inPlayerFocus;
     
public:
    Object();
    
    void createCopyOf(const Object& other);
    void create(const Texture2D &spriteSet, char* name, int height);
    void update(const AnimatedSprite &player);
    
    void setInPlayerFocus(bool val);
    
    bool getIfInPlayerFocus() const;
};

#endif
