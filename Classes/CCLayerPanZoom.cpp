/*
 * CCLayerPanZoom
 *
 * Copyright (c) 2011 Alexey Lang
 * Copyright (c) 2011 Pavel Guganov
 *
 * http://www.cocos2d-x.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "CCLayerPanZoom.h"


USING_NS_CC;

void CCLayerPanZoom::setMaxScale(float maxScale)
{
    _maxScale = maxScale;
    this->setScale(MIN(this->getScale(), _maxScale));
}

float CCLayerPanZoom::maxScale()
{
    return _maxScale;
}

void CCLayerPanZoom::setMinScale(float minScale)
{
    _minScale = minScale;
    this->setScale(MAX(this->getScale(), minScale));
}

float CCLayerPanZoom::minScale()
{
    return _minScale;
}

void CCLayerPanZoom::setRubberEffectRatio(float rubberEffectRatio)
{
    _rubberEffectRatio = rubberEffectRatio;
    
    // Avoid turning rubber effect On in frame mode.
    if (_mode == kCCLayerPanZoomModeFrame)
    {
        CCLOGERROR("CCLayerPanZoom#setRubberEffectRatio: rubber effect is not supported in frame mode.");
        _rubberEffectRatio = 0.0f;
    }
}

float CCLayerPanZoom::rubberEffectRatio()
{
    return _rubberEffectRatio;
}


CCLayerPanZoom* CCLayerPanZoom::layer()
{
    // 'layer' is an autorelease object
    CCLayerPanZoom *layer = CCLayerPanZoom::create();
    
    // return the scene
    return layer;
}

// on "init" you need to initialize your instance
bool CCLayerPanZoom::init()
{
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    //m_bIsRelativeAnchorPoint = true;
    m_bTouchEnabled = true;
    
    _maxScale = 3.0f;
    _minScale = 0.7f;
    _touches = CCArray::createWithCapacity(10);
    _touches->retain();
    
    _panBoundsRect = CCRectZero;
    _touchDistance = 0.0F;
    _maxTouchDistanceToClick = 315.0f;
    
    _mode = kCCLayerPanZoomModeSheet;
    _minSpeed = 100.0f;
    _maxSpeed = 1000.0f;
    _topFrameMargin = 100.0f;
    _bottomFrameMargin = 100.0f;
    _leftFrameMargin = 100.0f;
    _rightFrameMargin = 100.0f;
    
    _rubberEffectRatio = 0.0f;
    _rubberEffectRecoveryTime = 0.2f;
    _rubberEffectRecovering = false;
    _rubberEffectZooming = false;
    
    return true;
}

void CCLayerPanZoom::ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
    CCTouch *pTouch;
    CCSetIterator setIter;
    lastScrolling.clear();
    
    isMinMaxScaling = false;
    
    this->unschedule(schedule_selector(CCLayerPanZoom::deaccelerate));
    this->unschedule(schedule_selector(CCLayerPanZoom::deaccelerateScale));
    //this->recoverPositionAndScale();
    
    for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter)
    {
        pTouch = (CCTouch *)(*setIter);
        _touches->addObject(pTouch);
    }
    
    if (_touches->count() == 1)
    {
        _touchMoveBegan = false;
        time_t seconds;
        
        seconds = time (NULL);
        _singleTouchTimestamp = seconds/60;
    }
    else
        _singleTouchTimestamp = INFINITY;
}

void CCLayerPanZoom::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
    bool multitouch = _touches->count() > 1;
    if (multitouch)
    {
        // Get the two first touches
        CCTouch *touch1 = (CCTouch*)_touches->objectAtIndex(0);
        CCTouch *touch2 = (CCTouch*)_touches->objectAtIndex(1);
        // Get current and previous positions of the touches
        CCPoint curPosTouch1 = CCDirector::sharedDirector()->convertToGL(touch1->getLocationInView());
        CCPoint curPosTouch2 = CCDirector::sharedDirector()->convertToGL(touch2->getLocationInView());
        
        CCPoint prevPosTouch1 = CCDirector::sharedDirector()->convertToGL(touch1->getPreviousLocationInView());
        CCPoint prevPosTouch2 = CCDirector::sharedDirector()->convertToGL(touch2->getPreviousLocationInView());
        
        
        // Calculate current and previous positions of the layer relative the anchor point
        curPosLayer = ccpMidpoint(curPosTouch1, curPosTouch2);
        CCPoint prevPosLayer = ccpMidpoint(prevPosTouch1, prevPosTouch2);
        // Calculate new scale
        float prevScale = this->getScale();
        float curScale = this->getScale() * (ccpDistance(curPosTouch1, curPosTouch2) / ccpDistance(prevPosTouch1, prevPosTouch2));
        
        if ((prevScale + (curScale - prevScale)/2.0f) < minScale())
        {
            float diff = minScale() - (prevScale + (curScale - prevScale)/2.0f);
            curScale = prevScale + (curScale - prevScale)/(2.0f + diff*500.0f);
        }
        else if ((prevScale + (curScale - prevScale)/2.0f) > maxScale())
        {
            float diff = (prevScale + (curScale - prevScale)/2.0f) - maxScale();
            curScale = prevScale + (curScale - prevScale)/(2.0f + diff*30.0f);
        }
        else
        {
            curScale = prevScale + (curScale - prevScale)/2.0f;
        }
        

        if(!zoomStart){
            zoomStart = true;
            curScale = prevScale;
        }
        
        CCLOG("SCALE : %f", curScale);
        this->setScale( curScale);
        // Avoid scaling out from panBoundsRect when Rubber Effect is OFF.
/*        if (!_rubberEffectRatio)
        {
            this->setScale( MAX(this->getScale(), this->minPossibleScale()));
        }*/

        lastScaling.push_back((curScale - prevScale)/1.5f);
        // If scale was changed -> set new scale and fix position with new scale
        if (this->getScale() != prevScale)
        {
            if (_rubberEffectRatio)
            {
                _rubberEffectZooming = true;
            }
            realCurPosLayer = this->convertToNodeSpace(curPosLayer);
            float deltaX = (realCurPosLayer.x - this->getAnchorPoint().x * this->getContentSize().width) * (this->getScale() - prevScale);
            float deltaY = (realCurPosLayer.y - this->getAnchorPoint().y * this->getContentSize().height) * (this->getScale() - prevScale);
            this->setSimplePosition(ccp(this->getPosition().x - deltaX, this->getPosition().y - deltaY));
            _rubberEffectZooming = false;
        }
        // If current and previous position of the multitouch's center aren't equal -> change position of the layer
        /*if (!prevPosLayer.equals(curPosLayer))
        {
            this->setPosition(ccp(this->getPosition().x + curPosLayer.x - prevPosLayer.x,
                                  this->getPosition().y + curPosLayer.y - prevPosLayer.y));
        }*/
        // Don't click with multitouch
        _touchDistance = INFINITY;
    }
    else
    {
        // Get the single touch and it's previous & current position.
        CCTouch *touch = (CCTouch*)_touches->objectAtIndex(0);
        CCPoint curTouchPosition = CCDirector::sharedDirector()->convertToGL(touch->getLocationInView());
        CCPoint prevTouchPosition = CCDirector::sharedDirector()->convertToGL(touch->getPreviousLocationInView());
        
        /*if (!scrollStart || fabs(curTouchPosition.x - prevTouchPosition.x) > 100.0f || fabs(curTouchPosition.y - prevTouchPosition.y) > 100.0f)
        {
            scrollStart = true;
            curTouchPosition = prevTouchPosition;
        }*/
        
        // Always scroll in sheet mode.
        if (_mode == kCCLayerPanZoomModeSheet)
        {
            // Set new position of the layer.
            this->setPosition(ccp(this->getPosition().x + curTouchPosition.x - prevTouchPosition.x,
                                  this->getPosition().y + curTouchPosition.y - prevTouchPosition.y));
        }
        
        // Accumulate touch distance for all modes.
        _touchDistance += ccpDistance(curTouchPosition, prevTouchPosition);
        
        lastScrolling.push_back(ccp(curTouchPosition.x - prevTouchPosition.x, curTouchPosition.y - prevTouchPosition.y));
        
        // Inform delegate about starting updating touch position, if click isn't possible.
        if (_mode == kCCLayerPanZoomModeFrame)
        {
            if (_touchDistance > _maxTouchDistanceToClick && !_touchMoveBegan)
            {
                //ToDo add delegate here
                //[self.delegate layerPanZoom: self
                //  touchMoveBeganAtPosition: [self convertToNodeSpace: prevTouchPosition]];
                _touchMoveBegan = true;
            }
        }
    }
}

void CCLayerPanZoom::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
    _singleTouchTimestamp = INFINITY;
    zoomStart = false;
    scrollStart = false;
    scaleSub = 2.0f;
    
    // Process click event in single touch.
    //ToDo add delegate
    if (  (_touchDistance < _maxTouchDistanceToClick) /*&& (self.delegate) */
        && (_touches->count() == 1))
    {
        CCTouch *touch = (CCTouch*)_touches->objectAtIndex(0);
        CCPoint curPos = CCDirector::sharedDirector()->convertToGL(touch->getLocationInView());
        //ToDo add delegate
        /*[self.delegate layerPanZoom: self
         clickedAtPoint: [self convertToNodeSpace: curPos]
         tapCount: [touch tapCount]];*/
    }
    
    CCTouch *pTouch;
    CCSetIterator setIter;
    for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter)
    {
        pTouch = (CCTouch *)(*setIter);
        _touches->removeObject(pTouch);
    }
    
    if (_touches->count() == 0)
    {
        _touchDistance = 0.0f;
    }
    
    if (!lastScaling.empty())
    {
        float xDiff = 0.0f;
        float idx = 0;
        for (int i = lastScaling.size(); i >= 0; i--)
        {
            xDiff+=lastScaling[i];
            idx++;
            if (idx >= 5)
                break;
        }
        xDiff/=idx;
        
        deaccelScale = xDiff;
        lastScaling.clear();
        
        if (getScale() < minScale())
        {
            deaccelScale = 0.02f;
            this->schedule(schedule_selector(CCLayerPanZoom::deaccelerateScaleRecoveryMin));
            isMinMaxScaling = true;
        }
        else if (getScale() > maxScale())
        {
            deaccelScale = -0.04f;
            this->schedule(schedule_selector(CCLayerPanZoom::deaccelerateScaleRecoveryMax));
            isMinMaxScaling = true;
        }
    }
    
    if (!lastScrolling.empty() && !isMinMaxScaling)
    {
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
            if (idx >= 5)
                break;
        }
        xDiff/=idx;
        yDiff/=idx;
        
        deaccelDiff = CCPoint(xDiff, yDiff);
        lastScrolling.clear();
        
        this->schedule(schedule_selector(CCLayerPanZoom::deaccelerate));
    }
    
    if (!_touches->count() && !_rubberEffectRecovering && !isMinMaxScaling)
    {
        _touches->removeAllObjects();
        this->recoverPositionAndScale();
    }

    
    /*if (getScale() < minScale())
    {
        float deltaX = (realCurPosLayer.x - this->getAnchorPoint().x * this->getContentSize().width) * (minScale() - this->getScale());
        float deltaY = (realCurPosLayer.y - this->getAnchorPoint().y * this->getContentSize().height) * (minScale() - this->getScale());
        this->runAction(CCSpawn::createWithTwoActions(CCScaleTo::create(0.2f, minScale()), CCMoveTo::create(0.2f, ccp(this->getPosition().x - deltaX, this->getPosition().y - deltaY))));
    }
    else if (getScale() > maxScale())
    {
        float deltaX = (realCurPosLayer.x - this->getAnchorPoint().x * this->getContentSize().width) * (maxScale() - this->getScale());
        float deltaY = (realCurPosLayer.y - this->getAnchorPoint().y * this->getContentSize().height) * (maxScale() - this->getScale());
        this->runAction(CCSpawn::createWithTwoActions(CCScaleTo::create(0.2f, maxScale()), CCMoveTo::create(0.2f, ccp(this->getPosition().x - deltaX, this->getPosition().y - deltaY))));
    }*/
}

CCRect CCLayerPanZoom::getViewRect()
{
    CCPoint screenPos = this->convertToWorldSpace(CCPointZero);
    
    float scaleX = this->getScaleX();
    float scaleY = this->getScaleY();
    
    for (CCNode *p = m_pParent; p != NULL; p = p->getParent()) {
        scaleX *= p->getScaleX();
        scaleY *= p->getScaleY();
    }
    
    // Support negative scaling. Not doing so causes intersectsRect calls
    // (eg: to check if the touch was within the bounds) to return false.
    // Note, CCNode::getScale will assert if X and Y scales are different.
    if(scaleX<0.f) {
        screenPos.x += getContentSize().width*scaleX;
        scaleX = -scaleX;
    }
    if(scaleY<0.f) {
        screenPos.y += getContentSize().height*scaleY;
        scaleY = -scaleY;
    }
    
    return CCRectMake(screenPos.x, screenPos.y, getContentSize().width*scaleX, getContentSize().height*scaleY);
}

void CCLayerPanZoom::beforeDraw()
{
    m_bScissorRestored = false;
    CCRect frame = getViewRect();
    if (CCEGLView::sharedOpenGLView()->isScissorEnabled()) {
        m_bScissorRestored = true;
        m_tParentScissorRect = CCEGLView::sharedOpenGLView()->getScissorRect();
        //set the intersection of m_tParentScissorRect and frame as the new scissor rect
        if (frame.intersectsRect(m_tParentScissorRect)) {
            float x = MAX(frame.origin.x, m_tParentScissorRect.origin.x);
            float y = MAX(frame.origin.y, m_tParentScissorRect.origin.y);
            float xx = MIN(frame.origin.x+frame.size.width, m_tParentScissorRect.origin.x+m_tParentScissorRect.size.width);
            float yy = MIN(frame.origin.y+frame.size.height, m_tParentScissorRect.origin.y+m_tParentScissorRect.size.height);
            CCEGLView::sharedOpenGLView()->setScissorInPoints(x, y, xx-x, yy-y);
        }
    }
    else {
        glEnable(GL_SCISSOR_TEST);
        CCEGLView::sharedOpenGLView()->setScissorInPoints(frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
    }
}

void CCLayerPanZoom::afterDraw()
{
    if (m_bScissorRestored) {//restore the parent's scissor rect
        CCEGLView::sharedOpenGLView()->setScissorInPoints(m_tParentScissorRect.origin.x, m_tParentScissorRect.origin.y, m_tParentScissorRect.size.width, m_tParentScissorRect.size.height);
    }
    else {
        glDisable(GL_SCISSOR_TEST);
    }
}

void CCLayerPanZoom::deaccelerate(float dt)
{
    xBorder = false;
    yBorder = false;
    CCPoint newPosition = ccpAdd(getPosition(), deaccelDiff);
    this->setPosition(newPosition);
    bool isTrespassing = (xBorder && yBorder);
    
    deaccelDiff = ccpMult(deaccelDiff, 0.95f);
    if (xBorder)
        deaccelDiff.x = 0.0f;
    if (yBorder)
        deaccelDiff.y = 0.0f;
    
    if ((fabs(deaccelDiff.x) < 0.3f && fabs(deaccelDiff.y) < 0.3f) || isTrespassing)
    {
        this->unschedule(schedule_selector(CCLayerPanZoom::deaccelerate));
    }
}

void CCLayerPanZoom::deaccelerateScaleRecoveryMin()
{
    if (fabs(deaccelScale) > 0.04f)
    {
        if (deaccelScale < 0)
            deaccelScale = -0.04f;
        else
            deaccelScale = 0.04f;
    }
    
    float prevScale = this->getScale();
    float curScale = this->getScale() + deaccelScale;
    
    this->setScale( curScale);
    if (this->getScale() != prevScale)
    {
        CCPoint realCurPosLayer = this->convertToNodeSpace(curPosLayer);
        float deltaX = (realCurPosLayer.x - this->getAnchorPoint().x * this->getContentSize().width) * (this->getScale() - prevScale);
        float deltaY = (realCurPosLayer.y - this->getAnchorPoint().y * this->getContentSize().height) * (this->getScale() - prevScale);
        this->setSimplePosition(ccp(this->getPosition().x - deltaX, this->getPosition().y - deltaY));
    }
    deaccelScale*=0.97f;
    
    if (curScale >= (minScale() + 0.005f) && stageMinScale == 0)
    {
        deaccelScale *= -0.2f;
        stageMinScale = 1;
    }
    if (curScale <= (minScale() - 0.005f) && stageMinScale == 1)
    {
        deaccelScale *= -1.0f;
        stageMinScale = 2;
    }
    if (curScale >= minScale() && stageMinScale == 2)
    {
        lastScrolling.clear();
        this->unschedule(schedule_selector(CCLayerPanZoom::deaccelerateScaleRecoveryMin));
        this->recoverPositionAndScale();
        isRecoveryScale = false;
        stageMinScale = 0;
    }
}

void CCLayerPanZoom::deaccelerateScaleRecoveryMax()
{
    /*if (fabs(deaccelScale) > 0.04f)
    {
        if (deaccelScale < 0)
            deaccelScale = -0.04f;
        else
            deaccelScale = 0.04f;
    }*/
    
    float prevScale = this->getScale();
    float curScale = this->getScale() + deaccelScale;
    
    this->setScale( curScale);
    if (this->getScale() != prevScale)
    {
        CCPoint realCurPosLayer = this->convertToNodeSpace(curPosLayer);
        float deltaX = (realCurPosLayer.x - this->getAnchorPoint().x * this->getContentSize().width) * (this->getScale() - prevScale);
        float deltaY = (realCurPosLayer.y - this->getAnchorPoint().y * this->getContentSize().height) * (this->getScale() - prevScale);
        this->setSimplePosition(ccp(this->getPosition().x - deltaX, this->getPosition().y - deltaY));
    }
    deaccelScale*=0.97f;
    
    if (curScale <= (maxScale() - 0.01f) && stageMinScale == 0)
    {
        deaccelScale *= -0.2f;
        stageMinScale = 1;
    }
    if (curScale >= (maxScale() + 0.01f) && stageMinScale == 1)
    {
        deaccelScale *= -1.0f;
        stageMinScale = 2;
    }
    if (curScale <= maxScale() && stageMinScale == 2)
    {
        stageMinScale = 0;
        lastScrolling.clear();
        this->unschedule(schedule_selector(CCLayerPanZoom::deaccelerateScaleRecoveryMax));
        this->recoverPositionAndScale();
        isRecoveryScale = false;
    }
}

void CCLayerPanZoom::deaccelerateScale(float dt)
{
    if (fabs(deaccelScale) > 0.04f)
    {
        if (deaccelScale < 0)
            deaccelScale = -0.04f;
        else
            deaccelScale = 0.04f;
    }
    
    float prevScale = this->getScale();
    float curScale = this->getScale() + deaccelScale;
    
    this->setScale(curScale);
    if (this->getScale() != prevScale)
    {
        CCPoint realCurPosLayer = this->convertToNodeSpace(curPosLayer);
        float deltaX = (realCurPosLayer.x - this->getAnchorPoint().x * this->getContentSize().width) * (this->getScale() - prevScale);
        float deltaY = (realCurPosLayer.y - this->getAnchorPoint().y * this->getContentSize().height) * (this->getScale() - prevScale);
        this->setPosition(ccp(this->getPosition().x - deltaX, this->getPosition().y - deltaY));
    }
    
    deaccelScale*=0.97f;
    
    if (fabs(deaccelScale) < 0.005f)
    {
        this->unschedule(schedule_selector(CCLayerPanZoom::deaccelerateScale));
        this->recoverPositionAndScale();
        isRecoveryScale = false;
    }
}

void CCLayerPanZoom::ccTouchesCancelled(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent){
    
    CCTouch *pTouch;
    CCSetIterator setIter;
    for (setIter = pTouches->begin(); setIter != pTouches->end(); ++setIter)
    {
        pTouch = (CCTouch *)(*setIter);
        _touches->removeObject(pTouch);
    }
    
    if (_touches->count() == 0)
    {
        _touchDistance = 0.0f;
    }
}


// Updates position in frame mode.
void  CCLayerPanZoom::update(float dt){
    // Only for frame mode with one touch.
    if ( _mode == kCCLayerPanZoomModeFrame && _touches->count() == 1 )
    {
        // Do not update position if click is still possible.
        if (_touchDistance <= _maxTouchDistanceToClick)
            return;
        
        // Do not update position if pinch is still possible.
        time_t seconds;
        
        seconds = time (NULL);
        seconds /= 60;
        if (seconds - _singleTouchTimestamp < kCCLayerPanZoomMultitouchGesturesDetectionDelay)
            return;
        
        // Otherwise - update touch position. Get current position of touch.
        CCTouch *touch = (CCTouch*)_touches->objectAtIndex(0);
        CCPoint curPos = CCDirector::sharedDirector()->convertToGL(touch->getLocationInView());
        
        // Scroll if finger in the scroll area near edge.
        if (this->frameEdgeWithPoint(curPos) != kCCLayerPanZoomFrameEdgeNone)
        {
            this->setPosition(ccp(this->getPosition().x + dt * this->horSpeedWithPosition( curPos),
                                  this->getPosition().y + dt * this->vertSpeedWithPosition(curPos)));
        }
        
        // Inform delegate if touch position in layer was changed due to finger or layer movement.
        CCPoint touchPositionInLayer = this->convertToNodeSpace(curPos);
        if (!_prevSingleTouchPositionInLayer.equals(touchPositionInLayer))
        {
            _prevSingleTouchPositionInLayer = touchPositionInLayer;
            //ToDo add delegate
            //[self.delegate layerPanZoom: self
            //      touchPositionUpdated: touchPositionInLayer];
        }
        
    }
}

void  CCLayerPanZoom::onEnter(){
    CCLayer::onEnter();
    CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this, 0, false);
}

void  CCLayerPanZoom::onExit(){
    CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget(this);
    CCLayer::onExit();
}
void CCLayerPanZoom::setPanBoundsRect(CCRect rect){
    _panBoundsRect = rect;
    this->setScale(this->minPossibleScale());
    this->setPosition(this->getPosition());
}

bool CCLayerPanZoom::setSimplePosition(CCPoint  position)
{
    CCNode::setPosition(position);
    return true;
}

bool CCLayerPanZoom::setPosition(CCPoint  position){
    CCPoint prevPosition = this->getPosition();
    //May be it wouldnot work
    CCNode::setPosition(position);
    
    bool result = false;
    if (!_panBoundsRect.equals(CCRectZero) && !_rubberEffectZooming)
    {
        if (_rubberEffectRatio && _mode == kCCLayerPanZoomModeSheet)
        {
            if (!_rubberEffectRecovering)
            {
                float topDistance = this->topEdgeDistance();
                float bottomDistance = this->bottomEdgeDistance();
                float leftDistance = this->leftEdgeDistance();
                float rightDistance = this->rightEdgeDistance();
                float dx = this->getPosition().x - prevPosition.x;
                float dy = this->getPosition().y - prevPosition.y;
                if (bottomDistance || topDistance)
                {
                    CCNode::setPosition(ccp(this->getPosition().x,
                                            prevPosition.y + dy * _rubberEffectRatio));
                    result = true;
                    yBorder = true;
                }
                if (leftDistance || rightDistance)
                {
                    CCNode::setPosition(ccp(prevPosition.x + dx * _rubberEffectRatio,
                                            this->getPosition().y));
                    result = true;
                    xBorder = true;
                }
            }
        }
        else
        {
            CCRect boundBox = this->boundingBox();
            if (this->getPosition().x - boundBox.size.width * this->getAnchorPoint().x > _panBoundsRect.origin.x)
            {
                CCNode::setPosition(ccp(boundBox.size.width * this->getAnchorPoint().x + _panBoundsRect.origin.x,
                                        this->getPosition().y));
            }
            if (this->getPosition().y - boundBox.size.height * this->getAnchorPoint().y > _panBoundsRect.origin.y)
            {
                CCNode::setPosition(ccp(this->getPosition().x, boundBox.size.height * this->getAnchorPoint().y +
                                        _panBoundsRect.origin.y));
            }
            if (this->getPosition().x + boundBox.size.width * (1 - this->getAnchorPoint().x) < _panBoundsRect.size.width +
                _panBoundsRect.origin.x)
            {
                CCNode::setPosition(ccp(_panBoundsRect.size.width + _panBoundsRect.origin.x -
                                        boundBox.size.width * (1 - this->getAnchorPoint().x), this->getPosition().y));
            }
            if (this->getPosition().y + boundBox.size.height * (1 - this->getAnchorPoint().y) < _panBoundsRect.size.height +
                _panBoundsRect.origin.y)
            {
                CCNode::setPosition(ccp(this->getPosition().x, _panBoundsRect.size.height + _panBoundsRect.origin.y -
                                        boundBox.size.height * (1 - this->getAnchorPoint().y)));
            }
        }
    }
    
    return result;
}

void CCLayerPanZoom::setScale(float scale){
//    CCLayer::setScale( MIN(MAX(scale, _minScale), _maxScale));
    CCLayer::setScale(scale);
}

void CCLayerPanZoom::recoverPositionAndScale()
{
    if (getScale() < minPossibleScale())
        return;
    
    if (getScale() > maxScale())
        return;
    
    if (!_panBoundsRect.equals(CCRectZero))
    {
        CCSize winSize = CCDirector::sharedDirector()->getWinSize();
        float rightEdgeDistance = this->rightEdgeDistance();
        float leftEdgeDistance = this->leftEdgeDistance();
        float topEdgeDistance = this->topEdgeDistance();
        float bottomEdgeDistance = this->bottomEdgeDistance();
        float scale = this->minPossibleScale();
        
        if (!rightEdgeDistance && !leftEdgeDistance && !topEdgeDistance && !bottomEdgeDistance)
        {
            return;
        }
        
        if (this->getScale() < scale)
        {
            _rubberEffectRecovering = true;
            CCPoint newPosition = CCPointZero;
            if (rightEdgeDistance && leftEdgeDistance && topEdgeDistance && bottomEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * (this->getAnchorPoint().x - 0.5f);
                float dy = scale * this->getContentSize().height * (this->getAnchorPoint().y - 0.5f);
                newPosition = ccp(winSize.width * 0.5f + dx, winSize.height * 0.5f + dy);
            }
            else if (rightEdgeDistance && leftEdgeDistance && topEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * (this->getAnchorPoint().x - 0.5f);
                float dy = scale * this->getContentSize().height * (1.0f - this->getAnchorPoint().y);
                newPosition = ccp(winSize.width * 0.5f + dx, winSize.height - dy);
            }
            else if (rightEdgeDistance && leftEdgeDistance && bottomEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * (this->getAnchorPoint().x - 0.5f);
                float dy = scale * this->getContentSize().height * this->getAnchorPoint().y;
                newPosition = ccp(winSize.width * 0.5f + dx, dy);
            }
            else if (rightEdgeDistance && topEdgeDistance && bottomEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * (1.0f - this->getAnchorPoint().x);
                float dy = scale * this->getContentSize().height * (this->getAnchorPoint().y - 0.5f);
                newPosition = ccp(winSize.width  - dx, winSize.height  * 0.5f + dy);
            }
            else if (leftEdgeDistance && topEdgeDistance && bottomEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * this->getAnchorPoint().x;
                float dy = scale * this->getContentSize().height * (this->getAnchorPoint().y - 0.5f);
                newPosition = ccp(dx, winSize.height * 0.5f + dy);
            }
            else if (leftEdgeDistance && topEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * this->getAnchorPoint().x;
                float dy = scale * this->getContentSize().height * (1.0f - this->getAnchorPoint().y);
                newPosition = ccp(dx, winSize.height - dy);
            }
            else if (leftEdgeDistance && bottomEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * this->getAnchorPoint().x;
                float dy = scale * this->getContentSize().height * this->getAnchorPoint().y;
                newPosition = ccp(dx, dy);
            }
            else if (rightEdgeDistance && topEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * (1.0f - this->getAnchorPoint().x);
                float dy = scale * this->getContentSize().height * (1.0f - this->getAnchorPoint().y);
                newPosition = ccp(winSize.width - dx, winSize.height - dy);
            }
            else if (rightEdgeDistance && bottomEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * (1.0f - this->getAnchorPoint().x);
                float dy = scale * this->getContentSize().height * this->getAnchorPoint().y;
                newPosition = ccp(winSize.width - dx, dy);
            }
            else if (topEdgeDistance || bottomEdgeDistance)
            {
                float dy = scale * this->getContentSize().height * (this->getAnchorPoint().y - 0.5f);
                newPosition = ccp(this->getPosition().x, winSize.height * 0.5f + dy);
            }
            else if (leftEdgeDistance || rightEdgeDistance)
            {
                float dx = scale * this->getContentSize().width * (this->getAnchorPoint().x - 0.5f);
                newPosition = ccp(winSize.width * 0.5f + dx, this->getPosition().y);
            }
            
            CCMoveTo *moveToPosition = CCMoveTo::create( _rubberEffectRecoveryTime,newPosition);
            CCScaleTo *scaleToPosition = CCScaleTo::create( _rubberEffectRecoveryTime,scale);
            CCFiniteTimeAction *sequence = CCSpawn::create(scaleToPosition, moveToPosition, CCCallFunc::create( this, callfunc_selector(CCLayerPanZoom::recoverEnded)), NULL);
            this->runAction(sequence);
        }
        else
        {
            _rubberEffectRecovering = false;
            CCMoveTo *moveToPosition = CCMoveTo::create(_rubberEffectRecoveryTime,ccp(this->getPosition().x + rightEdgeDistance - leftEdgeDistance,
                                                                                      this->getPosition().y + topEdgeDistance - bottomEdgeDistance));
            CCFiniteTimeAction *sequence = CCSpawn::create(moveToPosition, CCCallFunc::create( this, callfunc_selector(CCLayerPanZoom::recoverEnded)), NULL);
            this->runAction(sequence);
        }
    }
}

void CCLayerPanZoom::recoverEnded(){
    _rubberEffectRecovering = false;
}

float CCLayerPanZoom::topEdgeDistance(){
    CCRect boundBox = this->boundingBox();
    return (int)(MAX(_panBoundsRect.size.height + _panBoundsRect.origin.y - this->getPosition().y -
                     boundBox.size.height * (1 - this->getAnchorPoint().y), 0));
}

float CCLayerPanZoom::leftEdgeDistance(){
    CCRect boundBox = this->boundingBox();
    return (int)(MAX(this->getPosition().x - boundBox.size.width * this->getAnchorPoint().x - _panBoundsRect.origin.x, 0));
}

float CCLayerPanZoom::bottomEdgeDistance(){
    CCRect boundBox = this->boundingBox();
    return (int)(MAX(this->getPosition().y - boundBox.size.height * this->getAnchorPoint().y - _panBoundsRect.origin.y, 0));
}

float CCLayerPanZoom::rightEdgeDistance(){
    CCRect boundBox = this->boundingBox();
    return (int)(MAX(_panBoundsRect.size.width + _panBoundsRect.origin.x - this->getPosition().x -
                     boundBox.size.width * (1 - this->getAnchorPoint().x), 0));
}

float CCLayerPanZoom::minPossibleScale(){
    if (!_panBoundsRect.equals(CCRectZero))
    {
        return MAX(_panBoundsRect.size.width / this->getContentSize().width,
                   _panBoundsRect.size.height /this->getContentSize().height);
    }
    else
    {
        return _minScale;
    }
}

CCLayerPanZoomFrameEdge CCLayerPanZoom::frameEdgeWithPoint( CCPoint point){
    bool isLeft = point.x <= _panBoundsRect.origin.x + _leftFrameMargin;
    bool isRight = point.x >= _panBoundsRect.origin.x + _panBoundsRect.size.width - _rightFrameMargin;
    bool isBottom = point.y <= _panBoundsRect.origin.y + _bottomFrameMargin;
    bool isTop = point.y >= _panBoundsRect.origin.y + _panBoundsRect.size.height - _topFrameMargin;
    
    if (isLeft && isBottom)
    {
        return kCCLayerPanZoomFrameEdgeBottomLeft;
    }
    if (isLeft && isTop)
    {
        return kCCLayerPanZoomFrameEdgeTopLeft;
    }
    if (isRight && isBottom)
    {
        return kCCLayerPanZoomFrameEdgeBottomRight;
    }
    if (isRight && isTop)
    {
        return kCCLayerPanZoomFrameEdgeTopRight;
    }
    
    if (isLeft)
    {
        return kCCLayerPanZoomFrameEdgeLeft;
    }
    if (isTop)
    {
        return kCCLayerPanZoomFrameEdgeTop;
    }
    if (isRight)
    {
        return kCCLayerPanZoomFrameEdgeRight;
    }
    if (isBottom)
    {
        return kCCLayerPanZoomFrameEdgeBottom;
    }
    
    return kCCLayerPanZoomFrameEdgeNone;
}

float CCLayerPanZoom::horSpeedWithPosition(CCPoint pos){
    CCLayerPanZoomFrameEdge edge = this->frameEdgeWithPoint(pos);
    float speed = 0.0f;
    if (edge == kCCLayerPanZoomFrameEdgeLeft)
    {
        speed = _minSpeed + (_maxSpeed - _minSpeed) *
        (_panBoundsRect.origin.x + _leftFrameMargin - pos.x) / _leftFrameMargin;
    }
    if (edge == kCCLayerPanZoomFrameEdgeBottomLeft || edge == kCCLayerPanZoomFrameEdgeTopLeft)
    {
        speed = _minSpeed + (_maxSpeed - _minSpeed) *
        (_panBoundsRect.origin.x + _leftFrameMargin - pos.x) / (_leftFrameMargin * sqrt(2.0f));
    }
    if (edge == kCCLayerPanZoomFrameEdgeRight)
    {
        speed = - (_minSpeed + (_maxSpeed - _minSpeed) *
                   (pos.x - _panBoundsRect.origin.x - _panBoundsRect.size.width +
                    _rightFrameMargin) / _rightFrameMargin);
    }
    if (edge == kCCLayerPanZoomFrameEdgeBottomRight || edge == kCCLayerPanZoomFrameEdgeTopRight)
    {
        speed = - (_minSpeed + (_maxSpeed - _minSpeed) *
                   (pos.x - _panBoundsRect.origin.x - _panBoundsRect.size.width +
                    _rightFrameMargin) / (_rightFrameMargin * sqrt(2.0f)));
    }
    return speed;
}

float CCLayerPanZoom::vertSpeedWithPosition(CCPoint pos){
    CCLayerPanZoomFrameEdge edge = this->frameEdgeWithPoint(pos);
    float speed = 0.0f;
    if (edge == kCCLayerPanZoomFrameEdgeBottom)
    {
        speed = _minSpeed + (_maxSpeed - _minSpeed) *
        (_panBoundsRect.origin.y + _bottomFrameMargin - pos.y) / _bottomFrameMargin;
    }
    if (edge == kCCLayerPanZoomFrameEdgeBottomLeft || edge == kCCLayerPanZoomFrameEdgeBottomRight)
    {
        speed = _minSpeed + (_maxSpeed - _minSpeed) *
        (_panBoundsRect.origin.y + _bottomFrameMargin - pos.y) / (_bottomFrameMargin * sqrt(2.0f));
    }
    if (edge == kCCLayerPanZoomFrameEdgeTop)
    {
        speed = - (_minSpeed + (_maxSpeed - _minSpeed) * 
                   (pos.y - _panBoundsRect.origin.y - _panBoundsRect.size.height + 
                    _topFrameMargin) / _topFrameMargin);
    }
    if (edge == kCCLayerPanZoomFrameEdgeTopLeft || edge == kCCLayerPanZoomFrameEdgeTopRight)
    {
        speed = - (_minSpeed + (_maxSpeed - _minSpeed) * 
                   (pos.y - _panBoundsRect.origin.y - _panBoundsRect.size.height + 
                    _topFrameMargin) / (_topFrameMargin * sqrt(2.0f)));
    }
    return speed;
} 
