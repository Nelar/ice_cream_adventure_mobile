#ifndef __LEFTDOWN_MENU_SCENE_H__
#define __LEFTDOWN_MENU_SCENE_H__

#include "cocos2d.h"
#include <vector>

using namespace std;
using namespace cocos2d;

enum eBackScene
{
	MainMenu,
	MapLayer
};

class LeftDownMenuScene : public cocos2d::CCLayer
{
public:

    virtual ~LeftDownMenuScene();
	virtual bool init();

	void soundCallback(CCObject* pSender);
	void musicCallback(CCObject* pSender);
	void helpCallback(CCObject* pSender);
	void cogwheelCallback(CCObject* pSender);
    void inviteCallback(CCObject* pSender);

	void menuSettingCallback(CCObject* pSender);

	void settingFinished(CCNode* sender);

	void tutorialCallback(CCObject* pSender);
	void closeTutorialCallback(CCObject* pSender);

	void setBackScene(eBackScene nScene);
	void tutorialFinished(CCNode* sender);

	bool isLock();

	CREATE_FUNC(LeftDownMenuScene);

	void hide();
    
    bool isIntersect(CCPoint location);
    
    void setLock(bool nLock) {isOpen = nLock; };
    
    virtual void changeOrientation();
    
    bool exitPress = false;
    
    void lock();
    void unlock(CCNode* sender);
    
    bool isInvite = false;

private:

	bool islock;

	CCMenuItemSprite* setting;
	CCMenuItemSprite* sound;
	CCMenuItemSprite* music;
	CCMenuItemSprite* ask;
	CCMenuItemSprite* cogwheel;
    CCMenuItemSprite* invite;
	
	CCMenuItemSprite* tutorialClose;

	CCSprite* settingBlob;

	bool isSetting;
	CCMenu* menu;

	CCSprite* tutorialPopup;
	CCMenu* tutorialMenu;

	CCSprite* tutorialSprite;
	

	int tutorialNum;
	eBackScene backScene;

	bool isSound;
	bool isMusic;

	CCSprite* soundClose;
	CCSprite* musicClose;

	CCSprite* musicNormal;
	CCSprite* musicSelected;

	CCSprite* soundNormal;
	CCSprite* soundSelected;
    
    bool isLockBlob;
    bool isTutorial;
    
    CCMenuItemSprite* tutorialNext;
    
    float popupScale;
    
    bool isOpen = false;

};

#endif