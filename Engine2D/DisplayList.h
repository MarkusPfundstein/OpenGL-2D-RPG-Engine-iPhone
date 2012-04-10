//
//  List.h
//  LinkedList
//
//  Created by Markus Pfundstein on 9/28/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef LinkedList_List_h
#define LinkedList_List_h

class AnimatedSprite;
class DisplayList
{
private:
    AnimatedSprite *itsObject;
    DisplayList  *itsNext;
    
public:
    DisplayList (AnimatedSprite *);
    ~DisplayList();
    
    void setNext(DisplayList * DisplayList) { itsNext = DisplayList; }
    
    DisplayList * getNext() const { return itsNext; }
    AnimatedSprite * getObject() const { return itsObject; }
    
    void Insert(DisplayList *);
    void Display();

};

// this 2 functions are copyright 2001 (C) Simon Tatham (adjusted to personal needs by Markus Pfundstein)
// http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.c
// ----------------------------------------------------
extern float cmp (DisplayList *a, DisplayList *b);

extern DisplayList* listsort(DisplayList *list, bool is_circular);

#endif
