#include "MapMenuLayer.h"
#include "MainMenuScene.h"
#include "GameScene.h"
#include "cGlobal.h"
#include "cComixScene.h"
#include "nMMP.h"
#include "IAP.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "cFacebook.h"
#include <GUI/CCScrollView/CCScrollView.h>

#include "utils.h"

#include "SimpleAudioEngine.h"
#include "CCLocalizedString.h"
#include "GameMapLayer.h"

using namespace CocosDenshion;


using namespace cocos2d;
using namespace extension;

MapMenuLayer::~MapMenuLayer()
{
    this->stopAllActions();
    this->unscheduleAllSelectors();
    this->removeAllChildrenWithCleanup(true);
}

bool MapMenuLayer::init()
{	
    this->setTouchEnabled(true);
    
    livesCount = NULL;
    livesTimePopap = NULL;
    
	if (!CCLayer::init())
		return false;
    
    if (LANDSCAPE)
    {
        if (WINSIZE.height == 640)
            this->setScale(0.88f);
        else
            this->setScale(1.0f);
    }
    else
    {
        if (WINSIZE.width == 640)
            this->setScale(0.88f);
        else
            this->setScale(1.0f);
    }

	menu = CCMenu::create();
	this->addChild(menu, 12);
    
    popaplayer = PopupLayer::create();
    this->addChild(popaplayer, 100);

	lock = false;
    isPopup = false;
    
    facebookPanel = CCSprite::createWithSpriteFrameName("common/popupFacebookPanel.png");
	
	this->addChild(facebookPanel, 11);
    facebookPanel->setVisible(false);

    if (!IPAD)
        popup = CCSprite::createWithSpriteFrameName("common/plashka.png");
    else
        popup = CCSprite::create("plashka.png");
    
	
    
    if (LANDSCAPE)
    {
        popup->setPosition(ccp(popup->getContentSize().width/2.0f, WINSIZE.height/2.0f));
        
        facebookPanel->setRotation(90.0f);
    	facebookPanel->setPosition(ccp(popup->getPositionX() + popup->getContentSize().width/2.0f + facebookPanel->getContentSize().height/2.0f, facebookPanel->getContentSize().width/2.0f));
        
        if (getScale() != 1.0f)
        {
            facebookPanel->setPosition(ccp(facebookPanel->getPositionX(), facebookPanel->getPositionY() - (facebookPanel->getContentSize().width*(1.0f - getScale())/2.0f)));
        }
    }
    else
    {
        facebookPanel->setPosition(ccp(facebookPanel->getContentSize().width/2.0f, facebookPanel->getContentSize().height/2.0f));
        
        if (getScale() != 1.0f)
        {
            facebookPanel->setPosition(ccp(facebookPanel->getPositionX() - (facebookPanel->getContentSize().width*(1.0f - getScale())/2.0f), facebookPanel->getPositionY()));
        }
            
        facebookPanel->setRotation(0.0f);
        
        popup->setPosition(ccp(WINSIZE.width/2.0f, facebookPanel->getPositionY() + facebookPanel->getContentSize().height/3.0f
                               +popup->getContentSize().height/1.9f));
    }

	startCenter = CCSprite::createWithSpriteFrameName("common/star_popup_disabled.png");
	startCenter->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.30f));
	popup->addChild(startCenter);

	startLeft = CCSprite::createWithSpriteFrameName("common/star_popup_disabled.png");
	startLeft->setPosition(ccp(popup->getContentSize().width / 4.0f, popup->getContentSize().height / 1.37f));
	startLeft->setRotation(-20.0f);
	popup->addChild(startLeft);

	startRight = CCSprite::createWithSpriteFrameName("common/star_popup_disabled.png");
	startRight->setPosition(ccp(popup->getContentSize().width / 4.0f * 3.0f, popup->getContentSize().height / 1.37f));
	startRight->setRotation(20.0f);
	popup->addChild(startRight);

	CCSprite* targetSubstrate = CCSprite::createWithSpriteFrameName("common/plashka_score_target.png");
	targetSubstrate->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.8f));
	popup->addChild(targetSubstrate);

	targetTitle = CCLabelTTF::create("Init", FONT_COMMON, FONT_SIZE_64);
	targetTitle->setPosition(ccp(targetSubstrate->getContentSize().width/2.0f, targetSubstrate->getContentSize().height/2.0f));

	targetSubstrate->addChild(targetTitle);

	CCLabelTTF* boosterTitle = CCLabelTTF::create(CCLocalizedString("SELECT_BOOSTER"), FONT_COMMON, FONT_SIZE_48);
	boosterTitle->setPosition(ccp(popup->getContentSize().width/2.0f, popup->getContentSize().height/2.1f));
	ccColor3B color;
	color.r = 0x4b;
	color.g = 0xd7;
	color.b = 0x20;
	boosterTitle->setColor(color);

	popup->addChild(boosterTitle);

	levelTitle = CCLabelTTF::create("Init", FONT_COMMON, FONT_SIZE_86);
	levelTitle->setPosition(ccp(popup->getContentSize().width/2.0f, popup->getContentSize().height/1.1f));
	color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
	levelTitle->setColor(color);

	popup->addChild(levelTitle);

	close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
		CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MapMenuLayer::closeCallback));
	close->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /2.5f, 
		popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + popup->getContentSize().height /2.4f);

	
	
    CCSprite* playSpriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* playSpriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    playSpriteSelected->setColor(ccGRAY);
    
    play = CCMenuItemSprite::create(playSpriteNormal,
		playSpriteSelected, this, menu_selector(MapMenuLayer::playCallback));
	play->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f, 
		popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f
		- popup->getContentSize().height /2.3f + play->getContentSize().height/2.0f);
	//play->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));

    labelTTF = CCLabelTTF::create(CCLocalizedString("PLAY", NULL), FONT_COMMON, FONT_SIZE_86);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    play->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

	boosterPlus_1 = CCSprite::createWithSpriteFrameName("common/plus.png");
	boosterPlus_2 = CCSprite::createWithSpriteFrameName("common/plus.png");
	boosterPlus_3 = CCSprite::createWithSpriteFrameName("common/plus.png");

	booster_1 = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/booster_normal.png"),
		CCSprite::createWithSpriteFrameName("common/booster_selected.png"), this, menu_selector(MapMenuLayer::booster_1_Callback));
	booster_1->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f - popup->getContentSize().width /5.0f, 
		popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);	
	boosterPlus_1->setPosition(ccp(booster_1->getContentSize().width / 1.2f, booster_1->getContentSize().height / 4.0f));
	booster_1->addChild(boosterPlus_1,2);
	//booster_1->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));

	booster_2 = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/booster_normal.png"),
		CCSprite::createWithSpriteFrameName("common/booster_selected.png"), this, menu_selector(MapMenuLayer::booster_2_Callback));
	booster_2->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f, 
		popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
	boosterPlus_2->setPosition(ccp(booster_1->getContentSize().width / 1.2f, booster_1->getContentSize().height / 4.0f));
	booster_2->addChild(boosterPlus_2,2);
	//booster_2->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));

	booster_3 = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/booster_normal.png"),
		CCSprite::createWithSpriteFrameName("common/booster_selected.png"), this, menu_selector(MapMenuLayer::booster_3_Callback));
	booster_3->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /5.0f, 
		popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
	boosterPlus_3->setPosition(ccp(booster_1->getContentSize().width / 1.2f, booster_1->getContentSize().height / 4.0f));
	booster_3->addChild(boosterPlus_3,2);
	//booster_3->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));


	menu->addChild(close);
	menu->addChild(play);
	menu->addChild(booster_1);
	menu->addChild(booster_2);
	menu->addChild(booster_3);

	this->addChild(popup,11);
    
    menu->setVisible(false);
    popup->setVisible(false);
    facebookPanel->setVisible(false);
    
    if (LANDSCAPE)
    {
        facebookPanel->setPosition(ccp(facebookPanel->getPositionX(),
                                           facebookPanel->getPositionY() + WINSIZE.height));
        
        popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() - WINSIZE.height));
        menu->setPosition(ccp(menu->getPositionX(), menu->getPositionY() - WINSIZE.height));
    }
    else
    {
        facebookPanel->setPosition(ccp(facebookPanel->getPositionX() - WINSIZE.width,
                                           facebookPanel->getPositionY()));
        
        popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() + WINSIZE.height));
        menu->setPosition(ccp(menu->getPositionX(), menu->getPositionY() + WINSIZE.height));
    }
    
    CCSprite* livesPanelDown = CCSprite::createWithSpriteFrameName("gameMap/livesPanel.png");
    livesPanelDown->setColor(ccc3(128, 128, 128));
    livesPanel =CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("gameMap/livesPanel.png"), livesPanelDown,
                                         this, menu_selector(MapMenuLayer::livesCallback));

    livesPanel->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f + livesPanel->getContentSize().width/1.9f,
                                CCDirector::sharedDirector()->getWinSize().height/2.0f - livesPanel->getContentSize().height/1.9f));
    
    livesMenu = CCMenu::create();
    livesMenu->addChild(livesPanel);
    
    this->addChild(livesMenu, 10);
    
    struct timeval now;
    gettimeofday(&now, NULL);
    long lastTime = OptionsPtr->getLastTime();
    long second = now.tv_sec - OptionsPtr->getLastTime();
    
    if (second < 0)
        OptionsPtr->setLifeCount(5);
    
    while (second >= 1800)
    {
        if (OptionsPtr->getLifeCount() < 5)
            OptionsPtr->setLifeCount(OptionsPtr->getLifeCount() + 1);
        OptionsPtr->setLastTime(OptionsPtr->getLastTime() + 1800);
        second = second - 1800;
    }

    if (OptionsPtr->getLifeCount() >= 5)
    {
        livesTime = CCLabelTTF::create(CCLocalizedString("FULL"), FONT_COMMON, FONT_SIZE_48);
        livesTime->setPosition(ccp(livesPanel->getContentSize().width/1.6f, livesPanel->getContentSize().height/2.6f));
        livesPanel->addChild(livesTime);
    }
    else
    {
        char buf[255];
        sprintf(buf, "%0*d:%0*d", 2, (int)(1800 - second)/60, 2, (int)(1800 - second)%60);
        livesTime = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_64);
        livesTime->setAnchorPoint(ccp(0.0f, 0.5f));
        livesTime->setPosition(ccp(livesPanel->getContentSize().width/2.3f, livesPanel->getContentSize().height/2.6f));
        livesPanel->addChild(livesTime);
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::timeCallback))));
    }
    
    char buf[8];
    int temp = OptionsPtr->getLifeCount();
    sprintf(buf, "%d", OptionsPtr->getLifeCount());
    
    livesCount = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_200);
    livesCount->setAnchorPoint(ccp(0.5f, 0.5f));
    livesCount->setPosition(ccp(livesPanel->getContentSize().width/5.8f, livesPanel->getContentSize().height/2.4f));
    livesPanel->addChild(livesCount);
    
    if (getScale() != 1.0f)
    {
        livesMenu->setPosition(ccp(livesMenu->getPositionX() - (this->getContentSize().width*(1.0f - getScale())/2.0f),
                                   livesMenu->getPositionY() + (this->getContentSize().height*(1.0f - getScale())/2.0f)));
    }
    
    help = NULL;
    helpModal = NULL;
    
    
    stageSprite = CCSprite::create("plashka.png");
    stageSprite->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    this->addChild(stageSprite);
     
    stageMenu = CCMenu::create();
    stageMenu->setPosition(0.0f, 0.0f);
    stageSprite->addChild(stageMenu, 100);
    
    
     
    stageLogo = CCSprite::createWithSpriteFrameName("common/icelittle.png");
    stageLogo->setPosition(ccp(stageSprite->getContentSize().width/2.0f, stageSprite->getContentSize().height/1.5f));
    stageSprite->addChild(stageLogo);
    
    stageTitle = CCLabelTTF::create(CCLocalizedString("UNLOCK_STAGE", NULL), FONT_COMMON, FONT_SIZE_64);
    stageTitle->setPosition(ccp(stageSprite->getContentSize().width/2.0f, stageSprite->getContentSize().height/1.1f));
    stageTitle->setColor(IceCreamPink);
    stageSprite->addChild(stageTitle);
     
    stageText = CCLabelTTF::create(CCLocalizedString("UNLOCK_STAGE_TEXT", NULL), FONT_COMMON, FONT_SIZE_64);
    stageText->setPosition(ccp(stageSprite->getContentSize().width/2.0f, stageSprite->getContentSize().height/3.0f));
    stageText->setColor(IceCreamPink);
    stageSprite->addChild(stageText);
     
    stageClose = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
    CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MapMenuLayer::hideUnlock));
    stageClose->setPosition(stageSprite->getContentSize().width - stageClose->getContentSize().width/2.0f, stageSprite->getContentSize().height - stageClose->getContentSize().height/2.0f);
    stageMenu->addChild(stageClose, 100);
     
     
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
    CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
     
     
    stageButton = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(MapMenuLayer::stageCallback));
    stageButton->setPosition(stageSprite->getContentSize().width/3.8f, stageSprite->getContentSize().height/7.0f);
    stageMenu->addChild(stageButton);
    
    spriteNormal = CCSprite::createWithSpriteFrameName("common/greenButton.png");
    spriteSelected = CCSprite::createWithSpriteFrameName("common/greenButton.png");
    spriteSelected->setColor(ccGRAY);

    
    stageButtonPass = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(MapMenuLayer::passCallback));
    stageButtonPass->setPosition(stageSprite->getContentSize().width/4.0f * 2.8f, stageSprite->getContentSize().height/7.0f);
    stageMenu->addChild(stageButtonPass);

    
    string priceLocale = string(CCLocalizedString("NEXT", NULL)) + string("\n(");
    if (IAP::sharedInstance().products.empty())
        priceLocale += string("1$");
    else
        priceLocale += string(IAP::sharedInstance().products[0]->priceLocale);
    priceLocale += string(")");
     
    labelTTF = CCLabelTTF::create(priceLocale.c_str(), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    stageButton->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("UNLOCK_STAGE_BUTTON", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    stageButtonPass->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));


    stageSprite->setPosition(ccp(stageSprite->getPositionX(), stageSprite->getPositionY() - WINSIZE.height));
    stageSprite->setVisible(false);
    
    
    
    lastStageSprite = CCSprite::create("plashka.png");
    lastStageSprite->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    this->addChild(lastStageSprite, 100);
    
    lastStageMenu = CCMenu::create();
    lastStageMenu->setPosition(0.0f, 0.0f);
    lastStageSprite->addChild(lastStageMenu, 100);
    
    
    
    lastStageLogo = CCSprite::createWithSpriteFrameName("common/icelittle.png");
    lastStageLogo->setPosition(ccp(lastStageSprite->getContentSize().width/2.0f, lastStageSprite->getContentSize().height/1.5f));
    lastStageSprite->addChild(lastStageLogo);
    
    lastStageTitle = CCLabelTTF::create(CCLocalizedString("END_LEVEL_TITLE", NULL), FONT_COMMON, FONT_SIZE_64);
    lastStageTitle->setPosition(ccp(lastStageSprite->getContentSize().width/2.0f, lastStageSprite->getContentSize().height/1.1f));
    lastStageTitle->setColor(IceCreamPink);
    lastStageSprite->addChild(lastStageTitle);
    
    lastStageText = CCLabelTTF::create(CCLocalizedString("END_LEVEL_TEXT", NULL), FONT_COMMON, FONT_SIZE_36);
    lastStageText->setPosition(ccp(lastStageSprite->getContentSize().width/2.0f, lastStageSprite->getContentSize().height/2.8f));
    lastStageText->setColor(IceCreamPink);
    lastStageSprite->addChild(lastStageText);
    
    lastStageClose = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
                                          CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MapMenuLayer::hideLastStage));
    lastStageClose->setPosition(lastStageSprite->getContentSize().width - lastStageClose->getContentSize().width/2.0f,
                                lastStageSprite->getContentSize().height - lastStageClose->getContentSize().height/2.0f);
    lastStageMenu->addChild(lastStageClose, 100);
    
    
    spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    
    lastStageButton = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(MapMenuLayer::newsLastStage));
    lastStageButton->setPosition(lastStageSprite->getContentSize().width/3.8f, lastStageSprite->getContentSize().height/7.0f);
    lastStageMenu->addChild(lastStageButton);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("NEWS_BUTTON", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    lastStageButton->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    spriteNormal = CCSprite::createWithSpriteFrameName("common/greenButton.png");
    spriteSelected = CCSprite::createWithSpriteFrameName("common/greenButton.png");
    spriteSelected->setColor(ccGRAY);
    
    
    lastStageSubscribe = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(MapMenuLayer::subscribeLastStage));
    lastStageSubscribe->setPosition(lastStageSprite->getContentSize().width/4.0f*2.8f, lastStageSprite->getContentSize().height/7.0f);
    lastStageMenu->addChild(lastStageSubscribe);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("SUBSCRIBE", NULL), FONT_COMMON, FONT_SIZE_54);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    lastStageSubscribe->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    

    lastStageSprite->setPosition(ccp(lastStageSprite->getPositionX(), lastStageSprite->getPositionY() - WINSIZE.height));
    lastStageSprite->setVisible(false);
    
    
    if (getNetworkStatus() && MMPPtr && MMPPtr->isBanner)
    {
        bannerMenu = CCMenu::create();
        bannerMenu->setPosition(0.0f, 0.0f);
        this->addChild(bannerMenu, 1000);
        
        string bannerFile = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + string("banner.png");
        CCSprite* bannerSprite = CCSprite::create(bannerFile.c_str());
        sprintf(landingUrl, "%s", MMPPtr->trackingUrl.c_str());
        
        if (!GlobalsPtr->bannerShow && bannerSprite != NULL)
        {
            
            CCSprite* bannerSpriteGray = CCSprite::create(bannerFile.c_str());
            //bannerSpriteGray->setColor(ccGRAY);
            
            bannerButton = CCMenuItemSprite::create(bannerSprite, bannerSprite, this, menu_selector(MapMenuLayer::bannerCallback));
            bannerButton->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, CCDirector::sharedDirector()->getWinSize().height/2.0f));
            bannerButton->setEnabled(false);
            
            
            CCSprite* bannerSpriteClose = CCSprite::createWithSpriteFrameName("common/close.png");
            CCSprite* bannerSpriteCloseGray = CCSprite::createWithSpriteFrameName("common/close_on.png");
            bannerSpriteCloseGray->setColor(ccGRAY);
            
            bannerButtonClose = CCMenuItemSprite::create(bannerSpriteClose, bannerSpriteCloseGray, this, menu_selector(MapMenuLayer::closeBanner));
            bannerMenu->addChild(bannerButtonClose, 5100);
            bannerButtonClose->setAnchorPoint(ccp(1.0f, 1.0f));
            bannerButtonClose->setScale(0.5f);
            bannerButtonClose->setOpacity(150);
            bannerButtonClose->setPosition(bannerButton->getPositionX() + bannerButton->getContentSize().width/2.0f,
                                           bannerButton->getPositionY() + bannerButton->getContentSize().height/2.0f);
            
            bannerButtonGo = CCMenuItemSprite::create(CCSprite::createWithSpriteFrame(bannerSpriteGray->displayFrame()), CCSprite::createWithSpriteFrame(bannerSpriteGray->displayFrame()), this, menu_selector(MapMenuLayer::bannerCallback));
            bannerButtonGo->setOpacity(0);
            bannerButtonGo->setScaleY(0.9f);
            bannerButtonGo->setEnabled(true);
            bannerMenu->addChild(bannerButtonGo, 5100);
            bannerButtonGo->setAnchorPoint(ccp(0.0f, 0.0f));
            bannerButtonGo->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f - bannerButtonGo->getContentSize().width/2.0f,
                                            CCDirector::sharedDirector()->getWinSize().height/2.0f - bannerButtonGo->getContentSize().height/2.0f));
            
            bannerButtonGo2 = CCMenuItemSprite::create(CCSprite::createWithSpriteFrame(bannerSpriteGray->displayFrame()), CCSprite::createWithSpriteFrame(bannerSpriteGray->displayFrame()), this, menu_selector(MapMenuLayer::bannerCallback));
            bannerButtonGo2->setOpacity(0);
            bannerButtonGo2->setScaleX(0.9f);
            bannerButtonGo2->setEnabled(true);
            bannerMenu->addChild(bannerButtonGo2, 5100);
            bannerButtonGo2->setAnchorPoint(ccp(0.0f, 0.0f));
            bannerButtonGo2->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f - bannerButtonGo2->getContentSize().width/2.0f,
                                             CCDirector::sharedDirector()->getWinSize().height/2.0f - bannerButtonGo2->getContentSize().height/2.0f));
            
            bannerMenu->addChild(bannerButton, 5000);
            
            isPopup = true;
            lock = true;
        }
        else
        {
            if (bannerButton)
            {
                if (!bannerButton->isVisible())
                {
                    bannerButton->setVisible(false);
                    bannerButton->setEnabled(false);
                    bannerButtonClose->setVisible(false);
                    bannerButtonClose->setEnabled(false);
                    bannerButtonGo->setVisible(false);
                    bannerButtonGo->setEnabled(false);
                    bannerButtonGo2->setVisible(false);
                    bannerButtonGo2->setEnabled(false);
                }
            }
        }
    }
    else
    {
        if (bannerButton)
        {
            if (!bannerButton->isVisible())
            {
                bannerButton->setVisible(false);
                bannerButton->setEnabled(false);
                bannerButtonClose->setVisible(false);
                bannerButtonClose->setEnabled(false);
                bannerButtonGo->setVisible(false);
                bannerButtonGo->setEnabled(false);
                bannerButtonGo2->setVisible(false);
                bannerButtonGo2->setEnabled(false);
            }
        }
    }
    
    if (bannerButton)
    {
        if (!bannerButton->isVisible())
        {
            bannerButton->setVisible(false);
            bannerButton->setEnabled(false);
            bannerButtonClose->setVisible(false);
            bannerButtonClose->setEnabled(false);
            bannerButtonGo->setVisible(false);
            bannerButtonGo->setEnabled(false);
            bannerButtonGo2->setVisible(false);
            bannerButtonGo2->setEnabled(false);
        }
    }

    return true;
}

void MapMenuLayer::bannerCallback(CCObject * obj)
{
    bannerButton->setVisible(false);
    bannerButton->setEnabled(false);
    bannerButtonClose->setVisible(false);
    bannerButtonClose->setEnabled(false);
    bannerButtonGo->setVisible(false);
    bannerButtonGo->setEnabled(false);
    bannerButtonGo2->setVisible(false);
    bannerButtonGo2->setEnabled(false);
    
    GlobalsPtr->bannerShow = true;
    isPopup = false;
    lock = false;
    
    goToLink(landingUrl);
}

void MapMenuLayer::closeBanner(CCObject * obj)
{
    
    bannerButton->setVisible(false);
    bannerButton->setEnabled(false);
    bannerButtonClose->setVisible(false);
    bannerButtonClose->setEnabled(false);
    bannerButtonGo->setVisible(false);
    bannerButtonGo->setEnabled(false);
    bannerButtonGo2->setVisible(false);
    bannerButtonGo2->setEnabled(false);
    
    isPopup = false;
    lock = false;
    
    GlobalsPtr->bannerShow = true;
}

void MapMenuLayer::closeLoading()
{
    if (!popup->isVisible())
    {
        lock = false;
        isPopup = false;
    }
    popaplayer->closeLoading();
    menu->setEnabled(true);
}

void MapMenuLayer::stageCallback(CCNode* node)
{
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
        
    stageSprite->runAction(CCSequence::createWithTwoActions(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))), CCHide::create()));
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (node->getTag() == 24)
        IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.unlocklevelpack1");
    if (node->getTag() == 36)
        IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.unlocklevelpack2");
    if (node->getTag() == 48)
        IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.unlocklevelpack3");
    if (node->getTag() == 60)
        IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.unlocklevelpack4");
    if (node->getTag() == 72)
        IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.unlocklevelpack");
    popaplayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(CONNECTION_TIME), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::closeLoading))));
}

void MapMenuLayer::passCallback(CCNode* node)
{
    hideUnlock();
    
    int numLevel;

    if (node->getTag() == 24)
        numLevel = 107;
    else if (node->getTag() == 36)
        numLevel = 108;
    else if (node->getTag() == 48)
        numLevel = 109;
    else if (node->getTag() == 60)
        numLevel = 110;
    else if (node->getTag() == 72)
        numLevel = 111;
    
    if (OptionsPtr->getLevelData(numLevel).levelType != Score)
        levelPopup(numLevel, OptionsPtr->getLevelData(numLevel).countStar, OptionsPtr->getLevelData(numLevel).targetScore, OptionsPtr->getLevelData(numLevel).levelType, BoosterCrystal, BoosterBomb, BoosterFish);
    else
        levelPopup(numLevel, OptionsPtr->getLevelData(numLevel).countStar, OptionsPtr->getLevelData(numLevel).targetScore, OptionsPtr->getLevelData(numLevel).levelType, BoosterCrystal, BoosterBomb, BoosterNone);
}

void MapMenuLayer::showUnlock(int numLevel)
{
    if (stageSprite->isVisible())
        return;
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    lock = true;
    isPopup = true;
    stageButton->setTag(numLevel);
    stageButtonPass->setTag(numLevel);
    stageSprite->setVisible(true);
    stageButton->setEnabled(true);
    stageSprite->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, WINSIZE.height))));
}

void MapMenuLayer::hideUnlock()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    lock = false;
    isPopup = false;
    stageButton->setEnabled(false);
    stageSprite->runAction(CCSequence::createWithTwoActions(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))), CCHide::create()));
}

void MapMenuLayer::showLastStage(int numLevel)
{
    if (lastStageSprite->isVisible())
        return;
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    lock = true;
    isPopup = true;
    lastStageSprite->setVisible(true);
    lastStageSprite->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, WINSIZE.height))));
}

void MapMenuLayer::newsLastStage()
{
    if (OptionsPtr->isFacebookConnection() && FacebookPtr->sessionIsOpened() && getNetworkStatus())
        FacebookPtr->endGame();
    hideLastStage();
}

void MapMenuLayer::subscribeLastStage()
{
    goToLink("https://www.facebook.com/pages/Ice-Cream-Adventure/538662456202163");
    hideLastStage();
}

void MapMenuLayer::hideLastStage()
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    lock = false;
    isPopup = false;
    lastStageSprite->runAction(CCSequence::createWithTwoActions(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))), CCHide::create()));
}

void MapMenuLayer::LastStageCallback(CCNode* node)
{
    lastStageButton->runAction(CCSequence::createWithTwoActions(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))), CCHide::create()));
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
}




void MapMenuLayer::addSocialLayer()
{
    social = SocialLayer::create();
    this->getParent()->addChild(social, 10);
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::checkMessage))));
}

void MapMenuLayer::checkMessage(CCNode* pSender)
{
    if (OptionsPtr->isFacebookConnection())
        FacebookPtr->checkMessages();
}

void MapMenuLayer::showMessageboard()
{
    if (social->isMessageBoard)
    {
        lock = false;
        isPopup = false;
    }
    else
    {
        if (lock)
            return;
        lock = true;
        isPopup = true;
        if (!FacebookPtr->messages.empty() && FacebookPtr->sessionIsOpened() && getNetworkStatus() && OptionsPtr->isFacebookConnection())
            social->showMessageboard();
    }
}

void MapMenuLayer::closeMessageboard()
{
    lock = false;
    isPopup = false;
}

void MapMenuLayer::hideLive()
{
    livesPanel->setVisible(false);
    livesMenu->setVisible(false);
}

void MapMenuLayer::showLive()
{
    livesPanel->setVisible(true);
    livesMenu->setVisible(true);
}

void MapMenuLayer::changeOrientation()
{
    popaplayer->changeOrientation();
    this->setContentSize(WINSIZE);
    this->setPosition(ccp(0.0f, 0.0f));
    menu->setContentSize(WINSIZE);
    menu->setPosition(WINSIZE.width/2.0f, WINSIZE.height/2.0f);
    
    social->setContentSize(WINSIZE);
    social->changeOrientation();
    
    if (LANDSCAPE)
    {
        popup->setPosition(ccp(popup->getContentSize().width/2.0f, WINSIZE.height/2.0f));
        facebookPanel->setRotation(90.0f);
    	facebookPanel->setPosition(ccp(popup->getPositionX() + popup->getContentSize().width/2.0f + facebookPanel->getContentSize().height/2.0f, facebookPanel->getContentSize().width/2.0f));
        
        if (getScale() != 1.0f)
        {
            facebookPanel->setPosition(ccp(facebookPanel->getPositionX(), facebookPanel->getPositionY() - (facebookPanel->getContentSize().width*(1.0f - getScale())/2.0f)));
        }
        
        
    }
    else
    {
        facebookPanel->setPosition(ccp(facebookPanel->getContentSize().width/2.0f, facebookPanel->getContentSize().height/2.0f));
        facebookPanel->setRotation(0.0f);
        
        if (getScale() != 1.0f)
        {
            facebookPanel->setPosition(ccp(facebookPanel->getPositionX() - (facebookPanel->getContentSize().width*(1.0f - getScale())/2.0f), facebookPanel->getPositionY()));
        }
        
        popup->setPosition(ccp(WINSIZE.width/2.0f, facebookPanel->getPositionY() + facebookPanel->getContentSize().height/3.0f
                               +popup->getContentSize().height/1.9f));
    }
    
    if (!lock)
    {
        menu->setVisible(false);
        popup->setVisible(false);
        facebookPanel->setVisible(false);
        
        if (LANDSCAPE)
        {
            facebookPanel->setPosition(ccp(facebookPanel->getPositionX(),
                                           facebookPanel->getPositionY() + WINSIZE.height));
            
            popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() - WINSIZE.height));
        }
        else
        {
            facebookPanel->setPosition(ccp(facebookPanel->getPositionX() - WINSIZE.width,
                                           facebookPanel->getPositionY()));
            
            popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() + WINSIZE.height));
        }
    }
    
    close->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /2.5f,
                       popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + popup->getContentSize().height /2.4f);
    
    play->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                      popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f
                      - popup->getContentSize().height /2.3f + play->getContentSize().height/2.0f);
    
    
    booster_1->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f - popup->getContentSize().width /5.0f,
                           popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
    booster_2->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                           popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
    
    booster_3->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /5.0f,
                           popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
    
    
    livesPanel->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f + livesPanel->getContentSize().width/1.9f,
                                CCDirector::sharedDirector()->getWinSize().height/2.0f - livesPanel->getContentSize().height/1.9f));
    
    livesMenu->setContentSize(WINSIZE);
    livesMenu->setPosition(WINSIZE.width/2.0f, WINSIZE.height/2.0f);
    
    if (lock && popup->isVisible())
    {
        if (help)
            help->setPosition(play->getPositionX() - play->getContentSize().width, play->getPositionY());
        if (helpModal)
            helpModal->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, CCDirector::sharedDirector()->getWinSize().height/1.5f));
    }
    
    if (getScale() != 1.0f)
    {
        livesMenu->setPosition(ccp(livesMenu->getPositionX() - (this->getContentSize().width*(1.0f - getScale())/2.0f),
                                   livesMenu->getPositionY() + (this->getContentSize().height*(1.0f - getScale())/2.0f)));
    }
    
    
    if (livesLayer)
    {
        livesLayer->setContentSize(WINSIZE);
        
        panelLivesLayer->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
        
        if (LANDSCAPE)
        {
            if (IPHONE_4 || IPHONE_5)
                panelLivesLayer->setScale(0.72f);
            else
                panelLivesLayer->setScale(0.75f);
        }
        else
            panelLivesLayer->setScale(1.0f);
    }
    
    stageSprite->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    if (bannerMenu)
    {
        bannerButton->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, CCDirector::sharedDirector()->getWinSize().height/2.0f));
        bannerButtonClose->setPosition(bannerButton->getPositionX() + bannerButton->getContentSize().width/2.0f,
                                       bannerButton->getPositionY() + bannerButton->getContentSize().height/2.0f);
        
        bannerButtonGo->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f - bannerButtonGo->getContentSize().width/2.0f,
                                        CCDirector::sharedDirector()->getWinSize().height/2.0f - bannerButtonGo->getContentSize().height/2.0f));
        
        bannerButtonGo2->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f - bannerButtonGo2->getContentSize().width/2.0f,
                                         CCDirector::sharedDirector()->getWinSize().height/2.0f - bannerButtonGo2->getContentSize().height/2.0f));
        
    }
    
    if (!stageSprite->isVisible())
        stageSprite->setPosition(ccp(stageSprite->getPositionX(), stageSprite->getPositionY() - WINSIZE.height));
    
    if (spriteLoading)
    {
        spriteLoading->removeFromParentAndCleanup(true);
        labelLoad->removeFromParentAndCleanup(true);
        if (IPAD)
        {
            if (LANDSCAPE)
                spriteLoading = CCSprite::create("loadingiPadLandscape.png");
            else
                spriteLoading = CCSprite::create("loadingiPadPortrait.png");
        }
        else if (IPAD_MINI)
        {
            if (LANDSCAPE)
                spriteLoading = CCSprite::create("loadingiPadMiniLandscape.png");
            else
                spriteLoading = CCSprite::create("loadingiPadMiniPortrait.png");
        }
        else if (IPHONE_4||IPHONE_5)
        {
            if (LANDSCAPE)
                spriteLoading = CCSprite::create("loadingIphoneLanscape.png");
            else
                spriteLoading = CCSprite::create("loadingIphonePortrait.png");
            spriteLoading->setScale(1.2f);
        }
        labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
        labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
        this->addChild(labelLoad,1001);
        spriteLoading->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
        this->addChild(spriteLoading, 1000);
    }
}

void MapMenuLayer::timeCallback(CCNode* sender)
{
    if (livesTime)
        livesTime->removeFromParentAndCleanup(true);
    if (livesCount)
        livesCount->removeFromParentAndCleanup(true);
    
    struct timeval now;
    gettimeofday(&now, NULL);
    long second = now.tv_sec - OptionsPtr->getLastTime();
    
    if (second < 0)
        OptionsPtr->setLifeCount(5);
    
    while (second >= 1800)
    {
        if (OptionsPtr->getLifeCount() < 5)
            OptionsPtr->setLifeCount(OptionsPtr->getLifeCount() + 1);
        OptionsPtr->setLastTime(OptionsPtr->getLastTime() + 1800);
        second = second - 1800;
    }
    
    if (OptionsPtr->getLifeCount() >= 5)
    {
        livesTime = CCLabelTTF::create(CCLocalizedString("FULL"), FONT_COMMON, FONT_SIZE_48);
        livesTime->setPosition(ccp(livesPanel->getContentSize().width/1.6f, livesPanel->getContentSize().height/2.6f));
        livesPanel->addChild(livesTime);
    }
    else
    {
        char buf[255];
        sprintf(buf, "%0*d:%0*d", 2, (int)(1800 - second)/60, 2, (int)(1800 - second)%60);
        livesTime = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_64);
        livesTime->setAnchorPoint(ccp(0.0f, 0.5f));
        livesTime->setPosition(ccp(livesPanel->getContentSize().width/2.3f, livesPanel->getContentSize().height/2.6f));
        livesPanel->addChild(livesTime);
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::timeCallback))));
        
        if (livesTimePopap)
        {
            livesTimePopap->setString(buf);
        }
    }
    
    char buf[8];
    int temp = OptionsPtr->getLifeCount();
    sprintf(buf, "%d", OptionsPtr->getLifeCount());
    
    livesCount = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_200);
    livesCount->setAnchorPoint(ccp(0.5f, 0.5f));
    livesCount->setPosition(ccp(livesPanel->getContentSize().width/5.8f, livesPanel->getContentSize().height/2.4f));
    livesPanel->addChild(livesCount);
}

void MapMenuLayer::livesCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    popupLives();
}

void MapMenuLayer::fishingEnded(CCNode* sender)
{
    if (currentLevel == OptionsPtr->getCurrentLevel() && OptionsPtr->isFacebookConnection() && FacebookPtr->sessionIsOpened())
    {
        char buf[255];
        sprintf(buf, "unlock%d", currentLevel);
        bool flag = CCUserDefault::sharedUserDefault()->getBoolForKey(buf, false);
        if (!((currentLevel-1)%12) && !flag)
            popaplayer->popupPost((char*)CCLocalizedString("POST_ON_WALL", NULL), (char*)CCLocalizedString("POST_ON_WALL_TEXT", NULL), (char*)CCLocalizedString("POST_ON_WALL", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(MapMenuLayer::unlockStage), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    lock = true;
}

void MapMenuLayer::unlockStage(CCNode* pSender)
{
    FacebookPtr->unlockStage(currentLevel);
}

void MapMenuLayer::closeEnded(CCNode* sender)
{
    facebookPanel->setVisible(false);
    menu->setVisible(false);
    popup->setVisible(false);
}

void MapMenuLayer::popupLives()
{
    if (lock)
        return;
    
    lock = true;
    
    isPopup = true;
    
    livesLayer = CCLayerColor::create(ccc4(0, 0, 0, 0));
    
    panelLivesLayer = CCSprite::createWithSpriteFrameName("common/panel.png");
    panelLivesLayer->setPosition(ccp(WINSIZE.width/2.0f + WINSIZE.width, WINSIZE.height/2.0f));
    livesLayer->addChild(panelLivesLayer);
    panelLivesLayer->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME,ccp(-WINSIZE.width, 0.0f))));
    
    CCLabelTTF* heartLabel;
    if (OptionsPtr->getLifeCount() > 0)
        heartLabel = CCLabelTTF::create(CCLocalizedString("GET_MORE_LIVES", NULL), FONT_COMMON, FONT_SIZE_64);
    else
        heartLabel = CCLabelTTF::create(CCLocalizedString("NO_MORE_LIVES", NULL), FONT_COMMON, FONT_SIZE_64);
    
    heartLabel->setPosition(ccp(panelLivesLayer->getContentSize().width/2.0f, 8.0f*panelLivesLayer->getContentSize().height/9.0f));
    heartLabel->setColor(IceCreamBlue);

    
    panelLivesLayer->addChild(heartLabel);
    
    CCSprite* heart;
    
    if (OptionsPtr->getLifeCount() <=0)
        heart = CCSprite::createWithSpriteFrameName("gameMap/heartBroken.png");
    else
        heart = CCSprite::createWithSpriteFrameName("gameMap/heart.png");
    
    heart->setPosition(ccp(panelLivesLayer->getContentSize().width/2.0, 3.0f*panelLivesLayer->getContentSize().height/4.0f));
    panelLivesLayer->addChild(heart);
    
    heartMenu = CCMenu::create();
    panelLivesLayer->addChild(heartMenu);
    heartMenu->setAnchorPoint(ccp(0.0f, 0.0f));
    heartMenu->setPosition(ccp(0.0f, 0.0f));
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    askFriend = CCMenuItemSprite::create(spriteNormal,
                                     spriteSelected, this, menu_selector(MapMenuLayer::askFriendCallback));
    askFriend->setPosition(ccp(panelLivesLayer->getContentSize().width/2.0f, panelLivesLayer->getContentSize().height/6.0f));
    heartMenu->addChild(askFriend);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("ASK_FRIEND", NULL), FONT_COMMON, FONT_SIZE_54);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    askFriend->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    
    
    
    spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    moreLives = CCMenuItemSprite::create(spriteNormal,
                                                           spriteSelected, this, menu_selector(MapMenuLayer::moreLivesCallback));
    moreLives->setPosition(ccp(panelLivesLayer->getContentSize().width/2.0f, panelLivesLayer->getContentSize().height/6.0f + askFriend->getContentSize().height));
    heartMenu->addChild(moreLives);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("MORE_LIVES", NULL), FONT_COMMON, FONT_SIZE_54);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    moreLives->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

    
    if (OptionsPtr->getLifeCount() >0)
    {
        moreLives->setVisible(false);
    }

    
    closeHeart = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
                                                            CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MapMenuLayer::closeHeartCallback));
    closeHeart->setPosition(ccp(panelLivesLayer->getContentSize().width - closeHeart->getContentSize().width/2.0f,
                                panelLivesLayer->getContentSize().height - closeHeart->getContentSize().height/2.0f));
    heartMenu->addChild(closeHeart);
    
    this->addChild(livesLayer, 100);
    
    
    
    CCLabelTTF* labelNotif = CCLabelTTF::create(CCLocalizedString("TIME_NEXT_LIVE", NULL), FONT_COMMON, FONT_SIZE_64);
	labelNotif->setPosition(ccp(panelLivesLayer->getContentSize().width / 2.0f, panelLivesLayer->getContentSize().height / 1.75f));
	panelLivesLayer->addChild(labelNotif);
	livesLayer->setOpacity(0);
    ccColor3B color;
    color.r = 0x91;
    color.g = 0x3b;
    color.b = 0x55;
	labelNotif->setColor(color);
    
    if (OptionsPtr->getLifeCount() >= 5)
        labelNotif->setVisible(false);
    else
        labelNotif->setVisible(true);
    
    
    livesTimePopap = CCLabelTTF::create("  ", FONT_COMMON, FONT_SIZE_86);
    livesTimePopap->setAnchorPoint(ccp(0.0f, 0.5f));
    livesTimePopap->setPosition(ccp(panelLivesLayer->getContentSize().width / 2.4f, panelLivesLayer->getContentSize().height / 2.0f));
    panelLivesLayer->addChild(livesTimePopap);
    
    CCSprite* spriteOn = CCSprite::createWithSpriteFrameName("common/onButton.png");
    
    CCLabelTTF* labelOn = CCLabelTTF::create(CCLocalizedString("ON", NULL), FONT_COMMON, FONT_SIZE_86);
    labelOn->setColor(ccWHITE);
    labelOn->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    spriteOn->addChild(labelOn);
    labelOn->setPosition(ccp(labelOn->getParent()->getContentSize().width/2.0f, labelOn->getParent()->getContentSize().height/2.0f));
    
    CCSprite* spriteOff = CCSprite::createWithSpriteFrameName("common/offButton.png");
    
    CCLabelTTF* labelOff = CCLabelTTF::create(CCLocalizedString("OFF", NULL), FONT_COMMON, FONT_SIZE_86);
    labelOff->setColor(ccWHITE);
    labelOff->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    spriteOff->addChild(labelOff);
    labelOff->setPosition(ccp(labelOff->getParent()->getContentSize().width/2.0f, labelOff->getParent()->getContentSize().height/2.0f));
    
    notif_1_Button = CCMenuItemSprite::create(spriteOn, spriteOn, this, menu_selector(MapMenuLayer::notif_1_Callback));
	notif_1_Button->setPosition(ccp(panelLivesLayer->getContentSize().width/2.0f, panelLivesLayer->getContentSize().height/2.5f));
    notif_1_Button->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
	heartMenu->addChild(notif_1_Button);
	notif_1_Button->setOpacity(0);
	notif_1_Button->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    
    
    if (OptionsPtr->isNotification())
    {
		notif_1_Button->setNormalImage(spriteOn);
		notif_1_Button->setSelectedImage(spriteOn);
    }
    else
    {
        notif_1_Button->setNormalImage(spriteOff);
		notif_1_Button->setSelectedImage(spriteOff);
    }
    
    if (OptionsPtr->getLifeCount() > 0)
        notif_1_Button->setVisible(false);
    else
        notif_1_Button->setVisible(true);
    
    
    if (LANDSCAPE)
    {
        if (IPHONE_4 || IPHONE_5)
            panelLivesLayer->setScale(0.72f);
        else
            panelLivesLayer->setScale(0.75f);
    }
    else
        panelLivesLayer->setScale(1.0f);
}

void MapMenuLayer::notif_1_Callback(CCObject* pSender)
{
    CCSprite* spriteOn = CCSprite::createWithSpriteFrameName("common/onButton.png");
    
    CCLabelTTF* labelOn = CCLabelTTF::create(CCLocalizedString("ON", NULL), FONT_COMMON, FONT_SIZE_86);
    labelOn->setColor(ccWHITE);
    labelOn->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    spriteOn->addChild(labelOn);
    labelOn->setPosition(ccp(labelOn->getParent()->getContentSize().width/2.0f, labelOn->getParent()->getContentSize().height/2.0f));
    
    CCSprite* spriteOff = CCSprite::createWithSpriteFrameName("common/offButton.png");
    
    CCLabelTTF* labelOff = CCLabelTTF::create(CCLocalizedString("OFF", NULL), FONT_COMMON, FONT_SIZE_86);
    labelOff->setColor(ccWHITE);
    labelOff->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    spriteOff->addChild(labelOff);
    labelOff->setPosition(ccp(labelOff->getParent()->getContentSize().width/2.0f, labelOff->getParent()->getContentSize().height/2.0f));
    
	if (OptionsPtr->isNotification())
	{
        OptionsPtr->setNotification(false);
		notif_1_Button->setNormalImage(spriteOff);
        notif_1_Button->setNormalImage(spriteOff);
        removeAllNotification();
	}
	else
	{
        OptionsPtr->setNotification(true);
        notif_1_Button->setNormalImage(spriteOn);
        notif_1_Button->setNormalImage(spriteOn);
	}
    OptionsPtr->save();
}

void MapMenuLayer::askFriendCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    closeHeartCallback(NULL);
    
    
    if (social->isMessageBoard)
    {
        lock = false;
        isPopup = false;
    }
    else
    {
        if (lock)
            return;
        lock = true;
        if (OptionsPtr->isFacebookConnection() && FacebookPtr->sessionIsOpened() && getNetworkStatus())
        {
            if (FacebookPtr->friendsScores.size() > 0)
            {
                social->showAskMessageboard();
                isPopup = true;
            }
            else
            {
                FacebookPtr->inviteFriends();
                lock = false;
                isPopup = false;
            }
        }
        else
        {
            FacebookPtr->loginWithInvite();
            lock = false;
            isPopup = false;
        }
    }
}

void MapMenuLayer::moreLivesCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.5lives");
    closeHeartCallback(NULL);
    popaplayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
}

void MapMenuLayer::closeHeartCallback(CCObject* pSender)
{
    lock = false;
    
    livesTimePopap->removeFromParentAndCleanup(true);
    livesTimePopap = NULL;
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    isPopup = false;
    livesLayer->removeFromParentAndCleanup(true);
    livesLayer = NULL;
    notif_1_Button->removeFromParentAndCleanup(true);
    notif_1_Button = NULL;
}

void MapMenuLayer::sendLife(CCObject* pSender)
{
    
}

void MapMenuLayer::updateFacebook()
{
    if (isPopup)
    {
        social->hideScoreBoard();
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::updateFacebookCallback))));
    }
    else
    {
        lock = false;
    }
}

void MapMenuLayer::updateFacebookCallback(CCNode* pSender)
{
    CCSprite* sprite;
    if (IPAD)
    {
        if (LANDSCAPE)
            sprite = CCSprite::create("loadingiPadLandscape.png");
        else
            sprite = CCSprite::create("loadingiPadPortrait.png");
    }
    else if (IPAD_MINI)
    {
        if (LANDSCAPE)
            sprite = CCSprite::create("loadingiPadMiniLandscape.png");
        else
            sprite = CCSprite::create("loadingiPadMiniPortrait.png");
    }
    else if (IPHONE_4||IPHONE_5)
    {
        if (LANDSCAPE)
            sprite = CCSprite::create("loadingIphoneLanscape.png");
        else
            sprite = CCSprite::create("loadingIphonePortrait.png");
        sprite->setScale(1.2f);
    }
    CCLabelTTF* labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
    labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
    this->addChild(labelLoad,1001);
    sprite->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    this->addChild(sprite, 1000);
    sprite->setVisible(false);
    sprite->setOpacity(0);
    sprite->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCShow::create(), CCFadeIn::create(0.3f),  NULL));
    
    labelLoad->setVisible(false);
    labelLoad->setOpacity(0);
    labelLoad->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCShow::create(), CCFadeIn::create(0.3f),  NULL));
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME*2.0f), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::reinitCallback))));
}

void MapMenuLayer::reinitCallback(CCNode* node)
{
    CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(levelNumForPost, true));
}

void MapMenuLayer::addBonus(CCNode* pSender)
{
    FacebookPtr->jubileePost();
}

void MapMenuLayer::levelPopup(int levelNum, int starCount, int targetScore, eLevelType typeLevel, eBoosterType first, eBoosterType second, eBoosterType third)
{
    if (OptionsPtr->getLifeCount() <=0)
    {
        popupLives();
        return;
    }
    
    levelNumForPost = levelNum;
    
    if (lock)
        return;
    changeOrientation();
	char buf[255];
    
    lock = true;
    
    isPopup = true;

	currentLevel = levelNum;
    
    firstBooster = first;
    secondBooster = second;
    thirdBooster = third;
    
    popup->setVisible(true);
	menu->setVisible(true);
    
    if (LANDSCAPE)
    {
        popup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::fishingEnded))));
    }
    else
    {
        popup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::fishingEnded))));
    }

	popup->setVisible(true);
	menu->setVisible(true);
    
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("HIGH_SCORE", NULL), FONT_COMMON, FONT_SIZE_36);
    ccColor3B color;
	color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    

	if (starCount == 1)
	{
        CCSprite* star = CCSprite::createWithSpriteFrameName("common/star.png");
		startLeft->setDisplayFrame(star->displayFrame());
	}
	else if (starCount == 2)
	{
        CCSprite* star2 = CCSprite::createWithSpriteFrameName("common/star2.png");
        CCSprite* star = CCSprite::createWithSpriteFrameName("common/star.png");
		startLeft->setDisplayFrame(star->displayFrame());
		startCenter->setDisplayFrame(star2->displayFrame());
	}
	else if (starCount >= 3)
	{
        CCSprite* star = CCSprite::createWithSpriteFrameName("common/star.png");
        CCSprite* star2 = CCSprite::createWithSpriteFrameName("common/star2.png");
        CCSprite* star3 = CCSprite::createWithSpriteFrameName("common/star3.png");
		startLeft->setDisplayFrame(star->displayFrame());
		startCenter->setDisplayFrame(star2->displayFrame());
		startRight->setDisplayFrame(star3->displayFrame());
	}
	else
	{
		startLeft->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/star_popup_disabled.png"));
		startCenter->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/star_popup_disabled.png"));
		startRight->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/star_popup_disabled.png"));
	}

	sprintf(buf, "%s: %d", CCLocalizedString("TARGET", NULL), targetScore);
	targetTitle->setString(buf);
	targetTitle->setPosition(ccp(targetTitle->getParent()->getContentSize().width/2.0f, targetTitle->getParent()->getContentSize().height/2.0f));


	sprintf(buf, "%s %d", CCLocalizedString("LEVEL"), levelNum);
	levelTitle->setString(buf);
    if (levelNum > 105)
    {
        levelTitle->setString(CCLocalizedString("EXTRA_LEVEL", NULL));
        levelTitle->setFontSize(FONT_SIZE_48);
    }
	levelTitle->setPosition(ccp(levelTitle->getParent()->getContentSize().width/2.0f, levelTitle->getParent()->getContentSize().height/1.1f));

    updateBoosters();
    
    if (typeLevel == Ice)
    {
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/iceStudy.png"), CCSprite::createWithSpriteFrameName("common/iceStudyDown.png"), this, menu_selector(MapMenuLayer::helpModalCallback));
        helpModal = CCSprite::createWithSpriteFrameName("common/iceStudyModal.png");
        
        CCLabelTTF* titlehelp = CCLabelTTF::create(CCLocalizedString("SETTING_HOW_PLAY", NULL), FONT_COMMON, FONT_SIZE_64);
        titlehelp->setColor(IceCreamBrown);
        helpModal->addChild(titlehelp);
        titlehelp->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/1.1f));
        
        CCLabelTTF* titlehelp2 = CCLabelTTF::create(CCLocalizedString("STUDY_ICE_TITLE", NULL), FONT_COMMON, FONT_SIZE_48);
        titlehelp2->setColor(IceCreamBrown2);
        helpModal->addChild(titlehelp2);
        titlehelp2->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/1.25f));
        
        CCLabelTTF* texthelp = CCLabelTTF::create(CCLocalizedString("STUDY_ICE_TEXT", NULL), FONT_COMMON, FONT_SIZE_48);
        texthelp->setColor(IceCreamBrown2);
        helpModal->addChild(texthelp);
        texthelp->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/6.0f));
    }
    else if (typeLevel == Score)
    {
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/scoreStudy.png"), CCSprite::createWithSpriteFrameName("common/scoreStudyDown.png"), this, menu_selector(MapMenuLayer::helpModalCallback));
        helpModal = CCSprite::createWithSpriteFrameName("common/scoreStudyModal.png");
        
        CCLabelTTF* helpLabel = CCLabelTTF::create("15", FONT_COMMON, FONT_SIZE_64);
        helpLabel->setColor(IceCreamBlue);
        helpModal->addChild(helpLabel);
        helpLabel->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/2.0f));
        
        CCLabelTTF* moves = CCLabelTTF::create(CCLocalizedString("MOVES", NULL), FONT_COMMON, FONT_SIZE_48);
        moves->setColor(IceCreamBlue);
        helpModal->addChild(moves);
        moves->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/1.65f));
        
        CCLabelTTF* titlehelp = CCLabelTTF::create(CCLocalizedString("SETTING_HOW_PLAY", NULL), FONT_COMMON, FONT_SIZE_64);
        titlehelp->setColor(IceCreamBrown);
        helpModal->addChild(titlehelp);
        titlehelp->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/1.1f));
        
        CCLabelTTF* titlehelp2 = CCLabelTTF::create(CCLocalizedString("STUDY_MOVES_TITLE", NULL), FONT_COMMON, FONT_SIZE_48);
        titlehelp2->setColor(IceCreamBrown2);
        helpModal->addChild(titlehelp2);
        titlehelp2->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/1.25f));
        
        CCLabelTTF* texthelp = CCLabelTTF::create(CCLocalizedString("STUDY_MOVES_TEXT", NULL), FONT_COMMON, FONT_SIZE_48);
        texthelp->setColor(IceCreamBrown2);
        helpModal->addChild(texthelp);
        texthelp->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/6.0f));
    }
    else if (typeLevel == BringDown)
    {
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/bringStudy.png"), CCSprite::createWithSpriteFrameName("common/bringStudyDown.png"), this, menu_selector(MapMenuLayer::helpModalCallback));
        helpModal = CCSprite::createWithSpriteFrameName("common/bringStudyModal.png");
        
        CCLabelTTF* titlehelp = CCLabelTTF::create(CCLocalizedString("SETTING_HOW_PLAY", NULL), FONT_COMMON, FONT_SIZE_64);
        titlehelp->setColor(IceCreamBrown);
        helpModal->addChild(titlehelp);
        titlehelp->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/1.1f));
        
        CCLabelTTF* titlehelp2 = CCLabelTTF::create(CCLocalizedString("STUDY_BRING_TITLE", NULL), FONT_COMMON, FONT_SIZE_48);
        titlehelp2->setColor(IceCreamBrown2);
        helpModal->addChild(titlehelp2);
        titlehelp2->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/1.25f));
        
        CCLabelTTF* texthelp = CCLabelTTF::create(CCLocalizedString("STUDY_BRING_TEXT", NULL), FONT_COMMON, FONT_SIZE_48);
        texthelp->setColor(IceCreamBrown2);
        helpModal->addChild(texthelp);
        texthelp->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/6.0f));
    }
    else if (typeLevel == Time)
    {
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/timeStudy.png"), CCSprite::createWithSpriteFrameName("common/timeStudyDown.png"), this, menu_selector(MapMenuLayer::helpModalCallback));
        helpModal = CCSprite::createWithSpriteFrameName("common/timeStudyModal.png");
        
        CCLabelTTF* titlehelp = CCLabelTTF::create(CCLocalizedString("SETTING_HOW_PLAY", NULL), FONT_COMMON, FONT_SIZE_64);
        titlehelp->setColor(IceCreamBrown);
        helpModal->addChild(titlehelp);
        titlehelp->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/1.1f));
        
        CCLabelTTF* titlehelp2 = CCLabelTTF::create(CCLocalizedString("STUDY_TIME_TITLE", NULL), FONT_COMMON, FONT_SIZE_48);
        titlehelp2->setColor(IceCreamBrown2);
        helpModal->addChild(titlehelp2);
        titlehelp2->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/1.25f));
        
        CCLabelTTF* texthelp = CCLabelTTF::create(CCLocalizedString("STUDY_TIME_TEXT", NULL), FONT_COMMON, FONT_SIZE_48);
        texthelp->setColor(IceCreamBrown2);
        helpModal->addChild(texthelp);
        texthelp->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/6.0f));
    }
    
    if (WINSIZE.width == 640)
        helpModal->setScale(0.8f);
    
    help->setPosition(play->getPositionX() - play->getContentSize().width, play->getPositionY());
    menu->addChild(help);
    
    this->addChild(helpModal,200);
    helpModal->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, CCDirector::sharedDirector()->getWinSize().height/1.5f));
    helpModal->setVisible(false);
    
    play->stopAllActions();
    play->setScale(0.7f);
	play->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    booster_1->stopAllActions();
    booster_1->setScale(0.7f);
	booster_1->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    booster_2->stopAllActions();
    booster_2->setScale(0.7f);
	booster_2->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    booster_3->stopAllActions();
    booster_3->setScale(0.7f);
	booster_3->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    help->stopAllActions();
    help->setScale(0.7f);
	help->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    close->stopAllActions();
    close->setScale(0.7f);
	close->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    if (typeLevel == Score)
        booster_3->setVisible(false);
    else
        booster_3->setVisible(true);
    
    social->showScoreboard(currentLevel);
}

void MapMenuLayer::updateBoosters()
{
    isPopup = true;
    char buf[255];
    
    if (boosterPlus_1)
    {
        for (int i = 0; i > boosterPlus_1->getChildrenCount(); i++)
        {
            if (((CCSprite*)boosterPlus_1->getChildren()->objectAtIndex(i)) != booster1Check)
                ((CCNode*)boosterPlus_1->getChildren()->objectAtIndex(i))->removeFromParentAndCleanup(true);
        }
    }
    if (boosterPlus_2)
    {
        for (int i = 0; i > boosterPlus_2->getChildrenCount(); i++)
        {
            if (((CCSprite*)boosterPlus_2->getChildren()->objectAtIndex(i)) != booster2Check)
                ((CCNode*)boosterPlus_2->getChildren()->objectAtIndex(i))->removeFromParentAndCleanup(true);
        }
    }
    if (boosterPlus_3)
    {
        for (int i = 0; i > boosterPlus_3->getChildrenCount(); i++)
        {
            if (((CCSprite*)boosterPlus_3->getChildren()->objectAtIndex(i)) != booster3Check)
                ((CCNode*)boosterPlus_3->getChildren()->objectAtIndex(i))->removeFromParentAndCleanup(true);
        }
    }

    if (firstBooster == BoosterCrystal)
	{
        booster_1->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/crystal.png");
		booster->setPosition(ccp(booster_1->getContentSize().width/2.1f, booster_1->getContentSize().height/1.9f));
        booster->setScale(0.8f);
		booster_1->addChild(booster);
        
        if (OptionsPtr->getCrystalCOunt() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_1->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getCrystalCOunt());
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            label->setColor(ccWHITE);

            boosterPlus_1->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_1->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
    

	if (secondBooster == BoosterBomb)
	{
        booster_2->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/bomb.png");
        booster->setScale(0.8f);
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/1.9f));
		booster_2->addChild(booster);
        
        if (OptionsPtr->getBombCount() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_2->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getBombCount());
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            label->setColor(ccWHITE);
            boosterPlus_2->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_2->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
    
    if (thirdBooster == BoosterFish)
	{
        booster_3->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/fish.png");
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/1.9f));
        booster->setScale(0.7f);
		booster_3->addChild(booster);
        
        if (OptionsPtr->getFishCount() > 0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_3->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getFishCount());
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            label->setColor(ccWHITE);

            boosterPlus_3->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_3->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}

    
    OptionsPtr->save();
}


void MapMenuLayer::helpModalCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    help->stopAllActions();
    help->setScale(0.7f);
	help->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    if (helpModal->isVisible())
        helpModal->setVisible(false);
    else
        helpModal->setVisible(true);
}

void MapMenuLayer::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, 1);
}

void MapMenuLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    if (helpModal)
        if (helpModal->isVisible())
            helpModal->setVisible(false);
}

bool MapMenuLayer::isLock()
{
	return lock;
}

void MapMenuLayer::closeCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    close->stopAllActions();
    close->setScale(0.7f);
	close->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
	lock = false;
    
    isPopup = false;
    
    help->removeFromParentAndCleanup(true);
    helpModal->removeFromParentAndCleanup(true);
    helpModal = NULL;
    
    if (WINSIZE.height < WINSIZE.width)
    {
        popup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))), CCCallFuncN::create(this,callfuncN_selector(MapMenuLayer::closeEnded))));
//        facebookPanel->runAction(CCMoveBy::create(0.3f, ccp(0.0f, WINSIZE.height)));
    }
    else
    {
        popup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))), CCCallFuncN::create(this,callfuncN_selector(MapMenuLayer::closeEnded))));
//        facebookPanel->runAction(CCMoveBy::create(0.3f, ccp(-WINSIZE.width, 0.0f)));
    }
    
    social->hideScoreBoard();
}

void MapMenuLayer::backCallback(CCObject* pSender)
{
	CCDirector::sharedDirector()->replaceScene(MainMenuScene::scene());
}

void MapMenuLayer::playCallback(CCObject* pSender)
{
    menu->setEnabled(false);
    if (WINSIZE.height < WINSIZE.width)
    {
        popup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))), CCCallFuncN::create(this,callfuncN_selector(MapMenuLayer::closeEnded))));
        //        facebookPanel->runAction(CCMoveBy::create(0.3f, ccp(0.0f, WINSIZE.height)));
    }
    else
    {
        popup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))), CCCallFuncN::create(this,callfuncN_selector(MapMenuLayer::closeEnded))));
        //        facebookPanel->runAction(CCMoveBy::create(0.3f, ccp(-WINSIZE.width, 0.0f)));
    }
    
    
    if (IPAD)
    {
        if (LANDSCAPE)
            spriteLoading = CCSprite::create("loadingiPadLandscape.png");
        else
            spriteLoading = CCSprite::create("loadingiPadPortrait.png");
    }
    else if (IPAD_MINI)
    {
        if (LANDSCAPE)
            spriteLoading = CCSprite::create("loadingiPadMiniLandscape.png");
        else
            spriteLoading = CCSprite::create("loadingiPadMiniPortrait.png");
    }
    else if (IPHONE_4||IPHONE_5)
    {
        if (LANDSCAPE)
            spriteLoading = CCSprite::create("loadingIphoneLanscape.png");
        else
            spriteLoading = CCSprite::create("loadingIphonePortrait.png");
        spriteLoading->setScale(1.2f);
    }
    labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
    labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
    this->addChild(labelLoad,1001);
    spriteLoading->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    this->addChild(spriteLoading, 1000);
    spriteLoading->setVisible(false);
    spriteLoading->setOpacity(0);
    spriteLoading->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCShow::create(), CCFadeIn::create(0.3f),  NULL));
    
    labelLoad->setVisible(false);
    labelLoad->setOpacity(0);
    labelLoad->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCShow::create(), CCFadeIn::create(0.3f),  NULL));
    
    social->hideScoreBoard();
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME*2.0f), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::playAfterLoad))));
}

void MapMenuLayer::playAfterLoad(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    play->stopAllActions();
    play->setScale(0.7f);
	play->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    OptionsPtr->save();
	menu->setVisible(false);
	popup->setVisible(false);
    if (currentLevel == 1 && OptionsPtr->getCurrentLevel() <= currentLevel)
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(1));
    else if (currentLevel == 13 && OptionsPtr->getCurrentLevel() <= currentLevel)
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(2));
    else if (currentLevel == 25 && OptionsPtr->getCurrentLevel() <= currentLevel)
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(3));
    else if (currentLevel == 37 && OptionsPtr->getCurrentLevel() <= currentLevel)
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(4));
    else if (currentLevel == 49 && OptionsPtr->getCurrentLevel() <= currentLevel)
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(5));
    else if (currentLevel == 61/* && OptionsPtr->getCurrentLevel() <= currentLevel*/)
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(6));
    else if (currentLevel == 73 && OptionsPtr->getCurrentLevel() <= currentLevel)
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(7));
    else if (currentLevel == 85 && OptionsPtr->getCurrentLevel() <= currentLevel)
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(8));
    else
        CCDirector::sharedDirector()->replaceScene(GameScene::scene(currentLevel));
	lock = false;
}

void MapMenuLayer::booster_1_Callback(CCObject* pSender)
{
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    booster_1->stopAllActions();
    booster_1->setScale(0.7f);
	booster_1->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    if (OptionsPtr->getCrystalCOunt() > 0)
    {
        if (GlobalsPtr->booster_1)
        {
            if (booster1Check)
            {
                booster1Check->removeFromParentAndCleanup(true);
                booster1Check = NULL;
            }
            GlobalsPtr->booster_1 = false;
        }
        else
        {
            booster1Check = CCSprite::create("okSend.png");
            boosterPlus_1->addChild(booster1Check,100);
            booster1Check->setPosition(ccp(boosterPlus_1->getContentSize().width/2.1f, boosterPlus_1->getContentSize().height/1.9f));
            booster1Check->setScale(0.71f);
            GlobalsPtr->booster_1 = true;
        }
        return;
    }
    
    menu->setEnabled(false);
    if (firstBooster == BoosterCrystal)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("COLOR_BOMB", NULL), (char*)CCLocalizedString("COLOR_BOMB_TEXT", NULL), GreenPopup, CrystalPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk1), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (firstBooster == BoosterBomb)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("STRIPPED_WRAPPED", NULL), (char*)CCLocalizedString("STRIPPED_WRAPPED_TEXT", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk2), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (firstBooster == BoosterDonut)
    {
        OptionsPtr->setDonuthCount(OptionsPtr->getDonutCount() + 1);
    }
    else if (firstBooster == BoosterFish)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("SWEET_PENGUIN", NULL), (char*)CCLocalizedString("SWEET_PENGUIN_TEXT", NULL), GreenPopup, FishPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk3), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
}

void MapMenuLayer::booster_2_Callback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    booster_2->stopAllActions();
    booster_2->setScale(0.7f);
	booster_2->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    if (OptionsPtr->getBombCount() > 0)
    {
        if (GlobalsPtr->booster_2)
        {
            if (booster2Check)
            {
                booster2Check->removeFromParentAndCleanup(true);
                booster2Check = NULL;
            }
            GlobalsPtr->booster_2 = false;
        }
        else
        {
            booster2Check = CCSprite::create("okSend.png");
            boosterPlus_2->addChild(booster2Check,100);
            booster2Check->setPosition(ccp(boosterPlus_2->getContentSize().width/2.1f, boosterPlus_2->getContentSize().height/1.9f));
            booster2Check->setScale(0.71f);
            GlobalsPtr->booster_2 = true;
        }
        return;
    }
    
    menu->setEnabled(false);
    if (secondBooster == BoosterCrystal)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("COLOR_BOMB", NULL), (char*)CCLocalizedString("COLOR_BOMB_TEXT", NULL), GreenPopup, CrystalPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk1), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (secondBooster == BoosterBomb)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("STRIPPED_WRAPPED", NULL), (char*)CCLocalizedString("STRIPPED_WRAPPED_TEXT", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk2), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (secondBooster == BoosterDonut)
    {
        OptionsPtr->setDonuthCount(OptionsPtr->getDonutCount() + 1);
    }
    else if (secondBooster == BoosterFish)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("SWEET_PENGUIN", NULL), (char*)CCLocalizedString("SWEET_PENGUIN_TEXT", NULL), GreenPopup, FishPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk3), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
}

void MapMenuLayer::booster_3_Callback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    booster_3->stopAllActions();
    booster_3->setScale(0.7f);
	booster_3->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    if (OptionsPtr->getFishCount() > 0)
    {
        if (GlobalsPtr->booster_3)
        {
            if (booster3Check)
            {
                booster3Check->removeFromParentAndCleanup(true);
                booster3Check = NULL;
            }
            GlobalsPtr->booster_3 = false;
        }
        else
        {
            booster3Check = CCSprite::create("okSend.png");
            boosterPlus_3->addChild(booster3Check,100);
            booster3Check->setPosition(ccp(boosterPlus_3->getContentSize().width/2.1f, boosterPlus_3->getContentSize().height/1.9f));
            booster3Check->setScale(0.71f);
            GlobalsPtr->booster_3 = true;
        }
        return;
    }
    
    menu->setEnabled(false);
    if (thirdBooster == BoosterCrystal)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("COLOR_BOMB", NULL), (char*)CCLocalizedString("COLOR_BOMB_TEXT", NULL), GreenPopup, CrystalPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk1), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (thirdBooster == BoosterBomb)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("STRIPPED_WRAPPED", NULL), (char*)CCLocalizedString("STRIPPED_WRAPPED_TEXT", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk2), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (thirdBooster == BoosterDonut)
    {
        OptionsPtr->setDonuthCount(OptionsPtr->getDonutCount() + 1);
    }
    else if (thirdBooster == BoosterFish)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("SWEET_PENGUIN", NULL), (char*)CCLocalizedString("SWEET_PENGUIN_TEXT", NULL), GreenPopup, FishPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk3), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
}

void MapMenuLayer::popupOk1(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.superelements");
    popaplayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(CONNECTION_TIME), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::closeLoading))));
    menu->setEnabled(false);
}

void MapMenuLayer::popupOk2(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.stripedandbomb");
    popaplayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(CONNECTION_TIME), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::closeLoading))));
    menu->setEnabled(false);
}

void MapMenuLayer::popupOk3(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.penguins");
    popaplayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(CONNECTION_TIME), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::closeLoading))));
    menu->setEnabled(false);
}

void MapMenuLayer::unclockMenu(CCNode* pSender)
{
    menu->setEnabled(true);
}

