#ifndef __GAME_MENU_H__
#define __GAME_MENU_H__

#include "cocos2d.h"

#include "SimpleAudioEngine.h"
#include <vector>
#include "Options.h"
#include "cPopupLayer.h"

#include <spine/spine.h>
#include <spine/spine-cocos2dx.h>

using namespace std;
using namespace cocos2d;
using namespace extension;

class GameMenuLayer : public cocos2d::CCLayer
{
public:

    virtual ~GameMenuLayer();
    
	virtual bool init(eLevelType ntype);	

	void setLevel(int lev);

	static GameMenuLayer* create(eLevelType ntype);

	void setTargetScore(int nTargetScore);
	void setCurrentScore(int nTargetScore);
	void setCountMoves(int nTargetScore);

	void setBringDownCurrent(int current);
	void setBringDownTarget(int target);
	int getBringDownCurrent();
	int getBringDownTarget();

	int getCountMoves();
	int getCurrentScore();
	int getTargetScore();

	void booster_1_Callback(CCObject* pSender);
	void booster_2_Callback(CCObject* pSender);
	void booster_3_Callback(CCObject* pSender);
    
	bool isLock();

	void banner(const char* name, const char* text, float delay, ccColor3B color);
    void bannerIce(const char* name, float delay, ccColor3B color);
    void bannerBringDown(const char* name, float delay, ccColor3B color);
	void end();
	void setLevelType(eLevelType nType);
	void setTime(int nTime);
	bool isTime();
	void startTime();

	void sugarCrash();
	void sweet();
	void tasty();
	void divine();
	void delicious();
    
    int getCurrentLevel(){return currentLevel;};

    bool isClear;
    
    virtual void changeOrientation(void);
    
    bool isBannerLock;
    bool isStrawberry;
    
    float speed;
    float accel;
    
    
    void snowCallback(CCObject* pSender);
    
    void createSnow();
    
    int time;
    
    CCMenuItemSprite* booster_1_Button;
	CCMenuItemSprite* booster_2_Button;
	CCMenuItemSprite* booster_3_Button;
    CCMenuItemSprite* snow;
    
    void popupExit(int iceTarget, int iceCount);
    
    int currentLevel;
    
    void addPopap();
    
    void addMovesInApp();
    void addHammer();
    void inappPopupCancel(CCNode* sender);
    
    int secondStar = -1;
    int thirdStar = -1;
    
    void updateProgressBar();
    
    CCLayer* createScoreLayer(int level);
    void hideAll();
    int getCountStar();
    
    eLevelType type;
    int targetScore;
	int currentScore;
    
    void closeLoading();
    bool isDialog = false;
    
    bool isThirdStar = false;
    CCSprite* bannerSprite;
    bool isLastAction = false;
    void inviteFriend();
    void addBonus(CCNode* pSender);
    void unlockLeftMenu(CCNode* pSender);
private:
    
    void playOnCallback(CCObject* pSender);
    void exitOnCallback(CCObject* pSender);
    
    CCMenuItemLabel* speedLabel;

	void timeCallback(CCNode* sender);
    void bannerCallback(CCNode* sender);
	int countMoves;

	int bringDownTarget;
	int bringDownCurrent;
    
    CCSprite* boost5;
    
    CCLayerColor* exitBack;

	CCSprite *upPanel;
	CCSprite *downPanel;
	CCSprite* progressBar;
    
    CCSprite* boostClear;

	CCLabelTTF* movesTitle;
	CCLabelTTF* labelCountMoves;
	
	CCNode* targetTitle;
    CCNode* scoreTitle;
	CCLabelTTF* labelTargetScore;

	CCLabelTTF* labelCurrentScore;

	CCMenu* menu;
    
    
    CCSprite* startPolosa_1;
    CCSprite* startPolosa_2;
    CCSprite* startPolosa_3;
    
    CCSprite* star1;
    CCSprite* star2;
    CCSprite* star3;

    bool lock;
    
    CCLabelTTF* labelBan;
    


	CCMenu* comMenu;
    
    bool isPopup;
    
    bool isLandscape;
    

    bool isSnow;
    
    CCParticleSystemQuad* particle;
    
    bool isWin;
    
    CCProgressTimer* timeGradient;
    
    int beginTime;
    
    
    
    CCSprite* upBack;
    CCSprite* downBack;
    CCMenu* menuExit;
    CCMenuItemSprite* playOn;
    CCMenuItemSprite* exitOn;
    
    PopupLayer* popupBooster;
    CCSprite* boosterPlus;
    
    void popupOk1(CCNode* pSender);
    void popupOk2(CCNode* pSender);
    void unclockMenu(CCNode* pSender);
    
    bool stopTime = false;
    
    bool isSugarCrash = false;
    
    float progressPolosa = 608.0f;
    
    CCSprite* mig = NULL;
    CCLabelTTF* labelTTF;
    int iceCountSave = 0;
    bool firstStep = true;
};

#endif