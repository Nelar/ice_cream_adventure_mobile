#ifndef __MAIN_MENU_SCENE_H__
#define __MAIN_MENU_SCENE_H__

#include "cocos2d.h"
#include <vector>

#include "cGlobal.h"
#include "cPopupLayer.h"
#include "cSocialMenu.h"

using namespace std;
using namespace cocos2d;

enum eSettingState
{
	Setting,
	Facebook,
	HowPlay,
	AskedQuestions,
	Restore,
	Reset,
	Notification
};


class MainMenuScene : public cocos2d::CCLayer
{
public:

	virtual bool init();
    virtual ~MainMenuScene();
	static cocos2d::CCScene* scene();

	void menuPlayCallback(CCObject* pSender);
	void menuFacebookCallback(CCObject* pSender);
	void menuSettingCallback(CCObject* pSender);
	void soundCallback(CCObject* pSender);
	void musicCallback(CCObject* pSender);
	void helpCallback(CCObject* pSender);
	void cogwheelCallback(CCObject* pSender);
    void inviteCallback(CCObject* pSender);

	void closeSettingCallback(CCObject* pSender);
	void howToPlaySettingCallback(CCObject* pSender);
	void askedCallback(CCObject* pSender);
	void restoreCallback(CCObject* pSender);
	void resetCallback(CCObject* pSender);
	void notificationCallback(CCObject* pSender);
	void facebookCallback(CCObject* pSender);

	void settingFinished(CCNode* sender);
	void closeFinished(CCNode* sender);
	void tutorialFinished(CCNode* sender);

	void notif_1_Callback(CCObject* pSender);

	void tutorialCallback(CCObject* pSender);
	void nextCallback(CCObject* pSender);
	void closeTutorialCallback(CCObject* pSender);

	void goCallback(CCObject* pSender);

	CREATE_FUNC(MainMenuScene);
    
    virtual void changeOrientation();
    
    void resetPopup(CCObject* pSender);
    void facebookPopupConncetion(CCObject* pSender);
    void facebookPopupOk(CCNode* pSender);
    
    void resetPopupOk(CCNode* pSender);
    void unclockMenu(CCNode* pSender);
    
    void addBonus(CCNode* pSender);
    
    void facebookButtonHide();
    
    void facebookCheck(CCNode* pSender);
    
    void facebookUnlock(CCNode* pSender);
    
    
    void registerWithTouchDispatcher();
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
private:
    
    
    void unlock(CCNode* sender);
    void lock();

	CCMenuItemSprite* play;
	CCMenuItemSprite* facebook;
	CCMenuItemSprite* setting;
	CCMenuItemSprite* sound;
	CCMenuItemSprite* music;
	CCMenuItemSprite* ask;
	CCMenuItemSprite* cogwheel;
    CCMenuItemSprite* invite;

	CCMenuItemSprite* close;
	CCMenuItemSprite* tutorialClose;

	CCSprite* settingBlob;

	bool isBoy;
	bool isSetting;

	CCSprite* popup;
	CCMenu* popupMenu;
	CCMenu* menu;

	CCSprite* tutorialPopup;
	CCMenu* tutorialMenu;

	CCLabelTTF* label;

	bool isLock;
    bool isPopup;
    bool isTutorial;

	eSettingState settingState;

	bool notif_1;

	CCMenuItemSprite* notif_1_Button;

	CCSprite* tutorial;
	CCSprite* tutorialSprite;

	int helpNum;
	int tutorialNum;

	bool isSound;
	bool isMusic;

	CCSprite* soundClose;
	CCSprite* musicClose;
    
    CCSprite* background;
    CCSprite* logo;
    CCSprite* iceBlob;
    CCSprite* boy;
    CCSprite* iceLit;
    CCSprite* heapIce;
    CCSprite* title;
    CCMenuItemSprite* faceConnect;
    CCMenuItemSprite* howPlay;
    CCMenuItemSprite* freqAsk;
    CCMenuItemSprite* restorePurshase;
    CCMenuItemSprite* reset;
    CCMenuItemSprite* notification;
    CCMenuItemSprite* tutorialNext;
    
    PopupLayer* popupLayer;
    
    float popupScale;
    SocialLayer* social;
};

#endif