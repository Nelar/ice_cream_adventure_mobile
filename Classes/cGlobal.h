#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "cSingleton.h"

#define IPHONE_4 (CCDirector::sharedDirector()->getWinSize().height == 960 || CCDirector::sharedDirector()->getWinSize().width == 960)
#define IPAD_MINI (CCDirector::sharedDirector()->getWinSize().height == 1024 || CCDirector::sharedDirector()->getWinSize().width == 1024)
#define IPAD (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)
#define IPHONE_5 (CCDirector::sharedDirector()->getWinSize().height == 1136 || CCDirector::sharedDirector()->getWinSize().width == 1136)

//com.destinygroup.icecreamadventure
//com.triada.icecream

#define IPHONE_MULTIPLIER ((CCDirector::sharedDirector()->getWinSize().height == 640 || CCDirector::sharedDirector()->getWinSize().width == 640)?0.97f:1.0f)
#define CELL_WIDTH  ((CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?143.0f*(IPHONE_MULTIPLIER):70.0f*(IPHONE_MULTIPLIER))
#define CELL_HEIGHT ((CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?157.0f*(IPHONE_MULTIPLIER):70.0f*(IPHONE_MULTIPLIER))

#define MOVE_DELTA 0.2f
#define REFILL_DELTA 0.1f
#define HELP_TIME 5.0f

#define MAX_COLUMN 9
#define MAX_ROW 9

#define SIMPLE_SCORE 60
#define STONE_SCORE 20
#define MEGA_SCORE 50
#define ICE_SCORE 1000
#define CAGE_SCORE 500
#define CHOCOLATE_SCORE 20

#define POPUP_SHOW_TIME 0.5f

#define ELEMENT_SCALE ((CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?0.85f*IPHONE_MULTIPLIER:0.86f*IPHONE_MULTIPLIER)

#define WINSIZE CCDirector::sharedDirector()->getWinSize()

#define  LANDSCAPE CCDirector::sharedDirector()->getWinSize().height < CCDirector::sharedDirector()->getWinSize().width


enum IceCreamScene
{
    Menu,
    Map,
    Comix,
    Game,
    Invalid
};

class Globals : public cSingleton<Globals>
{
public:
	Globals():iceCreamScene(Invalid) {};
	~Globals(){};
    IceCreamScene iceCreamScene;
};

#define GlobalsPtr	Globals::GetSingletonPtr()
#endif