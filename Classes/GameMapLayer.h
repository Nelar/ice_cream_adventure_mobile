#ifndef __GAME_MAP_SCENE_H__
#define __GAME_MAP_SCENE_H__

#include "cocos2d.h"
#include "MapMenuLayer.h"
#include "LeftDownMenu.h"
#include "IceMotionStreak.h"
#include <vector>
#include <GUI/CCScrollView/CCScrollView.h>

#include <spine/spine.h>
#include <spine/spine-cocos2dx.h>

#include "cMapScrollLayer.h"
#include "CCLayerPanZoom.h"

using namespace extension;
using namespace std;
using namespace cocos2d;


class GameMapLayer : public cocos2d::CCLayer, public CCScrollViewDelegate
{
public:

    virtual ~GameMapLayer();
    
	virtual bool init(int nextLevel);

	static cocos2d::CCScene* scene(int nextLevel = -1);

	void levelCallback(CCObject* pSender);
    void superLevelCallback(CCObject* pSender);
	
	static GameMapLayer* create(int nextLevel);

	void drawPoins(CCNode* sender);
    
    void pinch(CCObject* obj);
    
    virtual void update(CCNode* sender);
    
    void boyMoveFinished(CCNode* sender);
    
    virtual void changeOrientation();
    
    virtual void scrollViewDidScroll(CCScrollView* view);
    virtual void scrollViewDidZoom(CCScrollView* view);
    
    void registerWithTouchDispatcher();
    void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
    
    void updateMenuBooster();
    
    void updateFacebook();
    void showMessageBoard();
    void hideMessageBoard();
    
    void unclockMenu(CCNode* pSender);
    void addBonus(CCNode* pSender);
    void addPostBonus(CCNode* pSender);
    
    void closeLoading();
    
private:

    int nextLevelForSender = 0;
    void levelPicsDraw();
    void levelLabelsDraw();
	vector<CCMenuItemSprite*> levelArray;
    
    PopupLayer* popupLayer;

	CCMenu* levels;

	CCPoint m_center;
	float m_fRadius;
	float m_fAngle;

	IceMotionStreak* streak;

	MapMenuLayer* menu;
	LeftDownMenuScene* leftDownMenu;

	CCSprite* map;

	CCPoint beginPoint;
    CCPoint prevPoint;
    
    float prevDistance;

	vector<CCPoint> path;

	int currentIdx;
	vector<CCPoint> newPath;

	CCLayerPanZoom* layerGradient;

	CCTexture2D* texBlue;
    int touchNumber;
    
    bool isPinch;
    
    bool lock;
    
    CCSprite* boy;
    CCSprite* icelittle;
    
    CCMenuItemSprite* close;
    
    CCSprite* dark;
    
    
    vector<CCNode*> levelPics;
    
    float popupScale;
    
    int nextlevelCurr;
    
    bool levelLock;
    bool isBuyLive = false;
};

#endif