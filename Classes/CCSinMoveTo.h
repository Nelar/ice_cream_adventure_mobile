//
//  CCSinMoveTo.h
//  IceCreamSaga
//
//  Created by Nelar on 30.09.13.
//
//

#ifndef __IceCreamSaga__CCSinMoveTo__
#define __IceCreamSaga__CCSinMoveTo__

#include "cocos2d.h"
#include "cGlobal.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace cocos2d;

class CCSinMoveBy : public CCActionInterval
{
public:
    bool initWithDuration(float duration, const CCPoint& deltaPosition, float na, float nb, float nc, float nd);
    
    virtual CCObject* copyWithZone(CCZone* pZone);
    virtual void startWithTarget(CCNode *pTarget);
    virtual CCActionInterval* reverse(void);
    virtual void update(float time);
    
public:
    static CCSinMoveBy* create(float duration, const CCPoint& deltaPosition, float na, float nb, float nc, float nd);
protected:
    CCPoint m_positionDelta;
    CCPoint m_startPosition;
    CCPoint m_previousPosition;
    float a;
    float b;
    float c;
    float d;
    
    float startY;
};

#endif
