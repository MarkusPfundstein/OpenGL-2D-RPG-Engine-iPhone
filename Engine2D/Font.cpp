//
//  Font.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 9/25/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Font.h"

#include "Constants.h"

Font::Font(const Texture2D &texture, char* name) : Sprite2D()
{
    this->loadData(texture, 16, 16, 1, name);
}

Font::~Font()
{
    Texture2D::delete_texture(this->texture);
    this->delete_buffers();
}

void Font::draw_string(float _x, float _y, float size, float offset, float r, float g, float b, float a, const char *string, ...)
{
    this->beginX = _x;
    this->setX(_x);
    this->setY(_y);
    this->offsetX = offset*size;
    this->offsetY = size*20;
    
    char *stringToPrint;
    va_list args;
    va_start(args, string);
    vasprintf(&stringToPrint, string, args);
    va_end(args);
    
    glPushMatrix();
    
    //setMatrixToGUIMode();
    this->scale(size);
    
    glColor4f(r, g, b, a);
    
    this->draw(stringToPrint);
    
    glPopMatrix();

    free(stringToPrint), stringToPrint = NULL;
}

void Font::draw(const char* string)
{
    glBindTexture(GL_TEXTURE_2D, this->texture);
    
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glVertexPointer(3, GL_FLOAT, 0, (char*)NULL);
    

    float newX=this->rect.pos.x;
    float newY=this->rect.pos.y;
    
    
    for (unsigned long i = 0; i < strlen(string); i++)
    {
        newX += this->offsetX;
        if (i > 0)
        {
            if (string[i-1] == 'W' || string[i-1] =='w' || string[i-1] == 'M' || string[i-1] == 'm')
            {
                newX += this->offsetX/2;
            }
            else if (string[i-1] == 'I' || string[i-1] == 'i' || string[i-1] == 'l')
            {
                newX -= this->offsetX/2;
            }
        }
        this->setX(newX);
        this->setY(newY);
        switch (string[i])
        {
            case 'A':
                this->draw_to_scene(65);
                break;
            case 'B':
                this->draw_to_scene(66);
                break;
            case 'C':
                this->draw_to_scene(67);
                break;
            case 'D':
                this->draw_to_scene(68);
                break;
            case 'E':
                this->draw_to_scene(69);
                break;
            case 'F':
                this->draw_to_scene(70);
                break;
            case 'G':
                this->draw_to_scene(71);
                break;
            case 'H':
                this->draw_to_scene(72);
                break;
            case 'I':
                this->draw_to_scene(73);
                break;
            case 'J':
                this->draw_to_scene(74);
                break;
            case 'K':
                this->draw_to_scene(75);
                break;
            case 'L':
                this->draw_to_scene(76);
                break;
            case 'M':
                this->draw_to_scene(77);
                break;
            case 'N':
                this->draw_to_scene(78);
                break;
            case 'O':
                this->draw_to_scene(79);
                break;
            case 'P':
                this->draw_to_scene(80);
                break;
            case 'Q':
                this->draw_to_scene(81);
                break;
            case 'R':
                this->draw_to_scene(82);
                break;
            case 'S':
                this->draw_to_scene(83);
                break;
            case 'T':
                this->draw_to_scene(84);
                break;
            case 'U':
                this->draw_to_scene(85);
                break;
            case 'V':
                this->draw_to_scene(86);
                break;
            case 'W':
                this->draw_to_scene(87);
                break;
            case 'X':
                this->draw_to_scene(88);
                break;
            case 'Y':
                this->draw_to_scene(89);
                break;
            case 'Z':
                this->draw_to_scene(90);
                break;

                
            case 'a':
                this->draw_to_scene(97);
                break;
            case 'b':
                this->draw_to_scene(98);
                break;
            case 'c':
                this->draw_to_scene(99);
                break;
            case 'd':
                this->draw_to_scene(100);
                break;
            case 'e':
                this->draw_to_scene(101);
                break;
            case 'f':
                this->draw_to_scene(102);
                break;
            case 'g':
                this->draw_to_scene(103);
                break;
            case 'h':
                this->draw_to_scene(104);
                break;
            case 'i':
                this->draw_to_scene(105);
                break;
            case 'j':
                this->draw_to_scene(106);
                break;
            case 'k':
                this->draw_to_scene(107);
                break;
            case 'l':
                this->draw_to_scene(108);
                break;
            case 'm':
                this->draw_to_scene(109);
                break;
            case 'n':
                this->draw_to_scene(110);
                break;
            case 'o':
                this->draw_to_scene(111);
                break;
            case 'p':
                this->draw_to_scene(112);
                break;
            case 'q':
                this->draw_to_scene(113);
                break;
            case 'r':
                this->draw_to_scene(114);
                break;
            case 's':
                this->draw_to_scene(115);
                break;
            case 't':
                this->draw_to_scene(116);
                break;
            case 'u':
                this->draw_to_scene(117);
                break;
            case 'v':
                this->draw_to_scene(118);
                break;
            case 'w':
                this->draw_to_scene(119);
                break;
            case 'x':
                this->draw_to_scene(120);
                break;
            case 'y':
                this->draw_to_scene(121);
                break;
            case 'z':
                this->draw_to_scene(122);
                break;
                
            case '0':
                this->draw_to_scene(48);
                break;  
            case '1':
                this->draw_to_scene(49);
                break;
            case '2':
                this->draw_to_scene(50);
                break;
            case '3':
                this->draw_to_scene(51);
                break;
            case '4':
                this->draw_to_scene(52);
                break;
            case '5':
                this->draw_to_scene(53);
                break;
            case '6':
                this->draw_to_scene(54);
                break;
            case '7':
                this->draw_to_scene(55);
                break;
            case '8':
                this->draw_to_scene(56);
                break;
            case '9':
                this->draw_to_scene(57);
                break;
            case '!':
                this->draw_to_scene(33);
                break;
            case '?':
                this->draw_to_scene(63);
                break;
            case '.':
                this->draw_to_scene(46);
                break;
            case ',':
                this->draw_to_scene(44);
                break;
            case ':':
                this->draw_to_scene(58);
                break;
            case ';':
                this->draw_to_scene(59);
                break;
            case '/':
                this->draw_to_scene(47);
                break;
            case '-':
                this->draw_to_scene(130);
                break;
            case '|':
                this->draw_to_scene(124);
                break;
                
            case '#':
                newY+=this->offsetY;
                newX=this->beginX;
                break;
        }
    }
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}




