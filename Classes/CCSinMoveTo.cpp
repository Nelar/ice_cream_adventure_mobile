//
//  CCSinMoveTo.cpp
//  IceCreamSaga
//
//  Created by Nelar on 30.09.13.
//
//

#include "CCSinMoveTo.h"

CCSinMoveBy* CCSinMoveBy::create(float duration, const CCPoint& deltaPosition, float na, float nb, float nc, float nd)
{
    CCSinMoveBy *pRet = new CCSinMoveBy();
    pRet->initWithDuration(duration, deltaPosition, na, nb, nc, nd);
    pRet->autorelease();
    
    return pRet;
}

bool CCSinMoveBy::initWithDuration(float duration, const CCPoint& deltaPosition,  float na, float nb, float nc, float nd)
{
    if (CCActionInterval::initWithDuration(duration))
    {
        m_positionDelta = deltaPosition;
        a = na;
        b = nb;
        c = nc;
        d = nd;
        return true;
    }
    
    return false;
}

CCObject* CCSinMoveBy::copyWithZone(CCZone *pZone)
{
    CCZone* pNewZone = NULL;
    CCSinMoveBy* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = (CCSinMoveBy*)(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCSinMoveBy();
        pZone = pNewZone = new CCZone(pCopy);
    }
    
    CCActionInterval::copyWithZone(pZone);
    
    pCopy->initWithDuration(m_fDuration, m_positionDelta, a, b, c, d);
    
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
}

void CCSinMoveBy::startWithTarget(CCNode *pTarget)
{
    CCActionInterval::startWithTarget(pTarget);
    m_previousPosition = m_startPosition = pTarget->getPosition();
    startY = pTarget->getPositionY();
}

CCActionInterval* CCSinMoveBy::reverse(void)
{
    return CCSinMoveBy::create(m_fDuration, ccp( -m_positionDelta.x, -m_positionDelta.y), a, b, c, d);
}


void CCSinMoveBy::update(float t)
{
    if (m_pTarget)
    {
#if CC_ENABLE_STACKABLE_ACTIONS
        CCPoint currentPos = m_pTarget->getPosition();
        CCPoint diff = ccpSub(currentPos, m_previousPosition);
        m_startPosition = ccpAdd( m_startPosition, diff);
        CCPoint newPos =  ccpAdd( m_startPosition, ccpMult(m_positionDelta, t) );
        float y = startY + a + b*sin(c*newPos.x + d);
        newPos.y = y;
        float rot = c*newPos.x + d;
        rot = ((int)(rot*57.29577f))%360;
        m_pTarget->setRotation(45.0f);
        m_pTarget->setPosition(newPos);
        m_previousPosition = newPos;
#else
        m_pTarget->setPosition(ccpAdd( m_startPosition, ccpMult(m_positionDelta, t)));
#endif // CC_ENABLE_STACKABLE_ACTIONS
    }
}
