//
//  ScriptDef.h
//  Engine2D
//
//  Created by Markus Pfundstein on 9/26/11.
//  Copyright 2011 The Saints. All rights reserved.
//

#ifndef Engine2D_ScriptDef_h
#define Engine2D_ScriptDef_h

#define _END_OF_SCRIPT   "END_OF_SCRIPT"
#define _START_OF_SCRIPT "START_OF_SCRIPT"

#define _SCRIPT_NEW             "new"
#define _SCRIPT_LOAD            "load"
#define _SCRIPT_UNLOAD          "unload"
#define _SCRIPT_TRANSPORTTILE   "TransportTile"
#define _SCRIPT_EVENTTILE       "EventTile"
#define _SCRIPT_CHARSET         "Charset"
#define _SCRIPT_NPC             "NPC"
#define _SCRIPT_OBJECT          "OBJECT"
#define _SCRIPT_BATTLE          "BATTLE"
#define _SCRIPT_IF              "IF"
#define _SCRIPT_ENDIF           "ENDIF"
#define _SCRIPT_CHANGEVAL       "changeValue"
#define _SCRIPT_CHANGEBOOL      "changeSwitch"
#define _SCRIPT_GLOBALBOOL      "Switch"
#define _SCRIPT_GLOBALVAL       "Value"
#define _SCRIPT_INPUTDIALOG     "inputDialog"
#define _SCRIPT_INPUT1          "input1"
#define _SCRIPT_INPUT2          "input2"
#define _SCRIPT_INPUT3          "input3"
#define _SCRIPT_PLAYANI         "playAnimation"
#define _SCRIPT_SETANISTATE     "setAnimationState"
#define _SCRIPT_RESETANISTATE   "resetAnimationState"
#define _SCRIPT_PLAYER          "player"
#define _SCRIPT_THIS            "this"
#define _SCRIPT_PAUSE_ENGINE    "wait"
#define _SCRIPT_LOCKNPC         "lockNPC"
#define _SCRIPT_UNLOCKNPC       "unlockNPC"

#define _SCRIPT_DOWN            "DOWN"
#define _SCRIPT_RIGHT           "RIGHT"
#define _SCRIPT_LEFT            "LEFT"
#define _SCRIPT_UP              "UP"

#define _END_OF_DIALOG          "END_OF_DIALOG"
#define _END_OF_LINE            "]"

#define _KIFLAG_WALKAROUND      "NPC_WALKAROUND"
#define _KIFLAG_STAND_DOWN      "NPC_STAND_DOWN"
#define _KIFLAG_STAND_LEFT      "NPC_STAND_LEFT"
#define _KIFLAG_STAND_RIGHT     "NPC_STAND_RIGHT"
#define _KIFLAG_STAND_UP        "NPC_STAND_UP"

#define NPC_STAND_DOWN  0
#define NPC_STAND_LEFT  1
#define NPC_STAND_RIGHT 2
#define NPC_STAND_UP    3
#define NPC_WALKAROUND  4

#define MAX_GLOBAL_VARIABLES    1024

// for temporary use while reading out the script
typedef struct {
    int data;
    char* _string;
} TempTileWithFlags;



enum INPUTMODE {
    NO_INPUT = 0,
    INPUT_2_QUESTIONS = 2,          // to be conform with the script
    INPUT_3_QUESTIONS = 3
};


#endif
