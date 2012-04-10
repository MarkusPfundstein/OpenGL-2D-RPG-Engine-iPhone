//
//  DisplayList.cpp
//  Engine2D
//
//  Created by Markus Pfundstein on 9/29/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#include <iostream>
#include "AnimatedSprite.h"

DisplayList::DisplayList(AnimatedSprite* pChar):itsObject(pChar), itsNext(0)
{

}

DisplayList::~DisplayList()
{
    //printf("remove %s from list\n", this->getObject()->getName());
    if (itsNext)
        delete itsNext;
    itsObject = 0;
    itsNext = 0;
}
    
void DisplayList::Insert(DisplayList* newDisplayList)
{
    if (!itsNext)
        itsNext = newDisplayList;
    else
    {
        itsNext->Insert(newDisplayList);
    }
}
    
void DisplayList::Display()
{
    if (this->getObject()->getIsObject()==false)
        this->getObject()->drawAnimated();
    else
        this->getObject()->draw(this->getObject()->getAnimationState());
    
    if (itsNext)
        itsNext->Display();
}

// this 2 functions are copyright 2001 (C) Simon Tatham (adjusted to personal needs by Markus Pfundstein)
// http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.c
// ----------------------------------------------------
float cmp (DisplayList *a, DisplayList *b)
{
    return a->getObject()->getRect().pos.y - b->getObject()->getRect().pos.y;
}

DisplayList* listsort(DisplayList* list, bool is_circular)
{
    DisplayList *p, *q, *e, *tail, *oldhead;
    float insize, nmerges, psize, qsize, i;
    
    insize = 1;
    
    while (1)
    {
        p = list;
        oldhead = list;
        list = NULL;
        tail = NULL;
        
        nmerges = 0;
        
        while (p)
        {
            nmerges++;
            q = p;
            psize = 0;
            
            for (i = 0; i < insize; i++)
            {
                psize++;
                if (is_circular)
                    q = (q->getNext() == oldhead ? NULL : q->getNext());
                else
                    q = q->getNext();
                
                if (!q) break;
            }
            
            qsize = insize;
            
            while (psize > 0 || (qsize > 0 && q)) 
            {
                if (psize == 0) 
                {
                    e = q; q = q->getNext(); qsize--;
                    if (is_circular && q == oldhead) q = NULL;
                } 
                else if (qsize == 0 || !q) 
                {
                    e = p; p = p->getNext(); psize--;
                    if (is_circular && p == oldhead) p = NULL;
                } 
                else if (cmp(p,q) <= 0) 
                {
                    e = p; p = p->getNext(); psize--;
                    if (is_circular && p == oldhead) p = NULL;
                } 
                else 
                {
                    e = q; q = q->getNext(); qsize--;
                    if (is_circular && q == oldhead) q = NULL;
                }
                
                if (tail) 
                {
                    tail->setNext(e);
                } 
                else 
                {
                    list = e;
                }
                tail = e;
            }
            
            p = q;
        }
        if (is_circular) 
        {
            tail->setNext(list);
        } 
        else
            tail->setNext(NULL);
        
        if (nmerges <= 1)
            return list;
        
        insize *= 2;
    }
}