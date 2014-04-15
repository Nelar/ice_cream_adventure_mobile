//
//  cComixScene.cpp
//  IceCreamSaga
//
//  Created by Nelar on 10.09.13.
//
//

#include "cComixScene.h"
#include "cGlobal.h"
#include "SimpleAudioEngine.h"
#include "Options.h"
#include <spine/spine.h>
#include <spine/spine-cocos2dx.h>
#include "GameScene.h"
#include "GameMapLayer.h"

using namespace CocosDenshion;
using namespace cocos2d;
using namespace extension;

#define COMIX_SCALE 0.05f

ComixScene* ComixScene::create(int num)
{
	ComixScene *pRet = new ComixScene();
	if (pRet && pRet->init(num))
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


CCScene* ComixScene::scene(int num)
{
	CCScene * scene = NULL;
	do
	{
		scene = CCScene::create();
		CC_BREAK_IF(! scene);
        
		ComixScene *layer = ComixScene::create(num);
		CC_BREAK_IF(! layer);
        
		scene->addChild(layer);
	} while (0);
    
	return scene;
}

bool ComixScene::init(int num)
{
	if (!CCLayer::init())
		return false;
    
    GlobalsPtr->iceCreamScene = Comix;
    
    time = 0.0f;
    comixTime = 0.0f;
    pause = false;
    nextBubbleTime = -1.0f;
    comixNumber = num;
    pauseNumber = 0;

    
    CCSprite* spriteNormal = CCSprite::CCSprite::createWithSpriteFrameName("common/blueButton.png");
    CCSprite* spriteSelected = CCSprite::CCSprite::createWithSpriteFrameName("common/blueButton.png");
    spriteSelected->setColor(ccGRAY);
    skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(ComixScene::skipCallback));
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/3.0f, -CCDirector::sharedDirector()->getWinSize().height/2.4f);
    
    
    spriteNormal = CCSprite::CCSprite::createWithSpriteFrameName("common/blueButtonArrow.png");
    spriteSelected = CCSprite::CCSprite::createWithSpriteFrameName("common/blueButtonArrow.png");
    next = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(ComixScene::nextCallback));
    next->setCascadeOpacityEnabled(true);
    
    labelTTF = CCLabelTTF::create(CCLocalizedString("NEXT", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5, -5), 255, 8.0f);
    next->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));

    
    next->setPosition(CCDirector::sharedDirector()->getWinSize().width/3.0f, -CCDirector::sharedDirector()->getWinSize().height/2.4f);
    
    menu = CCMenu::create();
    menu->addChild(next);
    menu->addChild(skip);
    next->setVisible(false);
    this->addChild(menu, 15);
    
    fon = CCSprite::create("game/cell.png");
    this->addChild(fon, 10);
    
    fon->setColor(ccBLACK);
    fon->setScale(200.0f);
    fon->setOpacity(0);
    
    borderLeft = CCSprite::create("game/border.png");
    borderRight = CCSprite::create("game/border.png");
    
    if (LANDSCAPE)
    {
        borderLeft->setRotation(90.0f);
        borderRight->setRotation(90.0f);
        borderLeft->setPosition(ccp(borderLeft->getContentSize().height/2.0f, WINSIZE.height/2.0f));
        borderRight->setPosition(ccp(WINSIZE.width - borderLeft->getContentSize().height/2.0f, WINSIZE.height/2.0f));
        
        if (IPHONE_5)
        {
            borderLeft->setScale(2.1f);
            borderRight->setScale(2.1f);
        }
    }
    else
    {
        
        borderLeft->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height - borderLeft->getContentSize().height/2.0f));
        borderRight->setPosition(ccp(WINSIZE.width/2.0f, borderLeft->getContentSize().height/2.0f));
        if (IPHONE_5)
        {
            borderLeft->setScale(2.1f);
            borderRight->setScale(2.1f);
        }
    }
    
    this->addChild(borderLeft, 11);
    this->addChild(borderRight, 11);
    
    switch(num)
    {
        case 1:
            comix_1();
            level = 1;
            comixTime = 5.0f;
            break;
        case 2:
            comix_3();
            level = 13;
            comixTime = 12.0f;
            break;
        case 3:
            comix_4();
            level = 25;
            comixTime = 12.0f;
            break;
        case 4:
            comix_5();
            level = 37;
            comixTime = 13.5f;
            break;
        case 5:
            comix_6();
            level = 49;
            comixTime = 16.0f;
            break;
        case 6:
            comix_7();
            level = 61;
            comixTime = 16.0f;
            break;
        case 7:
            comix_8();
            level = 73;
            comixTime = 8.5f;
            break;
        case 8:
            level = 85;
            comix_9();
            comixTime = 14.5f;
            break;
        case 9:
            level = 57;
            comix_10();
            comixTime = 14.5f;
            break;
        case 10:
            level = 64;
            comix_11();
            comixTime = 16.5f;
            break;
        case 11:
            level = 71;
            comix_12();
            comixTime = 17.0f;
            break;
        case 12:
            level = 78;
            comix_13();
            comixTime = 20.0f;
            break;
        case 13:
            level = 85;
            comix_14();
            level = 85;
            comixTime = 15.0f;
            break;
        case 14:
            level = 92;
            comix_15();
            comixTime = 14.0f;
            break;
        case 16:
            comix_1_end();
            level = 12;
            comixTime = 4.5f;
            break;
        case 17:
            comix_3_end();
            level = 24;
            comixTime = 10.5f;
            break;
        case 18:
            comix_4_end();
            level = 36;
            comixTime = 10.0f;
            break;
        case 19:
            comix_5_end();
            level = 48;
            comixTime = 8.5f;
            break;
        case 20:
            comix_6_end();
            level = 60;
            comixTime = 8.0f;
            break;
        case 21:
            comix_7_end();
            level = 72;
            comixTime = 11.0f;
            break;
        case 22:
            comix_8_end();
            level = 84;
            comixTime = 8.5f;
            break;
    }
    
    this->schedule(schedule_selector(ComixScene::update));
    return true;
}

void ComixScene::update(float delta)
{
    if (!pause)
    {
        time+=delta;
        if (time >= (comixTime - 1.0f))
            skipCallback(this);
    }
}

/*
 "1. Hi! Before we can depart to the Ice Cream Land we have to prepare for journey!
 Ready! Let's not waste any time! Off we go!
 
 2. Woah! Isn't it a forest of ice cream? Let's see what is in there!
 Finally we were able to find an exit!
 
 3. How can we traverse those mountains? I will help you if you manage to complete my trials!
 I think we did it! Yes, you did it, you can now pass!
 
 4. This looks like a ship closing in! I need berries for my ice cream! Get them for me!
 Sure we will help you! Hey, we got your berries! Thank you, you can now be on your way!
 
 5. Now we can take our time to relax on the Caramel Meadows!
 That's all, it is time to go!
 
 6. Ouch. Seems like he is cursed! We have to help him!
 Ice should melt! Thank you guys, now I am finally free!"
 */

void ComixScene::pauseComix(CCNode* sender)
{
    pause = true;
    for (int i = 0; i < comix->getChildrenCount(); i++)
        ((CCSkeletonAnimation*)comix->getChildren()->objectAtIndex(i))->pauseSchedulerAndActions();
    
    if (text)
    {
        text->removeFromParentAndCleanup(true);
        text = NULL;
    }
    text = CCLabelTTF::create("", FONT_COMMON, FONT_SIZE_40);
    text->setColor(ccBLACK);
    if (!IPAD)
    {
        text->setFontSize(50);
    }
    
    next->setVisible(true);
    comix->addChild(text, 100);
    if (comixNumber == 1)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(1220, 790));
            text->setString(CCLocalizedString("COMIX_1_1", NULL));
            text->runAction(CCFadeIn::create(1.0f));
        }
    }
    else if (comixNumber == 16)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(1340, 840));
            text->setString(CCLocalizedString("COMIX_1_2", NULL));
            text->runAction(CCFadeIn::create(1.0f));
        }
    }
    else if (comixNumber == 2)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(1000, 1020));
            text->setString(CCLocalizedString("COMIX_2_1", NULL));
            text->runAction(CCFadeIn::create(1.0f));
        }
    }
    else if (comixNumber == 17)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(1200, 1150));
            text->setString(CCLocalizedString("COMIX_2_2", NULL));
            text->runAction(CCFadeIn::create(1.0f));
        }
    }
    else if (comixNumber == 3)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(730, 1000));
            text->setString(CCLocalizedString("COMIX_3_1", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 1)
        {
            text->setPosition(ccp(700, 1650));
            text->setString(CCLocalizedString("COMIX_3_2", NULL));
            text->runAction(CCFadeIn::create(1.0f));
        }
    }
    else if (comixNumber == 18)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(730, 1000));
            text->setString(CCLocalizedString("COMIX_3_3", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 1)
        {
            text->setPosition(ccp(700, 1650));
            text->setString(CCLocalizedString("COMIX_3_4", NULL));
            text->runAction(CCFadeIn::create(1.0f));
        }
    }
    else if (comixNumber == 4)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(540, 950));
            text->setString(CCLocalizedString("COMIX_4_1", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 1)
        {
            text->setPosition(ccp(1200, 1450));
            text->setString(CCLocalizedString("COMIX_4_2", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 2)
        {
            text->setPosition(ccp(530, 950));
            text->setString(CCLocalizedString("COMIX_4_3", NULL));
            text->runAction(CCFadeIn::create(1.0f));
        }
    }
    else if (comixNumber == 19)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(550, 950));
            text->setString(CCLocalizedString("COMIX_4_4", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 1)
        {
            text->setPosition(ccp(1200, 1450));
            text->setString(CCLocalizedString("COMIX_4_5", NULL));
            text->runAction(CCFadeIn::create(1.0f));
        }
    }
    
    else if (comixNumber == 5)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(790, 1340));
            text->setString(CCLocalizedString("COMIX_5_1", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
    }
    else if (comixNumber == 20)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(790, 1340));
            text->setString(CCLocalizedString("COMIX_5_2", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
    }
    
    else if (comixNumber == 6)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(1100, 640));
            text->setString("What happened?");
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 1)
        {
            text->setPosition(ccp(1100, 1450));
            text->setString("The ice has almost melted!\n we need the ice for the rink!");
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 2)
        {
            text->setPosition(ccp(400, 700));
            text->setString("Ok! I think,\n we can help you!");
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
    }
    else if (comixNumber == 21)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(1100, 640));
            text->setString("Now you have\n a rink again");
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 1)
        {
            text->setPosition(ccp(1100, 1450));
            text->setString("Thank you guys!\n Good Luck!");
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
    }
    
    else if (comixNumber == 7)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(500, 950));
            text->setString(CCLocalizedString("COMIX_6_1", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 1)
        {
            text->setPosition(ccp(500, 950));
            text->setString(CCLocalizedString("COMIX_6_2", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
    }
    else if (comixNumber == 22)
    {
        if (pauseNumber == 0)
        {
            text->setPosition(ccp(500, 950));
            text->setString(CCLocalizedString("COMIX_6_3", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
        else if (pauseNumber == 1)
        {
            text->setPosition(ccp(1150, 1190));
            text->setString(CCLocalizedString("COMIX_6_4", NULL));
            text->runAction(CCFadeIn::create(1.0f));
            pauseNumber++;
        }
    }
}

void ComixScene::iceCookieHide(CCNode* sender)
{
    iceCookie->setVisible(false);
}

void ComixScene::resumeComix(CCNode* sender)
{
    pause = false;
    for (int i = 0; i < comix->getChildrenCount(); i++)
        ((CCSkeletonAnimation*)comix->getChildren()->objectAtIndex(i))->resumeSchedulerAndActions();
    next->setVisible(false);
 
    if (comixNumber == 1)
        text->runAction(CCFadeOut::create(1.0f));
    if (comixNumber == 22 && pauseNumber == 1)
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::iceCookieHide))));
    if (nextBubbleTime > 0.0f)
    {
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(nextBubbleTime), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
        if (comixNumber == 4 && nextBubbleTime == 4.0f)
            nextBubbleTime = 3.0f;
        else if (comixNumber == 5 && nextBubbleTime == 4.0f)
            nextBubbleTime = 3.0f;
        else if (comixNumber == 6 && nextBubbleTime == 3.5f)
            nextBubbleTime = 3.0f;
        else if (comixNumber == 8 && nextBubbleTime == 3.5f)
            nextBubbleTime = 3.5f;
        else if (comixNumber == 9 && nextBubbleTime == 3.0f)
            nextBubbleTime = 3.5f;
        else if (comixNumber == 10 && nextBubbleTime == 3.0f)
            nextBubbleTime = 3.5f;
        else if (comixNumber == 14 && nextBubbleTime == 2.5f)
            nextBubbleTime = 3.0f;
        else
            nextBubbleTime = -1.0f;
    }
    
    text->setVisible(false);
}

void ComixScene::changeOrientation()
{
    this->setContentSize(WINSIZE);
    menu->setContentSize(WINSIZE);
    menu->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height/2.0f));
    next->setPosition(CCDirector::sharedDirector()->getWinSize().width/3.0f, -CCDirector::sharedDirector()->getWinSize().height/2.4f);
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/3.0f, -CCDirector::sharedDirector()->getWinSize().height/2.4f);
    
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));

        borderLeft->setRotation(90.0f);
        borderRight->setRotation(90.0f);
        borderLeft->setPosition(ccp(borderLeft->getContentSize().height/2.0f, WINSIZE.height/2.0f));
        borderRight->setPosition(ccp(WINSIZE.width - borderLeft->getContentSize().height/2.0f, WINSIZE.height/2.0f));
        
        if (IPHONE_5)
        {
            borderLeft->setScale(2.1f);
            borderRight->setScale(2.1f);
        }
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
        
        borderLeft->setRotation(0.0f);
        borderRight->setRotation(0.0f);
        borderLeft->setPosition(ccp(WINSIZE.width/2.0f, WINSIZE.height - borderLeft->getContentSize().height/2.0f));
        borderRight->setPosition(ccp(WINSIZE.width/2.0f, borderLeft->getContentSize().height/2.0f));
        
        if (IPHONE_5)
        {
            borderLeft->setScale(2.1f);
            borderRight->setScale(2.1f);
        }
    }    
}

void ComixScene::comix_1()
{
    float x = 650;
    float y = 65;
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/1/1 boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 2);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/1/icecream.json", "comix/1/icecream.atlas");
    icecream->addAnimation("walk-copy", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream,2);
    
    CCSkeletonAnimation* arials = CCSkeletonAnimation::createWithFile("comix/1/arials.json", "comix/1/arials.atlas");
    arials->addAnimation("animation", false);
    arials->setPosition(ccp(x, y));
    comix->addChild(arials, 3);
    
    bubbles = CCSkeletonAnimation::createWithFile("comix/1/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 2);
 
    CCSkeletonAnimation* zad = CCSkeletonAnimation::createWithFile("comix/1/zad.json", "comix/1/zad.atlas");
    zad->addAnimation("animation", false);
    zad->setPosition(ccp(x, y));
    comix->addChild(zad);
   
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/1/front.json", "comix/1/front.atlas");
    front->addAnimation("animation", false);
    front->setPosition(ccp(x, y));
    comix->addChild(front);
    
    this->addChild(comix, 10);
    

    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
}

void ComixScene::comix_1_end()
{
    float x = 650;
    float y = 65;
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/1/1 boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy2", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 2);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/1/icecream.json", "comix/1/icecream.atlas");
    icecream->addAnimation("walk-copy2", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream,2);
    
    CCSkeletonAnimation* arials = CCSkeletonAnimation::createWithFile("comix/1/arials.json", "comix/1/arials.atlas");
    arials->addAnimation("animation-copy", false);
    arials->setPosition(ccp(x, y));
    comix->addChild(arials, 3);
    
    bubbles = CCSkeletonAnimation::createWithFile("comix/1/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation-copy", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 2);
    
    CCSkeletonAnimation* zad = CCSkeletonAnimation::createWithFile("comix/1/zad.json", "comix/1/zad.atlas");
    zad->addAnimation("animation-copy", false);
    zad->setPosition(ccp(x, y));
    comix->addChild(zad);
    
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/1/front.json", "comix/1/front.atlas");
    front->addAnimation("animation-copy", false);
    front->setPosition(ccp(x, y));
    comix->addChild(front);
    
    this->addChild(comix, 10);
    
    
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
}

void ComixScene::comix_2()
{
    float x = 650;
    float y = 65;
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/2/begin/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy2", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 2);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/2/begin/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk-copy", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream,2);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/2/begin/bubbles.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation-copy", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 2);
    
    CCSkeletonAnimation* skeleton_copy = CCSkeletonAnimation::createWithFile("comix/2/begin/skeleton-copy.json", "comix/2/skeleton-copy.atlas");
    skeleton_copy->addAnimation("animation", false);
    skeleton_copy->setPosition(ccp(x, y));
    comix->addChild(skeleton_copy, 1);
    
    CCSkeletonAnimation* top = CCSkeletonAnimation::createWithFile("comix/2/begin/top.json", "comix/2/top.atlas");
    top->addAnimation("animation", false);
    top->setPosition(ccp(x, y));
    comix->addChild(top, 3);
    
    this->addChild(comix);

    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(4.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
}

void ComixScene::comix_2_end()
{
    float x = 650;
    float y = 65;
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/2/begin/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy2", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 2);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/2/begin/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk-copy", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream,2);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/2/begin/bubbles.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation-copy", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 2);
    
    CCSkeletonAnimation* skeleton_copy = CCSkeletonAnimation::createWithFile("comix/2/begin/skeleton-copy.json", "comix/2/skeleton-copy.atlas");
    skeleton_copy->addAnimation("animation", false);
    skeleton_copy->setPosition(ccp(x, y));
    comix->addChild(skeleton_copy, 1);
    
    CCSkeletonAnimation* top = CCSkeletonAnimation::createWithFile("comix/2/begin/top.json", "comix/2/top.atlas");
    top->addAnimation("animation", false);
    top->setPosition(ccp(x, y));
    comix->addChild(top, 3);
    
    this->addChild(comix);
    
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(4.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
}


void ComixScene::comix_3()
{
    float x = 640;
    float y = 65;
    
    comix = CCLayer::create();
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/3/begin/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy2", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 2);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/3/begin/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk-copy", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream,2);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/3/begin/bubbles.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation-copy", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 2);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/3/begin/back.json", "comix/3/begin/back.atlas");
    back->addAnimation("animation", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 1);
    
/*    CCSkeletonAnimation* forward = CCSkeletonAnimation::createWithFile("comix/3/begin/top.json", "comix/3/begin/top.atlas");
    forward->addAnimation("animation", false);
    comix->addChild(forward, 4);*/
    
    
    this->addChild(comix);
    
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(4.7f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
}

void ComixScene::comix_3_end()
{
    float x = 550;
    float y = 130;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/3/end/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk out", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 3);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/3/end/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk out", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream,2);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/3/end/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 2);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/3/end/back.json", "comix/3/end/back.atlas");
    back->addAnimation("animation", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/3/end/front.json", "comix/3/end/front.atlas");
    front->addAnimation("animation", false);
    front->setPosition(ccp(x, y));
    comix->addChild(front, 4);
    
    
    this->addChild(comix);
    
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(6.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
}


void ComixScene::comix_4()
{
    float x = 580;
    float y = 100;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/4/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/4/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/4/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* bubbles2 = CCSkeletonAnimation::createWithFile("comix/4/bubbles-copy.json", "comix/bubbles.atlas");
    bubbles2->addAnimation("animation-copy", false);
    bubbles2->setPosition(ccp(x, y));
    comix->addChild(bubbles2, 4);
    
    CCSkeletonAnimation* bubbles3 = CCSkeletonAnimation::createWithFile("comix/4/bubbles.json", "comix/bubbles.atlas");
    bubbles3->addAnimation("animation-copy", false);
    bubbles3->setPosition(ccp(x, y));
    comix->addChild(bubbles3, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/4/back.json", "comix/4/back.atlas");
    back->addAnimation("animation", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* giant = CCSkeletonAnimation::createWithFile("comix/4/giant.json", "comix/4/giant.atlas");
    giant->addAnimation("animation", false);
    giant->setPosition(ccp(x, y));
    comix->addChild(giant, 2);
    
    CCSkeletonAnimation* mountain = CCSkeletonAnimation::createWithFile("comix/4/mountains forward.json", "comix/4/mountains forward.atlas");
    mountain->addAnimation("animation", false);
    mountain->setPosition(ccp(x, y));
    comix->addChild(mountain, 3);
    
    
    
    this->addChild(comix);
    
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(4.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 5.0f;
}

void ComixScene::comix_4_end()
{
    float x = 580;
    float y = 100;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/4/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy2", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/4/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk-copy", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/4/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation-copy", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* bubbles2 = CCSkeletonAnimation::createWithFile("comix/4/bubbles-copy.json", "comix/bubbles.atlas");
    bubbles2->addAnimation("animation-copy2", false);
    bubbles2->setPosition(ccp(x, y));
    comix->addChild(bubbles2, 4);
    
    CCSkeletonAnimation* bubbles3 = CCSkeletonAnimation::createWithFile("comix/4/bubbles.json", "comix/bubbles.atlas");
    bubbles3->addAnimation("animation-copy2", false);
    bubbles3->setPosition(ccp(x, y));
    comix->addChild(bubbles3, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/4/back.json", "comix/4/back.atlas");
    back->addAnimation("animation-copy", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* giant = CCSkeletonAnimation::createWithFile("comix/4/giant.json", "comix/4/giant.atlas");
    giant->addAnimation("animation-copy", false);
    giant->setPosition(ccp(x, y));
    comix->addChild(giant, 2);
    
    CCSkeletonAnimation* mountain = CCSkeletonAnimation::createWithFile("comix/4/mountains forward.json", "comix/4/mountains forward.atlas");
    mountain->addAnimation("animation-copy", false);
    mountain->setPosition(ccp(x, y));
    comix->addChild(mountain, 3);
    
    this->addChild(comix);
    
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 3.0f;
}


void ComixScene::comix_5()
{
    float x = 500;
    float y = 135;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));

    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/5/boy.json", "comix/boy.atlas");
    boy->addAnimation("stay in boat", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/5/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("stay in boat", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/5/bubbles boy.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* boat = CCSkeletonAnimation::createWithFile("comix/5/boat.json", "comix/5/boat.atlas");
    boat->addAnimation("animation", false);
    boat->setPosition(ccp(x, y));
    comix->addChild(boat, 2);
    
    CCSkeletonAnimation* pirate = CCSkeletonAnimation::createWithFile("comix/5/pirate cookies.json", "comix/5/pirate cookies.atlas");
    pirate->addAnimation("animation", false);
    pirate->setPosition(ccp(x, y));
    comix->addChild(pirate, 3);
    
    CCSkeletonAnimation* ship = CCSkeletonAnimation::createWithFile("comix/5/ship.json", "comix/5/ship.atlas");
    ship->addAnimation("animation", false);
    ship->setPosition(ccp(x, y));
    comix->addChild(ship, 1);
    
    
    this->addChild(comix);
    
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(4.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 4.0f;
}

void ComixScene::comix_5_end()
{
    float x = 500;
    float y = 135;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/5/boy.json", "comix/boy.atlas");
    boy->addAnimation("stay in boat-copy", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/5/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("stay in boat-copy", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/5/bubbles boy.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation-copy", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* boat = CCSkeletonAnimation::createWithFile("comix/5/boat.json", "comix/5/boat.atlas");
    boat->addAnimation("animation-copy", false);
    boat->setPosition(ccp(x, y));
    comix->addChild(boat, 2);
    
    CCSkeletonAnimation* pirate = CCSkeletonAnimation::createWithFile("comix/5/pirate cookies.json", "comix/5/pirate cookies.atlas");
    pirate->addAnimation("animation-copy", false);
    pirate->setPosition(ccp(x, y));
    comix->addChild(pirate, 3);
    
    CCSkeletonAnimation* ship = CCSkeletonAnimation::createWithFile("comix/5/ship.json", "comix/5/ship.atlas");
    ship->addAnimation("animation-copy", false);
    ship->setPosition(ccp(x, y));
    comix->addChild(ship, 1);
    
    
    this->addChild(comix);
    
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 2.5f;
}


void ComixScene::comix_6()
{
    float x = 610;
    float y = 185;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));

    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/6/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk rest 2", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/6/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk rest", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/6/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/6/back.json", "comix/6/back.atlas");
    back->addAnimation("animation", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 2);
    
    CCSkeletonAnimation* tap = CCSkeletonAnimation::createWithFile("comix/6/tap front.json", "comix/6/tap front.atlas");
    tap->addAnimation("animation", false);
    tap->setPosition(ccp(x, y));
    comix->addChild(tap, 5);
    
    
    this->addChild(comix);
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(6.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 3.5f;
}

void ComixScene::comix_6_end()
{
    float x = 610;
    float y = 185;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/6/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk rest 2-copy", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/6/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk rest-copy", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/6/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation-copy", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/6/back.json", "comix/6/back.atlas");
    back->addAnimation("animation-copy", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 2);
    
    CCSkeletonAnimation* tap = CCSkeletonAnimation::createWithFile("comix/6/tap front.json", "comix/6/tap front.atlas");
    tap->addAnimation("animation-copy", false);
    tap->setPosition(ccp(x, y));
    comix->addChild(tap, 5);
    
    
    this->addChild(comix);
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
}


void ComixScene::comix_7()
{
    float x = 470;
    float y = 175;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/7/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk slide", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/7/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk slide", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/7/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/7/back.json", "comix/7/back.atlas");
    back->addAnimation("animation", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* backCow = CCSkeletonAnimation::createWithFile("comix/7/back cow.json", "comix/7/back cow.atlas");
    backCow->addAnimation("animation", false);
    backCow->setPosition(ccp(x, y));
    comix->addChild(backCow, 2);
    
    CCSkeletonAnimation* frontCow = CCSkeletonAnimation::createWithFile("comix/7/front cows.json", "comix/7/front cows.atlas");
    frontCow->addAnimation("animation", false);
    frontCow->setPosition(ccp(x, y));
    comix->addChild(frontCow, 3);
    
    
    this->addChild(comix);
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(6.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 3.5f;
}

void ComixScene::comix_7_end()
{
    float x = 470;
    float y = 175;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/7/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk slide-copy", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/7/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk slide-copy", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/7/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation-copy", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/7/back.json", "comix/7/back.atlas");
    back->addAnimation("animation-copy", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* backCow = CCSkeletonAnimation::createWithFile("comix/7/back cow.json", "comix/7/back cow.atlas");
    backCow->addAnimation("animation-copy", false);
    backCow->setPosition(ccp(x, y));
    comix->addChild(backCow, 2);
    
    CCSkeletonAnimation* frontCow = CCSkeletonAnimation::createWithFile("comix/7/front cows.json", "comix/7/front cows.atlas");
    frontCow->addAnimation("animation-copy", false);
    frontCow->setPosition(ccp(x, y));
    comix->addChild(frontCow, 3);
    
    
    this->addChild(comix);
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 3.5f;
}


void ComixScene::comix_8()
{
    float x = 520;
    float y = 125;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/8/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/8/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk on", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/8/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/8/back.json", "comix/8/back.atlas");
    back->addAnimation("animation", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* cookie = CCSkeletonAnimation::createWithFile("comix/8/cookie.json", "comix/8/cookie.atlas");
    cookie->addAnimation("animation-copy", false);
    cookie->setPosition(ccp(x, y));
    comix->addChild(cookie, 3);

    CCSkeletonAnimation* iceCookie = CCSkeletonAnimation::createWithFile("comix/8/ice cookie.json", "comix/8/ice cookie.atlas");
    iceCookie->addAnimation("animation-copy", false);
    iceCookie->setPosition(ccp(x, y));
    comix->addChild(iceCookie, 3);
    
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/8/front.json", "comix/8/front.atlas");
    front->addAnimation("animation", false);
    front->setPosition(ccp(x, y));
    comix->addChild(front, 5);
    
    
    this->addChild(comix);
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 3.5f;
}

void ComixScene::comix_8_end()
{
    float x = 520;
    float y = 125;
    
    comix = CCLayerColor::create(ccc4(0, 0, 0, 255));
    comix->setAnchorPoint(ccp(0.0f, 0.0f));
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/8/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy2", false);
    boy->setPosition(ccp(x, y));
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/8/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk on-copy", false);
    icecream->setPosition(ccp(x, y));
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/8/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation-copy", false);
    bubbles->setPosition(ccp(x, y));
    comix->addChild(bubbles, 6);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/8/back.json", "comix/8/back.atlas");
    back->addAnimation("animation-copy", false);
    back->setPosition(ccp(x, y));
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* cookie = CCSkeletonAnimation::createWithFile("comix/8/cookie.json", "comix/8/cookie.atlas");
     cookie->addAnimation("animation-copy2", false);
     cookie->setPosition(ccp(x, y));
     comix->addChild(cookie, 3);
     
     iceCookie = CCSkeletonAnimation::createWithFile("comix/8/ice cookie.json", "comix/8/ice cookie.atlas");
     iceCookie->addAnimation("animation-copy2", false);
     iceCookie->setPosition(ccp(x, y));
     comix->addChild(iceCookie, 3);
    
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/8/front.json", "comix/8/front.atlas");
    front->addAnimation("animation", false);
    front->setPosition(ccp(x, y));
    comix->addChild(front, 5);
    
    
    this->addChild(comix);
    if (LANDSCAPE)
    {
        float scale = WINSIZE.height/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp((WINSIZE.width - 2048.0f*scale)/2.0f, 0.0f));
    }
    else
    {
        float scale = WINSIZE.width/2048.0f + COMIX_SCALE;
        comix->setScale(scale);
        comix->setPosition(ccp(0.0f, (WINSIZE.height - 2048.0f*scale)/2.0f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 4.0f;
}


void ComixScene::comix_9()
{
    skipCallback(NULL);
    return;
    comix = CCLayer::create();
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/9/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy", false);
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/9/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk", false);
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/9/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/9/back.json", "comix/9/back.atlas");
    back->addAnimation("animation", false);
    comix->addChild(back, 1);
    
 /*   CCSkeletonAnimation* cookie = CCSkeletonAnimation::createWithFile("comix/9/cookie.json", "comix/9/cookie.atlas");
    cookie->addAnimation("animation-copy2", false);
    comix->addChild(cookie, 3);
    
    CCSkeletonAnimation* cookie2 = CCSkeletonAnimation::createWithFile("comix/9/cookie-copy.json", "comix/9/cookie-copy.atlas");
    cookie2->addAnimation("animation-copy2", false);
    comix->addChild(cookie2, 3);
   */
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/9/front fountain.json", "comix/9/front fountain.atlas");
    front->addAnimation("animation", false);
    comix->addChild(front, 3);
    
    
    this->addChild(comix);
    if (WINSIZE.height < WINSIZE.width && (WINSIZE.height == 768 || WINSIZE.height == 640))
    {
        comix->setAnchorPoint(ccp(0.0f, 0.0f));
        comix->setPosition(ccp(0.0f, 0.0f));
        comix->setScale(0.5f);
    }
    
    if (WINSIZE.height == 1024)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/1.9f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.7f));
    }
    else if (WINSIZE.height == 1136)
    {
        comix->setScale(0.6f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.5f,
                               -CCDirector::sharedDirector()->getWinSize().height/3.0f));
    }
    else if (WINSIZE.height == 960)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.0f));
    }
    else
    {
        comix->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/4.0f,
                               CCDirector::sharedDirector()->getWinSize().height/27.3f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 3.0f;
}

void ComixScene::comix_10()
{
    skipCallback(NULL);
    return;
    comix = CCLayer::create();
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/10/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy", false);
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/10/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk", false);
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/10/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/10/back.json", "comix/10/back.atlas");
    back->addAnimation("animation", false);
    comix->addChild(back, 1);
    /*
    CCSkeletonAnimation* cookie = CCSkeletonAnimation::createWithFile("comix/10/cookie.json", "comix/10/cookie.atlas");
    cookie->addAnimation("animation-copy2", false);
    comix->addChild(cookie, 3);
    */
    CCSkeletonAnimation* car = CCSkeletonAnimation::createWithFile("comix/10/car.json", "comix/10/car.atlas");
    car->addAnimation("animation", false);
    comix->addChild(car, 3);
    
    
    this->addChild(comix);
    
    if (WINSIZE.height < WINSIZE.width && (WINSIZE.height == 768 || WINSIZE.height == 640))
    {
        comix->setAnchorPoint(ccp(0.0f, 0.0f));
        comix->setPosition(ccp(0.0f, 0.0f));
        comix->setScale(0.5f);
    }

    if (WINSIZE.height == 1024)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/1.9f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.3f));
    }
    else if (WINSIZE.height == 1136)
    {
        comix->setScale(0.6f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.5f,
                               -CCDirector::sharedDirector()->getWinSize().height/3.0f));
    }
    else if (WINSIZE.height == 960)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.0f));
    }
    else
    {
        comix->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/4.0f,
                               CCDirector::sharedDirector()->getWinSize().height/27.3f));
    }
    
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 3.0f;
}

void ComixScene::comix_11()
{
    skipCallback(NULL);
    return;
    comix = CCLayer::create();
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/11/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk to light", false);
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/11/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk to light", false);
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/11/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    comix->addChild(bubbles, 4);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/11/back.json", "comix/11/back.atlas");
    back->addAnimation("animation", false);
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* bird = CCSkeletonAnimation::createWithFile("comix/11/bird.json", "comix/11/bird.atlas");
    bird->addAnimation("animation", false);
    comix->addChild(bird, 2);
     
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/11/front.json", "comix/11/front.atlas");
    front->addAnimation("animation", false);
    comix->addChild(front, 3);
    
    
    this->addChild(comix);
    
    if (WINSIZE.height < WINSIZE.width && (WINSIZE.height == 768 || WINSIZE.height == 640))
    {
        comix->setAnchorPoint(ccp(0.0f, 0.0f));
        comix->setPosition(ccp(0.0f, 0.0f));
        comix->setScale(0.5f);
    }

    
    if (WINSIZE.height == 1024)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/1.9f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.3f));
    }
    else if (WINSIZE.height == 1136)
    {
        comix->setScale(0.6f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.5f,
                               -CCDirector::sharedDirector()->getWinSize().height/3.0f));
    }
    else if (WINSIZE.height == 960)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.0f));
    }
    else
    {
        comix->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/4.0f,
                               CCDirector::sharedDirector()->getWinSize().height/27.3f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = -1.0f;
}

void ComixScene::comix_12()
{
    skipCallback(NULL);
    return;
    comix = CCLayer::create();
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/12/boy.json", "comix/12/boy.atlas");
    boy->addAnimation("walk swim", false);
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/12/icecream.json", "comix/12/icecream.atlas");
    icecream->addAnimation("walk", false);
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/12/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    comix->addChild(bubbles, 6);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/12/back.json", "comix/12/back.atlas");
    back->addAnimation("animation", false);
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* whale = CCSkeletonAnimation::createWithFile("comix/12/whale.json", "comix/12/whale.atlas");
    whale->addAnimation("animation-copy", false);
    comix->addChild(whale, 5);
    
    CCSkeletonAnimation* whale2 = CCSkeletonAnimation::createWithFile("comix/12/whale 2.json", "comix/12/whale 2.atlas");
    whale2->addAnimation("animation-copy front", false);
    comix->addChild(whale2, 5);
    
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/12/front.json", "comix/12/front.atlas");
    front->addAnimation("animation", false);
    comix->addChild(front, 3);
    
    
    this->addChild(comix);
    
    if (WINSIZE.height < WINSIZE.width && (WINSIZE.height == 768 || WINSIZE.height == 640))
    {
        comix->setAnchorPoint(ccp(0.0f, 0.0f));
        comix->setPosition(ccp(0.0f, 0.0f));
        comix->setScale(0.5f);
    }

    
    if (WINSIZE.height == 1024)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/1.9f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.3f));
    }
    else if (WINSIZE.height == 1136)
    {
        comix->setScale(0.6f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.5f,
                               -CCDirector::sharedDirector()->getWinSize().height/3.0f));
    }
    else if (WINSIZE.height == 960)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.0f));
    }
    else
    {
        comix->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/4.0f,
                               CCDirector::sharedDirector()->getWinSize().height/27.3f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(5.0f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 6.0f;
}

void ComixScene::comix_13()
{
    skipCallback(NULL);
    return;
    comix = CCLayer::create();
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/13/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk tr", false);
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/13/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk tr", false);
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/13/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    comix->addChild(bubbles, 6);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/13/back.json", "comix/13/back.atlas");
    back->addAnimation("animation", false);
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/13/front house.json", "comix/13/front house.atlas");
    front->addAnimation("animation", false);
    comix->addChild(front, 6);
    
    
    this->addChild(comix);
    
    if (WINSIZE.height < WINSIZE.width && (WINSIZE.height == 768 || WINSIZE.height == 640))
    {
        comix->setAnchorPoint(ccp(0.0f, 0.0f));
        comix->setPosition(ccp(0.0f, 0.0f));
        comix->setScale(0.5f);
    }

    
    if (WINSIZE.height == 1024)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/1.9f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.3f));
    }
    else if (WINSIZE.height == 1136)
    {
        comix->setScale(0.6f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.5f,
                               -CCDirector::sharedDirector()->getWinSize().height/3.0f));
    }
    else if (WINSIZE.height == 960)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.0f));
    }
    else
    {
        comix->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/4.0f,
                               CCDirector::sharedDirector()->getWinSize().height/27.3f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = -1.0f;
}

void ComixScene::comix_14()
{
    skipCallback(NULL);
    return;
    comix = CCLayer::create();
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/14/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk in", false);
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/14/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk in", false);
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/14/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    comix->addChild(bubbles, 6);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/14/back.json", "comix/14/back.atlas");
    back->addAnimation("animation", false);
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* guardLeft = CCSkeletonAnimation::createWithFile("comix/14/guard left.json", "comix/14/guard left.atlas");
    guardLeft->addAnimation("animation", false);
    comix->addChild(guardLeft, 3);
    
    CCSkeletonAnimation* guardLeft2 = CCSkeletonAnimation::createWithFile("comix/14/guard left-copy.json", "comix/14/guard left-copy.atlas");
    guardLeft2->addAnimation("animation", false);
    comix->addChild(guardLeft2, 3);
    
    
    this->addChild(comix);
    
    if (WINSIZE.height < WINSIZE.width && (WINSIZE.height == 768 || WINSIZE.height == 640))
    {
        comix->setAnchorPoint(ccp(0.0f, 0.0f));
        comix->setPosition(ccp(0.0f, 0.0f));
        comix->setScale(0.5f);
    }

    
    if (WINSIZE.height == 1024)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/1.9f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.3f));
    }
    else if (WINSIZE.height == 1136)
    {
        comix->setScale(0.6f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.5f,
                               -CCDirector::sharedDirector()->getWinSize().height/3.0f));
    }
    else if (WINSIZE.height == 960)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.0f));
    }
    else
    {
        comix->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/4.0f,
                               CCDirector::sharedDirector()->getWinSize().height/27.3f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 2.5f;
}

void ComixScene::comix_15()
{
    skipCallback(NULL);
    return;
    comix = CCLayer::create();
    comix->setContentSize(CCSize(2048, 2048));
    
    CCSkeletonAnimation* boy = CCSkeletonAnimation::createWithFile("comix/15/boy.json", "comix/boy.atlas");
    boy->addAnimation("walk-copy", false);
    comix->addChild(boy, 4);
    
    CCSkeletonAnimation* icecream = CCSkeletonAnimation::createWithFile("comix/15/icecream.json", "comix/icecream.atlas");
    icecream->addAnimation("walk", false);
    comix->addChild(icecream, 4);
    
    CCSkeletonAnimation* bubbles = CCSkeletonAnimation::createWithFile("comix/15/bubbles-copy2.json", "comix/bubbles.atlas");
    bubbles->addAnimation("animation", false);
    comix->addChild(bubbles, 6);
    
    CCSkeletonAnimation* back = CCSkeletonAnimation::createWithFile("comix/15/back.json", "comix/15/back.atlas");
    back->addAnimation("animation", false);
    comix->addChild(back, 1);
    
    CCSkeletonAnimation* front = CCSkeletonAnimation::createWithFile("comix/15/front.json", "comix/15/front.atlas");
    front->addAnimation("animation", false);
    comix->addChild(front, 5);
    
    
    
    this->addChild(comix);
    
    if (WINSIZE.height < WINSIZE.width && (WINSIZE.height == 768 || WINSIZE.height == 640))
    {
        comix->setAnchorPoint(ccp(0.0f, 0.0f));
        comix->setPosition(ccp(0.0f, 0.0f));
        comix->setScale(0.5f);
    }

    
    if (WINSIZE.height == 1024 || WINSIZE.height == 640)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/1.9f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.3f));
    }
    else if (WINSIZE.height == 1136)
    {
        comix->setScale(0.6f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.5f,
                               -CCDirector::sharedDirector()->getWinSize().height/3.0f));
    }
    else if (WINSIZE.height == 960)
    {
        comix->setScale(0.5f);
        comix->setPosition(ccp(-CCDirector::sharedDirector()->getWinSize().width/2.0f,
                               -CCDirector::sharedDirector()->getWinSize().height/2.0f));
    }
    else
    {
        comix->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/4.0f,
                               CCDirector::sharedDirector()->getWinSize().height/27.3f));
    }
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(5.5f), CCCallFuncN::create(this, callfuncN_selector(ComixScene::pauseComix))));
    nextBubbleTime = 2.0f;
}

void ComixScene::skipCallback(CCObject* pSender)
{
    if (comixNumber == 16)
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(13));
    else if (comixNumber == 17)
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(25));
    else if (comixNumber == 18)
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(37));
    else if (comixNumber == 19)
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(49));
    else if (comixNumber == 20)
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(61));
    else if (comixNumber == 21)
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(73));
    else if (comixNumber == 22)
        CCDirector::sharedDirector()->replaceScene(GameMapLayer::scene(85));
    else
        CCDirector::sharedDirector()->replaceScene(GameScene::scene(level));
}

void ComixScene::nextCallback(CCObject* pSender)
{
    resumeComix(this);
}