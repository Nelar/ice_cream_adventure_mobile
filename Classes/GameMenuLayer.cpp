#include "GameMenuLayer.h"
#include "MainMenuScene.h"
#include "GameMapLayer.h"
#include "cGlobal.h"
#include "GameScene.h"
#include "Options.h"
#include <time.h>
#include "SimpleAudioEngine.h"
#include "cComixScene.h"
#include "IAP.h"
#include "cFacebook.h"

using namespace CocosDenshion;

using namespace cocos2d;

GameMenuLayer* GameMenuLayer::create(eLevelType ntype)
{
	GameMenuLayer *pRet = new GameMenuLayer(); 
	if (pRet && pRet->init(ntype)) 
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

void GameMenuLayer::snowCallback(CCObject* pSender)
{
    if (IPHONE_4)
        return;
    
    if (isSnow)
    {
        isSnow = false;
        particle->setVisible(false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("snow", false);
    }
    else
    {
        isSnow = true;
        particle->setVisible(true);
        CCUserDefault::sharedUserDefault()->setBoolForKey("snow", true);
    }
    CCUserDefault::sharedUserDefault()->flush();
}

GameMenuLayer::~GameMenuLayer()
{
    this->stopAllActions();
    this->unscheduleAllSelectors();
    this->removeAllChildrenWithCleanup(true);
}

void GameMenuLayer::addPopap()
{
    popupBooster = PopupLayer::create();
    this->getParent()->addChild(popupBooster, 200);
}

void GameMenuLayer::updateProgressBar()
{
    if (LANDSCAPE)
    {
        startPolosa_1->setPosition(ccp(downPanel->getContentSize().width/8.0f*7.4f, downPanel->getContentSize().height/20.0f + (progressPolosa/100.0f)*targetScore/((thirdStar)/100.0f) ));
        startPolosa_2->setPosition(ccp(downPanel->getContentSize().width/8.0f*7.4f, downPanel->getContentSize().height/20.0f+  (progressPolosa/100.0f)*secondStar/((thirdStar)/100.0f)));
        startPolosa_1->setRotation(-90.0f);
        startPolosa_2->setRotation(-90.0f);
    }
    else
    {
        startPolosa_1->setPosition(ccp(downPanel->getContentSize().width/5.8f + (progressPolosa/100.0f)*targetScore/((thirdStar)/100.0f), downPanel->getContentSize().height/8.0f));
        startPolosa_2->setPosition(ccp(downPanel->getContentSize().width/5.8f + (progressPolosa/100.0f)*secondStar/((thirdStar)/100.0f), downPanel->getContentSize().height/8.0f));
        startPolosa_1->setRotation(0.0f);
        startPolosa_2->setRotation(0.0f);
    }
}

bool GameMenuLayer::init(eLevelType ntype)
{
    if (!CCLayer::init())
		return false;
    
	boosterPlus = NULL;
	labelCountMoves = NULL;
	movesTitle = NULL;
	labelTargetScore = NULL;
	targetTitle = NULL;
    scoreTitle = NULL;
	labelCurrentScore = NULL;
    isClear = false;
    isBannerLock = false;
    isPopup = false;
    isSnow = true;
    beginTime = 0;
    timeGradient = NULL;
    
    speed = 0.35f;
    accel = 0.35f;
    
    if (rand()%2)
        isStrawberry = false;
    else
        isStrawberry = true;

	type = ntype;
	lock = false;
	menu = CCMenu::create();
    
    startPolosa_1 = CCSprite::createWithSpriteFrameName("game/stars.png");
    startPolosa_2 = CCSprite::createWithSpriteFrameName("game/stars.png");
    startPolosa_3 = CCSprite::createWithSpriteFrameName("game/3star.png");
    
    if (IPAD)
        progressPolosa = 608.0f;
    else
        progressPolosa = 304.0f;

    isLandscape = LANDSCAPE;
    if (WINSIZE.height < WINSIZE.width)
    {
        upPanel = CCSprite::createWithSpriteFrameName("game/panel_obsch.png");
        upPanel->setPosition(ccp(0.0f,
                                 WINSIZE.height));
        upPanel->setAnchorPoint(ccp(0.0f, 1.0f));

#ifdef NEW_ART
        downPanel = CCSprite::create("updateArt/gor_panel.png");
#else
        downPanel = CCSprite::createWithSpriteFrameName("game/gor_panel.png");
#endif
        downPanel->setPosition(ccp(downPanel->getContentSize().width/2.0f,
                                   WINSIZE.height/2.5f));
        
        startPolosa_1->setPosition(ccp(downPanel->getContentSize().width/8.0f*7.4f, downPanel->getContentSize().height/19.2f));
        startPolosa_1->setRotation(-90.0f);
        
        startPolosa_2->setPosition(ccp(downPanel->getContentSize().width/8.0f*7.4f, downPanel->getContentSize().height/19.2f));
        startPolosa_2->setRotation(-90.0f);
        
        startPolosa_3->setPosition(ccp(downPanel->getContentSize().width/8.0f*7.4f, downPanel->getContentSize().height/1.1f));
        startPolosa_3->setRotation(-90.0f);
    }
    else
    {
#ifdef NEW_ART
        upPanel = CCSprite::create("updateArt/panelHor.png");
#else
        upPanel = CCSprite::createWithSpriteFrameName("game/panelverh.png");
#endif
        upPanel->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - upPanel->getContentSize().width/2.0f,
                                 CCDirector::sharedDirector()->getWinSize().height - upPanel->getContentSize().height/2.0f));

#ifdef NEW_ART
        downPanel = CCSprite::create("updateArt/panelHorDown.png");
#else
        downPanel = CCSprite::createWithSpriteFrameName("game/score_moves_stars.png");
#endif
        
        downPanel->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   downPanel->getContentSize().height/1.7f));
        startPolosa_1->setPosition(ccp(downPanel->getContentSize().width/5.8f + (progressPolosa/100.0f)*100.0f, downPanel->getContentSize().height/8.0f));
        
        startPolosa_2->setPosition(ccp(downPanel->getContentSize().width/1.65f, downPanel->getContentSize().height/8.0f));
        
        startPolosa_3->setPosition(ccp(downPanel->getContentSize().width/1.25f, downPanel->getContentSize().height/8.0f));
    }
    
    downPanel->addChild(startPolosa_1, 2);
    downPanel->addChild(startPolosa_2, 2);
    downPanel->addChild(startPolosa_3, 2);
    

    star1 = CCSprite::createWithSpriteFrameName("common/star.png");
    star1->setScale(0.25f);
    star1->setVisible(false);
    startPolosa_1->addChild(star1, 2);
    star1->setPosition(ccp(startPolosa_1->getContentSize().width/2.15f, star1->getContentSize().height/1.7f*star1->getScale()));
    
    star2 = CCSprite::createWithSpriteFrameName("common/star2.png");
    star2->setScale(0.25f);
    star2->setVisible(false);
    startPolosa_2->addChild(star2, 2);
    star2->setPosition(ccp(startPolosa_2->getContentSize().width/2.15f, star2->getContentSize().height/1.7f*star2->getScale()));

    
    star3 = CCSprite::createWithSpriteFrameName("common/star3.png");
    star3->setVisible(false);
    star3->setScale(0.25f);
    startPolosa_3->addChild(star3, 2);
    star3->setPosition(ccp(startPolosa_1->getContentSize().width/2.15f, star3->getContentSize().height/1.7f*star3->getScale()));


	booster_1_Button = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/booster_normal.png"),
		CCSprite::createWithSpriteFrameName("common/booster_selected.png"), this, menu_selector(GameMenuLayer::booster_1_Callback));
	menu->addChild(booster_1_Button, -1);
	booster_1_Button->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    booster_1_Button->setVisible(false);
    
    if (WINSIZE.height < WINSIZE.width)
    {
        upPanel->setPosition(ccp(0.0f,
                                 WINSIZE.height));
        
        booster_1_Button->setPosition(booster_1_Button->getContentSize().width/2.0f - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                      CCDirector::sharedDirector()->getWinSize().height/2.0f - booster_1_Button->getContentSize().height/2.0f);
    }
    else
    {
        upPanel->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - upPanel->getContentSize().width/2.0f,
                                 CCDirector::sharedDirector()->getWinSize().height - upPanel->getContentSize().height/2.0f));
        
        booster_1_Button->setPosition(booster_1_Button->getContentSize().width/2.0f - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                      CCDirector::sharedDirector()->getWinSize().height/2.0f - booster_1_Button->getContentSize().height/2.2f);
    }

	booster_2_Button = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/booster_normal.png"),
		CCSprite::createWithSpriteFrameName("common/booster_selected.png"), this, menu_selector(GameMenuLayer::booster_2_Callback));
	booster_2_Button->setPosition(booster_1_Button->getPositionX() , booster_1_Button->getPositionY());
	menu->addChild(booster_2_Button, -1);
	booster_2_Button->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 0.85f, 0.75f), CCScaleTo::create(0.5f, 0.8f, 0.8f))));
    boostClear = CCSprite::createWithSpriteFrameName("common/clearBoost.png");
    boostClear->setAnchorPoint(ccp(-0.2f,-0.2f));
    boostClear->setScale(0.8f);
    booster_2_Button->addChild(boostClear);
    
    if (OptionsPtr->getHammerCount() > 0)
    {
        boosterPlus = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
        char buf[255];
        sprintf(buf, "%d", OptionsPtr->getHammerCount());
        CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_108);
        label->setColor(ccWHITE);
        
        boosterPlus->setPosition(ccp(booster_2_Button->getContentSize().width / 1.3f, booster_2_Button->getContentSize().height / 9.0f));
        boosterPlus->addChild(label);
        label->setPosition(ccp(boosterPlus->getContentSize().width/2.0f, boosterPlus->getContentSize().height/2.0f));
        
        boostClear->addChild(boosterPlus);
    }

	booster_3_Button = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/booster_normal.png"),
		CCSprite::createWithSpriteFrameName("common/booster_selected.png"), this, menu_selector(GameMenuLayer::booster_3_Callback));
	booster_3_Button->setPosition(booster_2_Button->getPositionX() + booster_1_Button->getContentSize().width*0.8f, booster_2_Button->getPositionY());
	menu->addChild(booster_3_Button, -1);
	booster_3_Button->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 0.85f, 0.75f), CCScaleTo::create(0.5f, 0.8f, 0.8f))));
    boost5 = CCSprite::createWithSpriteFrameName("common/arrow5.png");
    boost5->setAnchorPoint(ccp(-0.2f,-0.2f));
    boost5->setScale(0.8f);
    booster_3_Button->addChild(boost5);
    
    snow = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("game/snow.png"),
                                                      CCSprite::createWithSpriteFrameName("game/snow_selected.png"), this, menu_selector(GameMenuLayer::snowCallback));
    
    if (LANDSCAPE)
        snow->setPosition(-WINSIZE.width/3.2f, -WINSIZE.height/2.0f+ snow->getContentSize().height/1.5f);
    else
        snow->setPosition(WINSIZE.width/2.0f - snow->getContentSize().width/1.5f, -WINSIZE.height/2.0f+ snow->getContentSize().height/1.5f);
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
            snow->setPosition(-WINSIZE.width/3.2f, -WINSIZE.height/2.0f+ snow->getContentSize().height/2.0f);
        else
            snow->setPosition(WINSIZE.width/2.0f - snow->getContentSize().width/1.5f, -WINSIZE.height/2.0f+ snow->getContentSize().height/1.5f);
    }
    
    bool tempSnow = CCUserDefault::sharedUserDefault()->getBoolForKey("snow", false);
    if (tempSnow)
        isSnow = true;
    else
        isSnow = false;
    
    if (IPHONE_4)
    {
        snow->setVisible(false);
    }
    
	menu->addChild(snow, 150);
    
    snow->setOpacity(0);
    snow->runAction(CCSequence::create(CCDelayTime::create(4.0f), CCFadeIn::create(1.0f), NULL));
    
    
	
	this->addChild(upPanel);
	this->addChild(downPanel);
	this->addChild(menu,5);
	this->setTouchEnabled(true);

	upPanel->setPosition(ccp(upPanel->getPositionX(), upPanel->getPositionY() + 500));
	upPanel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCEaseOut::create(CCMoveBy::create(0.3f, ccp(0, -500.0f)), 2.5f)));

    if (WINSIZE.height < WINSIZE.width)
    {
        downPanel->setPosition(ccp(downPanel->getPositionX(), downPanel->getPositionY() - 1500));
        downPanel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCEaseOut::create(CCMoveBy::create(0.3f, ccp(0, 1500)), 2.5f)));
    }
    else
    {
        downPanel->setPosition(ccp(downPanel->getPositionX(), downPanel->getPositionY() - 500));
        downPanel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCEaseOut::create(CCMoveBy::create(0.3f, ccp(0, 500)), 2.5f)));
    }
 
	menu->setPosition(ccp(menu->getPositionX(), menu->getPositionY() + 500));
	menu->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCEaseOut::create(CCMoveBy::create(0.3f, ccp(0, -500.0f)), 2.5f)));

    return true;
}

void GameMenuLayer::popupExit(int iceTarget, int iceCount)
{
    if (isDialog)
        return;
    
    iceCount = iceTarget - iceCount;
    iceCountSave = iceCount;
    
    isDialog = true;
    exitBack = CCLayerColor::create(ccc4(0, 0, 0, 120));
    this->getParent()->addChild(exitBack, 100);
    ((CCLayer*)this->getParent())->setTouchEnabled(false);
    exitBack->setColor(ccBLACK);
    ((GameScene*)getParent())->leftDownMenu->setVisible(false);
    
    if (IPHONE_5 || IPHONE_4)
    {
        if (LANDSCAPE)
        {
            exitBack->setScale(1.0f);
        }
        else
        {
            exitBack->setScale(0.913f);
        }
    }
    
    float multuplier;
    
    if (IPAD)
        multuplier = 1.0f;
    else
        multuplier = 0.5f;
    
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    
    upBack = CCSprite::createWithSpriteFrameName("common/plate ltl.png");
    upBack->setPosition(ccp(WINSIZE.width/2.0f + WINSIZE.width, 4.0f*WINSIZE.height/5.0f));
    exitBack->addChild(upBack);
    upBack->runAction(CCMoveBy::create(0.2f, ccp(-WINSIZE.width, 0.0f)));
    
    downBack = CCSprite::createWithSpriteFrameName("common/plate.png");
    downBack->setPosition(ccp(WINSIZE.width/2.0f + WINSIZE.width, WINSIZE.height/4.0f));
    exitBack->addChild(downBack);
    downBack->runAction(CCMoveBy::create(0.2f, ccp(-WINSIZE.width, 0.0f)));
    
    CCSprite* guy = CCSprite::createWithSpriteFrameName("common/boy.png");
    guy->setPosition(ccp(downBack->getContentSize().width/4.0f, downBack->getContentSize().height/2.0f));
    downBack->addChild(guy);
    
    CCSprite* checkScore;

    if (currentScore >= targetScore)
        checkScore = CCSprite::createWithSpriteFrameName("common/check.png");
    else
        checkScore = CCSprite::createWithSpriteFrameName("common/checkNo.png");
    
    checkScore->setScale(0.5f);
    checkScore->setPosition(ccp(upBack->getContentSize().width/10.0f, upBack->getContentSize().height/2.0f));
    upBack->addChild(checkScore);
    
    CCSprite* star = CCSprite::createWithSpriteFrameName("common/star3.png");
    star->setScale(0.25f);
    star->setPosition(ccp(upBack->getContentSize().width/10.0f + checkScore->getContentSize().width*checkScore->getScale(), upBack->getContentSize().height/2.0f));
    upBack->addChild(star);
    
    char buf[255];
    sprintf(buf, "%d/%d", currentScore, targetScore);
    CCLabelTTF* scoreLabel = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_72);
    scoreLabel->setColor(color);
    scoreLabel->setAnchorPoint(ccp(0.0f, 0.5f));
    scoreLabel->setPosition(ccp(upBack->getContentSize().width/10.0f + checkScore->getContentSize().width*checkScore->getScale() + star->getContentSize().width*star->getScale(), upBack->getContentSize().height/2.0f));
    upBack->addChild(scoreLabel);
    
    CCLabelTTF* titleLabel;
    if (((GameScene*)getParent())->isEndDialog)
        if (type == Time)
            titleLabel = CCLabelTTF::create(CCLocalizedString("OUT_OF_TIME", NULL), FONT_COMMON, FONT_SIZE_72);
        else
            titleLabel = CCLabelTTF::create(CCLocalizedString("OUT_OF_MOVES", NULL), FONT_COMMON, FONT_SIZE_72);
    else
        titleLabel = CCLabelTTF::create(CCLocalizedString("PRESS_QUIT", NULL), FONT_COMMON, FONT_SIZE_54);
    
    
    titleLabel->setColor(color);
    titleLabel->setPosition(ccp(upBack->getContentSize().width/2.0f, 5.0f*upBack->getContentSize().height/6.0f));
    upBack->addChild(titleLabel);
    
    if (((GameScene*)getParent())->isEndDialog)
    {
        CCSprite* boost;
        
        if (type == Time)
            boost = CCSprite::createWithSpriteFrameName("common/time15.png");
        else
            boost = CCSprite::createWithSpriteFrameName("common/arrow5.png");
        
        boost->setPosition(ccp(downBack->getContentSize().width/2.15f,
                               4.2f*downBack->getContentSize().height/5.0f));
        downBack->addChild(boost);
    }
    
    if (type == BringDown)
    {
        CCSprite* checkBring;
        if (bringDownCurrent >= bringDownTarget)
            checkBring = CCSprite::createWithSpriteFrameName("common/check.png");
        else
            checkBring = CCSprite::createWithSpriteFrameName("common/checkNo.png");
        
        checkBring->setScale(0.5f);
        checkBring->setPosition(ccp(upBack->getContentSize().width/1.7f, upBack->getContentSize().height/2.0f));
        upBack->addChild(checkBring);
        
        CCSprite* bringScore = CCSprite::createWithSpriteFrameName("game/cookie.png");
        bringScore->setScale(0.5f);
        bringScore->setPosition(ccp(upBack->getContentSize().width/1.7f + checkBring->getContentSize().width*checkBring->getScale(), upBack->getContentSize().height/2.0f));
        upBack->addChild(bringScore);
        
        char buf[255];
        sprintf(buf, "%d/%d", bringDownCurrent, bringDownTarget);
        CCLabelTTF* bringLabel = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_72);
        bringLabel->setAnchorPoint(ccp(0.0f, 0.5f));
        bringLabel->setColor(color);
        bringLabel->setPosition(ccp(upBack->getContentSize().width/1.7f + bringScore->getContentSize().width*bringScore->getScale()  + checkBring->getContentSize().width*checkBring->getScale(), upBack->getContentSize().height/2.0f));
        upBack->addChild(bringLabel);
    }
    else if (type == Ice)
    {
        CCSprite* checkBring;
        if (iceCount >= iceTarget)
            checkBring = CCSprite::createWithSpriteFrameName("common/check.png");
        else
            checkBring = CCSprite::createWithSpriteFrameName("common/checkNo.png");
        
        checkBring->setScale(0.5f);
        checkBring->setPosition(ccp(upBack->getContentSize().width/1.7f, upBack->getContentSize().height/2.0f));
        upBack->addChild(checkBring);
        
        CCSprite* iceScore = CCSprite::createWithSpriteFrameName("game/ice.png");
        iceScore->setScale(0.5f);
        iceScore->setPosition(ccp(upBack->getContentSize().width/1.7f + checkBring->getContentSize().width*checkBring->getScale(), upBack->getContentSize().height/2.0f));
        upBack->addChild(iceScore);
        
        char buf[255];
        sprintf(buf, "%d/%d", iceCount, iceTarget);
        CCLabelTTF* iceLabel = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_72);
        iceLabel->setAnchorPoint(ccp(0.0f, 0.5f));
        iceLabel->setColor(color);
        iceLabel->setPosition(ccp(upBack->getContentSize().width/1.7f + iceScore->getContentSize().width*iceScore->getScale() + checkBring->getContentSize().width*checkBring->getScale(), upBack->getContentSize().height/2.0f));
        upBack->addChild(iceLabel);
    }
    
    menuExit = CCMenu::create();
    
    menuExit->setContentSize(WINSIZE);
    menuExit->setAnchorPoint(ccp(0.0f, 0.0f));
    menuExit->setPosition(ccp(WINSIZE.width, 0.0f));
    menuExit->runAction(CCMoveBy::create(0.2f, ccp(-WINSIZE.width, 0.0f)));
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    playOn = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameMenuLayer::playOnCallback));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("PLAY_ON", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    playOn->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

    
    playOn->setPosition(ccp(WINSIZE.width/2.0f + playOn->getContentSize().width/2.0f, WINSIZE.height/3.0f));
    menuExit->addChild(playOn);
    
    
    spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    exitOn = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameMenuLayer::exitOnCallback));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("EXIT_GAME", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    exitOn->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    exitOn->setPosition(ccp(WINSIZE.width/2.0f + playOn->getContentSize().width/2.0f, WINSIZE.height/3.0f - playOn->getContentSize().height*1.5f));
    menuExit->addChild(exitOn);
    
    exitBack->addChild(menuExit);
    
    if (LANDSCAPE)
    {
        downBack->setScaleY(0.9f);
        downBack->setPosition(ccp(WINSIZE.width/2.0f + WINSIZE.width, WINSIZE.height/3.4f));
        menuExit->setPositionY(menuExit->getPositionY() + 50);
    }
    else
    {
        downBack->setScaleY(1.0f);
    }
    
    playOn->stopAllActions();
    playOn->setScale(0.7f);
	playOn->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    exitOn->stopAllActions();
    exitOn->setScale(0.7f);
	exitOn->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    booster_1_Button->setEnabled(false);
    booster_2_Button->setEnabled(false);
    booster_3_Button->setEnabled(false);
    snow->setEnabled(false);
}

void GameMenuLayer::playOnCallback(CCObject* pSender)
{
    booster_1_Button->setEnabled(true);
    booster_2_Button->setEnabled(true);
    booster_3_Button->setEnabled(true);
    ((CCLayer*)this->getParent())->setTouchEnabled(true);
    isDialog = false;
    if (((GameScene*)getParent())->isEndDialog)
    {
        playOn->setEnabled(false);
        exitOn->setEnabled(false);
        if (type == Time)
        {
            popupBooster->popupBoosterInApp((char*)CCLocalizedString("EXTRA_TIME", NULL), (char*)CCLocalizedString("EXTRA_TIME_TEXT", NULL), GreenPopup, TimePopBoot, this, callfuncN_selector(GameMenuLayer::popupOk2), this, callfuncN_selector(GameMenuLayer::unclockMenu));
        }
        else
        {
            popupBooster->popupBoosterInApp((char*)CCLocalizedString("EXTRA_MOVES", NULL), (char*)CCLocalizedString("EXTRA_MOVES_TEXT", NULL), GreenPopup, MovesPopBoot, this, callfuncN_selector(GameMenuLayer::popupOk2), this, callfuncN_selector(GameMenuLayer::unclockMenu));
        }
    }
    else
    {
        ((GameScene*)getParent())->leftDownMenu->exitPress = false;
        ((GameScene*)getParent())->leftDownMenu->setVisible(true);
        ((GameScene*)getParent())->leftDownMenu->menuSettingCallback(NULL);
        exitBack->removeFromParentAndCleanup(true);
        exitBack = NULL;
    }
}

void GameMenuLayer::exitOnCallback(CCObject* pSender)
{
    booster_1_Button->setEnabled(true);
    booster_2_Button->setEnabled(true);
    booster_3_Button->setEnabled(true);
    isDialog = false;
    ((CCLayer*)this->getParent())->setTouchEnabled(true);
    if (((GameScene*)getParent())->isEndDialog)
    {
        ((GameScene*)getParent())->isLoseDialog = true;
        ((GameScene*)getParent())->destroyFinished(NULL);
    }
    else
    {
        ((GameScene*)getParent())->leftDownMenu->exitPress = false;
        ((GameScene*)getParent())->leftDownMenu->setVisible(true);
        ((GameScene*)getParent())->leftDownMenu->menuSettingCallback(NULL);
        ((GameScene*)getParent())->lose(NULL);
    }
    exitBack->removeFromParentAndCleanup(true);
    exitBack = NULL;
}

void GameMenuLayer::createSnow()
{
    if (currentLevel == 106)
        OptionsPtr->setLastGameLevel(30);
    else if (currentLevel == 107)
        OptionsPtr->setLastGameLevel(24);
    else if (currentLevel == 108)
        OptionsPtr->setLastGameLevel(36);
    else if (currentLevel == 109)
        OptionsPtr->setLastGameLevel(48);
    else if (currentLevel == 110)
        OptionsPtr->setLastGameLevel(60);
    else if (currentLevel == 111)
        OptionsPtr->setLastGameLevel(72);
    else
        OptionsPtr->setLastGameLevel(currentLevel);
    OptionsPtr->save();
    
    if (type == Time)
    {
        booster_3_Button->setEnabled(false);
        booster_3_Button->setColor(ccGRAY);
        booster_3_Button->setVisible(false);
        boost5->setColor(ccGRAY);
    }
    
    if (!IPHONE_4)
    {
        particle = CCParticleSnow::create();
        this->getParent()->addChild(particle, 7);
        particle->setPosition(CCDirector::sharedDirector()->getWinSize().width / 2.0f, CCDirector::sharedDirector()->getWinSize().height);
        particle->setGravity(ccp(0.0f, -10.0f));
        particle->setScale(1.0f);
        particle->setTangentialAccel(10.0f);
        particle->setTangentialAccelVar(10.0f);
        bool tempSnow = CCUserDefault::sharedUserDefault()->getBoolForKey("snow");
        if (!tempSnow)
        {
            particle->setVisible(false);
        }
        else
        {
            particle->setVisible(true);
        }
    }
}

void GameMenuLayer::addBonus(CCNode* pSender)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    
    if (FacebookPtr->sessionIsOpened() && getNetworkStatus())
    {
        FacebookPtr->inviteFriends();
    }
    else
    {
        FacebookPtr->loginWithInvite();
    }
}

void GameMenuLayer::inviteFriend()
{
    popupBooster->popupPost((char*)CCLocalizedString("INVITE_FRIENDS_TITLE", NULL), (char*)CCLocalizedString("INVITE_FRIENDS_TEXT", NULL), (char*)CCLocalizedString("INVITE_FRIENDS_BUTTON", NULL), GreenPopup, BombPopBoot,
                          this, callfuncN_selector(GameMenuLayer::addBonus), this, callfuncN_selector(GameMenuLayer::unlockLeftMenu));
}

void GameMenuLayer::unlockLeftMenu(CCNode* pSender)
{
    ((GameScene*)getParent())->unlockInvite();
}

void GameMenuLayer::changeOrientation(void)
{
    popupBooster->changeOrientation();
    
    if (particle)
        particle->setPosition(CCDirector::sharedDirector()->getWinSize().width / 2.0f, CCDirector::sharedDirector()->getWinSize().height);
    
    upPanel->stopAllActions();
    downPanel->stopAllActions();
    menu->stopAllActions();
    
    labelBan->setVisible(false);
    this->setContentSize(WINSIZE);
    this->setPosition(ccp(0.0f, 0.0f));
    menu->setContentSize(WINSIZE);
    menu->setPosition(WINSIZE.width/2.0f, WINSIZE.height/2.0f);
        
	
    if (WINSIZE.height < WINSIZE.width)
    {
        upPanel->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/panel_obsch.png")->displayFrame());
        upPanel->setPosition(ccp(0.0f,
                                 WINSIZE.height));
        upPanel->setAnchorPoint(ccp(0.0f, 1.0f));

#ifdef NEW_ART
        downPanel->setDisplayFrame(CCSprite::create("updateArt/gor_panel.png")->displayFrame());
#else
        downPanel->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game/gor_panel.png"));
#endif
        downPanel->setPosition(ccp(downPanel->getContentSize().width/2.0f,
                                   WINSIZE.height/2.5f));
        
        startPolosa_1->setPosition(ccp(downPanel->getContentSize().width/8.0f*7.4f, downPanel->getContentSize().height/19.2f));
        startPolosa_1->setRotation(-90.0f);
        
        startPolosa_2->setPosition(ccp(downPanel->getContentSize().width/8.0f*7.4f, downPanel->getContentSize().height/19.2f));
        startPolosa_2->setRotation(-90.0f);
        
        startPolosa_3->setPosition(ccp(downPanel->getContentSize().width/8.0f*7.4f, downPanel->getContentSize().height/1.1f));
        startPolosa_3->setRotation(-90.0f);                
    }
    else
    {
#ifdef NEW_ART
        upPanel->setDisplayFrame(CCSprite::create("updateArt/panelHor.png")->displayFrame());
#else
        upPanel->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game/panelverh.png"));
#endif
        upPanel->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - upPanel->getContentSize().width/2.0f,
                                 CCDirector::sharedDirector()->getWinSize().height - upPanel->getContentSize().height/2.0f));
        
        upPanel->setAnchorPoint(ccp(0.5f, 0.5f));
       
#ifdef NEW_ART
        downPanel->setDisplayFrame(CCSprite::create("updateArt/panelHorDown.png")->displayFrame());
#else
        downPanel->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game/score_moves_stars.png"));
#endif
        
        downPanel->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   downPanel->getContentSize().height/1.7f));
        startPolosa_3->setPosition(ccp(downPanel->getContentSize().width/1.25f, downPanel->getContentSize().height/8.0f));
        startPolosa_3->setRotation(0.0f);
    }
    
    updateProgressBar();
    
    if (WINSIZE.height < WINSIZE.width)
    {
        upPanel->setPosition(ccp(0.0f,
                                 WINSIZE.height));
        
        booster_1_Button->setPosition(booster_1_Button->getContentSize().width/2.0f - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                      CCDirector::sharedDirector()->getWinSize().height/2.0f - booster_1_Button->getContentSize().height/2.0f);
    }
    else
    {
        upPanel->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width - upPanel->getContentSize().width/2.0f,
                                 CCDirector::sharedDirector()->getWinSize().height - upPanel->getContentSize().height/2.0f));
        
        booster_1_Button->setPosition(booster_1_Button->getContentSize().width/2.0f - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                      CCDirector::sharedDirector()->getWinSize().height/2.0f - booster_1_Button->getContentSize().height/2.2f);
    }
    
    
	booster_2_Button->setPosition(booster_1_Button->getPositionX() , booster_1_Button->getPositionY());
	booster_3_Button->setPosition(booster_2_Button->getPositionX() + booster_1_Button->getContentSize().width*0.8f, booster_2_Button->getPositionY());
    
    setCountMoves(countMoves);
    if (type == BringDown)
    {
        setBringDownTarget(bringDownTarget);
        setBringDownCurrent(bringDownCurrent);
    }
    else
    {
        setTargetScore(targetScore);
    }
    
    if (scoreTitle)
		scoreTitle->removeFromParentAndCleanup(true);
    
    ccColor3B colorBlue;
	colorBlue.r = 0x29;
	colorBlue.g = 0x45;
	colorBlue.b = 0x93;
    
    
    scoreTitle = CCLabelTTF::create(CCLocalizedString("SCORE"), FONT_COMMON, FONT_SIZE_48);
    ((CCLabelTTF*)scoreTitle)->setColor(colorBlue);
    
    if (LANDSCAPE)
        scoreTitle->setPosition(ccp(downPanel->getContentSize().width/2.6f, downPanel->getContentSize().height/4.0f*1.6f));
    else
        scoreTitle->setPosition(ccp(downPanel->getContentSize().width/4.0f*3.0f, downPanel->getContentSize().height/4.0f*3.0f));
    
    downPanel->addChild(scoreTitle);
    
    setCurrentScore(currentScore);
    
    if (type == Time && timeGradient)
    {
        if(LANDSCAPE)
            timeGradient->setPosition(ccp(downPanel->getContentSize().width/2.5f, downPanel->getContentSize().height/4.0f*3.0f));
        else
            timeGradient->setPosition(ccp(downPanel->getContentSize().width/4.0f, downPanel->getContentSize().height/4.0f*2.5f));
    }
    
    if (LANDSCAPE)
        snow->setPosition(-WINSIZE.width/3.2f, -WINSIZE.height/2.0f+ snow->getContentSize().height/1.5f);
    else
        snow->setPosition(WINSIZE.width/2.0f - snow->getContentSize().width/1.5f, -WINSIZE.height/2.0f+ snow->getContentSize().height/1.5f);

    
    if (exitBack != NULL)
    {
        exitBack->setContentSize(WINSIZE);
        menuExit->setContentSize(WINSIZE);
        menuExit->setAnchorPoint(ccp(0.0f, 0.0f));
        menuExit->setPosition(ccp(0.0f, 0.0f));
        
        upBack->setPosition(ccp(WINSIZE.width/2.0f, 4.0f*WINSIZE.height/5.0f));
        downBack->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/4.0f));
        
        if (LANDSCAPE)
        {
            downBack->setScaleY(0.9f);
            downBack->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/3.4f));
            if (IPHONE_5 || IPHONE_4)
                exitBack->setScale(1.0f);
            
            menuExit->setPositionY(menuExit->getPositionY() + 50);
        }
        else
        {
            if (IPHONE_5 || IPHONE_4)
                exitBack->setScale(0.913f);
            downBack->setScaleY(1.0f);
        }
        
        playOn->setPosition(ccp(WINSIZE.width/2.0f + playOn->getContentSize().width/2.0f, WINSIZE.height/3.0f));
        exitOn->setPosition(ccp(WINSIZE.width/2.0f + playOn->getContentSize().width/2.0f, WINSIZE.height/3.0f - playOn->getContentSize().height*1.5f));
    }
    
    if (countMoves <= 5 && type != Time)
    {
        if (mig)
        {
            mig->removeFromParentAndCleanup(true);
            mig = NULL;
        }
        
        mig = CCSprite::createWithSpriteFrameName("game/circle.png");
        downPanel->addChild(mig, 1);
        mig->setPosition(labelCountMoves->getPosition());
        mig->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.2f), CCScaleTo::create(0.2f, 0.5f))));
    }
}

void GameMenuLayer::startTime()
{
	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.0f), CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::timeCallback))));	

	if (labelCountMoves)
		labelCountMoves->removeFromParentAndCleanup(true);
    
    timeGradient = CCProgressTimer::create(CCSprite::create("game/gradient.png"));
    timeGradient->setType(kCCProgressTimerTypeRadial);
    timeGradient->setReverseProgress(true);
    timeGradient->setPercentage(100.0f);
    timeGradient->setScale(0.8f);
    if(LANDSCAPE)
        timeGradient->setPosition(ccp(downPanel->getContentSize().width/2.5f, downPanel->getContentSize().height/4.0f*3.0f));
    else
        timeGradient->setPosition(ccp(downPanel->getContentSize().width/4.0f, downPanel->getContentSize().height/4.0f*2.5f));
    downPanel->addChild(timeGradient, 2);

	char buf[255];
	sprintf(buf, "%d",  time);
    beginTime = time;

	labelCountMoves = CCLabelTTF::create( buf, FONT_COMMON, FONT_SIZE_40);
    ccColor3B color;
	color.r = 0x29;
	color.g = 0x45;
	color.b = 0x93;
	labelCountMoves->setColor(color);
    if(LANDSCAPE)
        labelCountMoves->setPosition(ccp(downPanel->getContentSize().width/2.5f, downPanel->getContentSize().height/4.0f*3.0f));
    else
        labelCountMoves->setPosition(ccp(downPanel->getContentSize().width/4.0f, downPanel->getContentSize().height/4.0f*2.4f));

	downPanel->addChild(labelCountMoves, 3);
}

void GameMenuLayer::timeCallback(CCNode* sender)
{
    if (!isDialog && !(((GameScene*)getParent())->timesLock) && !isBannerLock && !(((GameScene*)getParent())->timesLeftDownLock))
        time--;
	if (time > 0)
		this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::timeCallback))));
    if (time == 0)
    {
        ((GameScene*)getParent())->timeUpdate(NULL);
        
    }

	if (labelCountMoves)
		labelCountMoves->removeFromParentAndCleanup(true);

	char buf[255];
	sprintf(buf, "%d", time);

	labelCountMoves = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_54);
    ccColor3B color;
	color.r = 0x29;
	color.g = 0x45;
	color.b = 0x93;
	labelCountMoves->setColor(color);
    float tempPer = ((float)time)/ (((float)beginTime)/100.0f);
    timeGradient->setPercentage(tempPer);
	if(LANDSCAPE)
    {
        labelCountMoves->setPosition(ccp(downPanel->getContentSize().width/2.5f, downPanel->getContentSize().height/4.0f*3.0f));
    }
    else
    {
        labelCountMoves->setPosition(ccp(downPanel->getContentSize().width/4.0f, downPanel->getContentSize().height/4.0f*2.4f));
    }

	downPanel->addChild(labelCountMoves, 3);
}

void GameMenuLayer::setLevelType(eLevelType nType)
{
	type = nType;
}

void GameMenuLayer::end()
{	
	upPanel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCEaseOut::create(CCMoveBy::create(0.3f, ccp(0, 500.0f)), 2.5f)));
	
    if (WINSIZE.height < WINSIZE.width)
        downPanel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCEaseOut::create(CCMoveBy::create(0.3f, ccp(0, -1500.0f)), 2.5f)));
    else
        downPanel->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCEaseOut::create(CCMoveBy::create(0.3f, ccp(0, -500.0f)), 2.5f)));

	menu->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCEaseOut::create(CCMoveBy::create(0.3f, ccp(0, 500.0f)), 2.5f)));
    snow->setVisible(false);
}


void GameMenuLayer::banner(const char* name, const char* text, float delay, ccColor3B color)
{
    if (bannerSprite)
        bannerSprite->removeAllChildrenWithCleanup(true);

	bannerSprite = CCSprite::create(name);
	this->addChild(bannerSprite, 5);
	bannerSprite->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, 
		CCDirector::sharedDirector()->getWinSize().height + bannerSprite->getContentSize().height/2.0f));
    
    
    CCSprite* icelittle = CCSprite::create("updateArt/icelittle.png");
    icelittle->setPosition(ccp(bannerSprite->getContentSize().width/12.0f, bannerSprite->getContentSize().height/2.0f));
    bannerSprite->addChild(icelittle);
		
	labelBan = CCLabelTTF::create(text, FONT_COMMON, FONT_SIZE_64);
	bannerSprite->addChild(labelBan);
	labelBan->setPosition(ccp(bannerSprite->getContentSize().width/1.6f, bannerSprite->getContentSize().height/2.0f));
	labelBan->setColor(color);

	bannerSprite->runAction(CCSequence::create(CCDelayTime::create(delay), 
                                               CCEaseBackOut::create(CCMoveBy::create(0.5f, ccp(0, -CCDirector::sharedDirector()->getWinSize().height/1.7f))),
                                               CCDelayTime::create(1.5f),
                                               CCMoveBy::create(0.2f, ccp(0.0f, -bannerSprite->getContentSize().height/4.0f)),
                                               CCEaseOut::create(CCMoveBy::create(0.5f, ccp(0, CCDirector::sharedDirector()->getWinSize().height/1.7f)), 0.5f),
                                               CCHide::create(), NULL));
}

void GameMenuLayer::bannerIce(const char* name, float delay, ccColor3B color)
{
    if (bannerSprite)
        bannerSprite->removeAllChildrenWithCleanup(true);
    
	bannerSprite = CCSprite::create(name);
	this->addChild(bannerSprite, 5);
	bannerSprite->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                  CCDirector::sharedDirector()->getWinSize().height + bannerSprite->getContentSize().height/2.0f));
    
    
    CCSprite* icelittle = CCSprite::create("updateArt/icelittleice.png");
    icelittle->setPosition(ccp(bannerSprite->getContentSize().width/2.0f, bannerSprite->getContentSize().height/0.85f));
    bannerSprite->addChild(icelittle);
    
	labelBan = CCLabelTTF::create(CCLocalizedString("BANNER_TEXT_ICE"), FONT_COMMON, FONT_SIZE_48);
	bannerSprite->addChild(labelBan);
    bannerSprite->setCascadeOpacityEnabled(true);
	labelBan->setPosition(ccp(bannerSprite->getContentSize().width/2.0f, bannerSprite->getContentSize().height/2.0f));
	labelBan->setColor(color);
    
	bannerSprite->runAction(CCSequence::create(CCDelayTime::create(delay),
                                               CCEaseBackOut::create(CCMoveBy::create(0.5f, ccp(0, -CCDirector::sharedDirector()->getWinSize().height/1.7f))),
                                               CCDelayTime::create(1.5f),
                                               CCMoveBy::create(0.2f, ccp(0.0f, -bannerSprite->getContentSize().height/4.0f)),
                                               CCEaseOut::create(CCMoveBy::create(0.5f, ccp(0, CCDirector::sharedDirector()->getWinSize().height/1.7f)), 0.5f),
                                               CCHide::create(), NULL));
}

void GameMenuLayer::bannerBringDown(const char* name, float delay, ccColor3B color)
{
    if (bannerSprite)
        bannerSprite->removeAllChildrenWithCleanup(true);
    
	bannerSprite = CCSprite::create(name);
	this->addChild(bannerSprite, 5);
	bannerSprite->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                  CCDirector::sharedDirector()->getWinSize().height + bannerSprite->getContentSize().height/2.0f));
    
    CCSprite* icelittle = CCSprite::create("updateArt/icelittle.png");
    icelittle->setPosition(ccp(bannerSprite->getContentSize().width/12.0f, bannerSprite->getContentSize().height/2.0f));
    bannerSprite->addChild(icelittle);
    
	labelBan = CCLabelTTF::create(CCLocalizedString("BANNER_TEXT_BRING"), FONT_COMMON, FONT_SIZE_48);
	bannerSprite->addChild(labelBan);
    bannerSprite->setCascadeOpacityEnabled(true);
	labelBan->setPosition(ccp(bannerSprite->getContentSize().width/1.6f, bannerSprite->getContentSize().height/2.0f));
	labelBan->setColor(color);
    
	bannerSprite->runAction(CCSequence::create(CCDelayTime::create(delay),
                                               CCEaseBackOut::create(CCMoveBy::create(0.5f, ccp(0, -CCDirector::sharedDirector()->getWinSize().height/2.0f))),
                                               CCDelayTime::create(1.5f),
                                               CCMoveBy::create(0.2f, ccp(0.0f, -bannerSprite->getContentSize().height/4.0f)),
                                               CCEaseOut::create(CCMoveBy::create(0.5f, ccp(0, CCDirector::sharedDirector()->getWinSize().height/2.0f)), 0.5f),
                                               CCHide::create(), NULL));
}

void GameMenuLayer::bannerCallback(CCNode* sender)
{
    isBannerLock = false;
    isTextBanner = false;
}

void GameMenuLayer::sugarCrash()
{
    if (isBannerLock)
        return;
    if (isSugarCrash)
        return;
    
    isSugarCrash = true;
    SimpleAudioEngine::sharedEngine()->playEffect("sound/super_crash.mp3");
	bannerSprite = CCSprite::createWithSpriteFrameName("game/sugarCrash.png");
	this->addChild(bannerSprite, 5);
	bannerSprite->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, 
		CCDirector::sharedDirector()->getWinSize().height/2.0f));
	bannerSprite->setScale(0.01f);
    if (IPHONE_4 || IPHONE_5)
        bannerSprite->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 0.8f), CCFadeIn::create(0.3f)),
                                                   CCDelayTime::create(1.0f), CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 0.01f), CCFadeOut::create(0.3f)), CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::bannerCallback)), NULL));
    else
        bannerSprite->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 1.0f), CCFadeIn::create(0.3f)),
                                                   CCDelayTime::create(1.0f), CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 0.01f), CCFadeOut::create(0.3f)), CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::bannerCallback)), NULL));
    isBannerLock = true;
}

void GameMenuLayer::sweet()
{
    if (isBannerLock)
        return;
    
    isTextBanner = true;
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/sweet.mp3");
	bannerSprite = CCSprite::createWithSpriteFrameName("game/sweet.png");
	this->addChild(bannerSprite, 5);
	bannerSprite->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
		CCDirector::sharedDirector()->getWinSize().height/2.0f));
	bannerSprite->setScale(0.01f);
	bannerSprite->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 1.0f), CCFadeIn::create(0.3f)), 
		CCDelayTime::create(1.0f),
		CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 0.01f), CCFadeOut::create(0.3f)),CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::bannerCallback)), NULL));
    isBannerLock = true;
}

void GameMenuLayer::tasty()
{
    if (isBannerLock)
        return;
    
    isTextBanner = true;

    SimpleAudioEngine::sharedEngine()->playEffect("sound/tasty.mp3");
	bannerSprite = CCSprite::createWithSpriteFrameName("game/tasty.png");
	this->addChild(bannerSprite, 5);
	bannerSprite->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, 
		CCDirector::sharedDirector()->getWinSize().height/2.0f));
	bannerSprite->setScale(0.01f);
	bannerSprite->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 1.0f), CCFadeIn::create(0.3f)), 
		CCDelayTime::create(1.0f),
		CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 0.01f), CCFadeOut::create(0.3f)), CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::bannerCallback)),NULL));
    isBannerLock = true;
}

void GameMenuLayer::divine()
{
    if (isBannerLock)
        return;

    isTextBanner = true;

    SimpleAudioEngine::sharedEngine()->playEffect("sound/divine.mp3");
	bannerSprite = CCSprite::createWithSpriteFrameName("game/divine.png");
	this->addChild(bannerSprite, 5);
	bannerSprite->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, 
		CCDirector::sharedDirector()->getWinSize().height/2.0f));
	bannerSprite->setScale(0.01f);
	bannerSprite->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 1.0f), CCFadeIn::create(0.3f)), 
		CCDelayTime::create(1.0f),
		CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 0.01f), CCFadeOut::create(0.3f)), CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::bannerCallback)),NULL));
    
    isBannerLock = true;
}

void GameMenuLayer::delicious()
{
    if (isBannerLock)
        return;
    
    isTextBanner = true;

    SimpleAudioEngine::sharedEngine()->playEffect("sound/delicious.mp3");
	bannerSprite = CCSprite::createWithSpriteFrameName("game/delicious.png");
	this->addChild(bannerSprite, 5);
	bannerSprite->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, 
		CCDirector::sharedDirector()->getWinSize().height/2.0f));
	bannerSprite->setScale(0.01f);
	bannerSprite->runAction(CCSequence::create(CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 1.0f), CCFadeIn::create(0.3f)), 
		CCDelayTime::create(1.0f),
		CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 0.01f), CCFadeOut::create(0.3f)),CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::bannerCallback)), NULL));
    isBannerLock = true;
}

int GameMenuLayer::getTargetScore()
{
	return targetScore;
}

void GameMenuLayer::setLevel(int lev)
{
	currentLevel = lev;
}

void GameMenuLayer::hideAll()
{
    menu->setVisible(false);
    downPanel->setVisible(false);
    upPanel->setVisible(false);
    labelBan->setVisible(false);
    snow->setVisible(false);
}

int GameMenuLayer::getCountStar()
{
    int coS = currentScore / targetScore;
    if (currentScore>=targetScore)
        coS = 1;
    if (currentScore>=secondStar)
        coS = 2;
    if (currentScore>=thirdStar)
        coS = 3;
    
    return coS;
}

void GameMenuLayer::touch()
{
    if (bannerSprite && bannerSprite->isVisible())
    {
        bannerSprite->stopAllActions();
        bannerSprite->runAction(CCSequence::create(CCFadeOut::create(0.2f), CCHide::create(), NULL));
    }
}


void GameMenuLayer::booster_1_Callback(CCObject* pSender)
{
}

void GameMenuLayer::booster_2_Callback(CCObject* pSender)
{
    if (isClear)
        return;
    if (OptionsPtr->getHammerCount() <= 0)
    {
        ((CCLayer*)getParent())->setTouchEnabled(false);
        menu->setTouchEnabled(false);
        popupBooster->popupBoosterInApp((char*)CCLocalizedString("ICE_HAMMER", NULL), (char*)CCLocalizedString("ICE_HAMMER_TEXT", NULL), GreenPopup, ChupaPopBoot, this, callfuncN_selector(GameMenuLayer::popupOk1), this, callfuncN_selector(GameMenuLayer::unclockMenu));
    }
    else
    {
        OptionsPtr->setHammerCount(OptionsPtr->getHammerCount() - 1);
        isClear = true;
        if (boosterPlus)
        {
            boosterPlus->removeFromParentAndCleanup(true);
            boosterPlus = NULL;
        }
        
        if (OptionsPtr->getHammerCount() > 0)
        {
            boosterPlus = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            char buf[255];
            sprintf(buf, "%d", OptionsPtr->getHammerCount());
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_108);
            label->setColor(ccWHITE);
            
            boosterPlus->setPosition(ccp(booster_2_Button->getContentSize().width / 1.3f, booster_2_Button->getContentSize().height / 9.0f));
            boosterPlus->addChild(label);
            label->setPosition(ccp(boosterPlus->getContentSize().width/2.0f, boosterPlus->getContentSize().height/2.0f));
            
            boostClear->addChild(boosterPlus);
        }
    }
    OptionsPtr->save();
}

void GameMenuLayer::closeLoading()
{
    popupBooster->closeLoading();
    ((GameScene*)getParent())->leftDownMenu->exitPress = false;
    ((GameScene*)getParent())->leftDownMenu->setVisible(true);
    menu->setTouchEnabled(true);
}

void GameMenuLayer::booster_3_Callback(CCObject* pSender)
{
    ((CCLayer*)getParent())->setTouchEnabled(false);
    menu->setTouchEnabled(false);
    popupBooster->popupBoosterInApp((char*)CCLocalizedString("EXTRA_MOVES", NULL), (char*)CCLocalizedString("EXTRA_MOVES_TEXT", NULL), GreenPopup, MovesPopBoot, this, callfuncN_selector(GameMenuLayer::popupOk2), this, callfuncN_selector(GameMenuLayer::unclockMenu));
}

void GameMenuLayer::addHammer()
{
    OptionsPtr->setHammerCount(OptionsPtr->getHammerCount() + 3);
    
    if (boosterPlus)
        boosterPlus->removeFromParentAndCleanup(true);
    
    boosterPlus = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
    char buf[255];
    sprintf(buf, "%d", OptionsPtr->getHammerCount());
    CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_108);

    label->setColor(ccWHITE);
    
    boosterPlus->setPosition(ccp(booster_2_Button->getContentSize().width / 1.3f, booster_2_Button->getContentSize().height / 9.0f));
    boosterPlus->addChild(label);
    label->setPosition(ccp(boosterPlus->getContentSize().width/2.0f, boosterPlus->getContentSize().height/2.0f));
    
    boostClear->addChild(boosterPlus);
    
    OptionsPtr->save();
}

void GameMenuLayer::addMovesInApp()
{
    if (type == Time)
    {
        time+=16;
        if (time > 0)
        {
            this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::timeCallback))));
        }
    }
    else
    {
        setCountMoves(getCountMoves() + 5);
    }
    isLastAction = false;
}

void GameMenuLayer::popupOk1(CCNode* pSender)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.hammer");
    popupBooster->loading((char*)CCLocalizedString("CONNECTION", NULL));
    menu->setTouchEnabled(false);
}

void GameMenuLayer::popupOk2(CCNode* pSender)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    if (((GameScene*)getParent())->isEndDialog)
    {
        playOn->setEnabled(true);
        exitOn->setEnabled(true);
    }
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.5moves");
    popupBooster->loading((char*)CCLocalizedString("CONNECTION", NULL));
    menu->setTouchEnabled(false);
    ((GameScene*)getParent())->isEndDialog = false;
    if (exitBack)
    {
        ((GameScene*)getParent())->leftDownMenu->exitPress = false;
        ((GameScene*)getParent())->leftDownMenu->setVisible(true);
        exitBack->removeFromParentAndCleanup(true);
        exitBack = NULL;
        //this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(10.0f), CCCallFuncN::create(this, callfuncN_selector(GameMenuLayer::inappPopupCancel))));
    }
}

void GameMenuLayer::inappPopupCancel(CCNode* sender)
{
    if (countMoves == 0 && type != Time)
    {
        ((GameScene*)getParent())->isEndDialog = true;
        popupExit(iceCountSave, 0);
    }
    else if (type == Time && time == 0)
    {
        ((GameScene*)getParent())->isEndDialog = true;
        popupExit(iceCountSave, 0);
    }
}

void GameMenuLayer::unclockMenu(CCNode* pSender)
{
    if (((GameScene*)getParent())->isEndDialog)
    {
        if (playOn)
            playOn->setEnabled(true);
        if (exitOn)
            exitOn->setEnabled(true);
    }
    menu->setTouchEnabled(true);
    ((CCLayer*)getParent())->setTouchEnabled(true);
}

bool GameMenuLayer::isLock()
{
    return lock;
}

int GameMenuLayer::getCountMoves()
{
    if (type == Time)
        return 1;
	return countMoves;
}

int GameMenuLayer::getCurrentScore()
{
	return currentScore;
}

void GameMenuLayer::setCountMoves(int nTargetScore)
{
	if (type == Time)
		return;
    
	countMoves = nTargetScore;
	if (movesTitle)
		movesTitle->removeFromParentAndCleanup(true);

	if (labelCountMoves)
		labelCountMoves->removeFromParentAndCleanup(true);
    
    ccColor3B color;
	color.r = 0x29;
	color.g = 0x45;
	color.b = 0x93;

	movesTitle = CCLabelTTF::create(CCLocalizedString("MOVES"), FONT_COMMON, FONT_SIZE_48);
    movesTitle->setColor(color);
    
    if (LANDSCAPE)
        movesTitle->setPosition(ccp(downPanel->getContentSize().width/2.6f, downPanel->getContentSize().height/4.0f*3.4f));
    else
        movesTitle->setPosition(ccp(downPanel->getContentSize().width/4.0f, downPanel->getContentSize().height/4.0f*3.0f));
    
	downPanel->addChild(movesTitle);

	char buf[255];
	sprintf(buf, "%d", nTargetScore);

	labelCountMoves = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_64);
	labelCountMoves->setColor(color);
    if (LANDSCAPE)
        labelCountMoves->setPosition(ccp(downPanel->getContentSize().width/2.6f, downPanel->getContentSize().height/4.0f*3.0f));
    else
        labelCountMoves->setPosition(ccp(downPanel->getContentSize().width/4.0f, downPanel->getContentSize().height/4.0f*2.3f));


	downPanel->addChild(labelCountMoves, 2);
    
    if (countMoves <= 5 && type != Time)
    {
        labelCountMoves->setColor(ccRED);
        if (mig)
        {
            mig->removeFromParentAndCleanup(true);
            mig = NULL;
        }
        mig = CCSprite::createWithSpriteFrameName("game/circle.png");
        downPanel->addChild(mig, 1);
        mig->setPosition(labelCountMoves->getPosition());
        mig->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.2f, 1.2f), CCScaleTo::create(0.2f, 0.5f))));
    }
}

void GameMenuLayer::setCurrentScore(int nTargetScore)
{
	currentScore = nTargetScore;
    
    if (firstStep && nTargetScore!=0)
    {
        firstStep = false;
        OptionsPtr->setLifeCount(OptionsPtr->getLifeCount() - 1);
        
        if (GlobalsPtr->booster_2)
        {
            OptionsPtr->setBombCount(OptionsPtr->getBombCount() - 1);
            GlobalsPtr->booster_2 = false;
        }
        
        if (GlobalsPtr->booster_1)
        {
            OptionsPtr->setCrystalCount(OptionsPtr->getCrystalCOunt() - 1);
            GlobalsPtr->booster_1 = false;
        }
        
        if (GlobalsPtr->booster_3 && type != Score)
        {
            OptionsPtr->setFishCount(OptionsPtr->getFishCount() - 1);
            GlobalsPtr->booster_3 = false;
        }
    }
	
	if (labelCurrentScore)
		labelCurrentScore->removeFromParentAndCleanup(true);

	char buf[255];
	sprintf(buf, "%d", nTargetScore);
	labelCurrentScore = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_64);
	ccColor3B color;
	color.r = 0x29;
	color.g = 0x45;
	color.b = 0x93;
	labelCurrentScore->setColor(color);
    if (LANDSCAPE)
        labelCurrentScore->setPosition(ccp(downPanel->getContentSize().width/2.6f, downPanel->getContentSize().height/4.0f*1.2f));
    else
        labelCurrentScore->setPosition(ccp(downPanel->getContentSize().width/4.0f*3.0f, downPanel->getContentSize().height/4.0f*2.3f));

	downPanel->addChild(labelCurrentScore);

	CCTexture2D* tex;
    tex = CCTextureCache::sharedTextureCache()->addImage("game/progressbar.png");

	int purcent = ((float)currentScore / (((float)thirdStar)/100.0f));
	if (purcent > 100)
		purcent = 100;

    if (progressBar)
        progressBar->removeFromParentAndCleanup(true);
    
    float widthBar = purcent*(tex->getPixelsWide() /100.0f);
	progressBar = CCSprite::createWithTexture(tex, CCRect(0.0f, 0.0f, widthBar, tex->getPixelsHigh()));
	progressBar->setAnchorPoint(ccp(0.0f, 0.5f));
	downPanel->addChild(progressBar);
    
    if (LANDSCAPE)
    {
        progressBar->setRotation(-90.0f);
        progressBar->setPosition(ccp(downPanel->getContentSize().width/8.0f*7.05f , downPanel->getContentSize().height/14.2f));
    }
    else
    {
        progressBar->setRotation(0.0f);
        progressBar->setPosition(ccp(downPanel->getContentSize().width/6.05f , downPanel->getContentSize().height/5.4f));
    }
    
    ccBlendFunc blend;
    blend.dst = GL_ONE;
    blend.src = GL_ONE;
    
    if (currentScore >= targetScore)
    {
        if (!star1->isVisible())
        {
            SimpleAudioEngine::sharedEngine()->playEffect("sound/star_1.mp3");
            CCParticleSystemQuad* sun =  CCParticleFire::createWithTotalParticles(10);
            star1->addChild(sun, 10);
            sun->setPosition(ccp(star1->getContentSize().width/2.0f, star1->getContentSize().height/2.0f));
            sun->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/snow.png"));
            sun->setSpeed(0.1f);
            sun->setBlendAdditive(true);
            sun->setBlendFunc(blend);
        }
        star1->setVisible(true);
    }
    if (currentScore >= secondStar)
    {
        if (!star2->isVisible())
        {
            SimpleAudioEngine::sharedEngine()->playEffect("sound/star_2.mp3");
            CCParticleSystemQuad* sun =  CCParticleFire::createWithTotalParticles(10);
            star2->addChild(sun, 10);
            sun->setPosition(ccp(star2->getContentSize().width/2.0f, star2->getContentSize().height/2.0f));
            sun->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/snow.png"));
            sun->setSpeed(0.1f);
            sun->setBlendAdditive(true);
            sun->setBlendFunc(blend);
        }
        star2->setVisible(true);
    }
    if (currentScore >= thirdStar)
    {
        if (!star3->isVisible())
        {
            SimpleAudioEngine::sharedEngine()->playEffect("sound/star_3.mp3");
            CCParticleSystemQuad* sun =  CCParticleFire::createWithTotalParticles(10);
            star3->addChild(sun, 10);
            sun->setPosition(ccp(star3->getContentSize().width/2.0f, star3->getContentSize().height/2.0f));
            sun->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/snow.png"));
            sun->setSpeed(0.1f);
            sun->setBlendAdditive(true);
            sun->setBlendFunc(blend);
        }
        star3->setVisible(true);
        isThirdStar = true;
    }
}

void GameMenuLayer::setBringDownCurrent(int current)
{
	if (labelTargetScore)
		labelTargetScore->removeFromParentAndCleanup(true);
	bringDownCurrent = current;
	char buf[255];
	sprintf(buf, "%d / %d", bringDownCurrent, bringDownTarget);
	labelTargetScore = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_54);
    ccColor3B color;
	color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
	labelTargetScore->setColor(color);

    
    if (targetTitle)
		targetTitle->removeFromParentAndCleanup(true);
    
	if (type == BringDown)
	{
        if (isStrawberry)
            targetTitle = CCSprite::createWithSpriteFrameName("game/cookie.png");
        else
            targetTitle = CCSprite::createWithSpriteFrameName("game/klubnika.png");
        if (LANDSCAPE)
            targetTitle->setPosition(ccp(upPanel->getContentSize().width/4.0f, upPanel->getContentSize().height/3.5f));
        else
            targetTitle->setPosition(ccp(upPanel->getContentSize().width/1.4f, upPanel->getContentSize().height/2.0f));
		targetTitle->setScale(0.6f);
		upPanel->addChild(targetTitle);
	}

    
    if (LANDSCAPE)
    {
        labelTargetScore->setPosition(ccp(targetTitle->getContentSize().width + labelTargetScore->getContentSize().width*labelTargetScore->getScale(), upPanel->getContentSize().height/4.0f));
    }
    else
    {
        labelTargetScore->setPosition(ccp(upPanel->getContentSize().width/1.31f + targetTitle->getContentSize().width/1.5f + labelTargetScore->getContentSize().width /2.0f, upPanel->getContentSize().height/2.0f));
    }
	upPanel->addChild(labelTargetScore);
}

void GameMenuLayer::setBringDownTarget(int target)
{
	bringDownTarget = target;
}

int GameMenuLayer::getBringDownCurrent()
{
	return bringDownCurrent;
}

int GameMenuLayer::getBringDownTarget()
{
	return bringDownTarget;
}

void GameMenuLayer::setTime(int nTime)
{
	time = nTime;
}
bool GameMenuLayer::isTime()
{
    if (type != Time)
        return true;
    
	if (time <= 0)
		return false;
	return true;
}

void GameMenuLayer::setTargetScore(int nTargetScore)
{		
	targetScore = nTargetScore;	

	if (targetTitle)
		targetTitle->removeFromParentAndCleanup(true);
    if (scoreTitle)
		scoreTitle->removeFromParentAndCleanup(true);

	if (labelTargetScore)
		labelTargetScore->removeFromParentAndCleanup(true);
    
    
    
    ccColor3B color;
	color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    
    ccColor3B colorBlue;
	colorBlue.r = 0x29;
	colorBlue.g = 0x45;
	colorBlue.b = 0x93;
    
    
    scoreTitle = CCLabelTTF::create(CCLocalizedString("SCORE"), FONT_COMMON, FONT_SIZE_48);
    ((CCLabelTTF*)scoreTitle)->setColor(colorBlue);
    
    if (LANDSCAPE)
        scoreTitle->setPosition(ccp(downPanel->getContentSize().width/2.6f, downPanel->getContentSize().height/4.0f*1.6f));
    else
        scoreTitle->setPosition(ccp(downPanel->getContentSize().width/4.0f*3.0f, downPanel->getContentSize().height/4.0f*3.0f));




	targetTitle = CCLabelTTF::create(CCLocalizedString("TARGET"), FONT_COMMON, FONT_SIZE_54);
    ((CCLabelTTF*)targetTitle)->setColor(color);
    
    if (LANDSCAPE)
        targetTitle->setPosition(ccp(upPanel->getContentSize().width/2.0f, upPanel->getContentSize().height/2.6f));
    else
        targetTitle->setPosition(ccp(upPanel->getContentSize().width/1.4f, upPanel->getContentSize().height/2.2f));
    
	upPanel->addChild(targetTitle);
    downPanel->addChild(scoreTitle);

	char buf[255];
	sprintf(buf, "%d", nTargetScore);
	labelTargetScore = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_54);
    labelTargetScore->setColor(color);
    if (LANDSCAPE)
    {
        labelTargetScore->setPosition(ccp(upPanel->getContentSize().width/2.0f,
                                          upPanel->getContentSize().height/5.0f));
        labelTargetScore->setAnchorPoint(ccp(0.5f, 0.5f));
    }
    else
    {
        labelTargetScore->setPosition(ccp(upPanel->getContentSize().width/4.0f*3.2f,
                                          upPanel->getContentSize().height/2.25f));
        labelTargetScore->setAnchorPoint(ccp(0.0f, 0.5f));
    }

	upPanel->addChild(labelTargetScore);
}

CCLayer* GameMenuLayer::createScoreLayer(int level)
{
    return  NULL;
}
