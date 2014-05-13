//
//  LoadingScene.h
//  IceCreamAdventure
//
//  Created by Nelar on 30.01.14.
//
//

#ifndef IceCreamAdventure_LoadingScene_h
#define IceCreamAdventure_LoadingScene_h
#include "cocos2d.h"
#include <HttpClient.h>

using namespace cocos2d;
using namespace extension;

class LoadingScene : public cocos2d::CCLayer
{
public:
	virtual bool init();
    virtual ~LoadingScene();
	static cocos2d::CCScene* scene();
    
	CREATE_FUNC(LoadingScene);
    
    virtual void changeOrientation();
    
    void loadingFinished(CCNode* sender);
    
    void linkCallback(CCObject * sender);

    void moreGamesLoadedCallback(CCNode *sender);
    void iconsServerResponse(CCHttpClient * client, CCHttpResponse * response);
    void moreGamesServerResponse(CCHttpClient * client, CCHttpResponse * response);
    void moreGamesRequest();
    void mmpTrackingInit();
    void trackingServerResponse(CCHttpClient * client, CCHttpResponse * response);
    
private:

    CCSprite* background;
    CCLabelTTF* labelLoad;
};

#endif
