//
//  WorldMap.h
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef OpenGL_WorldMap_h
#define OpenGL_WorldMap_h

#include <OpenGLES/ES1/gl.h>
#include "Helpers.h"
#include "Constants.h"
#include "DisplayList.h"

class Texture2D;
class NPC;
class AnimatedSprite;
class Object;
class Script;
class WorldMap
{
    int nRows;
    int nCols;
    
    // for sprites
    GLuint vbo;
    GLuint texture;
    GLuint texobjects;
    GLuint *tbo;
    
    
    // display list draws NPCS and Objects in the right order to the screen.. objects get added while parsing the world from the worldfile and removed in the destructor. 
    DisplayList* displayList;
    // arrays of npcs and objects
    NPC *npcs;
    Object *objects;
    
    // 2d array of tiles
    TILE **tiles;
    
    // for teleporting issues
    Point2D newDestination;

    // new world is the world the player will get teleported to if he hits a transport tile, will be called in the suicide method if we delete the orld world and load a new world
    char newWorld[30];
    
    char battleList[MAX_BATTLES_PER_MAP][30];
    int numberDifferentBattles;
    u_int8_t possibility;
    
    // counters for sprites, npcs and obects
    // numSprites defines how much sprite we have in the tileset texture. 16x16 for example
    u_int8_t numSprites;
    u_int8_t npcs_counter;
    u_int8_t objects_counter;

    bool drawCollisionGrid;
    bool isActive;
    bool hasBattle;
    
    void drawCharacters();
    void parseLoadedFile(const char* filename);
    void create_buffers();
    void create_tbo(const GLfloat *texcoords, GLuint id);
public:

    WorldMap();
    ~WorldMap();
    
    bool initWorld(const char* filename, Script &_script);

    void draw(const Rect2D &viewpoint);
    
    void addTileFlagAt(int i, int j, uint8_t flag, uint8_t flagprop1, uint8_t flagprop2, const char* optString);
    
    void addNPCS(u_int8_t counter, NPC *newNPCs);
    void addObjects(u_int8_t counter, Object *newObjects);
    
    void createDisplayList(AnimatedSprite &_char);
    
    void suicide(Point2D newDest, const char* newMap);
    
    enum WORLD_UPDATE_CALLBACK updateWorld(AnimatedSprite &_char);
    
    bool getIsActive() const;
    
    float get_world_width() const;
    float get_world_height() const;
    
    TILE& getTileAt(int i, int j) const;
    Point2D getPlayerEntryPoint() const;
    
    char* getNewWorld();
    Point2D getNewDestination() const;
    
    u_int8_t getNPCcounter() const;
    u_int8_t getObjectCounter() const;
    
    NPC& getNPC(int number) const;
    Object& getObject(int number) const;
    
    void setNumberDifferentBattles(int n);
    int getNumberDifferentBattles() const;
    void setBattleListItem(u_int8_t atPosition, const char* scriptName);
    char* getBattleListItem(u_int8_t atPosition);
    void setPossibility(u_int8_t val);
    u_int8_t getPossibility() const;
    void setHasBattle(bool val);
    bool getHasBattle() const;
};

#endif
