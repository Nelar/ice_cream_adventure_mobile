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
#include "CCLocalizedString.h"

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
    if (LANDSCAPE)
        messageboard->setScale(0.75f);
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
    CCSprite* greyBack = CCSprite::create("message/moreGamesPlate.png");
    layer->addChild(background);
    background->addChild(greyBack);
    greyBack->setPosition(ccp(background->getContentSize().width/2.03f, background->getContentSize().height/2.1f));
    
    title = CCLabelTTF::create(CCLocalizedString("LEARN_MORE", NULL), FONT_COMMON, FONT_SIZE_64);
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
    
    if(IPAD_MINI)
        fontSize /= 2;
    
    if(IPHONE_4 || IPHONE_5)
        fontSize /= 1.8f;
    
    CCScrollView* scrollView = CCScrollView::create(CCSize(greyBack->getContentSize().width, greyBack->getContentSize().height));
    scrollView->setDirection(kCCScrollViewDirectionVertical);
    scrollView->setClippingToBounds(true);
    
    CCSprite* panelTemp = CCSprite::createWithSpriteFrameName("common/greenPlate.png");
    
    CCLayer* container = CCLayer::create();
    int sizeMoreGames = GlobalsPtr->globalMoreGames.size();
    CCLog("Count more games %d", GlobalsPtr->globalMoreGames.size());
    float containerHeight = panelTemp->getContentSize().height*1.1f * GlobalsPtr->globalMoreGames.size();
    container->setContentSize(CCSize(greyBack->getContentSize().width, containerHeight));
    
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
        CCNode* nodeIcon = CCNode::create();
        CCSprite* plate = CCSprite::createWithSpriteFrameName("common/greenPlate.png");
        nodeIcon->setContentSize(CCSize(plate->getContentSize().width, plate->getContentSize().height/1.2f));
        nodeIcon->addChild(plate);
        nodeIcon->setAnchorPoint(ccp(0.5f, 0.5f));
        plate->setPosition(ccp(nodeIcon->getContentSize().width/2.0f, nodeIcon->getContentSize().height/2.0f));
        plate->setScaleY(0.8333f);
        
        nodeIcon->addChild(iconMoreGames);
        CCSize size = CCSize(nodeIcon->getContentSize().height/1.8f, nodeIcon->getContentSize().height/1.8f);
        iconMoreGames->setPosition(ccp(nodeIcon->getContentSize().width/4.5f,  nodeIcon->getContentSize().height/1.6f));
        iconMoreGames->setScaleX(size.width/iconMoreGames->getContentSize().width);
        iconMoreGames->setScaleY(size.height/iconMoreGames->getContentSize().height);
        
        CCMenu* menu = CCMenu::create();
        menu->setContentSize(nodeIcon->getContentSize());
        menu->setAnchorPoint(ccp(0.0f, 0.0f));
        menu->setPosition(nodeIcon->getPosition());
        CCMenuItemLabel* nameLabel = CCMenuItemLabel::create(CCLabelTTF::create(GlobalsPtr->globalMoreGames[i].name.c_str(), FONT_COMMON, FONT_SIZE_40) , NULL, NULL);
        nameLabel->setAnchorPoint(ccp(0.0f, 1.0f));
        
        CCMenuItemLabel* descLabel = CCMenuItemLabel::create(CCLabelTTF::create(GlobalsPtr->globalMoreGames[i].description.c_str(), FONT_COMMON, FONT_SIZE_26, CCSize(nodeIcon->getContentSize().width/1.9f, nodeIcon->getContentSize().height/2.0f), kCCTextAlignmentLeft) , NULL, NULL);
        descLabel->setAnchorPoint(ccp(0.0f, 1.0f));
        descLabel->setColor(ccWHITE);
        
        CCSprite* installButton = CCSprite::createWithSpriteFrameName("common/greenButton.png");
        CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("FREE", NULL), FONT_COMMON, FONT_SIZE_64);
        labelTTF->setColor(ccWHITE);
        labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
        installButton->addChild(labelTTF);
        labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
        CCSprite* installButtonDown = CCSprite::createWithSpriteFrameName("common/greenButton.png");
        labelTTF = CCLabelTTF::create(CCLocalizedString("FREE", NULL), FONT_COMMON, FONT_SIZE_64);
        labelTTF->setColor(ccWHITE);
        labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
        installButtonDown->addChild(labelTTF);
        labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
        installButtonDown->setColor(ccGRAY);
        CCMenuItemSprite* installLabel = CCMenuItemSprite::create(installButton, installButtonDown, this, menu_selector(MoreGamesLayer::messageCallback));
        installLabel->setScale(0.8f);
        
        installLabel->setTag(i);
        ((CCSprite*)(installLabel->getSelectedImage()))->setColor(ccGRAY);
        
        CCMenuItemLabel* genrelLabel = CCMenuItemLabel::create(CCLabelTTF::create("Destiny Development", FONT_COMMON, FONT_SIZE_32), NULL, NULL);
        genrelLabel->setAnchorPoint(ccp(0.0f, 1.0f));
        menu->addChild(nameLabel);
        menu->addChild(installLabel);
        menu->addChild(genrelLabel);
        nodeIcon->addChild(descLabel);
        
        nodeIcon->addChild(menu);
        installLabel->setPosition(ccp(nodeIcon->getContentSize().width/1.15f, nodeIcon->getContentSize().height/11.0f));
        nameLabel->setPosition(ccp(nodeIcon->getContentSize().width/2.3f, nodeIcon->getContentSize().height/1.10f));
        descLabel->setPosition(ccp(nodeIcon->getContentSize().width/2.3f, nodeIcon->getContentSize().height/1.5f));
        genrelLabel->setPosition(ccp(nodeIcon->getContentSize().width/2.3f, nodeIcon->getContentSize().height/1.25f));
        
        installLabel->setPosition(ccp(nodeIcon->getContentSize().width/1.3f, nodeIcon->getContentSize().height/7.0f));
        
        for (int j = 0 ; j < GlobalsPtr->globalMoreGames[i].stars; j++)
        {
            CCSprite* star = CCSprite::create("star.png");
            star->setAnchorPoint(ccp(0.5f, 0.5f));
            nodeIcon->addChild(star);
            star->setPosition(ccp(nodeIcon->getContentSize().width/12.0f + star->getContentSize().width/1.5f*j, star->getContentSize().height/1.5f));
        }
        
        container->addChild(nodeIcon, 1);
        nodeIcon->setPosition(ccp(container->getContentSize().width/2.0f, container->getContentSize().height - nodeIcon->getContentSize().height/1.8f - nodeIcon->getContentSize().height*1.1f*i));
        nodeIcon->setScale(1.08f);
    }
    cocos2d::CCUserDefault::sharedUserDefault()->flush();
    scrollView->setContainer(container);
    greyBack->addChild(scrollView, 100);
    scrollView->setPosition(ccp(0.0f, 0.0f));
    scrollView->setVisible(true);
    scrollView->setContentOffset(ccp(0.0f, -(containerHeight - greyBack->getContentSize().height)));

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















