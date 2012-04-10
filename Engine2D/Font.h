//
//  Font.h
//  Engine2D
//
//  Created by Markus Pfundstein on 9/25/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_Font_h
#define Engine2D_Font_h

#include "Sprite2D.h"


class Font : public Sprite2D
{
private:
    float offsetX;
    float offsetY;
    float beginX;
    
    void draw(const char* string);
public:
    Font(const Texture2D &texture, char* name);
    ~Font();
    
    void draw_string(float _x, float _y, float size, float offset, float r, float g, float b, float a, const char *string, ...);
};

#endif
