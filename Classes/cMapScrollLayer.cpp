#include "cMapScrollLayer.h"
#include "cGlobal.h"

#define DEACCEL_COUNT 5
#define DEACCEL_OFFSET 150.0f

using namespace cocos2d;


bool MapScrollLayer::init()
{
	if (!CCLayer::init())
		return false;
    
    multiplierX = 1.0f;
    multiplierY = 1.0f;
    setTouchEnabled(true);
    isZooming = false;
    
    touches = new CCArray();
    
    lastScrolling.clear();
	return true;
}

MapScrollLayer::~MapScrollLayer()
{
}

void MapScrollLayer::changeOrientation()
{
    return;
}

void MapScrollLayer::registerWithTouchDispatcher()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, 1);
}

void MapScrollLayer::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    this->unschedule(schedule_selector(MapScrollLayer::deaccelerate));
    CCTouch* touch = (CCTouch*)( pTouches->anyObject());
	CCPoint location = touch->getLocation();
}

void MapScrollLayer::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
    if (pTouches->count() > 1)
    {
        CCSetIterator it = pTouches->begin();
        CCTouch *touch1 = (CCTouch*)(*(it++));;
        CCTouch *touch2 = (CCTouch*)(*it);
        
        CCPoint curPosTouch1 = CCDirector::sharedDirector()->convertToGL(touch1->getLocationInView());
        CCPoint curPosTouch2 = CCDirector::sharedDirector()->convertToGL(touch2->getLocationInView());
        
        CCPoint prevPosTouch1 = CCDirector::sharedDirector()->convertToGL(touch1->getPreviousLocationInView());
        CCPoint prevPosTouch2 = CCDirector::sharedDirector()->convertToGL(touch2->getPreviousLocationInView());
        
        
        CCPoint curPosLayer = ccpMidpoint(curPosTouch1, curPosTouch2);
        CCPoint prevPosLayer = ccpMidpoint(prevPosTouch1, prevPosTouch2);
        
        float prevScale = this->getScale();
        float curScale = this->getScale() * (ccpDistance(curPosTouch1, curPosTouch2) / ccpDistance(prevPosTouch1, prevPosTouch2));
        
        if(!zoomStart){
            zoomStart = true;
            curScale = prevScale;
        }
        
        if (curScale < minScale || curScale > maxScale)
            curScale = prevScale;
        
        this->setScale( curScale);

        if (this->getScale() != prevScale)
        {
            CCPoint realCurPosLayer = this->convertToNodeSpace(curPosLayer);
            float deltaX = (this->getAnchorPoint().x * this->getContentSize().width) * (this->getScale() - prevScale);
            float deltaY = (this->getAnchorPoint().y * this->getContentSize().height) * (this->getScale() - prevScale);
            this->setPosition(ccp(this->getPosition().x - deltaX, this->getPosition().y - deltaY));
        }
        if (!prevPosLayer.equals(curPosLayer))
        {
            this->setPosition(ccp(this->getPosition().x + curPosLayer.x - prevPosLayer.x,
                                  this->getPosition().y + curPosLayer.y - prevPosLayer.y));
        }
        return;
    }
 
    CCTouch* touch = (CCTouch*)( pTouches->anyObject());
	CCPoint location = touch->getLocation();
    CCPoint prevLocation = touch->getPreviousLocation();
    CCPoint diff = ccpSub(location, prevLocation);
    CCPoint newPosition = ccpAdd(getPosition(), diff);
    
    float scale = this->getScale();
    float contentSizeWidth = this->getContentSize().width*this->getScale();
    float leftOffset = -(contentSizeWidth - WINSIZE.width);
    
    float contentSizeHeight = this->getContentSize().height*this->getScale();
    float downOffset = -(contentSizeHeight - WINSIZE.height);
    
    if (newPosition.x < leftOffset)
    {
        float offsetBorder =  leftOffset - newPosition.x;
        diff.x/=(offsetBorder/7.0f);
    }
    else if (newPosition.x > 0.0f)
    {
        float offsetBorder =  newPosition.x;
        diff.x/=(offsetBorder/7.0f);
    }
    
    if (newPosition.y < downOffset)
    {
        float offsetBorder =  downOffset - newPosition.y;
        diff.y/=(offsetBorder/15.0f);
    }
    else if (newPosition.y > 0.0f)
    {
        float offsetBorder =  newPosition.y;
        diff.y/=(offsetBorder/7.0f);
    }
    
    newPosition = ccpAdd(getPosition(), diff);
    this->setPosition(newPosition);
    
    lastScrolling.push_back(diff);
}

void MapScrollLayer::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    if (pTouches->count() > 1)
    {
        zoomStart = false;
        return;
    }
    CCTouch* touch = (CCTouch*)( pTouches->anyObject());
	CCPoint location = touch->getLocation();
    CCPoint prevLocation = touch->getPreviousLocation();
    CCPoint diff = ccpSub(location, prevLocation);
    
    CCPoint newPosition = getPosition();
    
    float scale = this->getScale();
    float contentSizeWidth = this->getContentSize().width*this->getScale();
    
    float leftOffset = -(contentSizeWidth - WINSIZE.width);
    
    float contentSizeHeight = this->getContentSize().height*this->getScale();
    float downOffset = -(contentSizeHeight - WINSIZE.height);
    
    bool isTrespassing = false;
    
    if (newPosition.x < leftOffset)
    {
        float offsetBorder =  leftOffset - newPosition.x;
        this->runAction(CCMoveBy::create(0.3f, ccp(offsetBorder, 0.0f)));
        isTrespassing = true;
    }
    else if (newPosition.x > 0.0f)
    {
        float offsetBorder = newPosition.x;
        this->runAction(CCMoveBy::create(0.3f, ccp(-offsetBorder, 0.0f)));
        isTrespassing = true;
    }
    
    if (newPosition.y < downOffset)
    {
        float offsetBorder =  downOffset - newPosition.y;
        this->runAction(CCMoveBy::create(0.3f, ccp(0.0f, offsetBorder)));
        isTrespassing = true;
    }
    else if (newPosition.y > 0.0f)
    {
        float offsetBorder =  newPosition.y;
        this->runAction(CCMoveBy::create(0.3f, ccp(0.0f, -offsetBorder)));
        isTrespassing = true;
    }
    
    if (!isTrespassing)
    {
        lastScrolling.push_back(diff);
        float xDiff = 0.0f;
        float yDiff = 0.0f;
        float idx = 0;
        for (int i = lastScrolling.size(); i >= 0; i--)
        {
            if (fabs(lastScrolling[i].x) > 1000.0f || fabs(lastScrolling[i].y) > 1000.0f)
                continue;
            
            xDiff+=lastScrolling[i].x;
            yDiff+=lastScrolling[i].y;
            idx++;
            if (idx >= DEACCEL_COUNT)
                break;
        }
        xDiff/=idx;
        yDiff/=idx;
        
        deaccelDiff = CCPoint(xDiff, yDiff);
        
        lastScrolling.clear();
        this->schedule(schedule_selector(MapScrollLayer::deaccelerate));
    }
}

void MapScrollLayer::deaccelerate(float dt)
{
    CCPoint newPosition = ccpAdd(getPosition(), deaccelDiff);
    this->setPosition(newPosition);
    
    deaccelDiff = ccpMult(deaccelDiff, 0.95f);
    
    float scale = this->getScale();
    float contentSizeWidth = this->getContentSize().width*this->getScale();
    float leftOffset = -(contentSizeWidth - WINSIZE.width);
    float contentSizeHeight = this->getContentSize().height*this->getScale();
    float downOffset = -(contentSizeHeight - WINSIZE.height);
    
    bool isTrespassing = false;
    
    if (newPosition.x < (leftOffset - DEACCEL_OFFSET))
    {
        float offsetBorder =  leftOffset - newPosition.x;
        this->runAction(CCMoveBy::create(0.3f, ccp(offsetBorder, 0.0f)));
        isTrespassing = true;
    }
    else if (newPosition.x > DEACCEL_OFFSET)
    {
        float offsetBorder = newPosition.x;
        this->runAction(CCMoveBy::create(0.3f, ccp(-offsetBorder, 0.0f)));
        isTrespassing = true;
    }
    
    if (newPosition.y < (downOffset - DEACCEL_OFFSET))
    {
        float offsetBorder =  downOffset - newPosition.y;
        this->runAction(CCMoveBy::create(0.3f, ccp(0.0f, offsetBorder)));
        isTrespassing = true;
    }
    else if (newPosition.y > DEACCEL_OFFSET)
    {
        float offsetBorder =  newPosition.y;
        this->runAction(CCMoveBy::create(0.3f, ccp(0.0f, -offsetBorder)));
        isTrespassing = true;
    }
    
    if ((fabs(deaccelDiff.x) < 0.3f && fabs(deaccelDiff.y) < 0.3f) || isTrespassing)
    {
        this->unschedule(schedule_selector(MapScrollLayer::deaccelerate));
    }
}

void MapScrollLayer::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
    
}


