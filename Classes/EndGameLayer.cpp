#include "EndGameLayer.h"
#include "MainMenuScene.h"
#include "GameScene.h"
#include "cGlobal.h"
#include "IAP.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "cFacebook.h"
#include <GUI/CCScrollView/CCScrollView.h>
#include "GameMapLayer.h"
#include "cComixScene.h"

#include "utils.h"

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

using namespace cocos2d;
using namespace extension;

EndGameLayer::~EndGameLayer()
{
    this->stopAllActions();
    this->unscheduleAllSelectors();
    this->removeAllChildrenWithCleanup(true);
}

bool EndGameLayer::init()
{
    this->setTouchEnabled(true);
    
	if (!CCLayer::init())
		return false;
    

    if (IPHONE_4 || IPHONE_5)
        this->setScale(0.88f);
    else
        this->setScale(1.0f);
    
	menu = CCMenu::create();
	this->addChild(menu, 12);
    
    popaplayer = PopupLayer::create();
    this->addChild(popaplayer, 100);
    
	lock = false;
    isPopup = false;
    
	
    if (!IPAD)
        popup = CCSprite::createWithSpriteFrameName("common/plashka.png");
    else
        popup = CCSprite::create("plashka.png");
    
	
    
    if (LANDSCAPE)
        popup->setPosition(ccp(popup->getContentSize().width/2.0f, WINSIZE.height/2.0f));
    else
        popup->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height - popup->getContentSize().height/2.0f*this->getScale()));
    
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
    
	targetSubstrate = CCSprite::createWithSpriteFrameName("common/plashka_score_target.png");
	targetSubstrate->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.8f));
	popup->addChild(targetSubstrate);
    
	int fontSize = 0;
    if (IPAD)
        fontSize = 72;
    else
        fontSize = 36;
    
	targetTitle = CCLabelTTF::create("Init", FONT_COMMON, FONT_SIZE_48);
	targetTitle->setPosition(ccp(targetSubstrate->getContentSize().width/2.0f, targetSubstrate->getContentSize().height/2.0f));
    
	targetSubstrate->addChild(targetTitle);
    
	boosterTitle = CCLabelTTF::create(CCLocalizedString("SELECT_BOOSTER"), FONT_COMMON, FONT_SIZE_64);
	boosterTitle->setPosition(ccp(popup->getContentSize().width/2.0f, popup->getContentSize().height/2.1f));
	ccColor3B color;
	color.r = 0x4b;
	color.g = 0xd7;
	color.b = 0x20;
	boosterTitle->setColor(color);
    
	popup->addChild(boosterTitle);
    
    if (IPAD)
        fontSize = 100;
    else
        fontSize = 50;
    
	levelTitle = CCLabelTTF::create("Init", FONT_COMMON, FONT_SIZE_86);
	levelTitle->setPosition(ccp(popup->getContentSize().width/2.0f, popup->getContentSize().height/1.1f));
	color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
	levelTitle->setColor(color);
    
    
	popup->addChild(levelTitle);
    
	close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"),
                                     CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(EndGameLayer::closeCallback));
	close->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /2.5f,
                       popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + popup->getContentSize().height /2.4f);
    
	CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
	play = CCMenuItemSprite::create(spriteNormal,
                                    spriteSelected, this, menu_selector(EndGameLayer::playCallback));
	play->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                      popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f
                      - popup->getContentSize().height /2.3f + play->getContentSize().height/2.0f);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("PLAY", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    play->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

    
    
	play->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
    spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    retry = CCMenuItemSprite::create(spriteNormal,
                                     spriteSelected, this, menu_selector(EndGameLayer::retryCallback));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("RETRY", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    retry->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

	retry->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f - retry->getContentSize().width/1.8f,
                       popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f
                       - popup->getContentSize().height /2.3f + play->getContentSize().height/2.0f);
    retry->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
    
    spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
	next = CCMenuItemSprite::create(spriteNormal,
                                    spriteSelected, this, menu_selector(EndGameLayer::nextCallback));
	next->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + next->getContentSize().width/1.8f,
                      popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f
                      - popup->getContentSize().height /2.3f + play->getContentSize().height/2.0f);
    next->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_64);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    next->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

    
    
    
	boosterPlus_1 = CCSprite::createWithSpriteFrameName("common/plus.png");
	boosterPlus_2 = CCSprite::createWithSpriteFrameName("common/plus.png");
	boosterPlus_3 = CCSprite::createWithSpriteFrameName("common/plus.png");
    
	booster_1 = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/booster_normal.png"),
                                         CCSprite::createWithSpriteFrameName("common/booster_selected.png"), this, menu_selector(EndGameLayer::booster_1_Callback));
	booster_1->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f - popup->getContentSize().width /5.0f,
                           popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
	boosterPlus_1->setPosition(ccp(booster_1->getContentSize().width / 1.2f, booster_1->getContentSize().height / 4.0f));
	booster_1->addChild(boosterPlus_1,2);
	booster_1->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
	booster_2 = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/booster_normal.png"),
                                         CCSprite::createWithSpriteFrameName("common/booster_selected.png"), this, menu_selector(EndGameLayer::booster_2_Callback));
	booster_2->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                           popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
	boosterPlus_2->setPosition(ccp(booster_1->getContentSize().width / 1.2f, booster_1->getContentSize().height / 4.0f));
	booster_2->addChild(boosterPlus_2,2);
	booster_2->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
	booster_3 = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/booster_normal.png"),
                                         CCSprite::createWithSpriteFrameName("common/booster_selected.png"), this, menu_selector(EndGameLayer::booster_3_Callback));
	booster_3->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /5.0f,
                           popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
	boosterPlus_3->setPosition(ccp(booster_1->getContentSize().width / 1.2f, booster_1->getContentSize().height / 4.0f));
	booster_3->addChild(boosterPlus_3,2);
	booster_3->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f))));
    
    guiWin = CCSkeletonAnimation::createWithFile("boyHappy/boy happy.json", "boyHappy/boy happy.atlas");
	guiWin->setPosition(ccp(popup->getContentSize().width /5.5f, popup->getContentSize().height / 4.8f));
    guiWin->setAnimation("stay happy", true);
    popup->addChild(guiWin);
    if (!IPAD)
        guiWin->setScale(0.5f);
    
    guiFailed = CCSkeletonAnimation::createWithFile("boySad/boy sad.json", "boySad/boy sad.atlas");
    guiFailed->setPosition(ccp(popup->getContentSize().width / 5.0f, popup->getContentSize().height / 4.8f));
    guiFailed->setAnimation("stay sad", true);
    popup->addChild(guiFailed);
    if (!IPAD)
        guiFailed->setScale(0.5f);
    
    failed = CCLabelTTF::create(CCLocalizedString("FAILED"), FONT_COMMON, FONT_SIZE_140);
	failed->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 1.35f));
    failed->setColor(IceCreamPink);
	popup->addChild(failed);
    
    scoreTitle = CCLabelTTF::create("w", FONT_COMMON, FONT_SIZE_64);
	color.r = 0x4b;
	color.g = 0xd7;
	color.b = 0x20;
	scoreTitle->setColor(ccWHITE);

	scoreTitle->setPosition(ccp(popup->getContentSize().width / 2.0f, popup->getContentSize().height / 3.2f));
	popup->addChild(scoreTitle);
    
    targetPopupTitle = CCLabelTTF::create("1", FONT_COMMON, FONT_SIZE_64);
    targetPopupTitle->setPosition(ccp(targetSubstrate->getPosition().x + targetSubstrate->getContentSize().width/6.0f, targetSubstrate->getPosition().y));
    popup->addChild(targetPopupTitle);

    
	menu->addChild(close);
	menu->addChild(play);
    menu->addChild(next);
    menu->addChild(retry);
	menu->addChild(booster_1);
	menu->addChild(booster_2);
	menu->addChild(booster_3);
    
	this->addChild(popup,11);
    
    if (LANDSCAPE)
    {
        popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() - WINSIZE.height));
        menu->setPosition(ccp(menu->getPositionX(), menu->getPositionY() - WINSIZE.height));
    }
    else
    {
        popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() + WINSIZE.height));
        menu->setPosition(ccp(menu->getPositionX(), menu->getPositionY() + WINSIZE.height));
    }
    
    help = NULL;
    helpModal = NULL;
    
    return true;
}

void EndGameLayer::addSocialLayer()
{
    social = SocialLayer::create();
    this->getParent()->addChild(social, 10);
}

void EndGameLayer::changeOrientation()
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
    }
    else
    {
        popup->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height - popup->getContentSize().height/2.0f*this->getScale()));
    }
    
    if (!lock)
    {
        menu->setVisible(false);
        popup->setVisible(false);
        
        if (LANDSCAPE)
            popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() - WINSIZE.height));
        else
            popup->setPosition(ccp(popup->getPositionX(), popup->getPositionY() + WINSIZE.height));
    }
    
    close->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /2.5f,
                       popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f + popup->getContentSize().height /2.4f);
    
    play->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                      popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f
                      - popup->getContentSize().height /2.3f + play->getContentSize().height/2.0f);
    retry->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f - retry->getContentSize().width/1.8f,
                       popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f
                       - popup->getContentSize().height /2.3f + play->getContentSize().height/2.0f);
    next->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + next->getContentSize().width/1.8f,
                      popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f
                      - popup->getContentSize().height /2.3f + play->getContentSize().height/2.0f);
    
    if (!startLeft->isVisible())
        retry->setPosition(play->getPosition());
    
    
    booster_1->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f - popup->getContentSize().width /5.0f,
                           popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
    booster_2->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f,
                           popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
    
    booster_3->setPosition(popup->getPosition().x - CCDirector::sharedDirector()->getWinSize().width/2.0f + popup->getContentSize().width /5.0f,
                           popup->getPosition().y - CCDirector::sharedDirector()->getWinSize().height/2.0f - popup->getContentSize().height /6.5f);
    
    
    if (lock && popup->isVisible())
    {
        if (help)
            help->setPosition(play->getPositionX() - play->getContentSize().width, play->getPositionY());
        if (helpModal)
            helpModal->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, CCDirector::sharedDirector()->getWinSize().height/1.5f));
    }
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            this->setScale(0.88f);
            social->setScale(1.0f);
        }
        else
        {
            this->setScale(0.8f);
            social->setScale(0.91f);
        }
    }
    
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
            spriteLoading->setScale(1.0f/this->getScale());
        }
        
        labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
        labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
        this->addChild(labelLoad,1001);
        spriteLoading->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
        this->addChild(spriteLoading, 1000);
    }
}

void EndGameLayer::closeEnded(CCNode* sender)
{
    menu->setVisible(false);
    popup->setVisible(false);
}


void EndGameLayer::levelPopup(int levelNum, int starCount, int targetScore, eLevelType typeLevel, eBoosterType first, eBoosterType second, eBoosterType third)
{
    //changeOrientation();
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
        popup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, WINSIZE.height))));
        menu->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, WINSIZE.height))));
    }
    else
    {
        popup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, -WINSIZE.height))));
        menu->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, -WINSIZE.height))));
    }
    

    
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("HIGH_SCORE"), FONT_COMMON, FONT_SIZE_64);
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
        close->setVisible(false);
    }

	levelTitle->setPosition(ccp(levelTitle->getParent()->getContentSize().width/2.0f, levelTitle->getParent()->getContentSize().height/1.1f));
    
    updateBoosters();
    
    if (typeLevel == Ice)
    {
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/iceStudy.png"), CCSprite::createWithSpriteFrameName("common/iceStudyDown.png"), this, menu_selector(EndGameLayer::helpModalCallback));
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
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/scoreStudy.png"), CCSprite::createWithSpriteFrameName("common/scoreStudyDown.png"), this, menu_selector(EndGameLayer::helpModalCallback));
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
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/bringStudy.png"), CCSprite::createWithSpriteFrameName("common/bringStudyDown.png"), this, menu_selector(EndGameLayer::helpModalCallback));
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
        help = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/timeStudy.png"), CCSprite::createWithSpriteFrameName("common/timeStudyDown.png"), this, menu_selector(EndGameLayer::helpModalCallback));
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
    
    social->showScoreboard(currentLevel);
    
    startLeft->setVisible(true);
	startRight->setVisible(true);
	startCenter->setVisible(true);
    
    boosterTitle->setVisible(true);
    booster_1->setVisible(true);
    booster_2->setVisible(true);
    booster_3->setVisible(true);
    targetTitle->setVisible(true);
    
    play->setVisible(true);
    retry->setVisible(false);
    next->setVisible(false);
    
    guiWin->setVisible(false);
    guiFailed->setVisible(false);
    targetSubstrate->setScale(1.0f);
    scoreTitle->setVisible(false);
    failed->setVisible(false);
    targetPopupTitle->setVisible(false);
    
    play->stopAllActions();
    play->setScale(0.7f);
	play->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));

    close->stopAllActions();
    close->setScale(0.7f);
	close->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
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
    
    if (typeLevel == Score)
        booster_3->setVisible(false);
}

void EndGameLayer::postOnWall(CCNode* pSender)
{
    if (currentLevel == (OptionsPtr->getCurrentLevel() - 1))
    {
        if (currentLevel == 2 || currentLevel == 3 || currentLevel == 4 || currentLevel == 6)
        {
            if (currentLevel == 6)
            {
                if (!OptionsPtr->getTutorialPost() && FacebookPtr->sessionIsOpened())
                {
                    popaplayer->popupPost((char*)CCLocalizedString("POST_ON_WALL", NULL), (char*)CCLocalizedString("POST_ON_WALL_TEXT", NULL), (char*)CCLocalizedString("POST_ON_WALL", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(EndGameLayer::endTutorial), this, callfuncN_selector(EndGameLayer::unclockMenu));
                    menu->setEnabled(false);
                }
            }
        }
        
        if (!(currentLevel%12) && FacebookPtr->sessionIsOpened())
        {
            popaplayer->popupPost((char*)CCLocalizedString("POST_ON_WALL", NULL), (char*)CCLocalizedString("POST_ON_WALL_TEXT", NULL), (char*)CCLocalizedString("POST_ON_WALL", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(EndGameLayer::endStage), this, callfuncN_selector(EndGameLayer::unclockMenu));
            menu->setEnabled(false);
        }
    }
}

void EndGameLayer::popupWin(int countStart, int countScore, int currentL)
{
    isWin = true;
    currentLevel = currentL;

    isPopup = true;
    
    if (OptionsPtr->getLifeCount() < 5)
    {
        OptionsPtr->setLifeCount(OptionsPtr->getLifeCount() + 1);
        OptionsPtr->save();
    }
    
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
    
    
    SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sound/outro.mp3", true);
    
    FacebookPtr->setPointsInLevel(countScore, currentLevel);
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::postOnWall))));
    
    if (currentLevel == OptionsPtr->getCurrentLevel())
    {
        FacebookPtr->completeLevel(currentLevel);
        if (currentLevel == 2 || currentLevel == 3 || currentLevel == 4 || currentLevel == 6)
        {
            if (currentLevel == 6)
            {
             //   FacebookPtr->completeTutorial(" ");
            }
        }
        
        switch (currentLevel) {
            case 12:
                FacebookPtr->completeStage(CCLocalizedString("VALLEY"));
                break;
            case 24:
                FacebookPtr->completeStage(CCLocalizedString("FOREST"));
                break;
            case 36:
                FacebookPtr->completeStage(CCLocalizedString("MOUNTAIN"));
                break;
            case 48:
                FacebookPtr->completeStage(CCLocalizedString("SEA"));
                break;
            case 60:
                FacebookPtr->completeStage(CCLocalizedString("MEADOWS"));
                break;
            case 72:
                FacebookPtr->completeStage(CCLocalizedString("FARM"));
                break;
            case 84:
                FacebookPtr->completeStage(CCLocalizedString("CITY"));
                break;
                
            default:
                break;
        }
    }

   // changeOrientation();
	char buf[255];
    
    lock = true;
    
    isPopup = true;
    
    popup->setVisible(true);
	menu->setVisible(true);
    
    if (LANDSCAPE)
    {
        popup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, WINSIZE.height))));
        menu->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, WINSIZE.height))));
    }
    else
    {
        popup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, -WINSIZE.height))));
        menu->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, -WINSIZE.height))));
    }
    
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("HIGH_SCORE"), FONT_COMMON, FONT_SIZE_64);
    ccColor3B color;
	color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);

    
    boosterTitle->setVisible(false);
    booster_1->setVisible(false);
    booster_2->setVisible(false);
    booster_3->setVisible(false);
    
	if (countStart == 1)
	{
		CCSprite* temp =  CCSprite::createWithSpriteFrameName("common/star_popup_disabled.png");
		temp->setPosition(startLeft->getPosition());
		popup->addChild(temp, 1);
		temp->setRotation(startLeft->getRotation());
        CCSprite* star = CCSprite::createWithSpriteFrameName("common/star.png");
		startLeft->setDisplayFrame(star->displayFrame());
        
		startLeft->setScale(6.0f);
		startLeft->setOpacity(0);
		startLeft->runAction(CCSequence::create(CCDelayTime::create(0.5f), CCSpawn::create(CCScaleTo::create(0.5f, 1.0f), CCRotateBy::create(0.5f,720), CCFadeIn::create(0.5f), NULL), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::explosionCallback)), NULL));
        
        startLeft->setTag(1);
        temp->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCFadeOut::create(0.01f)));
	}
	else if (countStart == 2)
	{
		CCSprite* temp =  CCSprite::createWithSpriteFrameName("common/star_popup_disabled.png");
		temp->setPosition(startLeft->getPosition());
		popup->addChild(temp, 1);
		temp->setRotation(startLeft->getRotation());
        
		CCSprite* temp2 =  CCSprite::createWithSpriteFrameName("common/star_popup_disabled.png");
		temp2->setPosition(startCenter->getPosition());
		popup->addChild(temp2, 1);
		temp2->setRotation(startCenter->getRotation());
        
        CCSprite* star = CCSprite::createWithSpriteFrameName("common/star.png");
        CCSprite* star2 = CCSprite::createWithSpriteFrameName("common/star2.png");
		startLeft->setDisplayFrame(star->displayFrame());
		startCenter->setDisplayFrame(star2->displayFrame());
        
        startLeft->setTag(1);
        startCenter->setTag(2);
        
		startLeft->setScale(6.0f);
		startCenter->setScale(6.0f);
		startLeft->setOpacity(0);
		startCenter->setOpacity(0);
		startLeft->runAction(CCSequence::create(CCDelayTime::create(0.5f), CCSpawn::create(CCScaleTo::create(0.5f, 1.0f), CCRotateBy::create(0.5f,720), CCFadeIn::create(0.5f), NULL), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::explosionCallback)), NULL));
		startCenter->runAction(CCSequence::create(CCDelayTime::create(1.5f), CCSpawn::create(CCScaleTo::create(0.5f, 1.0f), CCRotateBy::create(0.5f,720), CCFadeIn::create(0.5f), NULL), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::explosionCallback)), NULL));
        
        temp->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCFadeOut::create(0.01f)));
        temp2->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCFadeOut::create(0.01f)));
	}
	else if (countStart >= 3)
	{
		CCSprite* temp =  CCSprite::createWithSpriteFrameName("common/star_popup_disabled.png");
		temp->setPosition(startLeft->getPosition());
		popup->addChild(temp, 1);
		temp->setRotation(startLeft->getRotation());
        
		CCSprite* temp2 =  CCSprite::createWithSpriteFrameName("common/star_popup_disabled.png");
		temp2->setPosition(startCenter->getPosition());
		popup->addChild(temp2, 1);
		temp2->setRotation(startCenter->getRotation());
        
		CCSprite* temp3 =  CCSprite::createWithSpriteFrameName("common/star_popup_disabled.png");
		temp3->setPosition(startRight->getPosition());
		popup->addChild(temp3, 1);
		temp3->setRotation(startRight->getRotation());
        
        CCSprite* star = CCSprite::createWithSpriteFrameName("common/star.png");
        CCSprite* star2 = CCSprite::createWithSpriteFrameName("common/star2.png");
        CCSprite* star3 = CCSprite::createWithSpriteFrameName("common/star3.png");
        
        
		startLeft->setDisplayFrame(star->displayFrame());
		startCenter->setDisplayFrame(star2->displayFrame());
		startRight->setDisplayFrame(star3->displayFrame());
        
		startLeft->setScale(6.0f);
		startCenter->setScale(6.0f);
		startRight->setScale(6.0f);
		startLeft->setOpacity(0);
		startCenter->setOpacity(0);
		startRight->setOpacity(0);
        
        startLeft->setTag(1);
        startCenter->setTag(2);
        startRight->setTag(3);
        
		startLeft->runAction(CCSequence::create(CCDelayTime::create(0.5f), CCSpawn::create(CCScaleTo::create(0.5f, 1.0f), CCRotateBy::create(0.5f,720), CCFadeIn::create(0.5f), NULL), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::explosionCallback)), NULL));
		startCenter->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCSpawn::create(CCScaleTo::create(0.5f, 1.0f), CCRotateBy::create(0.5f,720), CCFadeIn::create(0.5f), NULL), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::explosionCallback)), NULL));
		startRight->runAction(CCSequence::create(CCDelayTime::create(1.5f), CCSpawn::create(CCScaleTo::create(0.5f, 1.0f), CCRotateBy::create(0.5f,720), CCFadeIn::create(0.5f), NULL), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::explosionCallback)), NULL));
        
        temp->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCFadeOut::create(0.01f)));
        temp2->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCFadeOut::create(0.01f)));
        temp3->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeOut::create(0.01f)));
	}
	else
	{
		startLeft->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/star_popup_disabled.png"));
		startCenter->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/star_popup_disabled.png"));
		startRight->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("common/star_popup_disabled.png"));
	}
    
    play->setVisible(false);
    retry->setVisible(true);
    next->setVisible(true);
    
    guiWin->setVisible(true);
    guiFailed->setVisible(false);
    targetSubstrate->setScale(1.0f);
    scoreTitle->setVisible(false);
    failed->setVisible(false);
    targetPopupTitle->setVisible(false);
    
	sprintf(buf, "%s: %d", CCLocalizedString("SCORE"), countScore);
	targetTitle->setString(buf);
	targetTitle->setPosition(ccp(targetTitle->getParent()->getContentSize().width/2.0f, targetTitle->getParent()->getContentSize().height/2.0f));
    
    
	sprintf(buf, "%s %d", CCLocalizedString("LEVEL"), currentLevel);
	levelTitle->setString(buf);
    if (currentLevel > 105)
    {
        levelTitle->setString(CCLocalizedString("EXTRA_LEVEL", NULL));
        levelTitle->setFontSize(FONT_SIZE_48);
        close->setOpacity(0);
        close->setEnabled(false);
    }
	levelTitle->setPosition(ccp(levelTitle->getParent()->getContentSize().width/2.0f, levelTitle->getParent()->getContentSize().height/1.1f));
    
    if (currentLevel == 106 && OptionsPtr->getCurrentLevel() < 35)
        OptionsPtr->setCurrentLevel(35);
    else if (currentLevel == 107)
        OptionsPtr->setLevelData(24, OptionsPtr->getLevelData(24).countStar, OptionsPtr->getLevelData(24).countScore, OptionsPtr->getLevelData(24).levelType, false);
    else if (currentLevel == 108)
        OptionsPtr->setLevelData(36, OptionsPtr->getLevelData(36).countStar, OptionsPtr->getLevelData(36).countScore, OptionsPtr->getLevelData(36).levelType, false);
    else if (currentLevel == 109)
        OptionsPtr->setLevelData(48, OptionsPtr->getLevelData(48).countStar, OptionsPtr->getLevelData(48).countScore, OptionsPtr->getLevelData(48).levelType, false);
    else if (currentLevel == 110)
        OptionsPtr->setLevelData(60, OptionsPtr->getLevelData(60).countStar, OptionsPtr->getLevelData(60).countScore, OptionsPtr->getLevelData(60).levelType, false);
    else if (currentLevel == 111)
        OptionsPtr->setLevelData(72, OptionsPtr->getLevelData(72).countStar, OptionsPtr->getLevelData(72).countScore, OptionsPtr->getLevelData(72).levelType, false);
	else if (currentLevel == OptionsPtr->getCurrentLevel())
		OptionsPtr->setCurrentLevel(currentLevel + 1);
    
	int cStar;
	int cScore;;
    if (currentLevel > 105)
    {
        cStar = OptionsPtr->getLevelData(currentLevel).countStar;
        cScore = OptionsPtr->getLevelData(currentLevel).countScore;
    }
    else
    {
        cStar = OptionsPtr->getLevelData(currentLevel - 1).countStar;
        cScore = OptionsPtr->getLevelData(currentLevel - 1).countScore;
    }
        
	if (cStar < countStart)
		cStar = countStart;
	if (cScore < countScore)
		cScore = countScore;
    
    if (currentLevel > 105)
        OptionsPtr->setLevelData(currentLevel, cStar, cScore, OptionsPtr->getLevelData(currentLevel).levelType);
    else
        OptionsPtr->setLevelData(currentLevel - 1, cStar, cScore, OptionsPtr->getLevelData(currentLevel - 1).levelType);
    
	OptionsPtr->save();
    
    retry->stopAllActions();
    retry->setScale(0.7f);
	retry->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    next->stopAllActions();
    next->setScale(0.7f);
	next->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    close->stopAllActions();
    close->setScale(0.7f);
	close->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    close->stopAllActions();
    close->setScale(0.7f);
	close->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    menu->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::finishingEnded))));
    
    social->showScoreboard(currentLevel);
    
    
    getLevelDataFromGC();
}

void EndGameLayer::finishingEnded(CCNode* sender)
{
    if (currentLevel == 48 && !OptionsPtr->getJubPost() && OptionsPtr->isFacebookConnection() && FacebookPtr->sessionIsOpened())
    {
        popaplayer->popupPost((char*)CCLocalizedString("POST_ON_WALL", NULL), (char*)CCLocalizedString("POST_ON_WALL_TEXT", NULL), (char*)CCLocalizedString("POST_ON_WALL", NULL), GreenPopup, BombPopBoot,
                              this, callfuncN_selector(EndGameLayer::addBonus), this, callfuncN_selector(EndGameLayer::unclockMenu));
        menu->setEnabled(false);
    }
}

void EndGameLayer::addBonus(CCNode* pSender)
{
    FacebookPtr->jubileePost();
}

void EndGameLayer::unclockMenu(CCNode* pSender)
{
    menu->setEnabled(true);
}

void EndGameLayer::explosionCallback(CCNode* sender)
{
    CCParticleSystemQuad* explosion = CCParticleExplosion::createWithTotalParticles(50);
    sender->getParent()->addChild(explosion, 10);
    explosion->setPosition(sender->getPosition());
    explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/star.png"));
    explosion->setSpeed(100.0f);
    explosion->setSpeedVar(10.0f);
    explosion->setScale(7.0f);
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
    
    
    CCParticleSystemQuad* sun =  CCParticleFire::createWithTotalParticles(10);
    sender->getParent()->addChild(sun, 10);
    sun->setPosition(sender->getPosition());
    sun->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/snow.png"));
    sun->setSpeed(0.1f);
    sun->setBlendAdditive(true);
    sun->setBlendFunc(blend);
    
    if (sender->getTag() == 1)
        SimpleAudioEngine::sharedEngine()->playEffect("sound/star_1.mp3");
    else if (sender->getTag() == 2)
        SimpleAudioEngine::sharedEngine()->playEffect("sound/star_2.mp3");
    else if (sender->getTag() == 3)
        SimpleAudioEngine::sharedEngine()->playEffect("sound/star_3.mp3");
}

void EndGameLayer::popupLose(int countScore, eLevelType typeLevel, int currentL)
{
    if (lock)
        return;
    
    isWin = false;
    currentLevel = currentL;
    isPopup = true;
    
    struct timeval now;
    gettimeofday(&now, NULL);

    if (OptionsPtr->getLifeCount() == 5)
        OptionsPtr->setLastTime(now.tv_sec);
        
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
    
    
    //changeOrientation();
	char buf[255];
    
    lock = true;
    
    isPopup = true;
    
    popup->setVisible(true);
	menu->setVisible(true);
    
    if (LANDSCAPE)
    {
        popup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, WINSIZE.height))));
        menu->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, WINSIZE.height))));
    }
    else
    {
        popup->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, -WINSIZE.height))));
        menu->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME*2.0f, ccp(0, -WINSIZE.height))));
    }
    

    
    startLeft->setVisible(false);
	startRight->setVisible(false);
	startCenter->setVisible(false);
    
    boosterTitle->setVisible(false);
    booster_1->setVisible(false);
    booster_2->setVisible(false);
    booster_3->setVisible(false);
    targetTitle->setVisible(false);
    
    guiWin->setVisible(false);
    
	sprintf(buf, "%s:%d", CCLocalizedString("SCORE"), countScore);
    
    scoreTitle->setString(buf);
    scoreTitle->setColor(IceCreamPink);
    
	next->setVisible(false);
    play->setVisible(false);
	retry->setVisible(true);
	retry->setPosition(play->getPosition());
    
    if (typeLevel == BringDown)
        targetPopupTitle->setString(CCLocalizedString("DID_NOT_BRING"));
    else if (typeLevel == Ice)
        targetPopupTitle->setString(CCLocalizedString("DID_NOT_ICE"));
    else if (typeLevel == Score)
        targetPopupTitle->setString(CCLocalizedString("DID_NOT_MOVES"));
    else if (typeLevel == Time)
        targetPopupTitle->setString(CCLocalizedString("DID_NOT_MOVES"));
    
    targetSubstrate->setScale(1.5f);
    
    targetPopupTitle->setScale(0.9f);
    if (typeLevel == BringDown)
        targetPopupTitle->setScale(0.7f);
    
    sprintf(buf, "%s %d", CCLocalizedString("LEVEL"), currentLevel);
	levelTitle->setString(buf);
    if (currentLevel > 105)
    {
        levelTitle->setString(CCLocalizedString("EXTRA_LEVEL"));
        levelTitle->setFontSize(FONT_SIZE_48);
    }
    
	levelTitle->setPosition(ccp(levelTitle->getParent()->getContentSize().width/2.0f, levelTitle->getParent()->getContentSize().height/1.1f));
    
    close->stopAllActions();
    close->setScale(0.7f);
	close->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    retry->stopAllActions();
    retry->setScale(0.7f);
	retry->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCEaseElasticOut::create(CCScaleTo::create(0.5f, 1.0f)), CCRepeat::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, 1.05f, 0.95f), CCScaleTo::create(0.5f, 1.0f, 1.0f)), 100), NULL));
    
    
    social->showScoreboard(currentLevel);
}

void EndGameLayer::closeLoading()
{
    popaplayer->closeLoading();
    menu->setEnabled(true);
}

void EndGameLayer::updateBoosters()
{
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

void EndGameLayer::helpModalCallback(CCObject* pSender)
{
    if (helpModal->isVisible())
    {
        helpModal->setVisible(false);
        
        booster_1->setEnabled(true);
        booster_2->setEnabled(true);
        booster_3->setEnabled(true);
        close->setEnabled(true);
        play->setEnabled(true);
    }
    else
    {
        helpModal->setVisible(true);
        
        booster_1->setEnabled(false);
        booster_2->setEnabled(false);
        booster_3->setEnabled(false);
        close->setEnabled(false);
        play->setEnabled(false);
    }
}

void EndGameLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    if (helpModal)
        if (helpModal->isVisible())
        {
            helpModal->setVisible(false);
            booster_1->setEnabled(true);
            booster_2->setEnabled(true);
            booster_3->setEnabled(true);
            close->setEnabled(true);
            play->setEnabled(true);
        }
}

bool EndGameLayer::isLock()
{
	return lock;
}

void EndGameLayer::closeCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
	lock = false;
    menu->setEnabled(false);
    
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
        spriteLoading->setScale(1.0f/this->getScale());
    }
    
    labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
    labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
    this->addChild(labelLoad, 1001);
    spriteLoading->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    this->addChild(spriteLoading, 1000);
    spriteLoading->setVisible(false);
    spriteLoading->setOpacity(0);
    spriteLoading->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCShow::create(), CCFadeIn::create(0.3f),  NULL));
    labelLoad->setVisible(false);
    labelLoad->setOpacity(0);
    labelLoad->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCShow::create(), CCFadeIn::create(0.3f),  NULL));
    afterLoadingType = AfterLoadingTypeClose;
    
    if (getNetworkStatus() && FacebookPtr->sessionIsOpened() && OptionsPtr->isFacebookConnection())
    {
        GlobalsPtr->isLoadMap = true;
        FacebookPtr->getScores();
    }
    else
        this->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME*3.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::closeAfterLoading)), NULL));
}

void EndGameLayer::afterGetScores()
{
    if (afterLoadingType ==  AfterLoadingTypeClose)
    {
        this->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME*3.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::closeAfterLoading)), NULL));
    }
    else if (afterLoadingType ==AfterLoadingTypeNext)
    {
        this->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME*3.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::nextAfterLoading)), NULL));
    }
    else if (afterLoadingType == AfterLoadingTypeLives)
    {
        this->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME*3.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::nextWithLivePanel)), NULL));
    }
}

void EndGameLayer::closeAfterLoading(CCNode* node)
{
    if (isWin)
    {
        if (currentLevel == 12 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
            CCDirector::sharedDirector()->replaceScene(ComixScene::scene(16));
        else if (currentLevel == 24 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
            CCDirector::sharedDirector()->replaceScene(ComixScene::scene(17));
        else if (currentLevel == 36 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
            CCDirector::sharedDirector()->replaceScene(ComixScene::scene(18));
        else if (currentLevel == 48 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
            CCDirector::sharedDirector()->replaceScene(ComixScene::scene(19));
        else if (currentLevel == 60 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
            CCDirector::sharedDirector()->replaceScene(ComixScene::scene(20));
        else if (currentLevel == 72 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
            CCDirector::sharedDirector()->replaceScene(ComixScene::scene(21));
        else if (currentLevel == 84 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
            CCDirector::sharedDirector()->replaceScene(ComixScene::scene(22));
        else if (currentLevel == 106 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
            CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(35));
        else
            CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(-1));
    }
    else
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene());
}

void EndGameLayer::updateFacebook()
{
    if (isPopup)
    {
        social->hideScoreBoard();
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::updateFacebookCallback))));
    }
}

void EndGameLayer::updateFacebookCallback(CCNode* pSender)
{
    social->showScoreboard(currentLevel);
}

void EndGameLayer::backCallback(CCObject* pSender)
{
    menu->setEnabled(false);
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
	CCDirector::sharedDirector()->replaceScene(MainMenuScene::scene());
}

void EndGameLayer::playCallback(CCObject* pSender)
{
    menu->setEnabled(false);
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (OptionsPtr->getLifeCount() <= 0)
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(-1));
    else
        CCDirector::sharedDirector()->replaceScene(GameScene::scene(currentLevel));
    lock = false;
}

void EndGameLayer::nextCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    
    menu->setEnabled(false);
    if (LANDSCAPE)
    {
        popup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))), CCCallFuncN::create(this,callfuncN_selector(EndGameLayer::closeEnded))));
    }
    else
    {
        popup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))), CCCallFuncN::create(this,callfuncN_selector(EndGameLayer::closeEnded))));
    }
    
    social->hideScoreBoard();
    
    int lastLevel = OptionsPtr->getCurrentLevel();
    if ((currentLevel + 1) < lastLevel)
    {
        menu->setEnabled(false);
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::nextOldLevel))));
        return;
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
        spriteLoading->setScale(1.0f/this->getScale());
    }
    
    labelLoad = CCLabelTTF::create(CCLocalizedString("LOADING", NULL), FONT_COMMON, FONT_SIZE_86);
    labelLoad->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/10.0f));
    this->addChild(labelLoad,10001);
    spriteLoading->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    this->addChild(spriteLoading, 1000);
    spriteLoading->setVisible(false);
    spriteLoading->setOpacity(0);
    spriteLoading->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCShow::create(), CCFadeIn::create(0.3f),  NULL));
    labelLoad->setVisible(false);
    labelLoad->setOpacity(0);
    labelLoad->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME), CCShow::create(), CCFadeIn::create(0.3f),  NULL));

    afterLoadingType = AfterLoadingTypeNext;
    
    if (getNetworkStatus() && FacebookPtr->sessionIsOpened() && OptionsPtr->isFacebookConnection())
    {
        GlobalsPtr->isLoadMap = true;
        FacebookPtr->getScores();
    }
    else
        this->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME*3.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::nextAfterLoading)), NULL));
}

void EndGameLayer::nextAfterLoading(CCNode* node)
{
    if (currentLevel == 12 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(16));
    else if (currentLevel == 24 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(17));
    else if (currentLevel == 36 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(18));
    else if (currentLevel == 48 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(19));
    else if (currentLevel == 60 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(20));
    else if (currentLevel == 72 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(21));
    else if (currentLevel == 84 && OptionsPtr->getCurrentLevel() <= (currentLevel + 1))
        CCDirector::sharedDirector()->replaceScene(ComixScene::scene(22));
    else if (currentLevel == 106)
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(35));
    else
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(currentLevel + 1));
}

void EndGameLayer::nextWithLivePanel(CCNode* node)
{
    CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(-111));
}

void EndGameLayer::retryCallback(CCObject* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (help)
    {
        help->removeFromParentAndCleanup(true);
        helpModal->removeFromParentAndCleanup(true);
        helpModal = NULL;
    }
    
    if (LANDSCAPE)
    {
        popup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, -WINSIZE.height))), CCCallFuncN::create(this,callfuncN_selector(EndGameLayer::closeEnded))));
    }
    else
    {
        popup->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))));
        menu->runAction(CCSequence::createWithTwoActions(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0, WINSIZE.height))), CCCallFuncN::create(this,callfuncN_selector(EndGameLayer::closeEnded))));
    }
    
    social->hideScoreBoard();
    
    menu->setEnabled(false);
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::retryEnd))));
}

void EndGameLayer::nextOldLevel(CCNode* pSender)
{
    menu->setEnabled(true);
    if (OptionsPtr->getLifeCount() == 0)
    {
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
            spriteLoading->setScale(1.0f/this->getScale());
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
        
        afterLoadingType = AfterLoadingTypeLives;
        
        if (getNetworkStatus() && FacebookPtr->sessionIsOpened() && OptionsPtr->isFacebookConnection())
        {
            GlobalsPtr->isLoadMap = true;
            FacebookPtr->getScores();
        }
        else
            this->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME*3.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::nextWithLivePanel)), NULL));
        return;
    }
    if ((currentLevel + 1) < 106)
    {
        if (OptionsPtr->getLevelData(currentLevel).levelType != Score)
            levelPopup(currentLevel + 1, OptionsPtr->getLevelData(currentLevel).countStar, OptionsPtr->getLevelData(currentLevel).targetScore, OptionsPtr->getLevelData(currentLevel).levelType, BoosterCrystal, BoosterBomb, BoosterFish);
        else
            levelPopup(currentLevel + 1, OptionsPtr->getLevelData(currentLevel).countStar, OptionsPtr->getLevelData(currentLevel).targetScore, OptionsPtr->getLevelData(currentLevel).levelType, BoosterCrystal, BoosterBomb, BoosterNone);
    }
    else
    {
        if (OptionsPtr->getLevelData(currentLevel + 1).levelType != Score)
            levelPopup(currentLevel + 1, OptionsPtr->getLevelData(currentLevel + 1).countStar, OptionsPtr->getLevelData(currentLevel + 1).targetScore, OptionsPtr->getLevelData(currentLevel).levelType, BoosterCrystal, BoosterBomb, BoosterFish);
        else
            levelPopup(currentLevel + 1, OptionsPtr->getLevelData(currentLevel + 1).countStar, OptionsPtr->getLevelData(currentLevel + 1).targetScore, OptionsPtr->getLevelData(currentLevel).levelType, BoosterCrystal, BoosterBomb, BoosterNone);
    }
}

void EndGameLayer::retryEnd(CCNode* pSender)
{
    menu->setEnabled(true);
    if (OptionsPtr->getLifeCount() == 0)
    {
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
            spriteLoading->setScale(1.0f/this->getScale());
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
        
        afterLoadingType = AfterLoadingTypeLives;
        
        if (getNetworkStatus() && FacebookPtr->sessionIsOpened() && OptionsPtr->isFacebookConnection())
        {
            GlobalsPtr->isLoadMap = true;
            FacebookPtr->getScores();
        }
        else
            this->runAction(CCSequence::create(CCDelayTime::create(POPUP_SHOW_TIME*3.0f), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::nextWithLivePanel)), NULL));
        return;
    }
    if (currentLevel < 106)
    {
        if (OptionsPtr->getLevelData(currentLevel - 1).levelType != Score)
            levelPopup(currentLevel, OptionsPtr->getLevelData(currentLevel - 1).countStar, OptionsPtr->getLevelData(currentLevel - 1).targetScore, OptionsPtr->getLevelData(currentLevel - 1).levelType, BoosterCrystal, BoosterBomb, BoosterFish);
        else
            levelPopup(currentLevel, OptionsPtr->getLevelData(currentLevel - 1).countStar, OptionsPtr->getLevelData(currentLevel - 1).targetScore, OptionsPtr->getLevelData(currentLevel - 1).levelType, BoosterCrystal, BoosterBomb, BoosterNone);
    }
    else
    {
        if (OptionsPtr->getLevelData(currentLevel).levelType != Score)
            levelPopup(currentLevel, OptionsPtr->getLevelData(currentLevel).countStar, OptionsPtr->getLevelData(currentLevel).targetScore, OptionsPtr->getLevelData(currentLevel).levelType, BoosterCrystal, BoosterBomb, BoosterFish);
        else
            levelPopup(currentLevel, OptionsPtr->getLevelData(currentLevel).countStar, OptionsPtr->getLevelData(currentLevel).targetScore, OptionsPtr->getLevelData(currentLevel).levelType, BoosterCrystal, BoosterBomb, BoosterNone);
    }
}

void EndGameLayer::booster_1_Callback(CCObject* pSender)
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
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("COLOR_BOMB", NULL), (char*)CCLocalizedString("COLOR_BOMB_TEXT", NULL), GreenPopup, CrystalPopBoot, this, callfuncN_selector(EndGameLayer::popupOk1), this, callfuncN_selector(EndGameLayer::unclockMenu));
    }
    else if (firstBooster == BoosterBomb)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("STRIPPED_WRAPPED", NULL), (char*)CCLocalizedString("STRIPPED_WRAPPED_TEXT", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(EndGameLayer::popupOk2), this, callfuncN_selector(EndGameLayer::unclockMenu));
    }
    else if (firstBooster == BoosterDonut)
    {
        OptionsPtr->setDonuthCount(OptionsPtr->getDonutCount() + 1);
    }
    else if (firstBooster == BoosterFish)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("SWEET_PENGUIN", NULL), (char*)CCLocalizedString("SWEET_PENGUIN_TEXT", NULL), GreenPopup, FishPopBoot, this, callfuncN_selector(EndGameLayer::popupOk3), this, callfuncN_selector(EndGameLayer::unclockMenu));
    }
}

void EndGameLayer::booster_2_Callback(CCObject* pSender)
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
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("COLOR_BOMB", NULL), (char*)CCLocalizedString("COLOR_BOMB_TEXT", NULL), GreenPopup, CrystalPopBoot, this, callfuncN_selector(EndGameLayer::popupOk1), this, callfuncN_selector(EndGameLayer::unclockMenu));
    }
    else if (secondBooster == BoosterBomb)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("STRIPPED_WRAPPED", NULL), (char*)CCLocalizedString("STRIPPED_WRAPPED_TEXT", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(EndGameLayer::popupOk2), this, callfuncN_selector(EndGameLayer::unclockMenu));
    }
    else if (secondBooster == BoosterDonut)
    {
        OptionsPtr->setDonuthCount(OptionsPtr->getDonutCount() + 1);
    }
    else if (secondBooster == BoosterFish)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("SWEET_PENGUIN", NULL), (char*)CCLocalizedString("SWEET_PENGUIN_TEXT", NULL), GreenPopup, FishPopBoot, this, callfuncN_selector(EndGameLayer::popupOk3), this, callfuncN_selector(EndGameLayer::unclockMenu));
    }
}

void EndGameLayer::booster_3_Callback(CCObject* pSender)
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
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("COLOR_BOMB", NULL), (char*)CCLocalizedString("COLOR_BOMB_TEXT", NULL), GreenPopup, CrystalPopBoot, this, callfuncN_selector(EndGameLayer::popupOk1), this, callfuncN_selector(EndGameLayer::unclockMenu));
    }
    else if (thirdBooster == BoosterBomb)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("STRIPPED_WRAPPED", NULL), (char*)CCLocalizedString("STRIPPED_WRAPPED_TEXT", NULL), GreenPopup, BombPopBoot, this, callfuncN_selector(EndGameLayer::popupOk2), this, callfuncN_selector(EndGameLayer::unclockMenu));
    }
    else if (thirdBooster == BoosterDonut)
    {
        OptionsPtr->setDonuthCount(OptionsPtr->getDonutCount() + 1);
    }
    else if (thirdBooster == BoosterFish)
    {
        popaplayer->popupBoosterInApp((char*)CCLocalizedString("SWEET_PENGUIN", NULL), (char*)CCLocalizedString("SWEET_PENGUIN_TEXT", NULL), GreenPopup, FishPopBoot, this, callfuncN_selector(EndGameLayer::popupOk3), this, callfuncN_selector(EndGameLayer::unclockMenu));
    }
}
void EndGameLayer::popupOk1(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    popaplayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.superelements");
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(CONNECTION_TIME), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::closeLoading))));
    menu->setEnabled(false);
}

void EndGameLayer::popupOk2(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    popaplayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.stripedandbomb");
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(CONNECTION_TIME), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::closeLoading))));
    menu->setEnabled(false);
}

void EndGameLayer::popupOk3(CCNode* pSender)
{
    SimpleAudioEngine::sharedEngine()->playEffect("sound/pop_1.mp3");
    if (!getNetworkStatus())
    {
        alertNetwork();
        return;
    }
    popaplayer->loading((char*)CCLocalizedString("CONNECTION", NULL));
    IAP::sharedInstance().buyProduct("com.destiny.icecreamadventure.penguins");
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(CONNECTION_TIME), CCCallFuncN::create(this, callfuncN_selector(EndGameLayer::closeLoading))));
    menu->setEnabled(false);
}

void EndGameLayer::endTutorial(CCNode* pSender)
{
    FacebookPtr->endTutorial();
}

void EndGameLayer::endStage(CCNode* pSender)
{
    switch (currentLevel) {
        case 12:
            FacebookPtr->endStage(CCLocalizedString("VALLEY"));
            break;
        case 24:
            FacebookPtr->endStage(CCLocalizedString("FOREST"));
            break;
        case 36:
            FacebookPtr->endStage(CCLocalizedString("MOUNTAIN"));
            break;
        case 48:
            FacebookPtr->endStage(CCLocalizedString("SEA"));
            break;
        case 60:
            FacebookPtr->endStage(CCLocalizedString("MEADOWS"));
            break;
        case 72:
            FacebookPtr->endStage(CCLocalizedString("FARM"));
            break;
        case 84:
            FacebookPtr->endStage(CCLocalizedString("CITY"));
            break;
            
        default:
            break;
    }
}


