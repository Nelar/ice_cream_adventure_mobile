//
//  cComixScene.h
//  IceCreamSaga
//
//  Created by Nelar on 10.09.13.
//
//

#ifndef IceCreamSaga_cComixScene_h
#define IceCreamSaga_cComixScene_h

#include "cocos2d.h"
#include <vector>

#include <spine/spine.h>
#include <spine/spine-cocos2dx.h>

using namespace std;
using namespace cocos2d;
using namespace extension;

class ComixScene : public cocos2d::CCLayer
{
public:
    
    static ComixScene* create(int num);
	virtual bool init(int num);
    
	static cocos2d::CCScene* scene(int num);
    
    void skipCallback(CCObject* pSender);
    void nextCallback(CCObject* pSender);
    
    void pauseComix(CCNode* sender);
    void resumeComix(CCNode* sender);
    
    virtual void update(float delta);
    
    virtual void changeOrientation();
    
    void iceCookieHide(CCNode* sender);
    
private:
    
    void comix_1();
    void comix_1_end();
    void comix_2();
    void comix_2_end();
    void comix_3();
    void comix_3_end();
    void comix_4();
    void comix_4_end();
    void comix_5();
    void comix_5_end();
    void comix_6();
    void comix_6_end();
    void comix_7();
    void comix_7_end();
    void comix_8();
    void comix_8_end();
    void comix_9();
    void comix_10();
    void comix_11();
    void comix_12();
    void comix_13();
    void comix_14();
    void comix_15();

    CCMenuItemSprite* skip;
    CCMenuItemSprite* next;
    
    CCLayer* comix;
    
    int level;
    float time;
    bool pause;
    float comixTime;
    float nextBubbleTime;
    
    int comixNumber;
    CCSprite* fon;
    
    CCLabelTTF* text;
    
    CCSkeletonAnimation* bubbles;
    CCSkeletonAnimation* iceCookie;
    
    CCSprite* borderLeft;
    CCSprite* borderRight;
    
    CCMenu* menu;
    
    int pauseNumber;
};


#endif
