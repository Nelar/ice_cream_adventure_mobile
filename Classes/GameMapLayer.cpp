#include "GameMapLayer.h"
#include "GameScene.h"
#include "MainMenuScene.h"
#include "cGlobal.h"
#include <cmath>
#include "MMPInterface.h"
#include "MMP/Banner.h"
#include "cFacebook.h"
#include "cSocialMenu.h"
#include "cMapScrollLayer.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace cocos2d;
using namespace Core;
using namespace MarketingPlatform;

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

GameMapLayer::~GameMapLayer()
{
    this->stopAllActions();
    this->unscheduleAllSelectors();
    this->removeAllChildrenWithCleanup(true);
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("gameMap.plist");
    CCTextureCache::sharedTextureCache()->removeTextureForKey("gameMap.pvr.ccz");
}

CCScene* GameMapLayer::scene(int nextLevel)
{
	CCScene * scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(! scene);

		GameMapLayer *layer = GameMapLayer::create(nextLevel);
		CC_BREAK_IF(! layer);

		scene->addChild(layer);
	} while (0);

	return scene;
}

GameMapLayer* GameMapLayer::create(int nextLevel)
{
	GameMapLayer *pRet = new GameMapLayer(); 
	if (pRet && pRet->init(nextLevel))
	{ 
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

void GameMapLayer::scrollViewDidScroll(CCScrollView* view)
{
    int a = 0;
}

void GameMapLayer::scrollViewDidZoom(CCScrollView* view)
{
    int a = 0;
}

void GameMapLayer::updateMenuBooster()
{
    menu->updateBoosters();
}

void GameMapLayer::updateFacebook()
{
    menu->updateFacebook();
}

bool GameMapLayer::init(int nextLevel)
{
    CCDirector::sharedDirector()->setAnimationInterval(1.0f / 60.0f);
	if (!CCLayer::init())
		return false;
    
    if (FacebookPtr->sessionIsOpened())
        FacebookPtr->checkNotification();
    
    vector<sRequestData> requests = OptionsPtr->appRequests;
    
    GlobalsPtr->iceCreamScene = Map;
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("gameMap.plist", CCTextureCache::sharedTextureCache()->addPVRImage("gameMap.pvr.ccz"));
    
    if (LANDSCAPE)
    {
        if (WINSIZE.height == 640)
            popupScale = 0.85f;
        else
            popupScale = 1.0f;
    }
    else
    {
        if (WINSIZE.width == 640)
            popupScale = 0.85f;
        else
            popupScale = 1.0f;
    }
    
    nextlevelCurr = nextLevel;

	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sound/map_loop.mp3", true);

	this->setTouchEnabled(true);

	menu = MapMenuLayer::create();
	this->addChild(menu, 11);
    
    popupLayer = PopupLayer::create();
    this->addChild(popupLayer, 100);
    
    menu->addSocialLayer();

	leftDownMenu = LeftDownMenuScene::create();
	leftDownMenu->setBackScene(MainMenu);
	this->addChild(leftDownMenu, 10);
    
    if (OptionsPtr->getFirstGame() && FacebookPtr->sessionIsOpened())
    {
        OptionsPtr->setFirstGame();
        popupLayer->popupPost("Post on wall", "Post message on wall and get a bonus", "Post", GreenPopup, BombPopBoot,
                              this, callfuncN_selector(GameMapLayer::addPostBonus), this, callfuncN_selector(GameMapLayer::unclockMenu));
        leftDownMenu->setLock(true);
        leftDownMenu->setVisible(false);
        menu->setLock(true);
        OptionsPtr->save();
    }

	ccColor4B yellowColor;
	yellowColor.r = 0xe6;
	yellowColor.g = 0xe1;
	yellowColor.b = 0x88;
	yellowColor.a = 255;

	ccColor4B pinkColor;
	pinkColor.r = 0xfe;
	pinkColor.g = 0xb6;
	pinkColor.b = 0xd7;
	pinkColor.a = 255;
    
    ccColor4B blueColor;
	blueColor.r = 0x82;
	blueColor.g = 0xdb;
	blueColor.b = 0xdc;
	blueColor.a = 255;
    
    ccColor4B greenColor;
	greenColor.r = 0xaf;
	greenColor.g = 0xfe;
	greenColor.b = 0x75;
	greenColor.a = 255;

	vector<ccColor4B> colors;
    
    CCSize layerCS = CCSize(3500, 1000);
    float multiplier = 0.5f;
    if (WINSIZE.height < WINSIZE.width)
    {
        if (WINSIZE.width == 2048)
        {
            layerCS = CCSize(3500, 2000);
            multiplier = 1.0f;
        }
        else
        {
            layerCS = CCSize(1750, 1000);
            multiplier = 0.5f;
        }
    }
    else
    {
        if (WINSIZE.height == 2048)
        {
            layerCS = CCSize(3500, 2000);
            multiplier = 1.0f;
        }
        else
        {
            layerCS = CCSize(1750, 1000);
            multiplier = 0.5f;
        }
    }
    
    float offsetX = 200.0f*multiplier;
    float offsetY = 200.0f*multiplier;

	layerGradient = CCLayerPanZoom::create();//CCScrollView::create(CCSize(WINSIZE));
    if (WINSIZE.width != 2048 && WINSIZE.height != 2048)
        layerGradient->setContentSize(CCSize(2000, 7500));
    else
        layerGradient->setContentSize(CCSize(4500, 13000));
	layerGradient->setPosition(ccp(0.0f, 0.0f));

    
    
    layerGradient->setRubberEffectRatio(0.05f);
    layerGradient->setrubberEffectRecoveryTime(0.1f);
//    layerGradient->recoverPositionAndScale();
	this->addChild(layerGradient);
    
    
    CCTMXTiledMap *map = CCTMXTiledMap::create("tiles/wood.tmx");
	layerGradient->addChild(map, 1);
    map->setAnchorPoint(ccp(0.2f,0.1f));
    map->setPosition(ccp(offsetX, offsetY));
    if (WINSIZE.width != 2048 && WINSIZE.height != 2048)
        map->setScale(0.5f);
    
    CCTMXTiledMap *boardLeft = CCTMXTiledMap::create("tiles/board.tmx");
	layerGradient->addChild(boardLeft, 2);
    boardLeft->setAnchorPoint(ccp(0.0f,0.0f));
    boardLeft->setPosition(ccp(-138.0f*multiplier + offsetX, -134.0f*multiplier + offsetY));
    if (WINSIZE.width != 2048 && WINSIZE.height != 2048)
    {
        boardLeft->setScaleY(0.497f);
        boardLeft->setScaleX(0.495f);
    }
    if (WINSIZE.width == 2048 || WINSIZE.height == 2048)
    {
        boardLeft->setScaleY(0.994f);
        boardLeft->setScaleX(0.99f);
    }


	CCLayerGradient* layer_1 = CCLayerGradient::create(pinkColor, yellowColor);
	layer_1->setContentSize(layerCS);
    layer_1->setAnchorPoint(ccp(0.0f, 0.0f));
	layer_1->setPosition(ccp(offsetX, offsetY));
	layerGradient->addChild(layer_1,1);

    
	CCLayerGradient* layer_2 = CCLayerGradient::create(blueColor, pinkColor);
	layer_2->setContentSize(layerCS);
    layer_2->setAnchorPoint(ccp(0.0f, 0.0f));
	layer_2->setPosition(ccp(offsetX, 2000*multiplier + offsetY));
	layerGradient->addChild(layer_2, 1);

	CCLayerGradient* layer_3 = CCLayerGradient::create(greenColor, blueColor);
	layer_3->setContentSize(layerCS);
    layer_3->setAnchorPoint(ccp(0.0f, 0.0f));
	layer_3->setPosition(ccp(offsetX, 4000*multiplier + offsetY));
	layerGradient->addChild(layer_3, 1);

	CCLayerGradient* layer_4 = CCLayerGradient::create(yellowColor, greenColor);
	layer_4->setContentSize(layerCS);
    layer_4->setAnchorPoint(ccp(0.0f, 0.0f));
	layer_4->setPosition(ccp(offsetX, 6000*multiplier + offsetY));
	layerGradient->addChild(layer_4, 1);

	CCLayerGradient* layer_5 = CCLayerGradient::create(pinkColor, yellowColor);
	layer_5->setContentSize(layerCS);
    layer_5->setAnchorPoint(ccp(0.0f, 0.0f));
	layer_5->setPosition(ccp(offsetX, 8000*multiplier + offsetY));
	layerGradient->addChild(layer_5, 1);

	CCLayerGradient* layer_6 = CCLayerGradient::create(greenColor, pinkColor);
	layer_6->setContentSize(CCSize(3500*multiplier, 2350*multiplier));
    layer_6->setAnchorPoint(ccp(0.0f, 0.0f));
	layer_6->setPosition(ccp(offsetX, 10000*multiplier + offsetY));
	layerGradient->addChild(layer_6, 1);
    
    for (int i = 0; i < 41; i++)
    {
        CCSprite* frame = CCSprite::createWithSpriteFrameName("gameMap/frame.png");
        frame->setPosition(ccp(offsetX + 60*multiplier, offsetY + 210*multiplier + i*frame->getContentSize().width));
        frame->setRotation(90.0f);
        layerGradient->addChild(frame, 2);
    }
    
    CCSprite* frameleftDown = CCSprite::createWithSpriteFrameName("gameMap/frameAngle.png");
    frameleftDown->setPosition(ccp(offsetX + 49*multiplier, 54*multiplier + offsetY));
    frameleftDown->setFlipY(true);
    layerGradient->addChild(frameleftDown, 2);
    
    for (int i = 0; i < 11; i++)
    {
        CCSprite* frame = CCSprite::createWithSpriteFrameName("gameMap/frame.png");
        frame->setPosition(ccp(offsetX + 216*multiplier + i*frame->getContentSize().width, offsetY + 65*multiplier));
        layerGradient->addChild(frame, 2);
    }
    
    CCSprite* frameRightDown = CCSprite::createWithSpriteFrameName("gameMap/frameAngle.png");
    frameRightDown->setPosition(ccp(offsetX - 150*multiplier + 3500*multiplier, 54*multiplier + offsetY));
    frameRightDown->setFlipY(true);
    frameRightDown->setFlipX(true);
    layerGradient->addChild(frameRightDown, 2);
    
    for (int i = 0; i < 41; i++)
    {
        CCSprite* frame = CCSprite::createWithSpriteFrameName("gameMap/frame.png");
        frame->setPosition(ccp(offsetX - 160*multiplier  + 3500*multiplier, offsetY + 210*multiplier + i*frame->getContentSize().width));
        frame->setRotation(90.0f);
        layerGradient->addChild(frame, 2);
    }
    
    CCSprite* frameleftUp = CCSprite::createWithSpriteFrameName("gameMap/frameAngle.png");
    frameleftUp->setPosition(ccp(offsetX + 49*multiplier, 54*multiplier + offsetY + 12245*multiplier));
    layerGradient->addChild(frameleftUp, 2);
    
    for (int i = 0; i < 11; i++)
    {
        CCSprite* frame = CCSprite::createWithSpriteFrameName("gameMap/frame.png");
        frame->setPosition(ccp(offsetX + 216*multiplier + i*frame->getContentSize().width, offsetY + 65*multiplier + 12223*multiplier));
        layerGradient->addChild(frame, 2);
    }
    
    CCSprite* frameRightUp = CCSprite::createWithSpriteFrameName("gameMap/frameAngle.png");
    frameRightUp->setPosition(ccp(offsetX - 150*multiplier + 3500*multiplier, 54*multiplier + offsetY + 12245*multiplier));
    frameRightUp->setFlipX(true);
    layerGradient->addChild(frameRightUp, 2);
	
	CCSize s = CCDirector::sharedDirector()->getWinSize();
	path.push_back(ccp(280*multiplier, 600*multiplier));
	path.push_back(ccp(820*multiplier, 400*multiplier));
	path.push_back(ccp(1300*multiplier, 400*multiplier));
	path.push_back(ccp(1800*multiplier, 400*multiplier));
	path.push_back(ccp(2320*multiplier, 500*multiplier));	
	path.push_back(ccp(2880*multiplier, 550*multiplier));
	path.push_back(ccp(3000*multiplier, 1000*multiplier));	
	path.push_back(ccp(2450*multiplier, 1000*multiplier));
	path.push_back(ccp(1900*multiplier, 1000*multiplier));
	path.push_back(ccp(1300*multiplier, 1000*multiplier));
	path.push_back(ccp(800*multiplier, 1300*multiplier));
	path.push_back(ccp(650*multiplier, 1650*multiplier));
	path.push_back(ccp(1000*multiplier, 2000*multiplier));
	path.push_back(ccp(1500*multiplier, 1750*multiplier));
	path.push_back(ccp(2000*multiplier, 1550*multiplier));
	path.push_back(ccp(2600*multiplier, 1700*multiplier));	
	path.push_back(ccp(3000*multiplier, 1900*multiplier));	
	path.push_back(ccp(3200*multiplier, 2400*multiplier));	
	path.push_back(ccp(2860*multiplier, 2900*multiplier));	
	path.push_back(ccp(2300*multiplier, 3000*multiplier));	
	path.push_back(ccp(1900*multiplier, 2600*multiplier));
	path.push_back(ccp(1360*multiplier, 2400*multiplier));	
	path.push_back(ccp(750*multiplier, 2300*multiplier));
	path.push_back(ccp(280*multiplier, 2600*multiplier));
	path.push_back(ccp(200*multiplier, 3200*multiplier));
	path.push_back(ccp(700*multiplier, 3300*multiplier));
	path.push_back(ccp(950*multiplier, 2900*multiplier));
	path.push_back(ccp(1500*multiplier, 3000*multiplier));
	path.push_back(ccp(1760*multiplier, 3300*multiplier));
	path.push_back(ccp(2300*multiplier, 3400*multiplier));
	path.push_back(ccp(2750*multiplier, 3650*multiplier));
	path.push_back(ccp(2560*multiplier, 4100*multiplier));
	path.push_back(ccp(3100*multiplier, 4400*multiplier));
	path.push_back(ccp(2700*multiplier, 4760*multiplier));
	path.push_back(ccp(2200*multiplier, 4600*multiplier));
	path.push_back(ccp(1950*multiplier, 4150*multiplier));
	path.push_back(ccp(1450*multiplier, 4000*multiplier));
	path.push_back(ccp(1000*multiplier, 3900*multiplier));
	path.push_back(ccp(430*multiplier, 4000*multiplier));
	path.push_back(ccp(240*multiplier, 4500*multiplier));
	path.push_back(ccp(700*multiplier, 4600*multiplier));
	path.push_back(ccp(1100*multiplier, 4850*multiplier));
	path.push_back(ccp(1530*multiplier, 5000*multiplier));
	path.push_back(ccp(2100*multiplier, 4950*multiplier));
	path.push_back(ccp(2700*multiplier, 5100*multiplier));
	path.push_back(ccp(3050*multiplier, 5450*multiplier));
	path.push_back(ccp(3200*multiplier, 6000*multiplier));
	path.push_back(ccp(2900*multiplier, 6500*multiplier));
	path.push_back(ccp(2400*multiplier, 6500*multiplier));
	path.push_back(ccp(2100*multiplier, 6150*multiplier));
	path.push_back(ccp(1900*multiplier, 5600*multiplier));
	path.push_back(ccp(1500*multiplier, 5300*multiplier));
	path.push_back(ccp(930*multiplier, 5200*multiplier));
	path.push_back(ccp(460*multiplier, 5320*multiplier));
	path.push_back(ccp(380*multiplier, 5770*multiplier));
	path.push_back(ccp(870*multiplier, 5760*multiplier));
	path.push_back(ccp(1360*multiplier, 5760*multiplier));
	path.push_back(ccp(1630*multiplier, 6150*multiplier));
	path.push_back(ccp(2000*multiplier, 6600*multiplier));
	path.push_back(ccp(2500*multiplier, 6800*multiplier));
	path.push_back(ccp(2720*multiplier, 7100*multiplier));
	path.push_back(ccp(2120*multiplier, 7100*multiplier));
	path.push_back(ccp(1500*multiplier, 7150*multiplier));
	path.push_back(ccp(980*multiplier, 7000*multiplier));
	path.push_back(ccp(560*multiplier, 6700*multiplier));
	path.push_back(ccp(250*multiplier, 7200*multiplier));
	path.push_back(ccp(636*multiplier, 7500*multiplier));
	path.push_back(ccp(1150*multiplier, 7450*multiplier));
	path.push_back(ccp(1750*multiplier, 7470*multiplier));
	path.push_back(ccp(2350*multiplier, 7570*multiplier));
	path.push_back(ccp(2900*multiplier, 7500*multiplier));
	path.push_back(ccp(3250*multiplier, 7900*multiplier));
	path.push_back(ccp(3250*multiplier, 8500*multiplier));
	path.push_back(ccp(2950*multiplier, 9000*multiplier));
	path.push_back(ccp(2570*multiplier, 8950*multiplier));
	path.push_back(ccp(2650*multiplier, 8450*multiplier));
	path.push_back(ccp(2520*multiplier, 7950*multiplier));
	path.push_back(ccp(1970*multiplier, 7800*multiplier));
	path.push_back(ccp(1450*multiplier, 8000*multiplier));
	path.push_back(ccp(1100*multiplier, 8500*multiplier));	
	path.push_back(ccp(650*multiplier, 8700*multiplier));
	path.push_back(ccp(330*multiplier, 9000*multiplier));
	path.push_back(ccp(720*multiplier, 9450*multiplier));
	path.push_back(ccp(1180*multiplier, 9120*multiplier));
	path.push_back(ccp(1760*multiplier, 9000*multiplier));
	path.push_back(ccp(2200*multiplier, 9300*multiplier));
	path.push_back(ccp(2700*multiplier, 9600*multiplier));
	path.push_back(ccp(3200*multiplier, 9800*multiplier));
	path.push_back(ccp(3250*multiplier, 10450*multiplier));
	path.push_back(ccp(3000*multiplier, 10900*multiplier));
	path.push_back(ccp(2420*multiplier, 10900*multiplier));
	path.push_back(ccp(2000*multiplier, 10600*multiplier));
	path.push_back(ccp(1750*multiplier, 10050*multiplier));
	path.push_back(ccp(1280*multiplier, 9800*multiplier));
	path.push_back(ccp(720*multiplier, 9850*multiplier));
	path.push_back(ccp(350*multiplier, 10250*multiplier));
	path.push_back(ccp(400*multiplier, 10750*multiplier));
	path.push_back(ccp(600*multiplier, 11200*multiplier));
	path.push_back(ccp(1120*multiplier, 11450*multiplier));
	path.push_back(ccp(1700*multiplier, 11380*multiplier));
	path.push_back(ccp(2260*multiplier, 11200*multiplier));
	path.push_back(ccp(2800*multiplier, 11350*multiplier));
	path.push_back(ccp(3000*multiplier, 11850*multiplier));
	path.push_back(ccp(2500*multiplier, 11800*multiplier));
	path.push_back(ccp(1900*multiplier, 11850*multiplier));
    
    for (int i = 0; i < path.size(); i++)
    {
        path[i].x+=offsetX;
        path[i].y+=offsetY;
    }
	
	float tension = 0.0f;
	float coef = 0.0f;
	while (coef < 1.0f)
	{		
		CCPoint nP = ccCardinalSplineAt(path[0], path[0], path[1], path[2], tension, coef);
		newPath.push_back(nP);		
		coef+=0.1f;
	}	
	for (int i = 0; i < path.size() - 3; i++)
	{
		coef = 0.0f;
		while (coef < 1.0f)
		{			
			CCPoint nP = ccCardinalSplineAt(path[i], path[i + 1], path[i + 2], path[i + 3], tension, coef);
			newPath.push_back(nP);
			coef+=0.1f;
		}
	}
	coef = 0.0f;
	while (coef < 1.0f)
	{
		CCPoint nP = ccCardinalSplineAt(path[path.size() - 3], path[path.size() - 2], path[path.size() - 1], path[path.size() - 1], tension, coef);
		newPath.push_back(nP);			
		coef+=0.1f;
	}

	currentIdx = 0;


	texBlue = CCTextureCache::sharedTextureCache()->addImage("gameMap/kontur1.png");	
    if (WINSIZE.width != 2048 && WINSIZE.height != 2048)
        streak = IceMotionStreak::create(1.0f, 5.0f, 40.0f, ccWHITE, texBlue);
    else
        streak = IceMotionStreak::create(1.0f, 5.0f, 80.0f, ccWHITE, texBlue);

	streak->setPosition(newPath[currentIdx++]);
	layerGradient->addChild(streak, 4);
	drawPoins(this);


	levels = CCMenu::create();	
	layerGradient->addChild(levels, 5);
	for (int i = 0; i < path.size(); i++)
	{
		CCSprite* normal;
		CCSprite* select;
		CCSprite* disable;

		if (OptionsPtr->getLevelData(i).levelType == Score)
		{
			normal = CCSprite::create("gameMap/orange.png");
			select = CCSprite::create("gameMap/orange.png");
		}
		else if (OptionsPtr->getLevelData(i).levelType == Ice)
		{
			normal = CCSprite::create("gameMap/blue.png");
			select = CCSprite::create("gameMap/blue.png");
		}
		else if (OptionsPtr->getLevelData(i).levelType == BringDown)
		{
			normal = CCSprite::create("gameMap/green.png");
			select = CCSprite::create("gameMap/green.png");
		}
		else if (OptionsPtr->getLevelData(i).levelType == Time)
		{
			normal = CCSprite::create("gameMap/violet.png");
			select = CCSprite::create("gameMap/violet.png");
		}
        
        disable = CCSprite::create("gameMap/gray.png");

        select->setColor(ccGRAY);
        disable->setColor(ccGRAY);

		char buf[64];
		sprintf(buf, "%d", i + 1);
		CCLabelBMFont* labelNormal = CCLabelBMFont::create(buf, "fonts/Script MT Bold 36.fnt");
		CCLabelBMFont* labelSelect = CCLabelBMFont::create(buf, "fonts/Script MT Bold 36.fnt");
		labelNormal->setPosition(ccp(normal->getContentSize().width/2.0f, normal->getContentSize().height/2.0f));
		labelSelect->setPosition(ccp(select->getContentSize().width/2.0f, select->getContentSize().height/2.0f));
		labelSelect->setColor(ccGRAY);
        if (WINSIZE.width != 2048 && WINSIZE.height != 2048)
        {
            labelNormal->setScale(0.5f);
            labelSelect->setScale(0.5f);
        }


		normal->addChild(labelNormal);
		select->addChild(labelSelect);

		CCMenuItemSprite* level = CCMenuItemSprite::create(normal, select, disable, this, menu_selector(GameMapLayer::levelCallback));		
		level->setPosition(path[i].x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
			path[i].y - CCDirector::sharedDirector()->getWinSize().height/2.0f);
		levels->addChild(level);
		level->setTag(i);
        
        if (!((i+1)%12) || !((i)%12))
        {
            CCSprite* circleDown = CCSprite::create("gameMap/circleDown.png");
            circleDown->setPosition(ccp(level->getContentSize().width/2.0f, level->getContentSize().height/2.5f));
            level->addChild(circleDown, -2);
        }

		select->setScale(0.9f);
		select->setPosition(ccp(select->getContentSize().width/20.0f, select->getContentSize().height/20.0f));
		
		if (OptionsPtr->getLevelData(i).countStar == 1)
		{
			CCSprite* star_1 = CCSprite::createWithSpriteFrameName("common/star.png");
            star_1->setScale(0.3f);
			level->addChild(star_1);
			star_1->setPosition(ccp(star_1->getPositionX() - 80*multiplier + level->getContentSize().width/2.0f, star_1->getPositionY() - 10*multiplier));
		}
		else if (OptionsPtr->getLevelData(i).countStar == 2)
		{
			CCSprite* star_1 = CCSprite::createWithSpriteFrameName("common/star.png");
			level->addChild(star_1);
            star_1->setScale(0.3f);
			star_1->setPosition(ccp(star_1->getPositionX() - 80*multiplier + level->getContentSize().width/2.0f, star_1->getPositionY() - 10*multiplier));

			CCSprite* star_2 = CCSprite::createWithSpriteFrameName("common/star2.png");
			level->addChild(star_2);
            star_2->setScale(0.3f);
			star_2->setPosition(ccp(star_2->getPositionX() + level->getContentSize().width/2.0f, star_2->getPositionY() - 40*multiplier));
		}
		else if (OptionsPtr->getLevelData(i).countStar >= 3)
		{
			CCSprite* star_1 = CCSprite::createWithSpriteFrameName("common/star.png");
			level->addChild(star_1);
            star_1->setScale(0.3f);
			star_1->setPosition(ccp(star_1->getPositionX() - 80*multiplier + level->getContentSize().width/2.0f, star_1->getPositionY() - 10*multiplier));

			CCSprite* star_2 = CCSprite::createWithSpriteFrameName("common/star2.png");
			level->addChild(star_2);
            star_2->setScale(0.3f);
			star_2->setPosition(ccp(star_2->getPositionX() + level->getContentSize().width/2.0f, star_2->getPositionY() - 40*multiplier));

			CCSprite* star_3 = CCSprite::createWithSpriteFrameName("common/star3.png");
			level->addChild(star_3);
            star_3->setScale(0.3f);
			star_3->setPosition(ccp(star_3->getPositionX() + 80*multiplier + level->getContentSize().width/2.0f, star_3->getPositionY() - 10*multiplier));
		}
        
        if (FacebookPtr->sessionIsOpened())
        if ((i + 1) == OptionsPtr->getCurrentLevel())
		{
            CCSprite* circleFlik = CCSprite::createWithSpriteFrameName("gameMap/circleFlik.png");
            circleFlik->setPosition(ccp(level->getContentSize().width/2.0f, level->getContentSize().height/2.0f));
            level->addChild(circleFlik, -1);
            circleFlik->runAction(CCRepeatForever::create(CCSequence::create(CCScaleTo::create(0.3f, 0.7f), CCScaleTo::create(0.3f, 1.0f), NULL)));
            
            string str = FacebookPtr->getAvatar();
            CCSprite* avatar = CCSprite::create(str.c_str());
            level->addChild(avatar, 100);
            float avatarWidth = 128.0f*multiplier;
            avatar->setScaleX(avatarWidth/avatar->getContentSize().width);
            avatar->setScaleY(avatarWidth/avatar->getContentSize().height);
            avatar->setPosition(ccp(level->getContentSize().width/2.0f, level->getContentSize().height/2.0f - avatar->getContentSize().height*1.5f*avatar->getScaleY()));
            CCSprite* border = CCSprite::create("avatarBorder.png");
            border->setScaleX(((avatarWidth + 15)/border->getContentSize().width)/avatar->getScaleX());
            border->setScaleY(((avatarWidth + 15)/border->getContentSize().height)/avatar->getScaleY());
            border->setPosition(ccp(avatar->getContentSize().width/2.0f, avatar->getContentSize().height/2.0f));
            avatar->addChild(border);
        }
        
        for (int k = 0; k < FacebookPtr->friendsScores.size(); k++)
        {
            int friendCurrentLevel = -1;
            for (int j = FacebookPtr->friendsScores[k].scores.size() - 1; j >=0; j--)
            {
                if (FacebookPtr->friendsScores[k].scores[j] != 0)
                {
                    friendCurrentLevel = j;
                    break;
                }
            }
            if (FacebookPtr->sessionIsOpened())
            if ((i + 1) == friendCurrentLevel)
            {
                string avatarFileName = FacebookPtr->getWorkDirectory() + "/" + FacebookPtr->friendsScores[k].uid + ".png";
                CCSprite* avatar = CCSprite::create(avatarFileName.c_str());
                level->addChild(avatar, 100);
                float avatarWidth = 128.0f*multiplier;
                avatar->setScaleX(avatarWidth/avatar->getContentSize().width);
                avatar->setScaleY(avatarWidth/avatar->getContentSize().height);
                avatar->setPosition(ccp(level->getContentSize().width/2.0f, level->getContentSize().height/2.0f - avatar->getContentSize().height*1.5f*avatar->getScaleY()));
                
                CCSprite* border = CCSprite::create("avatarBorder.png");
                border->setScaleX(((avatarWidth + 15)/border->getContentSize().width)/avatar->getScaleX());
                border->setScaleY(((avatarWidth + 15)/border->getContentSize().height)/avatar->getScaleY());
                border->setPosition(ccp(avatar->getContentSize().width/2.0f, avatar->getContentSize().height/2.0f));
                avatar->addChild(border);
            }
        }

		if ((i + 1) > OptionsPtr->getCurrentLevel())
		{
            int numLev = (i/7) + 1;
			if (numLev > 9)
				numLev -=9;
            ccColor3B color;
            if (numLev == 1)
                color = ccBLUE;
            else if (numLev == 2)
                color = ccGREEN;
            else if (numLev == 3)
                color = ccMAGENTA;
            else if (numLev == 4)
                color = ccYELLOW;
            else if (numLev == 5)
                color = ccORANGE;
            else if (numLev == 6)
                color = ccRED;
            else if (numLev == 7)
                color = ccBLUE;
            else if (numLev == 8)
                color = ccGREEN;
            else if (numLev == 9)
                color = ccGREEN;
			level->setEnabled(false);
			level->setColor(color);
		}
	}
    
    CCSprite* normal;
    CCSprite* select;
    CCSprite* disable;
    
    normal = CCSprite::create("superLevel.png");
    disable = CCSprite::create("gameMap/gray.png");
    select = CCSprite::createWithSpriteFrame(normal->displayFrame());
    select->setColor(ccGRAY);
    
    CCMenuItemSprite* slevel = CCMenuItemSprite::create(normal, select, disable, this, menu_selector(GameMapLayer::superLevelCallback));
    slevel->setPosition(path[29].x - CCDirector::sharedDirector()->getWinSize().width/2.0f - 50*multiplier,
                       path[29].y - CCDirector::sharedDirector()->getWinSize().height/2.0f + 300*multiplier);
    levels->addChild(slevel);
    
    if (OptionsPtr->getCurrentLevel() < 31)
        slevel->setEnabled(false);

    
    levelPicsDraw();
    levelLabelsDraw();

    prevDistance = 0.0f;
    touchNumber = 0;
    isPinch = false;
    lock = false;
    
    this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameMapLayer::update)));
    
    boy = CCSprite::createWithSpriteFrameName("gameMap/char-1.png");
    boy->setScale(0.7f);
    boy->setPosition(ccp(path[OptionsPtr->getCurrentLevel() - 1].x - boy->getContentSize().width/2.5f*boy->getScale(),
                         path[OptionsPtr->getCurrentLevel() - 1].y + boy->getContentSize().height/1.5f*boy->getScale()));
    
    icelittle = CCSprite::createWithSpriteFrameName("gameMap/char-2.png");
    boy->addChild(icelittle);
    icelittle->setPosition(ccp(boy->getContentSize().width + icelittle->getContentSize().width/2.5f,
                               icelittle->getContentSize().height/2.0f));
    
    layerGradient->addChild(boy, 10);
    
    if (nextLevel > 0)
    {
        boy->setPosition(ccp(path[nextLevel - 2].x - boy->getContentSize().width/2.5f*boy->getScale(),
                             path[nextLevel - 2].y + boy->getContentSize().height/1.5f*boy->getScale()));
        
        boy->runAction(CCSequence::createWithTwoActions(CCMoveTo::create(2.0f, ccp(path[nextLevel - 1].x - boy->getContentSize().width/2.5f*boy->getScale(),                                                                                   path[nextLevel - 1].y + boy->getContentSize().height/1.5f*boy->getScale())),
            CCCallFuncN::create(this, callfuncN_selector( GameMapLayer::boyMoveFinished))));
        
 //       ((CCMenuItemSprite*)levels->getChildByTag(nextLevel))->setEnabled(false);
        SimpleAudioEngine::sharedEngine()->playEffect("sound/footstep.mp3");
        nextLevelForSender = nextLevel;
    }
    else
    {
        boy->setPosition(ccp(path[OptionsPtr->getLastGameLevel() - 1].x - boy->getContentSize().width/2.5f*boy->getScale(),
                             path[OptionsPtr->getLastGameLevel() - 1].y + boy->getContentSize().height/1.5f*boy->getScale()));
        nextLevel = OptionsPtr->getLastGameLevel();
    }
    
   /* layerGradient->setPosition(ccp(100,
                                 -path[nextLevel - 1].y * layerGradient->getScale() + CCDirector::sharedDirector()->getWinSize().height/2.0f));
	*/
    
    dark = CCSprite::create("game/cell.png");
    dark->setScale(1000);
    this->addChild(dark, 9);
    dark->setOpacity(0);
    dark->setColor(ccBLACK);
    this->setTouchEnabled(true);
    
    
    if (IPAD)
    {
        if (LANDSCAPE)
        {
            layerGradient->setMinScale(0.56f);
            layerGradient->setMaxScale(1.0f);
            
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 2400.0f, 3000.0f));
            
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            
            layerGradient->setScale(0.56f);
            layerGradient->setPosition(ccp(-40.0f, -path[nextLevel - 1].y * layerGradient->getScale() + CCDirector::sharedDirector()->getWinSize().height/2.0f));
            
            layerGradient->recoverPositionAndScale();            
        }
        else
        {
            layerGradient->setMinScale(0.42f);
            layerGradient->setMaxScale(1.0f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 1800.0f, 3000.0f));
            
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->setScale(0.42f);
            layerGradient->setPosition(ccp(-30.0f, -path[nextLevel - 1].y * layerGradient->getScale() + CCDirector::sharedDirector()->getWinSize().height/2.0f));
            layerGradient->recoverPositionAndScale();
        }
    }
    else if (IPHONE_4)
    {
        if (LANDSCAPE)
        {
            layerGradient->setMinScale(0.7f);
            layerGradient->setMaxScale(1.2f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 1030.0f, 2200.0f));
            
            layerGradient->setPosition(ccp(0.0f, 0.0f));
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->recoverPositionAndScale();
            
            layerGradient->setScale(0.7f);
        }
        else
        {
            layerGradient->setMinScale(0.5f);
            layerGradient->setMaxScale(1.2f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 680.0f, 2200.0f));
            
            layerGradient->setPosition(ccp(0.0f, 0.0f));
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->recoverPositionAndScale();
            
            layerGradient->setScale(0.5f);
        }
    }
    else if (IPHONE_5)
    {
        if (LANDSCAPE)
        {
            layerGradient->setMinScale(0.7f);
            layerGradient->setMaxScale(1.2f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 1200.0f, 2200.0f));
            
            layerGradient->setPosition(ccp(0.0f, 0.0f));
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->recoverPositionAndScale();
            
            layerGradient->setScale(0.7f);
        }
        else
        {
            layerGradient->setMinScale(0.5f);
            layerGradient->setMaxScale(1.2f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 680.0f, 2200.0f));
            
            layerGradient->setPosition(ccp(0.0f, 0.0f));
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->recoverPositionAndScale();
            
            layerGradient->setScale(0.5f);
        }
    }
    else
    {
        if (LANDSCAPE)
        {
            layerGradient->setMinScale(0.56f);
            layerGradient->setMaxScale(1.0f);
            
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 1050.0f, 2000.0f));
            
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            
            layerGradient->setScale(0.56f);
            layerGradient->setPosition(ccp(-20.0f, -15.0f));
            
            layerGradient->recoverPositionAndScale();
        }
        else
        {
            layerGradient->setMinScale(0.42f);
            layerGradient->setMaxScale(1.0f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 800.0f, 2000.0f));
            
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->setScale(0.42f);
            layerGradient->setPosition(ccp(-15.0f, -15.0f));
            layerGradient->recoverPositionAndScale();
        }
    }
    
	return true;
}

void GameMapLayer::showMessageBoard()
{
    menu->showMessageboard();
}

void GameMapLayer::hideMessageBoard()
{
    menu->showMessageboard();
}

void GameMapLayer::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, 1);
}

void GameMapLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    if (!menu->isPopup)
    {
        if (leftDownMenu->isLock())
            leftDownMenu->menuSettingCallback(NULL);
    }
    menu->ccTouchesBegan(NULL, NULL);
}

void GameMapLayer::superLevelCallback(CCObject* pSender)
{
    if (leftDownMenu->isLock())
		return;
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    layerGradient->clearTouches();
    menu->levelPopup(106, OptionsPtr->getLevelData(106).countStar, OptionsPtr->getLevelData(106).countScore, OptionsPtr->getLevelData(106).levelType, BoosterCrystal, BoosterBomb, BoosterFish);
}

void GameMapLayer::changeOrientation()
{
    leftDownMenu->changeOrientation();
    menu->changeOrientation();
    
    if (IPAD)
    {
        if (LANDSCAPE)
        {
            layerGradient->setMinScale(0.56f);
            layerGradient->setMaxScale(1.0f);
            
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 2400.0f, 3000.0f));
            
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            
            layerGradient->setScale(0.56f);
            layerGradient->setPosition(ccp(-40.0f, -30.0f));
            
            layerGradient->recoverPositionAndScale();
        }
        else
        {
            layerGradient->setMinScale(0.42f);
            layerGradient->setMaxScale(1.0f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 1800.0f, 3000.0f));
            
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->setScale(0.42f);
            layerGradient->setPosition(ccp(-30.0f, -30.0f));
            layerGradient->recoverPositionAndScale();
        }
    }
    else if (IPHONE_4)
    {
        if (LANDSCAPE)
        {
            layerGradient->setMinScale(0.7f);
            layerGradient->setMaxScale(1.2f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 1030.0f, 2200.0f));
            
            layerGradient->setPosition(ccp(0.0f, 0.0f));
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->recoverPositionAndScale();
            
            layerGradient->setScale(0.7f);
        }
        else
        {
            layerGradient->setMinScale(0.5f);
            layerGradient->setMaxScale(1.2f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 680.0f, 2200.0f));
            
            layerGradient->setPosition(ccp(0.0f, 0.0f));
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->recoverPositionAndScale();
            
            layerGradient->setScale(0.5f);
        }
    }
    else if (IPHONE_5)
    {
        if (LANDSCAPE)
        {
            layerGradient->setMinScale(0.7f);
            layerGradient->setMaxScale(1.2f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 1200.0f, 2200.0f));
            
            layerGradient->setPosition(ccp(0.0f, 0.0f));
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->recoverPositionAndScale();
            
            layerGradient->setScale(0.7f);
        }
        else
        {
            layerGradient->setMinScale(0.5f);
            layerGradient->setMaxScale(1.2f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 680.0f, 2200.0f));
            
            layerGradient->setPosition(ccp(0.0f, 0.0f));
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->recoverPositionAndScale();
            
            layerGradient->setScale(0.5f);
        }
    }
    else
    {
        if (LANDSCAPE)
        {
            layerGradient->setMinScale(0.56f);
            layerGradient->setMaxScale(1.0f);
            
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 1050.0f, 2000.0f));
            
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            
            layerGradient->setScale(0.56f);
            layerGradient->setPosition(ccp(-20.0f, -15.0f));
            
            layerGradient->recoverPositionAndScale();
        }
        else
        {
            layerGradient->setMinScale(0.42f);
            layerGradient->setMaxScale(1.0f);
            layerGradient->setPanBoundsRect(CCRect(0.0f, 0.0f, 800.0f, 2000.0f));
            
            layerGradient->setAnchorPoint(ccp(0.0f, 0.0f));
            layerGradient->setScale(0.42f);
            layerGradient->setPosition(ccp(-15.0f, -15.0f));
            layerGradient->recoverPositionAndScale();
        }
    }
    
    if (WINSIZE.width != 2048 && WINSIZE.height != 2048)
        layerGradient->setContentSize(CCSize(2000, 7500));
    else
        layerGradient->setContentSize(CCSize(4500, 13000));
    //scrollView->setViewSize(WINSIZE);
}

void GameMapLayer::boyMoveFinished(CCNode* sender)
{
    ((CCMenuItemSprite*)levels->getChildByTag(OptionsPtr->getCurrentLevel() - 1))->setEnabled(true);
    
    CCParticleSystemQuad* explosion = CCParticleExplosion::createWithTotalParticles(50);
    sender->addChild(explosion, 10);
    explosion->setPosition(path[nextlevelCurr - 1].x,
                           path[nextlevelCurr - 1].y);
    explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/star.png"));
    explosion->setSpeed(100.0f);
    explosion->setSpeedVar(10.0f);
    explosion->setScale(3.0f);
    explosion->setGravity(ccp(0.0f, -50.0f));
    explosion->setEndSpin(800.0f);
    explosion->setBlendAdditive(true);
    ccBlendFunc blend;
    blend.dst = GL_ONE;
    blend.src = GL_ONE;
    explosion->setBlendFunc(blend);
    
    explosion->setEndColor(ccc4f(0, 255, 255, 255));
    explosion->setEndColorVar(ccc4f(0, 0, 0, 0));
    explosion->setStartColor(ccc4f(229, 229, 229, 229));
    explosion->setStartColorVar(ccc4f(0, 0, 0, 25));
    explosion->setEndSize(0.0f);
    explosion->setLife(3.0f);

    CCNode* node = CCNode::create();
    node->setTag(nextLevelForSender - 1);
    levelCallback(node);
}

void GameMapLayer::unclockMenu(CCNode* pSender)
{
    leftDownMenu->setLock(false);
    menu->setLock(false);
    leftDownMenu->setVisible(true);
}

void GameMapLayer::addBonus(CCNode* pSender)
{
    FacebookPtr->inviteFriends();
}

void GameMapLayer::addPostBonus(CCNode* pSender)
{
    FacebookPtr->beginGamePost();
}

void GameMapLayer::update(CCNode* sender)
{
    if (leftDownMenu->isInvite)
    {
        leftDownMenu->setLock(true);
        leftDownMenu->setVisible(false);
        leftDownMenu->isInvite = false;
        popupLayer->popupPost("Invite friends", "Invite friends and get a bonus", "Invite", GreenPopup, BombPopBoot,
                              this, callfuncN_selector(GameMapLayer::addBonus), this, callfuncN_selector(GameMapLayer::unclockMenu));
    }
    else if ((leftDownMenu->isLock() || menu->isLock()) && !lock)
    {
        if (menu->isPopup)
            leftDownMenu->setVisible(false);
        leftDownMenu->setLock(true);
        menu->setLock(true);
        lock = true;
        levels->setEnabled(false);
        for (int i = 0; i < levels->getChildrenCount(); i++)
        {
            ((CCNode*)levels->getChildren()->objectAtIndex(i))->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(0.6f), CCScaleTo::create(0.1, 0.01f)));
        }
        for (int i = 0; i <levelPics.size(); i++)
        {
            levelPics[i]->runAction(CCFadeOut::create(0.6f));
        }
        boy->runAction(CCFadeOut::create(0.6f));
        icelittle->runAction(CCFadeOut::create(0.6f));
        layerGradient->setTouchEnabled(false);
        dark->runAction(CCFadeTo::create(0.5f, 150));
        menu->hideLive();
//        scrollView->setTouchEnabled(false);
        
        
    }
    else if ((!leftDownMenu->isLock() || !menu->isLock()) && lock)
    {
        leftDownMenu->setVisible(true);
        leftDownMenu->setTouchEnabled(true);
        leftDownMenu->setLock(false);
        menu->setLock(false);
        lock = false;
        levels->setEnabled(true);
        for (int i = 0; i < levels->getChildrenCount(); i++)
        {
            ((CCNode*)levels->getChildren()->objectAtIndex(i))->runAction(CCSequence::createWithTwoActions(CCFadeIn::create(0.6f), CCScaleTo::create(0.1f, 1.0f)));
        }
        for (int i = 0; i <levelPics.size(); i++)
        {
            levelPics[i]->runAction(CCFadeIn::create(0.6f));
        }
        boy->runAction(CCFadeIn::create(0.6f));
        icelittle->runAction(CCFadeIn::create(0.6f));
        layerGradient->setTouchEnabled(true);
        dark->runAction(CCFadeTo::create(0.5f, 0));
        menu->showLive();
        
        layerGradient->gettouches()->removeAllObjects();
        //scrollView->setTouchEnabled(true);
    }
    this->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(GameMapLayer::update)), NULL));
}

void GameMapLayer::levelLabelsDraw()
{
    float multiplier;
    
    if (IPAD)
        multiplier = 1.0f;
    else
        multiplier = 0.5f;
    
    CCSprite* lenta_1 = CCSprite::createWithSpriteFrameName("gameMap/lent_1.png");
    lenta_1->setPosition(ccp(path[3].x, path[3].y + 300*multiplier));
    lenta_1->setScale(1.0f);
    CCLabelBMFont* label_1 = CCLabelBMFont::create("Valley ice creame", "fonts/Script MT Bold 22.fnt");
    lenta_1->addChild(label_1);
    label_1->setPosition(ccp(lenta_1->getContentSize().width/2.0f, lenta_1->getContentSize().height/2.0f));
    label_1->setRotation(-4.0f);
    label_1->setScale(multiplier);
    layerGradient->addChild(lenta_1, 4);
    
    CCSprite* lenta_2 = CCSprite::createWithSpriteFrameName("gameMap/lent_2.png");
    lenta_2->setPosition(ccp(path[14].x - 300*multiplier, path[14].y + 500*multiplier));
    lenta_2->setScale(1.0f);
    CCLabelBMFont* label_2 = CCLabelBMFont::create("Sweet forest", "fonts/Script MT Bold 22.fnt");
    lenta_2->addChild(label_2);
    label_2->setPosition(ccp(lenta_2->getContentSize().width/2.0f, lenta_2->getContentSize().height/2.0f));
    label_2->setRotation(-4.0f);
    label_2->setScale(multiplier);
    layerGradient->addChild(lenta_2, 4);
    
    CCSprite* lenta_4 = CCSprite::createWithSpriteFrameName("gameMap/lent_3.png");
    lenta_4->setPosition(ccp(path[25].x - 200*multiplier, path[25].y + 250*multiplier));
    lenta_4->setScale(1.0f);
    CCLabelBMFont* label_4 = CCLabelBMFont::create("Mountain giants", "fonts/Script MT Bold 22.fnt");
    lenta_4->addChild(label_4);
    label_4->setPosition(ccp(lenta_4->getContentSize().width/2.0f, lenta_4->getContentSize().height/2.0f));
    label_4->setRotation(-4.0f);
    label_4->setScale(multiplier);
    layerGradient->addChild(lenta_4, 4);
    
    CCSprite* lenta_5 = CCSprite::createWithSpriteFrameName("gameMap/lent_4.png");
    lenta_5->setPosition(ccp(path[37].x - 80*multiplier, path[37].y + 200*multiplier));
    lenta_5->setScale(1.0f);
    CCLabelBMFont* label_5 = CCLabelBMFont::create("Pistachio sea", "fonts/Script MT Bold 22.fnt");
    lenta_5->addChild(label_5);
    label_5->setPosition(ccp(lenta_5->getContentSize().width/2.0f, lenta_5->getContentSize().height/2.0f));
    label_5->setRotation(-4.0f);
    label_5->setScale(multiplier);
    layerGradient->addChild(lenta_5, 4);
    
    CCSprite* lenta_6 = CCSprite::createWithSpriteFrameName("gameMap/lent_1.png");
    lenta_6->setPosition(ccp(path[52].x, path[52].y + 250*multiplier));
    lenta_6->setScale(1.0f);
    CCLabelBMFont* label_6 = CCLabelBMFont::create("Caramel meadows", "fonts/Script MT Bold 22.fnt");
    lenta_6->addChild(label_6);
    label_6->setPosition(ccp(lenta_6->getContentSize().width/2.0f, lenta_6->getContentSize().height/2.0f));
    label_6->setRotation(-4.0f);
    label_6->setScale(multiplier);
    layerGradient->addChild(lenta_6, 4);
    
    CCSprite* lenta_7 = CCSprite::createWithSpriteFrameName("gameMap/lent_2.png");
    lenta_7->setPosition(ccp(path[63].x - 200*multiplier, path[63].y + 200*multiplier));
    lenta_7->setScale(1.0f);
    CCLabelBMFont* label_7 = CCLabelBMFont::create("Farm ice Cream", "fonts/Script MT Bold 22.fnt");
    lenta_7->addChild(label_7);
    label_7->setPosition(ccp(lenta_7->getContentSize().width/2.0f, lenta_7->getContentSize().height/2.0f));
    label_7->setRotation(-4.0f);
    label_7->setScale(multiplier);
    layerGradient->addChild(lenta_7, 4);
    
    CCSprite* lenta_8 = CCSprite::createWithSpriteFrameName("gameMap/lent_3.png");
    lenta_8->setPosition(ccp(path[77].x, path[77].y + 850*multiplier));
    lenta_8->setScale(1.0f);
    CCLabelBMFont* label_8 = CCLabelBMFont::create("Ice city", "fonts/Script MT Bold 22.fnt");
    lenta_8->addChild(label_8);
    label_8->setPosition(ccp(lenta_8->getContentSize().width/2.0f, lenta_8->getContentSize().height/2.0f));
    label_8->setRotation(-4.0f);
    label_8->setScale(multiplier);
    layerGradient->addChild(lenta_8, 4);
}

void GameMapLayer::levelPicsDraw()
{
    int z = 3;
    float multiplier;
    
    if (IPAD)
        multiplier = 1.0f;
    else
        multiplier = 0.5f;

    
    CCSkeletonAnimation* levelPic_1;
    levelPic_1 = CCSkeletonAnimation::createWithFile("mapAnimation/1/arch.json", "mapAnimation/1/arch.atlas");
    levelPic_1->setPosition(ccp(path[0].x + 380*multiplier, path[0].y + 70*multiplier));
    levelPic_1->setAnimation("animation", true);
    levelPic_1->setScale(0.7f*multiplier);
    layerGradient->addChild(levelPic_1, z);
    
    levelPics.push_back(levelPic_1);
    
    
    CCNode* levelPic_2;
    if (OptionsPtr->getCurrentLevel() >= 12)
    {
        levelPic_2 = CCSkeletonAnimation::createWithFile("mapAnimation/2/candyforest.json", "mapAnimation/2/candyforest.atlas");
        ((CCSkeletonAnimation*)levelPic_2)->setAnimation("animation", true);
        levelPic_2->setPosition(ccp(path[16].x - 500*multiplier, path[16].y + 150*multiplier));
        levelPic_2->setScale(0.4f*multiplier);
    }
    else
    {
        levelPic_2 = CCSprite::createWithSpriteFrameName("gameMap/blurPics/level_2.png");
        levelPic_2->setAnchorPoint(ccp(0.5f, 0.5f));
        levelPic_2->setScale(1.3f);
        levelPic_2->setPosition(ccp(path[16].x - 500*multiplier, path[16].y + 350*multiplier));
    }
    layerGradient->addChild(levelPic_2, z);
    
    levelPics.push_back(levelPic_2);

    CCNode* levelPic_4;
    
    if (OptionsPtr->getCurrentLevel() >= 24)
    {
        levelPic_4 = CCSkeletonAnimation::createWithFile("mapAnimation/4/giant.json", "mapAnimation/4/giant.atlas");
        ((CCSkeletonAnimation*)levelPic_4)->setAnimation("animation-copy", true);
        levelPic_4->setPosition(ccp(path[25].x + 500*multiplier, path[25].y - 250*multiplier));
        levelPic_4->setScale(0.5f*multiplier);
    }
    else
    {
        levelPic_4 = CCSprite::createWithSpriteFrameName("gameMap/blurPics/level_4.png");
        levelPic_4->setAnchorPoint(ccp(0.5f, 0.5f));
        levelPic_4->setPosition(ccp(path[25].x + 500*multiplier, path[25].y + 100*multiplier));
    }
    layerGradient->addChild(levelPic_4, z);
    
    levelPics.push_back(levelPic_4);
    
    CCNode* levelPic_5;
    
    if (OptionsPtr->getCurrentLevel() >= 36)
    {
        levelPic_5 = CCNode::create();
        levelPic_5->setAnchorPoint(ccp(0.0f, 1.0f));
        levelPic_5->setPosition(ccp(path[36].x - 500*multiplier, path[36].y + 150*multiplier));
        
        CCSkeletonAnimation* ship = CCSkeletonAnimation::createWithFile("mapAnimation/5/ship.json", "mapAnimation/5/ship.atlas");
        ship->setAnimation("animation-copy2", true);
        levelPic_5->addChild(ship);
        
        CCSkeletonAnimation* pirate = CCSkeletonAnimation::createWithFile("mapAnimation/5/pirate cookies.json", "mapAnimation/5/pirate cookies.atlas");
        pirate->setAnimation("animation-copy", true);
        levelPic_5->addChild(pirate);
        
        levelPic_5->setScale(0.35f*multiplier);
    }
    else
    {
        levelPic_5 = CCSprite::createWithSpriteFrameName("gameMap/blurPics/level_5.png");
        levelPic_5->setAnchorPoint(ccp(0.5f, 0.5f));
        levelPic_5->setPosition(ccp(path[36].x - 300*multiplier, path[36].y + 450*multiplier));
    }
    
    layerGradient->addChild(levelPic_5, z);
    
    levelPics.push_back(levelPic_5);
    
    CCNode* levelPic_6;
    
    if (OptionsPtr->getCurrentLevel() >= 48)
    {
        levelPic_6 = CCSkeletonAnimation::createWithFile("mapAnimation/6/taps.json", "mapAnimation/6/taps.atlas");
        ((CCSkeletonAnimation*)levelPic_6)->setAnimation("animation", true);
        levelPic_6->setPosition(ccp(path[55].x, path[55].y + 300*multiplier));
        levelPic_6->setScale(0.35f*multiplier);
    }
    else
    {
        levelPic_6 = CCSprite::createWithSpriteFrameName("gameMap/blurPics/level_6.png");
        levelPic_6->setAnchorPoint(ccp(0.5f, 0.2f));
        levelPic_6->setPosition(ccp(path[55].x, path[55].y + 200*multiplier));
    }
    
    layerGradient->addChild(levelPic_6, z);
    
    levelPics.push_back(levelPic_6);
    
    CCNode* levelPic_7;
    
    if (OptionsPtr->getCurrentLevel() >= 60)
    {
        levelPic_7 = CCNode::create();
        levelPic_7->setAnchorPoint(ccp(0.5f, 0.5f));
        levelPic_7->setPosition(ccp(path[66].x - 280*multiplier, path[66].y + 250*multiplier));
        
        CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("mapAnimation/7/back.json", "mapAnimation/7/back.atlas");
        back->setAnimation("animation", true);
        levelPic_7->addChild(back, 1);
        
        CCSkeletonAnimation* backCow = CCSkeletonAnimation::createWithFile("mapAnimation/7/back cow.json", "mapAnimation/7/back cow.atlas");
        backCow->setAnimation("animation-copy", true);
        levelPic_7->addChild(backCow, 2);
        
        CCSkeletonAnimation* frontCow = CCSkeletonAnimation::createWithFile("mapAnimation/7/front cows.json", "mapAnimation/7/front cows.atlas");
        frontCow->setAnimation("animation", true);
        levelPic_7->addChild(frontCow, 3);
        
        levelPic_7->setScale(0.52f*multiplier);
    }
    else
    {
        levelPic_7 = CCSprite::createWithSpriteFrameName("gameMap/blurPics/level_7.png");
        levelPic_7->setAnchorPoint(ccp(0.8f, 0.0f));
        levelPic_7->setPosition(ccp(path[66].x + 300*multiplier, path[66].y + 100*multiplier));
    }
    
    layerGradient->addChild(levelPic_7, z);
    
    levelPics.push_back(levelPic_7);
    
    CCNode* levelPic_8;
    
    if (OptionsPtr->getCurrentLevel() >= 72)
    {
        levelPic_8 = CCSkeletonAnimation::createWithFile("mapAnimation/8/icecream-city.json", "mapAnimation/8/icecream-city.atlas");
        ((CCSkeletonAnimation*)levelPic_8)->setAnimation("animation", true);
        levelPic_8->setPosition(ccp(path[77].x + 50*multiplier, path[77].y + 330*multiplier));
        levelPic_8->setScale(0.5f*multiplier);
    }
    else
    {
        levelPic_8 = CCSprite::createWithSpriteFrameName("gameMap/blurPics/level_8.png");
        levelPic_8->setAnchorPoint(ccp(0.5f, 0.5f));
        levelPic_8->setPosition(ccp(path[77].x + 50*multiplier, path[77].y + 600*multiplier));
    }
    
    layerGradient->addChild(levelPic_8, z);
    
    levelPics.push_back(levelPic_8);
    
    CCSprite* way = CCSprite::create("superWay.png");
    way->setPosition(ccp(path[29].x, path[29].y));
    way->setAnchorPoint(ccp(0.5f, 0.0f));
    way->setScale(0.8f);
    layerGradient->addChild(way, z);
    
/*    CCSprite* levelPic_9;
    
    if (OptionsPtr->getCurrentLevel() >= 56)
        levelPic_9 = CCSprite::create("gameMap/level_9.png");
    else
        levelPic_9 = CCSprite::create("gameMap/blurPics/level_9.png");
    
    levelPic_9->setAnchorPoint(ccp(0.5f, 0.0f));
    levelPic_9->setPosition(ccp(path[55].x, path[55].y));
    layerGradient->addChild(levelPic_9, z);
    levelPics.push_back(levelPic_9);
    
    CCSprite* levelPic_10;
    
    if (OptionsPtr->getCurrentLevel() >= 63)
        levelPic_10 = CCSprite::create("gameMap/level_10.png");
    else
        levelPic_10 = CCSprite::create("gameMap/blurPics/level_10.png");
    
    levelPic_10->setAnchorPoint(ccp(0.2f, 0.2f));
    levelPic_10->setPosition(ccp(path[60].x, path[60].y));
    layerGradient->addChild(levelPic_10, z);
    levelPics.push_back(levelPic_10);
    
    CCSprite* levelPic_11;
    
    if (OptionsPtr->getCurrentLevel() >= 70)
        levelPic_11 = CCSprite::create("gameMap/level_11.png");
    else
        levelPic_11 = CCSprite::create("gameMap/blurPics/level_11.png");
    
    levelPic_11->setAnchorPoint(ccp(0.0f, 0.0f));
    levelPic_11->setPosition(ccp(path[78].x, path[78].y));
    layerGradient->addChild(levelPic_11, z);
    levelPics.push_back(levelPic_11);
    
    CCSprite* levelPic_12;
    
    if (OptionsPtr->getCurrentLevel() >= 77)
        levelPic_12 = CCSprite::create("gameMap/level_12.png");
    else
        levelPic_12 = CCSprite::create("gameMap/blurPics/level_12.png");
    
    levelPic_12->setAnchorPoint(ccp(0.5f, 1.0f));
    levelPic_12->setPosition(ccp(path[80].x, path[80].y));
    layerGradient->addChild(levelPic_12, z);
    levelPics.push_back(levelPic_12);
    
    CCSprite* levelPic_13;
    
    if (OptionsPtr->getCurrentLevel() >= 84)
        levelPic_13 = CCSprite::create("gameMap/level_13.png");
    else
        levelPic_13 = CCSprite::create("gameMap/blurPics/level_13.png");

    levelPic_13->setAnchorPoint(ccp(1.0f, 0.5f));
    levelPic_13->setPosition(ccp(path[88].x, path[88].y));
    layerGradient->addChild(levelPic_13, z);
    levelPics.push_back(levelPic_13);
    
    CCSprite* levelPic_14;
    
    if (OptionsPtr->getCurrentLevel() >= 91)
        levelPic_14 = CCSprite::create("gameMap/level_14.png");
    else
        levelPic_14 = CCSprite::create("gameMap/blurPics/level_14.png");
    
    levelPic_14->setAnchorPoint(ccp(-0.1f, 0.2f));
    levelPic_14->setPosition(ccp(path[95].x, path[95].y));
    layerGradient->addChild(levelPic_14, z);
    levelPics.push_back(levelPic_14);
    
    CCSprite* levelPic_15;
    
    if (OptionsPtr->getCurrentLevel() >= 98)
        levelPic_15 = CCSprite::create("gameMap/level_15.png");
    else
        levelPic_15 = CCSprite::create("gameMap/blurPics/level_15.png");
    
    levelPic_15->setAnchorPoint(ccp(1.2f, 0.5f));
    levelPic_15->setPosition(ccp(path[104].x, path[104].y));
    layerGradient->addChild(levelPic_15, z);
    levelPics.push_back(levelPic_15);*/
}

void GameMapLayer::drawPoins(CCNode* sender)
{
	if (currentIdx >= newPath.size())
		return;

	streak->setPosition(newPath[currentIdx]);
	
	bool flag = false;
	for (int i = 0; i < path.size(); i++)
	{
		if (newPath[currentIdx].x == path[i].x && newPath[currentIdx].y == path[i].y )
		{
			flag = true;
			streak = NULL;
			char buf[64];
			int numLev = (i/12) + 1;
			if (numLev > 9)
				numLev -=9;
			sprintf(buf, "gameMap/kontur%d.png", numLev);
            if (!((i+1)%12))
            {
                if (rand()%2)
                    strcpy(buf, "gameMap/relsi.png");
                else
                    strcpy(buf, "gameMap/dashline.png");
            }
			CCTexture2D* tex = CCTextureCache::sharedTextureCache()->addImage(buf);
            if (WINSIZE.width == 2048 || WINSIZE.height == 2048)
            {
            if ((i + 2) > OptionsPtr->getCurrentLevel() && ((i+1)%12))
            {
                tex = CCTextureCache::sharedTextureCache()->addImage("gameMap/wite.png");

                ccColor3B color;
                if (numLev == 1)
                    color = ccBLUE;
                else if (numLev == 2)
                    color = ccGREEN;
                else if (numLev == 3)
                    color = ccMAGENTA;
                else if (numLev == 4)
                    color = ccYELLOW;
                else if (numLev == 5)
                    color = ccORANGE;
                else if (numLev == 6)
                    color = ccRED;
                else if (numLev == 7)
                    color = ccBLUE;
                else if (numLev == 8)
                    color = ccGREEN;
                else if (numLev == 9)
                    color = ccGREEN;
                
                streak = IceMotionStreak::create(1.0f, 10.0f, 80.0f, color, tex);
            }
            else
                streak = IceMotionStreak::create(1.0f, 10.0f, 80.0f, ccWHITE, tex);
            }
            else
            {
                streak = IceMotionStreak::create(1.0f, 10.0f, 40.0f, ccWHITE, tex);
            }
			streak->reset();
			streak->setPosition(newPath[currentIdx++]);
			layerGradient->addChild(streak,4);
		}
	}
	if (!flag)
		currentIdx++;
	//streak->update(0.0f);
	drawPoins(this);
}

void GameMapLayer::levelCallback(CCObject* pSender)
{
	if (leftDownMenu->isLock())
		return;
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    layerGradient->clearTouches();

	int numLevel = ((CCMenuItemSprite*)pSender)->getTag();
    if (OptionsPtr->getLevelData(numLevel).levelType != Score)
        menu->levelPopup(numLevel + 1, OptionsPtr->getLevelData(numLevel).countStar, OptionsPtr->getLevelData(numLevel).targetScore, OptionsPtr->getLevelData(numLevel).levelType, BoosterCrystal, BoosterBomb, BoosterFish);
    else
        menu->levelPopup(numLevel + 1, OptionsPtr->getLevelData(numLevel).countStar, OptionsPtr->getLevelData(numLevel).targetScore, OptionsPtr->getLevelData(numLevel).levelType, BoosterCrystal, BoosterBomb, BoosterNone);
}