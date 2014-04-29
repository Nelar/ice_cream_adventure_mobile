#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "cSingleton.h"
#include "cocos2d.h"
#include "cMoreGamesLayer.h"
#include "CCLocalizedString.h"

#define IPHONE_4 (CCDirector::sharedDirector()->getWinSize().height == 960 || CCDirector::sharedDirector()->getWinSize().width == 960)
#define IPAD_MINI (CCDirector::sharedDirector()->getWinSize().height == 1024 || CCDirector::sharedDirector()->getWinSize().width == 1024)
#define IPAD (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)
#define IPHONE_5 (CCDirector::sharedDirector()->getWinSize().height == 1136 || CCDirector::sharedDirector()->getWinSize().width == 1136)

//com.destinygroup.icecreamadventure
//com.triada.icecream

#define IPHONE_MULTIPLIER ((CCDirector::sharedDirector()->getWinSize().height == 640 || CCDirector::sharedDirector()->getWinSize().width == 640)?0.86f:1.0f)
#define CELL_WIDTH  ((CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?143.0f*(IPHONE_MULTIPLIER):72.0f*(IPHONE_MULTIPLIER))
#define CELL_HEIGHT ((CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?157.0f*(IPHONE_MULTIPLIER):79.0f*(IPHONE_MULTIPLIER))

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

#define ELEMENT_SCALE ((CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?0.85f*IPHONE_MULTIPLIER:0.85f*IPHONE_MULTIPLIER)

#define WINSIZE CCDirector::sharedDirector()->getWinSize()

#define  LANDSCAPE (CCDirector::sharedDirector()->getWinSize().height < CCDirector::sharedDirector()->getWinSize().width)
#define FONT_COMMON "Appetite"

#define FONT_SIZE_200 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?170:85
#define FONT_SIZE_140 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?140:70
#define FONT_SIZE_86 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?86:43
#define FONT_SIZE_64 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?64:32
#define FONT_SIZE_54 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?54:27
#define FONT_SIZE_48 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?48:24
#define FONT_SIZE_40 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?40:20
#define FONT_SIZE_36 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?36:18
#define FONT_SIZE_32 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?32:16
#define FONT_SIZE_26 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?26:13
#define FONT_SIZE_22 (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?22:11
#define MULTIPLIER (CCDirector::sharedDirector()->getWinSize().height == 2048 || CCDirector::sharedDirector()->getWinSize().width == 2048)?1.0f:0.5f

#define ITUNES_LINK "https://itunes.apple.com/us/app/ice-cream-adventure/id705058125?l=ru&ls=1&mt=8"

#define CONNECTION_TIME 70.0f

#undef PROD


static const cocos2d::ccColor3B IceCreamPink={0xba, 0x29, 0x91};
static const cocos2d::ccColor3B IceCreamBlue={0x29, 0x29, 0x93};
static const cocos2d::ccColor3B IceCreamBrown={0x7f, 0x3e, 0x7a};
static const cocos2d::ccColor3B IceCreamBrown2={0x91, 0x3b, 0x55};

enum IceCreamScene
{
    Menu,
    Map,
    Comix,
    Game,
    Loading,
    Invalid
};

class Globals : public cSingleton<Globals>
{
public:
	Globals():iceCreamScene(Invalid), booster_1(false), booster_2(false), booster_3(false), bannerShow(false) {};
	~Globals(){};
    IceCreamScene iceCreamScene;
    vector<sMoreGames> globalMoreGames;
    bool booster_1;
    bool booster_2;
    bool booster_3;
    bool bannerShow;
    
    string currency;
    float price;
    
    bool isLoadMap = false;
};

#define GlobalsPtr	Globals::GetSingletonPtr()
#endif