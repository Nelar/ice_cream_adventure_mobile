#include "cGameObject.h"
#include "cGlobal.h"
#include "SimpleAudioEngine.h"
#include "CCSinMoveTo.h"
#include <spine/spine.h>
#include <spine/spine-cocos2dx.h>

using namespace cocos2d;
using namespace CocosDenshion;
using namespace extension;

sGameObject::sGameObject() : type(Simple), color(Red), x(0), y(0), sprite(NULL), prepareToDie(false), isSelected(false), isLock(false)
{

};

sGameObject::sGameObject(eTypeGameObject nType, eColorGameObject nColor, int nX, int nY, int nXZero, int nYZero)
{
    isScore = false;
	type = nType;
	color = nColor;
	x = nX;
	y = nY;
	prepareToDie = false;
	isSelected = false;
	isLock = false;
    isZombieBomb = false;
    isMegaBombHorizontal = false;
    isMegaBombVertical = false;
    isStrawberry = false;
    delayDestroy = 0.0f;
    isMegaBomb = false;
    delayStripe = 0.0f;
    isTime = false;

	xZero = nXZero;
	yZero = nYZero;

	switch (nColor)
	{
	case Blue:
		sprite = CCSprite::createWithSpriteFrameName("game/shadow/blue.png");
		break;
	case Green:
		sprite = CCSprite::createWithSpriteFrameName("game/shadow/green.png");
		break;
	case Orange:
		sprite = CCSprite::createWithSpriteFrameName("game/shadow/orange.png");
		break;
	case Puprle:
		sprite = CCSprite::createWithSpriteFrameName("game/shadow/violet.png");
		break;
	case Red:
		sprite = CCSprite::createWithSpriteFrameName("game/shadow/red.png");
		break;
	case Yellow:
//		sprite = CCSprite::createWithSpriteFrameName("game/shadow/yellow.png");
        sprite = CCSprite::create("updateArt/yellow.png");

		break;
	}

	if (nType == Cookie)
		sprite = CCSprite::createWithSpriteFrameName("game/shadow/klubnika.png");

	sprite->setPosition(ccp(y*CELL_WIDTH + xZero, yZero - x*CELL_HEIGHT));
    sprite->setAnchorPoint(ccp(0.55f, 0.42f));
	sprite->setScale(ELEMENT_SCALE);
    
    node = new ClipNode();
    node->addChild(sprite);
    node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, WINSIZE.height));
    
	crystalState = 0;
}

void sGameObject::setTime()
{
    CCSprite* labelTime = CCSprite::createWithSpriteFrameName("game/+5.png");
    labelTime->setPosition(ccp(sprite->getContentSize().width/2.0f, sprite->getContentSize().height/2.0f));
    sprite->addChild(labelTime);
    isTime = true;
}

void sGameObject::changeColor(eColorGameObject nColor)
{
	color = nColor;
    
	switch (nColor)
	{
	case Blue:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/blue.png")->displayFrame());
		break;
	case Green:
		sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/green.png")->displayFrame());
		break;
	case Orange:
		sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/orange.png")->displayFrame());
		break;
	case Puprle:
		sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/violet.png")->displayFrame());
		break;
	case Red:
		sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/red.png")->displayFrame());
		break;
	case Yellow:
		//sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/yellow.png")->displayFrame());
        sprite->setDisplayFrame(CCSprite::create("updateArt/yellow.png")->displayFrame());

		break;
    case Colorurless:
        sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/yellow.png")->displayFrame());
        break;
	}
    
    CCSprite* bang = CCSprite::createWithSpriteFrameName("game/krug.png");
    sprite->addChild(bang, sprite->getZOrder() + 1);
    bang->setScale(0.01f);
    bang->setOpacity(120);
    bang->runAction(CCSequence::create(CCScaleTo::create(0.3f, 0.8f),  NULL));
    bang->runAction(CCSequence::create(CCDelayTime::create(0.2f), CCFadeTo::create(0.2f, 0), CCCallFuncN::create(this, callfuncN_selector(sGameObject::cleaningBang)), NULL));
    sprite->setScale(0.01f);
    sprite->runAction(CCSequence::create(CCScaleTo::create(0.3f, ELEMENT_SCALE), NULL));
}

void sGameObject::changeColorCancelled(CCNode* sender)
{
    switch (color)
	{
        case Blue:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/blue.png")->displayFrame());
            break;
        case Green:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/green.png")->displayFrame());
            break;
        case Orange:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/orange.png")->displayFrame());
            break;
        case Puprle:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/violet.png")->displayFrame());
            break;
        case Red:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/red.png")->displayFrame());
            break;
        case Yellow:
//            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/yellow.png")->displayFrame());
            sprite->setDisplayFrame(CCSprite::create("updateArt/yellow.png")->displayFrame());
            break;
	}
}

void sGameObject::changeType(eTypeGameObject nType)
{
    if (type == Bomb)
    {
        sprite->removeAllChildrenWithCleanup(true);
        sprite->stopAllActions();
    }
    
	type = nType;
	string spriteName;
	string colorStr;
	switch (color)
	{
	case Blue:
		{
			colorStr = "blue";
			switch(type)
			{
			case Simple:
				spriteName = "game/shadow/blue.png";
				break;
			case Vertical:
                spriteName = "game/shadow/blue_vertical.png";
                break;
			case Horizontal:
				spriteName = "game/shadow/blue_horizontal.png";
				break;
			case Bomb:
				spriteName = "game/shadow/blue_bomb.png";
				break;
            case Fish:
                spriteName = "game/fishBlue.png";
                break;
			}
		}		
		break;
	case Green:
		{
			colorStr = "green";
			switch(type)
			{
			case Simple:
				spriteName = "game/shadow/green.png";
				break;
			case Vertical:
                spriteName = "game/shadow/green_vertical.png";
                break;
			case Horizontal:
				spriteName = "game/shadow/green_horizontal.png";
				break;
			case Bomb:
				spriteName = "game/shadow/green_bomb.png";
				break;
            case Fish:
                spriteName = "game/fishGreen.png";
                break;
			}
		}		
		break;
	case Orange:
		{
			colorStr = "orange";
			switch(type)
			{
			case Simple:
				spriteName = "game/shadow/orange.png";
				break;
			case Vertical:
                spriteName = "game/shadow/orange_vertical.png";
                break;
			case Horizontal:
				spriteName = "game/shadow/orange_horizontal.png";
				break;
			case Bomb:
				spriteName = "game/shadow/orange_bomb.png";
				break;
            case Fish:
                spriteName = "game/fishOrange.png";
                break;

			}
		}
		break;
	case Puprle:
		colorStr = "purple";
		switch(type)
		{
		case Simple:
			spriteName = "game/shadow/violet.png";
			break;
		case Vertical:
            spriteName = "game/shadow/violet_vertical.png";
            break;
		case Horizontal:
			spriteName = "game/shadow/violet_horizontal.png";
			break;
		case Bomb:
			spriteName = "game/shadow/violet_bomb.png";
			break;
        case Fish:
            spriteName = "game/fishViolet.png";
            break;

		}		
		break;
	case Red:
		{
			colorStr = "red";
			switch(type)
			{
			case Simple:
				spriteName = "game/shadow/red.png";
				break;
			case Vertical:
                spriteName = "game/shadow/red_vertical.png";
                break;
			case Horizontal:
				spriteName = "game/shadow/red_horizontal.png";
				break;
			case Bomb:
				spriteName = "game/shadow/red_bomb.png";
				break;
            case Fish:
                spriteName = "game/fishRed.png";
                break;

			}
		}
		break;
	case Yellow:
		{
			colorStr = "yellow";
			switch(type)
			{
			case Simple:
				spriteName = "game/shadow/yellow.png";
				break;
			case Vertical:
                spriteName = "game/shadow/yellow_vertical.png";
                break;
			case Horizontal:
				spriteName = "game/shadow/yellow_horizontal.png";
				break;
			case Bomb:
				spriteName = "game/shadow/yellow_bomb.png";
				break;
            case Fish:
                spriteName = "game/fishYellow.png";
                break;

			}
		}		
		break;
	}
    
	if (type == Crystal)
	{
		spriteName = "game/crystal.png";
		color = Colorurless;
	}
	else if (type == Cookie)
	{
        if (isStrawberry)
            spriteName = "game/shadow/golubika.png";
        else
            spriteName = "game/shadow/klubnika.png";

        sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName(spriteName.c_str())->displayFrame());
		color = Colorurless;
	}
    else if (type == Fish)
    {
        sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName(spriteName.c_str())->displayFrame());
    }
    else if (type == Bomb && (isZombieBomb || isMegaBomb))
    {
        
    }
    else if (type == Horizontal && color == Yellow)
    {
        sprite->setDisplayFrame(CCSprite::create("updateArt/horizontal.png")->displayFrame());
    }
    else if (type == Vertical && color == Yellow)
    {
        sprite->setDisplayFrame(CCSprite::create("updateArt/vertical.png")->displayFrame());
    }
    else
    {
        sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName(spriteName.c_str())->displayFrame());
    }
    
    if (type == Bomb)
	{
        if (isZombieBomb)
        {
            sprite->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.8f, ELEMENT_SCALE - 0.05f), CCScaleTo::create(0.8f, ELEMENT_SCALE))));
        }
        else if (!isMegaBomb)
        {
            CCSprite* spark = CCSprite::createWithSpriteFrameName("game/spark_1.png");
            sprite->addChild(spark);
            spark->setPosition(ccp(sprite->getContentSize().width / 5.0f, sprite->getContentSize().height/10.0f*8.0f));
            
            CCAnimation *animation = CCAnimation::create();
            
            animation->addSpriteFrame(CCSprite::createWithSpriteFrameName("game/spark_1.png")->displayFrame());
            animation->addSpriteFrame(CCSprite::createWithSpriteFrameName("game/spark_2.png")->displayFrame());
            
            animation->setDelayPerUnit(0.1f);
            animation->setRestoreOriginalFrame(true);
            animation->setLoops(1);
            
            CCAnimate *action = CCAnimate::create(animation);
            spark->runAction(CCRepeatForever::create(action));
            
            sprite->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCScaleTo::create(0.8f, ELEMENT_SCALE + 0.05f), CCScaleTo::create(0.8f, ELEMENT_SCALE))));
        }
	}
    
    if (type == Horizontal)
    {
        stripes.clear();
        switch (color)
        {
            case Blue:
                colorStr = "blue";
                break;
            case Green:
                colorStr = "green";
                break;
            case Orange:
                colorStr = "orange";
                break;
            case Puprle:
                colorStr = "violet";
                break;
            case Red:
                colorStr = "red";
                break;
            case Yellow:
                colorStr = "yellow";
                break;
        }
        
        for (int i = 1; i < 7; i++)
        {
            char buf[255];
            sprintf(buf, "game/stripe/%s_horizontal (%d).png", colorStr.c_str(), i);
            stripes.push_back(CCSprite::createWithSpriteFrameName(buf));
            stripes[stripes.size() - 1]->setVisible(false);
            
            stripes[i - 1]->setPosition(ccp(sprite->getContentSize().width/2.0f, sprite->getContentSize().height/2.0f));
            stripes[i - 1]->setScale(0.33f);
            sprite->addChild(stripes[i - 1]);
        }
    }
    
    if (type == Vertical)
    {
        stripes.clear();
        switch (color)
        {
            case Blue:
                colorStr = "blue";
                break;
            case Green:
                colorStr = "green";
                break;
            case Orange:
                colorStr = "orange";
                break;
            case Puprle:
                colorStr = "violet";
                break;
            case Red:
                colorStr = "red";
                break;
            case Yellow:
                colorStr = "yellow";
                break;
        }
        
        for (int i = 1; i < 7; i++)
        {
            char buf[255];
            sprintf(buf, "game/stripe/%s_vertical (%d).png", colorStr.c_str(), i);
            stripes.push_back(CCSprite::createWithSpriteFrameName(buf));
            stripes[stripes.size() - 1]->setVisible(false);
            
            stripes[i - 1]->setPosition(ccp(sprite->getContentSize().width/2.0f, sprite->getContentSize().height/2.0f));
            stripes[i - 1]->setScale(0.33f);
            sprite->addChild(stripes[i - 1]);
        }
    }

	if (type != Simple)
	{

	}

	if (type == Crystal)
	{
		sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/crystal.png")->displayFrame());
	}
    
    switch(type)
    {
        case Vertical:
            SimpleAudioEngine::sharedEngine()->playEffect("sound/rozhok_created.mp3");
            break;
        case Horizontal:
            SimpleAudioEngine::sharedEngine()->playEffect("sound/rozhok_created.mp3");
            break;
        case Bomb:
            SimpleAudioEngine::sharedEngine()->playEffect("sound/rozhok_created.mp3");
            break;
        case Crystal:
            SimpleAudioEngine::sharedEngine()->playEffect("sound/crystal_created.mp3");
            break;
    }
}

void sGameObject::cleaningBang(CCNode* sender)
{
    sender->removeFromParentAndCleanup(true);
}

void sGameObject::unvisibleObject(CCNode* sender)
{
    ((CCSprite*)sender)->setVisible(false);
}

void sGameObject::destroy(float delayC)
{
    CCNode* grandParent = NULL;
    
    bool shift = false;
    
    
    grandParent = ((CCNode*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));

    parentNext = sprite->getParent();
    sprite->removeFromParentAndCleanup(false);
    grandParent->addChild(sprite, 5);
    
    if (LANDSCAPE && !(isMegaBombHorizontal || isMegaBombVertical))
        sprite->setPosition(ccp(sprite->getPositionX() + CELL_WIDTH*2.0f, sprite->getPositionY() - CELL_HEIGHT/2.0f));
    
	char buf[255];
    float delay = delayDestroy;
	if (type == Simple ||  type == Cookie || type == Crystal)
	{
        delay = delayDestroy;
        sprintf(buf, "sound/pop_%d.mp3", 2 + rand()%12);
        SimpleAudioEngine::sharedEngine()->playEffect(buf);
        
        sprite->removeAllChildrenWithCleanup(true);
        
        CCSprite* bang = CCSprite::createWithSpriteFrameName("game/krug.png");
        sprite->getParent()->addChild(bang, sprite->getZOrder() + 1);
        bang->setPosition(sprite->getPosition());
        bang->setScale(0.01f);
        bang->setOpacity(190);
        bang->runAction(CCSequence::create(CCDelayTime::create(delay), CCScaleTo::create(0.3f, 0.8f),  NULL));
        bang->runAction(CCSequence::create(CCDelayTime::create(delay + 0.2f), CCFadeTo::create(0.2f, 0), CCCallFuncN::create(this, callfuncN_selector(sGameObject::cleaningBang)), NULL));
		this->sprite->runAction(CCSequence::create(CCDelayTime::create(delay), CCScaleTo::create(0.3f, 0.01f), CCFadeOut::create(0.01f), CCCallFuncN::create(this, callfuncN_selector(sGameObject::unvisibleObject)), NULL));
	}
    else if (type == Cookie)
	{
        
    }
    else if (type == Fish)
	{
		CCSprite* bang = CCSprite::createWithSpriteFrameName("game/krug.png");
        sprite->getParent()->addChild(bang, sprite->getZOrder() + 1);
        bang->setPosition(sprite->getPosition());
        bang->setScale(0.01f);
        bang->setOpacity(120);
        bang->runAction(CCSequence::create(CCDelayTime::create(delay), CCScaleTo::create(0.3f, 0.8f),  NULL));
        bang->runAction(CCSequence::create(CCDelayTime::create(delay + 0.2f), CCFadeTo::create(0.2f, 0), CCCallFuncN::create(this,callfuncN_selector(sGameObject::cleaningBang)), NULL));
		this->sprite->runAction(CCSequence::create(CCDelayTime::create(delay), CCScaleTo::create(0.3f, 0.01f), CCFadeOut::create(0.01f), NULL));
        
        CCSkeletonAnimation* fish;
        if (color == Red)
            fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishRed.atlas");
        if (color == Blue)
            fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishBlue.atlas");
        if (color == Green)
            fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishGreen.atlas");
        if (color == Yellow)
            fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishYellow.atlas");
        if (color == Orange)
            fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishOrange.atlas");
        if (color == Puprle)
            fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishViolet.atlas");
        if (color == Colorurless)
            fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishRed.atlas");
        
        fish->setAnimation("animation-copy2", true);

        if (IPAD)
            fish->setScale(0.3f);
        else
            fish->setScale(0.15f);
        
        
        ((CCNode*)sprite->getParent()->getParent())->addChild(fish, 15);
        fish->setPosition(sprite->getPosition());
        fish->setRotation(25.0f);
        fish->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(3.5f, ccp(-WINSIZE.width, 0)), CCFadeOut::create(0.1f)));
    }
	else if (type == Bomb)
	{
        sprite->removeAllChildrenWithCleanup(true);
		if (isMegaBombHorizontal)
		{
            sprite->stopAllActions();
            sprite->unscheduleAllSelectors();
            sprite->setOpacity(255);
            sprite->setVisible(true);
            stripes.clear();
            string colorStr;
            
            switch (color)
            {
                case Blue:
                    colorStr = "blue";
                    break;
                case Green:
                    colorStr = "green";
                    break;
                case Orange:
                    colorStr = "orange";
                    break;
                case Puprle:
                    colorStr = "violet";
                    break;
                case Red:
                    colorStr = "red";
                    break;
                case Yellow:
                    colorStr = "yellow";
                    break;
            }

            float offset = 2;
            for (int i = 1; i < 7; i++)
            {
                char buf[255];
                sprintf(buf, "game/stripe/%s_horizontal (%d).png", colorStr.c_str(), i);
                stripes.push_back(CCSprite::createWithSpriteFrameName(buf));
                stripes[stripes.size() - 1]->setVisible(true);
                stripes[i - 1]->setPosition(ccp(sprite->getContentSize().width/2.0f, sprite->getContentSize().height/2.0f));
                stripes[i - 1]->setAnchorPoint(ccp(0.5f, 0.5f));
                stripes[i - 1]->setScale(0.33f);
                sprite->addChild(stripes[i - 1]);
            }
            char buf[255];
            sprintf(buf, "game/stripes/%sHorizontal.png", colorStr.c_str());
            CCSprite* spriteUp = CCSprite::create(buf);
            spriteUp->setPosition(ccp(sprite->getContentSize().width/2.0f, sprite->getContentSize().height/2.0f));
            spriteUp->setScale(0.33f);
            sprite->addChild(spriteUp, 100);
            
            for (int i = 0; i < stripes.size();i++)
            {
                stripes[i]->setVisible(true);
                stripes[i]->setOpacity(255);
                if (i%2)
                {
                    stripes[i]->setScale(0.33f);
                    stripes[i]->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.0f),
                                                             CCDelayTime::create(0.15f), CCSpawn::createWithTwoActions(CCScaleTo::create(0.4f, 10.0f, 0.6f),CCMoveBy::create(0.7f, ccp(WINSIZE.width*1.5f, 0.0f))), CCFadeOut::create(0.1f), NULL));
                }
                else
                {
                    stripes[i]->setScale(0.33f);
                    stripes[i]->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.0f),
                                                             CCDelayTime::create(0.15f), CCSpawn::createWithTwoActions(CCScaleTo::create(0.4f, 10.0f, 0.6f),CCMoveBy::create(0.7f, ccp(-WINSIZE.width*1.5f, 0.0f))), CCFadeOut::create(0.1f), NULL));
                }
            }
            spriteUp->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.0f),
                                                   CCDelayTime::create(0.15f), CCHide::create(), NULL));
            sprite->setOpacity(0);
		}
        else if (isMegaBombVertical)
		{
            sprite->stopAllActions();
            sprite->setOpacity(255);
            sprite->setVisible(true);
            stripes.clear();
            string colorStr;
            
            switch (color)
            {
                case Blue:
                    colorStr = "blue";
                    break;
                case Green:
                    colorStr = "green";
                    break;
                case Orange:
                    colorStr = "orange";
                    break;
                case Puprle:
                    colorStr = "violet";
                    break;
                case Red:
                    colorStr = "red";
                    break;
                case Yellow:
                    colorStr = "yellow";
                    break;
            }
            
            
            for (int i = 1; i < 7; i++)
            {
                char buf[255];
                sprintf(buf, "game/stripe/%s_vertical (%d).png", colorStr.c_str(), i);
                stripes.push_back(CCSprite::createWithSpriteFrameName(buf));
                sprite->addChild(stripes[i - 1]);
                stripes[stripes.size() - 1]->setVisible(false);
                stripes[i - 1]->setPosition(ccp(sprite->getContentSize().width/2.0f, sprite->getContentSize().height/2.0f));
                stripes[i - 1]->setAnchorPoint(ccp(0.5f, 0.5f));
                stripes[i - 1]->setScale(0.33f);
            }
            
            char buf[255];
            sprintf(buf, "game/stripes/%sVertical.png", colorStr.c_str());
            CCSprite* spriteUp = CCSprite::create(buf);
            spriteUp->setPosition(ccp(sprite->getContentSize().width/2.0f, sprite->getContentSize().height/2.0f));
            spriteUp->setScale(0.33f);
            sprite->addChild(spriteUp, 100);

            
            for (int i = 0; i < stripes.size();i++)
            {
                stripes[i]->setZOrder(100);
                stripes[i]->setVisible(true);
                stripes[i]->setOpacity(255);
                if (i%2)
                {
                    stripes[i]->setScale(0.33f);
                    stripes[i]->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.0f),
                                                             CCDelayTime::create(0.15f), CCSpawn::createWithTwoActions(CCScaleTo::create(0.4f, 0.6f, 10.0f),CCMoveBy::create(0.7f, ccp(0.0f,WINSIZE.height*1.5f))), CCFadeOut::create(0.1f), NULL));
                }
                else
                {
                    stripes[i]->setScale(0.33f);
                    stripes[i]->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.0f),
                                                             CCDelayTime::create(0.15f), CCSpawn::createWithTwoActions(CCScaleTo::create(0.4f, 0.6f, 10.0f),CCMoveBy::create(0.7f, ccp(0.0f,-WINSIZE.height*1.5f))), CCFadeOut::create(0.1f), NULL));
                }
            }
            spriteUp->runAction(CCSequence::create(CCScaleTo::create(0.1f, 1.0f),
                                                   CCDelayTime::create(0.15f), CCHide::create(), NULL));
            sprite->setOpacity(0);
		}
        else if (isMegaBomb)
        {
            SimpleAudioEngine::sharedEngine()->playEffect("sound/bomb_explosion.mp3");
            float bombScale = 1.0f;
            /*lighting(sprite->getPosition(),
                    ccp(sprite->getPosition().x + CELL_WIDTH, sprite->getPosition().y), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y + CELL_HEIGHT/2.2f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y - CELL_HEIGHT/2.2f), bombScale, delay);
            
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - CELL_WIDTH, sprite->getPosition().y), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y + CELL_HEIGHT/2.2f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y - CELL_HEIGHT/2.2f), bombScale, delay);
            
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x, sprite->getPosition().y + CELL_HEIGHT), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + CELL_WIDTH/2.2f, sprite->getPosition().y + 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - CELL_WIDTH/2.2f, sprite->getPosition().y + 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x, sprite->getPosition().y - CELL_HEIGHT), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + CELL_WIDTH/2.2f, sprite->getPosition().y - 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - CELL_WIDTH/2.2f, sprite->getPosition().y - 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            
            /*lighting(sprite->getPosition(),
             ccp(sprite->getPosition().x - CELL_WIDTH, sprite->getPosition().y - CELL_HEIGHT), bombScale, delay);
             lighting(sprite->getPosition(),
             ccp(sprite->getPosition().x + CELL_WIDTH, sprite->getPosition().y - CELL_HEIGHT), bombScale, delay);
             lighting(sprite->getPosition(),
             ccp(sprite->getPosition().x - CELL_WIDTH, sprite->getPosition().y + CELL_HEIGHT), bombScale, delay);
             lighting(sprite->getPosition(),
             ccp(sprite->getPosition().x + CELL_WIDTH, sprite->getPosition().y + CELL_HEIGHT), bombScale, delay);*/
           /*
            CCSprite* expo = CCSprite::create("explos.png");
            expo->setAnchorPoint(ccp(1.0f, 1.0f));
            sprite->getParent()->addChild(expo, sprite->getZOrder() + 3);
            expo->setPosition(sprite->getPosition());
            expo->setOpacity(0);
            expo->runAction(CCSequence::create(CCDelayTime::create(delay*2.0f), CCFadeIn::create(0.01f), CCDelayTime::create(0.1f), CCFadeOut::create(0.01f), NULL));*/
            
        }
		else
		{
            delay = delayDestroy;
            SimpleAudioEngine::sharedEngine()->playEffect("sound/bomb_explosion.mp3");
            /*            CCSprite* bang = CCSprite::createWithSpriteFrameName("game/krug.png");
             sprite->getParent()->addChild(bang, sprite->getZOrder() + 1);
             bang->setPosition(sprite->getPosition());
             bang->setScale(0.01f);
             bang->setOpacity(120);
             bang->runAction(CCSequence::create(CCDelayTime::create(delay), CCScaleTo::create(0.3f, 0.8f),  NULL));
             bang->runAction(CCSequence::create(CCDelayTime::create(delay + 0.2f), CCFadeTo::create(0.2f, 0), CCCallFuncN::create(this, callfuncN_selector(sGameObject::cleaningBang)), NULL));*/
            this->sprite->runAction(CCSequence::create(CCDelayTime::create(delay), CCScaleTo::create(0.3f, 0.01f), CCFadeOut::create(0.01f), CCCallFuncN::create(this, callfuncN_selector(sGameObject::unvisibleObject)), NULL));
            
            float bombScale = 1.0f;
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + CELL_WIDTH, sprite->getPosition().y), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y + CELL_HEIGHT/2.2f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y - CELL_HEIGHT/2.2f), bombScale, delay);
            
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - CELL_WIDTH, sprite->getPosition().y), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y + CELL_HEIGHT/2.2f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y - CELL_HEIGHT/2.2f), bombScale, delay);
            
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x, sprite->getPosition().y + CELL_HEIGHT), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + CELL_WIDTH/2.2f, sprite->getPosition().y + 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - CELL_WIDTH/2.2f, sprite->getPosition().y + 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x, sprite->getPosition().y - CELL_HEIGHT), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + CELL_WIDTH/2.2f, sprite->getPosition().y - 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - CELL_WIDTH/2.2f, sprite->getPosition().y - 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + CELL_WIDTH, sprite->getPosition().y), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y + CELL_HEIGHT/2.2f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y - CELL_HEIGHT/2.2f), bombScale, delay);
            
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - CELL_WIDTH, sprite->getPosition().y), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y + CELL_HEIGHT/2.2f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - 3.0f*CELL_WIDTH/4.0f, sprite->getPosition().y - CELL_HEIGHT/2.2f), bombScale, delay);
            
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x, sprite->getPosition().y + CELL_HEIGHT), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + CELL_WIDTH/2.2f, sprite->getPosition().y + 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - CELL_WIDTH/2.2f, sprite->getPosition().y + 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            
            
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x, sprite->getPosition().y - CELL_HEIGHT), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x + CELL_WIDTH/2.2f, sprite->getPosition().y - 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            lighting(sprite->getPosition(),
                     ccp(sprite->getPosition().x - CELL_WIDTH/2.2f, sprite->getPosition().y - 3.0f*CELL_HEIGHT/4.0f), bombScale, delay);
            
            
            if (isNext)
                sprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f), CCCallFuncO::create(this, callfuncO_selector(sGameObject::setNext), this)));
		}
        sprite->setZOrder(sprite->getZOrder() + 5);
	}
	else if (type == Horizontal)
	{
        delay = delayDestroy;
        //sprite->removeAllChildrenWithCleanup(true);
        SimpleAudioEngine::sharedEngine()->playEffect("sound/line_explosion.mp3");
    
        for (int i = 0; i < stripes.size();i++)
        {
            stripes[i]->setVisible(true);
            if (i%2)
            {
                stripes[i]->runAction(CCSequence::create(CCDelayTime::create(delay), CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 5.0f, 0.33f), CCMoveBy::create(0.7f, ccp(WINSIZE.width, 0.0f))), CCFadeOut::create(0.1f), NULL));
            }
            else
            {
                stripes[i]->runAction(CCSequence::create(CCDelayTime::create(delay), CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 5.0f, 0.33f), CCMoveBy::create(0.7f, ccp(-WINSIZE.width, 0.0f))), CCFadeOut::create(0.1f), NULL));
            }
        }
        sprite->setOpacity(0);
        sprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delay + 0.7f), CCCallFuncN::create(this, callfuncN_selector(sGameObject::unvisibleObject))));
        sprite->setZOrder(sprite->getZOrder() + 5);
        if (isNext)
            sprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f), CCCallFuncO::create(this, callfuncO_selector(sGameObject::setNext), this)));
	}
	else if (type == Vertical)
	{
        delay = delayDestroy;
        SimpleAudioEngine::sharedEngine()->playEffect("sound/line_explosion.mp3");
        
        for (int i = 0; i < stripes.size();i++)
        {
            stripes[i]->setZOrder(100);
            stripes[i]->setVisible(true);
            if (i%2)
            {
                stripes[i]->runAction(CCSequence::create(CCDelayTime::create(delay), CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 0.33f, 5.0f), CCMoveBy::create(0.7f, ccp(0.0f,WINSIZE.height))), CCFadeOut::create(0.1f),
                                                         NULL));
            }
            else
            {
                stripes[i]->runAction(CCSequence::create(CCDelayTime::create(delay), CCSpawn::createWithTwoActions(CCScaleTo::create(0.3f, 0.33f, 5.0f), CCMoveBy::create(0.7f, ccp(0.0f, -WINSIZE.height))), CCFadeOut::create(0.1f),
                                                         NULL));
            }
        }
        sprite->setOpacity(0);
        sprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delay + 0.7f), CCCallFuncN::create(this, callfuncN_selector(sGameObject::unvisibleObject))));
        sprite->setZOrder(sprite->getZOrder() + 5);
        if (isNext)
            sprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f), CCCallFuncO::create(this, callfuncO_selector(sGameObject::setNext), this)));
	}
}

void sGameObject::lighting(CCPoint begin, CCPoint end, float bScale, float delay)
{
    CCNode* grandParent = NULL;
    grandParent = ((CCNode*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
    
    CCParticleSystem* meteorRight = CCParticleMeteor::create();
    meteorRight->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/fire.png"));
    meteorRight->setPosition(begin);
    meteorRight->setScale(1.0f);
    meteorRight->setGravity(ccp(0.0f, -100.0f));
    meteorRight->setLife(1.0f);
    grandParent->addChild(meteorRight, 100);
    meteorRight->runAction(CCSequence::create(CCDelayTime::create(delay + ((float)(rand()%20))/100.0f), CCMoveTo::create(0.2f, end), CCDelayTime::create(0.1f), CCScaleTo::create(0.2f, 0.01f), CCHide::create(), NULL));
}

void sGameObject::setNext(CCObject* sender)
{
    CCNode* saveParent = sprite->getParent();
    sprite->stopAllActions();
    sprite->setVisible(true);
    sprite->setOpacity(255);
    sprite->setZOrder(sprite->getZOrder() - 5);
    
    sprite->removeFromParentAndCleanup(false);
    parentNext->addChild(sprite, 15);
    switch (color)
    {
        case Blue:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/blue.png")->displayFrame());
            break;
        case Green:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/green.png")->displayFrame());;
            break;
        case Orange:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/orange.png")->displayFrame());
            break;
        case Puprle:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/violet.png")->displayFrame());
            break;
        case Red:
            sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/red.png")->displayFrame());
            break;
        case Yellow:
            //sprite->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/shadow/yellow.png")->displayFrame());
            sprite->setDisplayFrame(CCSprite::create("updateArt/yellow.png")->displayFrame());
            break;
    }
    
    sprite->setPosition(ccp(y*CELL_WIDTH + xZero, yZero - x*CELL_HEIGHT));
    sprite->setScale(ELEMENT_SCALE);
    
    changeType(nextType);
    isNext = false;
    
/*    if (LANDSCAPE)
        sprite->setPosition(ccp(sprite->getPositionX() + CELL_WIDTH*2.0f, sprite->getPositionY() - CELL_HEIGHT/2.0f));*/
}

void sGameObject::bombBoom(CCNode* sender)
{
    sprite->stopAllActions();
    changeType(Bomb);
}

void sGameObject::changeCristalSprite(CCNode* sender)
{
}

void sGameObject::lock()
{
	sprite->runAction(CCSequence::create(CCDelayTime::create(MOVE_DELTA-0.01f), CCCallFuncN::create(this, callfuncN_selector(sGameObject::unlock)), NULL));
	isLock = true;
}

void sGameObject::unlock(CCNode* sender)
{
	isLock = false;
}

void sGameObject::updateCurrentPosition(CCNode* sender)
{
 //   sprite->setPosition(ccp(y*CELL_WIDTH + xZero, yZero - x*CELL_HEIGHT));
}

bool sGameObject::moveToCurrentPosition()
{
    sprite->runAction(CCSequence::createWithTwoActions(CCMoveTo::create(MOVE_DELTA, ccp(y*CELL_WIDTH + xZero, yZero - x*CELL_HEIGHT)), CCCallFuncN::create(this, callfuncN_selector(sGameObject::updateCurrentPosition))));
    return true;
}

bool sGameObject::moveLeft()
{
	if (!y)
		return false;
	
	if (isLock)
		return false;

	lock();
//	sprite->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(MOVE_DELTA, ccp(- CELL_WIDTH, 0.0f)), CCCallFuncN::create(this, callfuncN_selector(sGameObject::updateCurrentPosition))));
	y--;
    moveToCurrentPosition();
	return true;
}

bool sGameObject::moveRight()
{
	if (y == (MAX_COLUMN - 1))
		return false;


	if (isLock)
		return false;
	lock();

	
//	sprite->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(MOVE_DELTA, ccp(CELL_WIDTH, 0.0f)), CCCallFuncN::create(this, callfuncN_selector(sGameObject::updateCurrentPosition))));
	y++;
    moveToCurrentPosition();
	return true;
}

bool sGameObject::moveDown()
{
	if (x == (MAX_ROW - 1))
		return false;

	if (isLock)
		return false;
	lock();

//	sprite->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(MOVE_DELTA, ccp(0.0f, - CELL_HEIGHT)), CCCallFuncN::create(this, callfuncN_selector(sGameObject::updateCurrentPosition))));
	x++;
    moveToCurrentPosition();
	return true;
}

bool sGameObject::moveUp()
{
	if (!x)
		return false;

	if (isLock)
		return false;
	lock();

//	sprite->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(MOVE_DELTA, ccp(0.0f, CELL_HEIGHT)), CCCallFuncN::create(this, callfuncN_selector(sGameObject::updateCurrentPosition))));
	x--;
    moveToCurrentPosition();
	return true;
}

bool sGameObject::moveLeftDown()
{
	if (!y)
		return false;

	if (x == (MAX_ROW - 1))
		return false;

	if (isLock)
		return false;
	lock();

	sprite->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(MOVE_DELTA, ccp(- CELL_WIDTH, - CELL_HEIGHT)), CCCallFuncN::create(this, callfuncN_selector(sGameObject::updateCurrentPosition))));
	y--;	
	x++;

//    moveToCurrentPosition();
	return true;
}

bool sGameObject::moveRightDown()
{
	if (y == (MAX_COLUMN - 1))
		return false;

	if (x == (MAX_ROW - 1))
		return false;

	if (isLock)
		return false;
	lock();

	sprite->runAction(CCSequence::createWithTwoActions(CCMoveBy::create(MOVE_DELTA, ccp(CELL_WIDTH, -CELL_HEIGHT)), CCCallFuncN::create(this, callfuncN_selector(sGameObject::updateCurrentPosition))));
	y++;
	x++;

//    moveToCurrentPosition();
	return true;
}
