//
//  Camera.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 9/25/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "Camera.h"
#include "Constants.h"

Camera::Camera()
{
    this->rect.width = SCREEN_WIDTH_IPHONE;
    this->rect.height = SCREEN_HEIGHT_IPHONE;
    this->rect.pos.x = 0;
    this->rect.pos.y = 0;
}

void Camera::setPosition(Point2D point)
{
    this->rect.pos.x = point.x;
    this->rect.pos.y = point.y;
}

void Camera::update(const AnimatedSprite &_char, const WorldMap &current_map)
{
    this->centerAroundObject(_char);
    
    if (this->rect.pos.x < 0)
        this->rect.pos.x = 0;
    else if (this->rect.pos.x + this->rect.width > current_map.get_world_width())
        this->rect.pos.x = current_map.get_world_width()-this->rect.width;
    
    if (this->rect.pos.y > 0)
        this->rect.pos.y = 0;
    else if (this->rect.pos.y - this->rect.height < -current_map.get_world_height())
        this->rect.pos.y = -current_map.get_world_height()+this->rect.height;
}

void Camera::centerAroundObject(const AnimatedSprite &_char)
{
    Point2D tempPoint;
    tempPoint.x = (_char.getRect().pos.x+_char.getRect().width/2);
    tempPoint.y = -(_char.getRect().pos.y+_char.getRect().height/2);
    
    tempPoint.x -= SCREEN_WIDTH_IPHONE/2;
    tempPoint.y += SCREEN_HEIGHT_IPHONE/2;
    
    this->rect.pos = tempPoint;
}

Rect2D Camera::getRect() const
{
    return this->rect;
}
