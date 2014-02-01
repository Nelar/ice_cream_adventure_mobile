//
//  cSocialMenu.cpp
//  IceCreamAdventure
//
//  Created by Nelar on 06.11.13.
//
//

#include "cMoreGamesLayer.h"
#include "cGlobal.h"
#include "Options.h"
#include "SimpleAudioEngine.h"
#include "GameMapLayer.h"

using namespace cocos2d;
using namespace CocosDenshion;

MoreGamesLayer::~MoreGamesLayer()
{
    
}

bool MoreGamesLayer::init()
{
	if (!CCLayer::init())
		return false;
    
    userNodeSize = 400.0f;
    multiplier = 1.0f;
    messageboard = NULL;
    
    if (!IPAD)
    {
        userNodeSize = 200.0f;
        multiplier = 0.5f;
    }
	return true;
}

void MoreGamesLayer::changeOrientation()
{
    if (messageboard)
    {
        deleteMessageboard(NULL);
        showMessageboard();
    }
    return;
}

void MoreGamesLayer::showMessageboard()
{
    messageboard = createMessageboard();
    messageboard->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f + WINSIZE.height));
    messageboard->runAction(CCEaseBackOut::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, -WINSIZE.height))));
    isMessageBoard = true;
    this->addChild(messageboard);
}

void MoreGamesLayer::hideMessageboard()
{
    isMessageBoard = false;
    messageboard->runAction(CCEaseBackIn::create(CCMoveBy::create(POPUP_SHOW_TIME, ccp(0.0f, WINSIZE.height))));
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(POPUP_SHOW_TIME), CCCallFuncN::create(this, callfuncN_selector(MoreGamesLayer::deleteMessageboard))));
}

void MoreGamesLayer::deleteMessageboard(CCNode* node)
{
    messageboard->removeFromParentAndCleanup(true);
    messageboard = NULL;
}

CCNode* MoreGamesLayer::createMessageboard()
{
    CCNode* layer = CCNode::create();
    CCSprite* background = CCSprite::createWithSpriteFrameName("common/panel.png");
    CCSprite* greyBack = CCSprite::create("message/greyPlate.png");
    layer->addChild(background);
    background->addChild(greyBack);
    greyBack->setPosition(ccp(background->getContentSize().width/2.03f, background->getContentSize().height/2.1f));
    greyBack->setScaleY(1.75f);
    
    title = CCLabelTTF::create("Learn more our other games", "Script MT Bold", 48);
    title->setColor(IceCreamPink);
    title->setPosition(ccp(background->getContentSize().width/2.0f, background->getContentSize().height/1.1f));
    background->addChild(title);
    
    CCMenu* menu = CCMenu::create();
    background->addChild(menu, 10);
    menu->setContentSize(background->getContentSize());
    menu->setAnchorPoint(ccp(0.0f, 0.0f));
    menu->setPosition(background->getPosition());
    
    CCMenuItemSprite* close = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/close.png"), CCSprite::createWithSpriteFrameName("common/close_on.png"), this, menu_selector(MoreGamesLayer::closeMessageboardCallback));
    close->setPosition(ccp(background->getContentSize().width - close->getContentSize().width/2.0f, background->getContentSize().height - close->getContentSize().height/2.0f));
    menu->addChild(close);
    
    
    float fontSize = 40;
    
    if(CCDirector::sharedDirector()->getWinSize().height < 2048)
        fontSize /= 2;
    
    if(CCDirector::sharedDirector()->getWinSize().width == 320)
        fontSize /= 1.8f;
    
    CCScrollView* scrollView = CCScrollView::create(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height/1.25f));
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setClippingToBounds(true);
    
    CCSprite* panelTemp = CCSprite::createWithSpriteFrameName("common/greenPlate.png");
    
    CCLayer* container = CCLayer::create();
    int sizeMoreGames = GlobalsPtr->globalMoreGames.size();
    CCLog("Count more games %d", GlobalsPtr->globalMoreGames.size());
    float containerHeight = panelTemp->getContentSize().height*1.1f * GlobalsPtr->globalMoreGames.size();
    container->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, containerHeight));
    
    float scaleNode = 1.08f;
    
    for (int i = 0; i < GlobalsPtr->globalMoreGames.size(); i++)
    {
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesCompatibility" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].compatibility);
        cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(string("MoreGamesRating" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].rating);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesName" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].name);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesUrl" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].url);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesPrice" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].price);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesDescription" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].description);
        cocos2d::CCUserDefault::sharedUserDefault()->setIntegerForKey(string("MoreGamesStars" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].stars);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesGenre" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].genre);
        cocos2d::CCUserDefault::sharedUserDefault()->setStringForKey(string("MoreGamesIconUrl" + to_string(i)).c_str() , GlobalsPtr->globalMoreGames[i].iconUrl);
        
        char buf[255];
        sprintf(buf, "%d", i);
        string iconFile = cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath() + string("icon") + buf + string(".png");
        
        CCSprite* iconMoreGames = CCSprite::create(iconFile.c_str());
        CCSprite* nodeIcon = CCSprite::createWithSpriteFrameName("common/greenPlate.png");
        nodeIcon->setScaleX(scaleNode);
        
        nodeIcon->addChild(iconMoreGames);
        CCSize size = CCSize(nodeIcon->getContentSize().height/1.8f, nodeIcon->getContentSize().height/1.8f);
        
        iconMoreGames->setScaleX(size.width/iconMoreGames->getContentSize().width /1.05f);
        iconMoreGames->setScaleY(size.height/iconMoreGames->getContentSize().height);
        iconMoreGames->setPosition(ccp(nodeIcon->getContentSize().width/5.0f,  nodeIcon->getContentSize().height/1.7f));
        
        CCMenu* menu = CCMenu::create();
        menu->setContentSize(nodeIcon->getContentSize());
        menu->setAnchorPoint(ccp(0.0f, 0.0f));
        menu->setPosition(nodeIcon->getPosition());
        CCMenuItemLabel* nameLabel = CCMenuItemLabel::create(CCLabelTTF::create(GlobalsPtr->globalMoreGames[i].name.c_str(), "Script MT Bold", fontSize*1.2f) , NULL, NULL);
        nameLabel->setAnchorPoint(ccp(0.0f, 1.0f));
        nameLabel->setScaleX(1.0f/scaleNode);
        
        CCMenuItemLabel* descLabel = CCMenuItemLabel::create(CCLabelTTF::create(GlobalsPtr->globalMoreGames[i].description.c_str(), "Script MT Bold", fontSize*0.7f, CCSize(nodeIcon->getContentSize().width/1.7f, nodeIcon->getContentSize().height/2.0f), kCCTextAlignmentLeft) , NULL, NULL);
        descLabel->setAnchorPoint(ccp(0.0f, 1.0f));
        descLabel->setColor(ccWHITE);
        descLabel->setScaleX(1.0f/scaleNode);
        
        CCMenuItemSprite* installLabel = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("common/play.png"), CCSprite::createWithSpriteFrameName("common/play_selected.png"), this, menu_selector(MoreGamesLayer::messageCallback));
        
        installLabel->setTag(i);
        ((CCSprite*)(installLabel->getSelectedImage()))->setColor(ccGRAY);
        installLabel->setScaleX(1.0f/scaleNode);
        
        CCMenuItemLabel* genrelLabel = CCMenuItemLabel::create(CCLabelTTF::create("Destiny Development", "Arial", fontSize*0.7f), NULL, NULL);
        genrelLabel->setAnchorPoint(ccp(0.0f, 1.0f));
        genrelLabel->setScaleX(1.0f/scaleNode);
        menu->addChild(nameLabel);
        menu->addChild(installLabel);
        menu->addChild(genrelLabel);
        nodeIcon->addChild(descLabel);
        
        nodeIcon->addChild(menu);
        installLabel->setPosition(ccp(nodeIcon->getContentSize().width/1.15f, nodeIcon->getContentSize().height/11.0f));
        nameLabel->setPosition(ccp(nodeIcon->getContentSize().width/2.4f, nodeIcon->getContentSize().height/1.18f));
        descLabel->setPosition(ccp(nodeIcon->getContentSize().width/2.4f, nodeIcon->getContentSize().height/1.7f));
        genrelLabel->setPosition(ccp(nodeIcon->getContentSize().width/2.4f, nodeIcon->getContentSize().height/1.33f));
        
        installLabel->setPosition(ccp(nodeIcon->getContentSize().width/1.3f, nodeIcon->getContentSize().height/7.0f));
        
        for (int j = 0 ; j < GlobalsPtr->globalMoreGames[i].stars; j++)
        {
            CCSprite* star = CCSprite::create("star.png");
            star->setAnchorPoint(ccp(0.5f, 0.5f));
            nodeIcon->addChild(star);
            star->setPosition(ccp(nodeIcon->getContentSize().width/12.0f + star->getContentSize().width/1.5f*j, star->getContentSize().height/1.5f));
            star->setScaleX(1.0f/scaleNode);
        }
        
        nodeIcon->setPosition(ccp(background->getContentSize().width/2.0f, background->getContentSize().height/2.0f));
        background->addChild(nodeIcon, 100);
        
     //   container->addChild(nodeIcon, 1);
       // nodeIcon->setPosition(ccp(container->getContentSize().width/2.0f, container->getContentSize().height - nodeIcon->getContentSize().height/1.9f - nodeIcon->getContentSize().height*1.1f*i));
    }
/*    cocos2d::CCUserDefault::sharedUserDefault()->flush();
    scrollView->setContainer(container);
    background->addChild(scrollView, 100);
    scrollView->setPosition(ccp(0.0f, CCDirector::sharedDirector()->getWinSize().height*0.1f));
    scrollView->setVisible(false);
    if(CCDirector::sharedDirector()->getWinSize().height*CCDirector::sharedDirector()->getContentScaleFactor() == 1024)
    {
        scrollView->setContentOffset(ccp(0.0f, panelTemp->getContentSize().height/2.1f));
    }
    else if(CCDirector::sharedDirector()->getWinSize().width == 320)
    {
        scrollView->setContentOffset(ccp(0.0f, panelTemp->getContentSize().height/2.1f));
    }*/

    return layer;
}

void MoreGamesLayer::closeMessageboardCallback(CCObject* pSender)
{
    hideMessageboard();
}

CCNode* MoreGamesLayer::createMessage(sMoreGames game)
{
    CCNode* node = CCNode::create();
    return node;
}

void MoreGamesLayer::messageCallback(CCObject* pSender)
{
    CCMenuItemSprite* sender = (CCMenuItemSprite*)pSender;
}















