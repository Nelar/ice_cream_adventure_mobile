#ifndef __END_MENU_H__
#define __END_MENU_H__

#include "cocos2d.h"

#include "SimpleAudioEngine.h"
#include <vector>
#include "Options.h"
#include "cPopupLayer.h"
#include "cSocialMenu.h"
#include "utils.h"
#include "MapMenuLayer.h"
#include <spine/spine.h>
#include <spine/spine-cocos2dx.h>

using namespace std;
using namespace cocos2d;

using namespace extension;

enum AfterLoadingType
{
    AfterLoadingTypeClose = 0,
    AfterLoadingTypeNext,
    AfterLoadingTypeLives
};


class EndGameLayer : public cocos2d::CCLayer
{
public:
    
	virtual bool init();
    virtual ~EndGameLayer();
    
	void menuCloseCallback(CCObject* pSender);
    
	CREATE_FUNC(EndGameLayer);
    
	void backCallback(CCObject* pSender);
	void playCallback(CCObject* pSender);
	void closeCallback(CCObject* pSender);
	void booster_1_Callback(CCObject* pSender);
	void booster_2_Callback(CCObject* pSender);
	void booster_3_Callback(CCObject* pSender);
    void helpModalCallback(CCObject* pSender);
    
    void nextCallback(CCObject* pSender);
    void retryCallback(CCObject* pSender);
    
	bool isLock();
    
	void levelPopup(int levelNum, int starCount, int targetScore, eLevelType typeLevel, eBoosterType first, eBoosterType second, eBoosterType third);
    void popupWin(int countStart, int countScore, int currentL);
	void popupLose(int countScore, eLevelType typeLevel, int currentL);
    
    void setLock(bool nLock) {lock = nLock; };
    
    void updateBoosters();
    
    virtual void changeOrientation();
    
    void closeEnded(CCNode* sender);
    
    virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    
    CCSprite* helpModal;
    bool isPopup = false;
    
    void addSocialLayer();
    
    void updateFacebook();
    void updateFacebookCallback(CCNode* pSender);
    
    void closeLoading();
    
    void nextAfterLoading(CCNode* node);
    void nextWithLivePanel(CCNode* node);
    void closeAfterLoading(CCNode* node);
    
    void afterGetScores();
    
    void finishingEnded(CCNode* sender);
    void addBonus(CCNode* pSender);
    void unclockMenu(CCNode* pSender);
private:
    
    void explosionCallback(CCNode* sender);
    
	bool lock;
	CCMenu* menu;
	CCSprite* popup;
	
	CCSprite* startCenter;
	CCSprite* startLeft;
	CCSprite* startRight;
    CCLabelTTF * targetTitle;
	CCLabelTTF* levelTitle;
    
	int currentLevel;
    
    CCLayer* score;
    
    CCMenuItemSprite* play;
    CCMenuItemSprite* next;
    CCMenuItemSprite* retry;
    
	CCMenuItemSprite* booster_1;
	CCMenuItemSprite* booster_2;
	CCMenuItemSprite* booster_3;
    
    CCMenuItemSprite* help;
    CCSprite* facebookPanel;
    
    
    eBoosterType firstBooster;
    eBoosterType secondBooster;
    eBoosterType thirdBooster;
    
    
    CCSprite* boosterPlus_1;
    CCSprite* boosterPlus_2;
    CCSprite* boosterPlus_3;
    
    CCMenuItemSprite* close;
    
    PopupLayer* popaplayer;
    
    void postOnWall(CCNode* pSender);
    
    void popupOk1(CCNode* pSender);
    void popupOk2(CCNode* pSender);
    void popupOk3(CCNode* pSender);
    void endTutorial(CCNode* pSender);
    void endStage(CCNode* pSender);
    
    void retryEnd(CCNode* pSender);
    void nextOldLevel(CCNode* pSender);
    
    SocialLayer* social;
    
    CCLabelTTF* boosterTitle;
    CCSprite* targetSubstrate;
    
    int currentlevel;
    bool isWin = false;
    
    CCLabelTTF* failed;
    CCSkeletonAnimation* guiWin;
    CCSkeletonAnimation* guiFailed;
    CCLabelTTF* scoreTitle;
    CCLabelTTF* targetPopupTitle;
    
    CCLabelTTF* labelTTF;
    
    CCSprite* booster1Check = NULL;
    CCSprite* booster2Check = NULL;
    CCSprite* booster3Check = NULL;
    
    AfterLoadingType afterLoadingType;
    
    CCSprite* spriteLoading = NULL;
    CCLabelTTF* labelLoad = NULL;
};

#endif