#ifndef __MAPSCROLL_SCENE_H__
#define __MAPSCROLL_SCENE_H__

#include "cocos2d.h"
#include <vector>

using namespace cocos2d;

class MapScrollLayer : public cocos2d::CCLayer
{
public:
    
    virtual ~MapScrollLayer();
	virtual bool init();
    
	CREATE_FUNC(MapScrollLayer);
    
    virtual void changeOrientation();
    
    void registerWithTouchDispatcher();
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);
    
    
    float minScale = 0.7f;
    float maxScale = 1.2f;
private:
    
    CCLayerColor* watcher;
    
    CCPoint deaccelDiff;
    
    void deaccelerate(float dt);
    
    float multiplierX;
    float multiplierY;
    
    std::vector<CCPoint> lastScrolling;
    CCArray* touches;
    
    bool isZooming;
    
    bool zoomStart = false;
};

#endif