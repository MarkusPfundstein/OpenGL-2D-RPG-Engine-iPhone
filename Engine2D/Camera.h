//
//  Camera.h
//  Engine2D
//
//  Created by Markus Pfundstein on 9/25/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Camera_h
#define Engine2D_Camera_h

#include "Helpers.h"
#include "AnimatedSprite.h"
#include "WorldMap.h"

class Camera
{
private:
    Rect2D rect;  
    
    void centerAroundObject(const AnimatedSprite &_char);
public:
    Camera();
    
    void update(const AnimatedSprite& _char, const WorldMap &current_map);
    void setPosition(Point2D point);
    
    Rect2D getRect() const;
};

#endif
