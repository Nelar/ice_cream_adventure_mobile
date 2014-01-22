#include "MapMenuLayer.h"
#include "MainMenuScene.h"
#include "GameScene.h"
#include "cGlobal.h"
#include "cComixScene.h"
#include "MMPInterface.h"
#include "IAP.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "cFacebook.h"
#include <GUI/CCScrollView/CCScrollView.h>

#include "MMPInterface.h"
#include "MMP/Banner.h"
#include "utils.h"

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

using namespace Core;
using namespace MarketingPlatform;

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

	int fontSize = 0;
    if (IPAD)
        fontSize = 72;
    else
        fontSize = 36;

	targetTitle = CCLabelBMFont::create("Init", "fonts/Script MT Bold 22.fnt");
	targetTitle->setPosition(ccp(targetSubstrate->getContentSize().width/2.0f, targetSubstrate->getContentSize().height/2.0f));

    if (!IPAD)
        targetTitle->setScale(0.5f);

	targetSubstrate->addChild(targetTitle);

	CCLabelBMFont* boosterTitle = CCLabelBMFont::create("Select boosters", "fonts/Script MT Bold 22.fnt");
	boosterTitle->setPosition(ccp(popup->getContentSize().width/2.0f, popup->getContentSize().height/2.1f));
	ccColor3B color;
	color.r = 0x4b;
	color.g = 0xd7;
	color.b = 0x20;
	boosterTitle->setColor(color);

    if (!IPAD)
        boosterTitle->setScale(0.5f);

	popup->addChild(boosterTitle);

    if (IPAD)
        fontSize = 100;
    else
        fontSize = 50;

	levelTitle = CCLabelBMFont::create("Init", "fonts/Script MT Bold 22.fnt");
	levelTitle->setPosition(ccp(popup->getContentSize().width/2.0f, popup->getContentSize().height/1.1f));
	color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
	levelTitle->setColor(color);

    if (!IPAD)
        levelTitle->setScale(0.5f);

	popup->addChild(levelTitle);

	close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
		CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MapMenuLayer::closeCallback));
	close->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /2.5f, 
		popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + popup->getContentSize().height /2.4f);

	CCSprite* playNormal = CCSprite::createWithSpriteFrameName("common/play.png");
	CCSprite* playSelected = CCSprite::createWithSpriteFrameName("common/play_selected.png");
	play = CCMenuItemSprite::create(playNormal, 
		playSelected, this, menu_selector(MapMenuLayer::playCallback));
	play->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f, 
		popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f
		- popup->getContentSize().height /2.3f + play->getContentSize().height/2.0f);
	//play->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));


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
        livesTime = CCSprite::createWithSpriteFrameName("gameMap/full.png");
        livesTime->setPosition(ccp(livesPanel->getContentSize().width/1.6f, livesPanel->getContentSize().height/2.6f));
        livesPanel->addChild(livesTime);
    }
    else
    {
        char buf[255];
        sprintf(buf, "%0*d:%0*d", 2, (int)(1800 - second)/60, 2, (int)(1800 - second)%60);
        livesTime = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
        if (!IPAD)
            livesTime->setScale(0.5f);
        livesTime->setAnchorPoint(ccp(0.0f, 0.5f));
        livesTime->setPosition(ccp(livesPanel->getContentSize().width/2.3f, livesPanel->getContentSize().height/2.6f));
        livesPanel->addChild(livesTime);
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::timeCallback))));
    }
    
    char buf[8];
    int temp = OptionsPtr->getLifeCount();
    sprintf(buf, "%d", OptionsPtr->getLifeCount());
    
    livesCount = CCLabelBMFont::create(buf, "fonts/Script MT Bold 36.fnt");
    if (!IPAD)
        livesCount->setScale(0.5f);
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
    
    
/*    bannerMMP =  MMPInterface::Instance()->GetBanner();
    if (bannerMMP != NULL)
    {
        CCSprite* spriteMMP = bannerMMP->GetSprite();
        if (spriteMMP != NULL)
        {
            CCSprite* spriteMMPDown = CCSprite::createWithSpriteFrame(spriteMMP->displayFrame());
            spriteMMPDown->setColor(ccGRAY);
            CCMenuItemSprite* banner = CCMenuItemSprite::create(spriteMMP, spriteMMPDown, this, menu_selector(MapMenuLayer::bannerCallback));
            banner->setPosition(ccp(WINSIZE.width/2.0f - spriteMMP->getContentSize().width/2.0f, WINSIZE.height/2.0f - spriteMMP->getContentSize().height/2.0f));

            CCMenuItemSprite* closeBanner = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"), CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MapMenuLayer::closeBanner));
            closeBanner->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
        
            livesMenu->addChild(banner, 100);
            livesMenu->addChild(closeBanner, 100);
        }
    }*/

    return true;
}

void MapMenuLayer::closeBanner(CCObject* pSender)
{
    
}

void MapMenuLayer::bannerCallback(CCObject* pSender)
{
    goToLink(bannerMMP->GetLandingUrl().c_str());
}


void MapMenuLayer::addSocialLayer()
{
    social = SocialLayer::create();
    this->getParent()->addChild(social, 10);
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::checkMessage))));
}

void MapMenuLayer::checkMessage(CCNode* pSender)
{
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
        if (!FacebookPtr->messages.empty() && FacebookPtr->sessionIsOpened())
            social->showMessageboard();
    }
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
            panelLivesLayer->setScale(0.75f);
        else
            panelLivesLayer->setScale(1.0f);
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
        livesTime = CCSprite::createWithSpriteFrameName("gameMap/full.png");
        livesTime->setPosition(ccp(livesPanel->getContentSize().width/1.6f, livesPanel->getContentSize().height/2.6f));
        livesPanel->addChild(livesTime);
    }
    else
    {
        char buf[255];
        sprintf(buf, "%0*d:%0*d", 2, (int)(1800 - second)/60, 2, (int)(1800 - second)%60);
        livesTime = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
        if (!IPAD)
            livesTime->setScale(0.5f);
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
    
    livesCount = CCLabelBMFont::create(buf, "fonts/Script MT Bold 36.fnt");
    if (!IPAD)
        livesCount->setScale(0.5f);
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
    if (levelNumForPost == 25 && !OptionsPtr->getJubPost())
    {
        popaplayer->popupPost("Post on wall", "Post on wall and get a bonus", "Post", GreenPopup, BombPopBoot,
                              this, callfuncN_selector(MapMenuLayer::addBonus), this, callfuncN_selector(MapMenuLayer::unclockMenu));
        menu->setEnabled(false);
    }
    
    
    if (currentLevel == OptionsPtr->getCurrentLevel())
    {
        if (!(currentLevel%8))
            popaplayer->popupPost("Post on wall", "Post on wall and get a bonus", "Post", GreenPopup, BombPopBoot,
                                  this, callfuncN_selector(MapMenuLayer::unlockStage), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    lock = true;
}

void MapMenuLayer::unlockStage(CCNode* pSender)
{
    FacebookPtr->unlockStage();
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
    
    CCLabelBMFont* heartLabel;
    if (OptionsPtr->getLifeCount() > 0)
        heartLabel = CCLabelBMFont::create("Get more lives!", "fonts/Script MT Bold 22.fnt");
    else
        heartLabel = CCLabelBMFont::create("No more lives!", "fonts/Script MT Bold 22.fnt");
    
    heartLabel->setPosition(ccp(panelLivesLayer->getContentSize().width/2.0f, 8.0f*panelLivesLayer->getContentSize().height/9.0f));

    if (!IPAD)
        heartLabel->setScale(0.5f);
    
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
    
    askFriend = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("gameMap/askFriend.png"),
                                     CCSprite::createWithSpriteFrameName("gameMap/askFriendDown.png"), this, menu_selector(MapMenuLayer::askFriendCallback));
    askFriend->setPosition(ccp(panelLivesLayer->getContentSize().width/2.0f, panelLivesLayer->getContentSize().height/6.0f));
    heartMenu->addChild(askFriend);
    
    moreLives = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("gameMap/moreLives.png"),
                                                           CCSprite::createWithSpriteFrameName("gameMap/moreLivesDown.png"), this, menu_selector(MapMenuLayer::moreLivesCallback));
    moreLives->setPosition(ccp(panelLivesLayer->getContentSize().width/2.0f, panelLivesLayer->getContentSize().height/6.0f + askFriend->getContentSize().height));
    heartMenu->addChild(moreLives);
    
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
    
    
    
    CCLabelBMFont* labelNotif = CCLabelBMFont::create("Time to next life:", "fonts/Script MT Bold 22.fnt");
	labelNotif->setPosition(ccp(panelLivesLayer->getContentSize().width / 2.0f, panelLivesLayer->getContentSize().height / 1.75f));
	panelLivesLayer->addChild(labelNotif);
	livesLayer->setOpacity(0);
    ccColor3B color;
    color.r = 0x91;
    color.g = 0x3b;
    color.b = 0x55;
	labelNotif->setColor(color);
    
    if (!IPAD)
        labelNotif->setScale(0.5f);
    
    if (OptionsPtr->getLifeCount() >= 5)
        labelNotif->setVisible(false);
    else
        labelNotif->setVisible(true);
    
    
    livesTimePopap = CCLabelBMFont::create("  ", "fonts/Script MT Bold 22.fnt");
    if (!IPAD)
        livesTimePopap->setScale(0.5f);
    livesTimePopap->setAnchorPoint(ccp(0.0f, 0.5f));
    livesTimePopap->setPosition(ccp(panelLivesLayer->getContentSize().width / 2.4f, panelLivesLayer->getContentSize().height / 2.0f));
    panelLivesLayer->addChild(livesTimePopap);
    
    
    notif_1_Button = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/off.png"),
                                                                CCSprite::createWithSpriteFrameName("common/on.png"), this, menu_selector(MapMenuLayer::notif_1_Callback));
	notif_1_Button->setPosition(ccp(panelLivesLayer->getContentSize().width/2.0f, panelLivesLayer->getContentSize().height/2.5f));
    notif_1_Button->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
	heartMenu->addChild(notif_1_Button);
	notif_1_Button->setOpacity(0);
	notif_1_Button->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.2f), CCFadeIn::create(0.2f)));
    if (OptionsPtr->isNotification())
		notif_1_Button->setNormalImage(CCSprite::createWithSpriteFrameName("common/on.png"));
	else
		notif_1_Button->setNormalImage(CCSprite::createWithSpriteFrameName("common/off.png"));
    
    if (OptionsPtr->getLifeCount() > 0)
        notif_1_Button->setVisible(false);
    else
        notif_1_Button->setVisible(true);
    
    
    if (LANDSCAPE)
        panelLivesLayer->setScale(0.75f);
    else
        panelLivesLayer->setScale(1.0f);
}

void MapMenuLayer::notif_1_Callback(CCObject* pSender)
{
	if (OptionsPtr->isNotification())
	{
        OptionsPtr->setNotification(false);
		notif_1_Button->setNormalImage(CCSprite::createWithSpriteFrameName("common/off.png"));
        removeAllNotification();
	}
	else
	{
        OptionsPtr->setNotification(true);
		notif_1_Button->setNormalImage(CCSprite::createWithSpriteFrameName("common/on.png"));
	}
    OptionsPtr->save();
}

void MapMenuLayer::askFriendCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
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
        isPopup = true;
        if (FacebookPtr->sessionIsOpened())
        {
            if (FacebookPtr->friendsScores.size() > 0)
                social->showAskMessageboard();
            else
                FacebookPtr->inviteFriends();
        }
    }
}

void MapMenuLayer::moreLivesCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.5lives");
    closeHeartCallback(NULL);
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

CCLayer* MapMenuLayer::createScoreLayer(int level)
{
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    
    CCScrollView* scoreView;
    if (LANDSCAPE)
    {
        scoreView = CCScrollView::create(CCSize(facebookPanel->getContentSize().height, facebookPanel->getContentSize().width));
        scoreView->setPosition(ccp(facebookPanel->getContentSize().height/2.0f, facebookPanel->getContentSize().width/2.0f));
        scoreView->setDirection(kCCScrollViewDirectionVertical);
        scoreView->setPosition(ccp(scoreView->getPosition().x, scoreView->getPosition().y - scoreView->getContentSize().height));
    }
    else
    {
        scoreView = CCScrollView::create(facebookPanel->getContentSize());
        scoreView->setPosition(ccp(0.0f, facebookPanel->getContentSize().height/10.0f));
        scoreView->setDirection(kCCScrollViewDirectionHorizontal);
    }
    
    scoreView->setBounceable(true);
    
    int multiplier = 1.0f;
    
    if (!IPAD)
        multiplier = 0.5f;
    
    float offsetNode = facebookPanel->getContentSize().height/5.0f;
    
    vector<int> sortingArr;
    sortingArr.push_back(FacebookPtr->myScores[level]);
    for (int i = 0; i < FacebookPtr->friendsScores.size(); i++)
        sortingArr.push_back(FacebookPtr->friendsScores[i].scores[level]);
    
    
    std::sort(sortingArr.begin(), sortingArr.end());
    
    vector<int> temp = FacebookPtr->myScores;
    
    if (FacebookPtr->myScores[level] != 0)
    {
        CCLayerColor* node = CCLayerColor::create(ccc4(255, 0, 0, 0));
        node->setContentSize(CCSize(facebookPanel->getContentSize().height*0.8f, facebookPanel->getContentSize().height*0.8f));
        
        string str = FacebookPtr->getAvatar();
        CCSprite* avatar = CCSprite::create(str.c_str());
        node->addChild(avatar);
        float avatarWidth = 128.0f*multiplier;
        avatar->setScaleX(avatarWidth/avatar->getContentSize().width);
        avatar->setScaleY(avatarWidth/avatar->getContentSize().height);
        avatar->setPosition(ccp(node->getContentSize().height*0.3f, node->getContentSize().height*0.6f));
        CCSprite* border = CCSprite::create("avatarBorder.png");
        border->setScaleX(((avatarWidth + 15)/border->getContentSize().width)/avatar->getScaleX());
        border->setScaleY(((avatarWidth + 15)/border->getContentSize().height)/avatar->getScaleY());
        border->setPosition(ccp(avatar->getContentSize().width/2.0f, avatar->getContentSize().height/2.0f));
        avatar->addChild(border);
        
        char buf[255];
        sprintf(buf, "%s\n%d", FacebookPtr->name.c_str(), FacebookPtr->myScores[level]);
        CCLabelTTF* name = CCLabelTTF::create(buf, "Arial", 50);
        name->setPosition(ccp(node->getContentSize().width/1.9f, node->getContentSize().height/5.0f));
        name->setColor(ccBLACK);
        name->setScale(multiplier);
        node->addChild(name);
        
        int place = 0;
        for (int i = 0; i < sortingArr.size(); i++)
        {
            if (sortingArr[i] == FacebookPtr->myScores[level])
                place = sortingArr.size() - i - 1;
        }
        sprintf(buf, "%d", place + 1);
        CCLabelBMFont* position = CCLabelBMFont::create(buf, "fonts/Script MT Bold 36.fnt");
        position->setColor(color);
        position->setPosition(ccp(node->getContentSize().height*0.2f, node->getContentSize().height/5.0f));
        if(!IPAD)
            position->setScale(0.5f);
        node->addChild(position);

        
        if (LANDSCAPE)
        {
            scoreView->addChild(node);
            node->setAnchorPoint(ccp(0.0f, 0.0f));
            node->setPosition(ccp(0.0f, scoreView->getContentSize().height - offsetNode - node->getContentSize().width*0.7f*place));
        }
        else
        {
            scoreView->addChild(node);
            node->setAnchorPoint(ccp(0.0f, 0.0f));
            node->setPosition(ccp(offsetNode + node->getContentSize().width*0.7f*place, 0.0f));
        }
        
    }
    
    for (int i = 0; i < FacebookPtr->friendsScores.size(); i++)
    {
        if (FacebookPtr->friendsScores[i].scores[level] == 0)
            continue;
        
        CCLayerColor* node = CCLayerColor::create(ccc4(255, 0, 0, 0));
        node->setContentSize(CCSize(facebookPanel->getContentSize().height*0.8f, facebookPanel->getContentSize().height*0.8f));
        
        string str = FacebookPtr->getWorkDirectory() + "/" + FacebookPtr->friendsScores[i].uid + ".png";
        CCSprite* avatar = CCSprite::create(str.c_str());
        node->addChild(avatar);
        float avatarWidth = 128.0f*multiplier;
        avatar->setScaleX(avatarWidth/avatar->getContentSize().width);
        avatar->setScaleY(avatarWidth/avatar->getContentSize().height);
        avatar->setPosition(ccp(node->getContentSize().height*0.3f, node->getContentSize().height*0.6f));
        CCSprite* border = CCSprite::create("avatarBorder.png");
        border->setScaleX(((avatarWidth + 15)/border->getContentSize().width)/avatar->getScaleX());
        border->setScaleY(((avatarWidth + 15)/border->getContentSize().height)/avatar->getScaleY());
        border->setPosition(ccp(avatar->getContentSize().width/2.0f, avatar->getContentSize().height/2.0f));
        avatar->addChild(border);
        
        char buf[255];
        sprintf(buf, "%s\n%d", FacebookPtr->friendsScores[i].name.c_str(), FacebookPtr->friendsScores[i].scores[level]);
        CCLabelTTF* name = CCLabelTTF::create(buf, "Arial", 50);
        name->setPosition(ccp(node->getContentSize().width/1.9f, node->getContentSize().height/5.0f));
        name->setColor(ccBLACK);
        name->setScale(multiplier);
        node->addChild(name);
        
        int place = 0;
        for (int j = 0; j < sortingArr.size(); j++)
        {
            if (sortingArr[j] == FacebookPtr->friendsScores[i].scores[level])
                place = sortingArr.size() - j - 1;
        }
        sprintf(buf, "%d", place + 1);
        CCLabelBMFont* position = CCLabelBMFont::create(buf, "fonts/Script MT Bold 36.fnt");
        position->setColor(color);
        position->setPosition(ccp(node->getContentSize().height*0.2f, node->getContentSize().height/5.0f));
        if(!IPAD)
            position->setScale(0.5f);
        node->addChild(position);
        
        CCMenu* fMenu = CCMenu::create();
        node->addChild(fMenu);
        
        CCSprite* lifeSend = CCSprite::create("lifeSend.png");
        CCSprite* lifeSendDown = CCSprite::create("lifeSend.png");
        lifeSendDown->setColor(ccGRAY);
        
        CCMenuItemSprite* sendFriend = CCMenuItemSprite::create(lifeSend,
                                                                lifeSendDown, this, menu_selector(MapMenuLayer::sendLife));
        fMenu->addChild(sendFriend);
        fMenu->setPosition(ccp(0.0f, 0.0f));
        sendFriend->setPosition(ccp(node->getContentSize().height*0.7f, node->getContentSize().height*0.6f));
        
        if (LANDSCAPE)
        {
            scoreView->addChild(node);
            node->setAnchorPoint(ccp(0.0f, 0.0f));
            node->setPosition(ccp(0.0f, scoreView->getContentSize().height - offsetNode - node->getContentSize().width*0.7f*place));
        }
        else
        {
            scoreView->addChild(node);
            node->setAnchorPoint(ccp(0.0f, 0.0f));
            node->setPosition(ccp(offsetNode + node->getContentSize().width*0.7f*place, 0.0f));
        }
    }
    
  /*  if (LANDSCAPE)
    {
        scoreView->setPosition(ccp(facebookPanel->getPositionX(), facebookPanel->getPositionY()));
    }*/
    
    return scoreView;
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
}

void MapMenuLayer::updateFacebookCallback(CCNode* pSender)
{
    social->showScoreboard(currentLevel);
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
    
    CCLabelBMFont* label = CCLabelBMFont::create("High score", "fonts/Script MT Bold 22.fnt");
    ccColor3B color;
	color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    if (!IPAD)
        label->setScale(0.5f);
    

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

	sprintf(buf, "Target: %d", targetScore);
	targetTitle->setString(buf);
	targetTitle->setPosition(ccp(targetTitle->getParent()->getContentSize().width/2.0f, targetTitle->getParent()->getContentSize().height/2.0f));	
	targetTitle->setScale(0.8f);

    if (!IPAD)
        targetTitle->setScale(0.4f);


	sprintf(buf, "Level %d", levelNum);
	levelTitle->setString(buf);
    if (levelNum > 105)
    {
        levelTitle->setString("Extra Level");
    }
	levelTitle->setPosition(ccp(levelTitle->getParent()->getContentSize().width/2.0f, levelTitle->getParent()->getContentSize().height/1.1f));

    updateBoosters();
    
    if (typeLevel == Ice)
    {
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/iceStudy.png"), CCSprite::createWithSpriteFrameName("common/iceStudyDown.png"), this, menu_selector(MapMenuLayer::helpModalCallback));
        helpModal = CCSprite::createWithSpriteFrameName("common/iceStudyModal.png");
    }
    else if (typeLevel == Score)
    {
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/scoreStudy.png"), CCSprite::createWithSpriteFrameName("common/scoreStudyDown.png"), this, menu_selector(MapMenuLayer::helpModalCallback));
        helpModal = CCSprite::createWithSpriteFrameName("common/scoreStudyModal.png");
        
        CCLabelBMFont* helpLabel = CCLabelBMFont::create("15", "fonts/Script MT Bold 22.fnt");
        ccColor3B colorHelp;
        colorHelp.r = 0x29;
        colorHelp.g = 0x45;
        colorHelp.b = 0x93;
        helpLabel->setColor(colorHelp);
        
        helpModal->addChild(helpLabel);
        helpLabel->setPosition(ccp(helpModal->getContentSize().width/2.0f, helpModal->getContentSize().height/2.0f));
    }
    else if (typeLevel == BringDown)
    {
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/bringStudy.png"), CCSprite::createWithSpriteFrameName("common/bringStudyDown.png"), this, menu_selector(MapMenuLayer::helpModalCallback));
        helpModal = CCSprite::create("bringStudyModal.png");
    }
    else if (typeLevel == Time)
    {
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/timeStudy.png"), CCSprite::createWithSpriteFrameName("common/timeStudyDown.png"), this, menu_selector(MapMenuLayer::helpModalCallback));
        helpModal = CCSprite::createWithSpriteFrameName("common/timeStudyModal.png");
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
    
    social->showScoreboard(currentLevel);
}

void MapMenuLayer::updateBoosters()
{
    char buf[255];
    
    if (boosterPlus_1)
        boosterPlus_1->removeAllChildrenWithCleanup(true);
    if (boosterPlus_2)
        boosterPlus_2->removeAllChildrenWithCleanup(true);
    if (boosterPlus_3)
        boosterPlus_3->removeAllChildrenWithCleanup(true);
    
    float ipScale = 1.0f;

    float bScale = 1.0f;

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
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_1->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_1->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
	else if (firstBooster == BoosterBomb)
	{
        booster_1->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/bomb.png");
        booster->setScale(0.8f);
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/2.0f));
		booster_1->addChild(booster);
        
        if (OptionsPtr->getBombCount() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_1->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getBombCount());
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_1->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_1->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
	else if (firstBooster == BoosterFish)
	{
        booster_1->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/fish.png");
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/2.0f));
		booster_1->addChild(booster);
        
        if (OptionsPtr->getFishCount() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_1->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getFishCount());
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_1->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_1->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
    else if (firstBooster == BoosterDonut)
	{
        booster_1->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/fish.png");
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/2.0f));
		booster_1->addChild(booster);
        
        if (OptionsPtr->getDonutCount() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_1->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getDonutCount());
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_1->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_1->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
	else
	{
		booster_1->setVisible(false);
	}
    
	if (secondBooster == BoosterCrystal)
	{
        booster_2->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/crystal.png");
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/2.0f));
		booster_2->addChild(booster);
        
        if (OptionsPtr->getCrystalCOunt() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("gameMap/boosterBack.png");
            boosterPlus_2->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getCrystalCOunt());
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_2->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_2->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
	else if (secondBooster == BoosterBomb)
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
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_2->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_2->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
	else if (secondBooster == BoosterFish)
	{
        booster_2->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/fish.png");
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/2.0f));
		booster_2->addChild(booster);
        
        if (OptionsPtr->getFishCount() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_2->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getFishCount());
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_2->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_2->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
    else if (secondBooster == BoosterDonut)
	{
        booster_2->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/fish.png");
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/2.0f));
		booster_2->addChild(booster);
        
        if (OptionsPtr->getDonutCount() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_2->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getDonutCount());
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            label->setScale(ipScale);
            
            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_2->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_2->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
	else
	{
		booster_2->setVisible(false);
	}
    
	if (thirdBooster == BoosterCrystal)
	{
        booster_3->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/crystal.png");
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/2.0f));
		booster_3->addChild(booster);
        
        if (OptionsPtr->getCrystalCOunt() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_3->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getCrystalCOunt());
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_3->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_3->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
	else if (thirdBooster == BoosterBomb)
	{
        booster_3->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/bomb.png");
//        booster->setScale(0.6f);
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/2.0f));
		booster_3->addChild(booster);
        
        if (OptionsPtr->getBombCount() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_3->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getBombCount());
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            label->setScale(ipScale);
            
            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_3->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_3->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
	else if (thirdBooster == BoosterFish)
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
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");
            label->setColor(ccWHITE);
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_3->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_3->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
    else if (thirdBooster == BoosterDonut)
	{
        booster_3->setVisible(true);
		CCSprite* booster = CCSprite::createWithSpriteFrameName("common/fish.png");
		booster->setPosition(ccp(booster_1->getContentSize().width/2.0f, booster_1->getContentSize().height/2.0f));
		booster_3->addChild(booster);
        
        if (OptionsPtr->getDonutCount() >0)
        {
            CCSprite* temp = CCSprite::createWithSpriteFrameName("common/boosterBack.png");
            boosterPlus_3->setDisplayFrame(temp->displayFrame());
            sprintf(buf, "%d", OptionsPtr->getDonutCount());
            CCLabelBMFont* label = CCLabelBMFont::create(buf, "fonts/Script MT Bold 22.fnt");;
            label->setColor(ccWHITE);
            label->setScale(ipScale);

            if (!IPAD)
                label->setScale(0.4f);

            boosterPlus_3->addChild(label);
            label->setPosition(ccp(boosterPlus_1->getContentSize().width/2.0f, boosterPlus_1->getContentSize().height/2.0f));
        }
        else
        {
            boosterPlus_3->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/plus.png"));
        }
	}
	else
	{
		booster_3->setVisible(false);
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
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME*1.5f), CCCallFuncN::create(this, callfuncN_selector(MapMenuLayer::playAfterLoad))));
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
    else if (currentLevel == 61 && OptionsPtr->getCurrentLevel() <= currentLevel)
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
    
    menu->setEnabled(false);
    if (firstBooster == BoosterCrystal)
    {
        popaplayer->popupBoosterInApp("Color bomb x3", "Start the game with a Color Bomb", GreenPopup, CrystalPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk1), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (firstBooster == BoosterBomb)
    {
        popaplayer->popupBoosterInApp("Striped&Wrapped x3", "Start the game with a striped and wrapped", GreenPopup, BombPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk2), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (firstBooster == BoosterDonut)
    {
        OptionsPtr->setDonuthCount(OptionsPtr->getDonutCount() + 1);
    }
    else if (firstBooster == BoosterFish)
    {
        popaplayer->popupBoosterInApp("Ice Penguin x3", "Add a penguin to the ice mix, \n wich clears three pieces of ice.", GreenPopup, FishPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk3), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
}

void MapMenuLayer::booster_2_Callback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    booster_2->stopAllActions();
    booster_2->setScale(0.7f);
	booster_2->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    menu->setEnabled(false);
    if (secondBooster == BoosterCrystal)
    {
        popaplayer->popupBoosterInApp("Color bomb x3", "Start the game with a Color Bomb", GreenPopup, CrystalPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk1), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (secondBooster == BoosterBomb)
    {
        popaplayer->popupBoosterInApp("Striped&Wrapped x3", "Start the game with a striped and wrapped", GreenPopup, BombPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk2), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (secondBooster == BoosterDonut)
    {
        OptionsPtr->setDonuthCount(OptionsPtr->getDonutCount() + 1);
    }
    else if (secondBooster == BoosterFish)
    {
        popaplayer->popupBoosterInApp("Ice penguin x3", "Add a penguin to the ice mix, \n wich clears three pieces of ice.", GreenPopup, FishPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk3), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
}

void MapMenuLayer::booster_3_Callback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    booster_3->stopAllActions();
    booster_3->setScale(0.7f);
	booster_3->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    menu->setEnabled(false);
    if (thirdBooster == BoosterCrystal)
    {
        popaplayer->popupBoosterInApp("Color bomb x3", "Start the game with a Color Bomb", GreenPopup, CrystalPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk1), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (thirdBooster == BoosterBomb)
    {
        popaplayer->popupBoosterInApp("Striped&Wrapped x3", "Start the game with a striped and wrapped", GreenPopup, BombPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk2), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
    else if (thirdBooster == BoosterDonut)
    {
        OptionsPtr->setDonuthCount(OptionsPtr->getDonutCount() + 1);
    }
    else if (thirdBooster == BoosterFish)
    {
        popaplayer->popupBoosterInApp("Ice penguin x3", "Add a penguin to the ice mix, \n wich clears three pieces of ice.", GreenPopup, FishPopBoot, this, callfuncN_selector(MapMenuLayer::popupOk3), this, callfuncN_selector(MapMenuLayer::unclockMenu));
    }
}

void MapMenuLayer::popupOk1(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.superelements");
}

void MapMenuLayer::popupOk2(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.stripedandbomb");
}

void MapMenuLayer::popupOk3(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.penguins");
}

void MapMenuLayer::unclockMenu(CCNode* pSender)
{
    menu->setEnabled(true);
}

