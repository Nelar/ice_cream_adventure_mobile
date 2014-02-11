#ifndef __MAP_MENU_H__
#define __MAP_MENU_H__

#include "cocos2d.h"

#include "SimpleAudioEngine.h"
#include <vector>
#include "Options.h"
#include "cPopupLayer.h"
#include "cSocialMenu.h"
#include "MMPInterface.h"
#include "MMP/Banner.h"
#include "utils.h"

using namespace Core;
using namespace MarketingPlatform;
using namespace std;
using namespace cocos2d;

enum eBoosterType
{
	BoosterCrystal,
	BoosterBomb,
    BoosterFish,
    BoosterDonut,
	BoosterNone
};

class MapMenuLayer : public cocos2d::CCLayer
{
public:

	virtual bool init();
    virtual ~MapMenuLayer();

	void menuCloseCallback(CCObject* pSender);

	CREATE_FUNC(MapMenuLayer);

	void backCallback(CCObject* pSender);
	void playCallback(CCObject* pSender);
	void closeCallback(CCObject* pSender);
	void booster_1_Callback(CCObject* pSender);
	void booster_2_Callback(CCObject* pSender);
	void booster_3_Callback(CCObject* pSender);
    void helpModalCallback(CCObject* pSender);
    
    void livesCallback(CCObject* pSender);

	bool isLock();

	void levelPopup(int levelNum, int starCount, int targetScore, eLevelType typeLevel, eBoosterType first, eBoosterType second, eBoosterType third);

    void setLock(bool nLock) {lock = nLock; };
    
    void updateBoosters();
    
    void timeCallback(CCNode* sender);
    
    virtual void changeOrientation();
    
    void fishingEnded(CCNode* sender);
    void closeEnded(CCNode* sender);
    
    void hideLive();
    void showLive();
    
    void registerWithTouchDispatcher();
    virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    
    CCSprite* helpModal;
    bool isPopup;
    
    void popupLives();
    void askFriendCallback(CCObject* pSender);
    void moreLivesCallback(CCObject* pSender);
    void closeHeartCallback(CCObject* pSender);
    
    void sendLife(CCObject* pSender);
    
    CCLayer* createScoreLayer(int level);
    
    void addSocialLayer();
    
    void bannerCallback(CCObject* pSender);
    void closeBanner(CCObject* pSender);
    
    void updateFacebook();
    
    void showMessageboard();
    
    void notif_1_Callback(CCObject* pSender);
    
    void checkMessage(CCNode* pSender);
    
    void playAfterLoad(CCNode* pSender);
private:

	bool lock;
	CCMenu* menu;
	CCSprite* popup;
	
	CCSprite* startCenter;
	CCSprite* startLeft;
	CCSprite* startRight;
	CCLabelTTF* targetTitle;
	CCLabelTTF* levelTitle;

	int currentLevel;
    
    CCLayer* score;
    
    CCMenuItemSprite* play;

	CCMenuItemSprite* booster_1;
	CCMenuItemSprite* booster_2;
	CCMenuItemSprite* booster_3;
    
    CCMenuItemSprite* help;
    
    CCMenu* livesMenu;
    CCMenuItemSprite* livesPanel;
    
    
    eBoosterType firstBooster;
    eBoosterType secondBooster;
    eBoosterType thirdBooster;
    
    
    CCSprite* boosterPlus_1;
    CCSprite* boosterPlus_2;
    CCSprite* boosterPlus_3;
    
    CCSprite* facebookPanel;
    
    CCMenuItemSprite* close;
    
    CCLayerColor* livesLayer;
    
    CCLabelTTF* livesCount;
    
    CCSprite* panelLivesLayer;
    CCMenu* heartMenu;
    CCMenuItemSprite* askFriend;
    CCMenuItemSprite* moreLives;
    CCMenuItemSprite* closeHeart;
    
    CCNode* livesTime;
    
    PopupLayer* popaplayer;
    
    void popupOk1(CCNode* pSender);
    void popupOk2(CCNode* pSender);
    void popupOk3(CCNode* pSender);
    
    void addBonus(CCNode* pSender);
    void unlockStage(CCNode* pSender);
    void unclockMenu(CCNode* pSender);
    
    void updateFacebookCallback(CCNode* pSender);
    
    CCLabelTTF* livesTimePopap;
    CCMenuItemSprite* notif_1_Button;
    
    SocialLayer* social;
    Banner* bannerMMP;
    
    CCLabelTTF* labelTTF;
    
    int levelNumForPost = 0;
};

#endif