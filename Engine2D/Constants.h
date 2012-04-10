//
//  Constants.h
//  OpenGL
//
//  Created by Markus Pfundstein on 9/10/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef OpenGL_Constants_h
#define OpenGL_Constants_h

// Screen Resolutions for iPhone/iPad
#define SCREEN_WIDTH_IPHONE    480
#define SCREEN_HEIGHT_IPHONE   320

#define DRAW_COLLISION_GRID    1
#define SHOW_FPS               1
#define SHOW_PLAYER_POS        1
#define SHOW_MONSTER_INFO      1

#define NUMBER_SPRITES_TILESET 16

#define NUMBER_SPRITES_CHARSET_X 16
#define NUMBER_SPRITES_CHARSET_Y 8
#define NUMBER_SPRITES_GUISET_X  2
#define NUMBER_SPRITES_GUISET_Y  1
#define NUMBER_SPRITES_OBJECTS_X 2
#define NUMBER_SPRITES_OBJECTS_Y 2

#define TILE_SIZE              32

#define MAX_BATTLES_PER_MAP 8

#define CHARACTER_COLL_OFFSETY  2
#define CHARACTER_COLL_OFFSETX  10
#define COL_OFFSET_DOWN        0
#define COL_OFFSET_LEFT        0
#define COL_OFFSET_RIGHT       0
#define COL_OFFSET_UP          0

#define NEW_GAME_START_X       18
#define NEW_GAME_START_Y       23

#define TILE_FLAG_TRANSPORTTILE 21
#define TILE_FLAG_EVENTTILE     17

enum ViewDirection {
    DOWN = 0,
    LEFT,
    RIGHT,
    UP
};

enum GameStates {
    INTRO = 0,
    MAIN_MENU = 1,
    WORLD_MAP = 2,
    DIALOG = 3,
    BATTLE = 4
};

enum WORLD_UPDATE_CALLBACK {
    CALLBACK_NO_BATTLE = 0,
    CALLBACK_BATTLE = 1,
};

enum ACTION_CODES {
    NO_ACTION = -1,
    ACTION_NPC = 1,
    ACTION_OBJECT = 2,
    ACTION_EVENTTILE = 3
};

#endif
