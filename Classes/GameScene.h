#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

#include "GameMenuLayer.h"
#include "EndGameLayer.h"
#include "LeftDownMenu.h"
#include "cGameObject.h"
#include "cGlobal.h"
#include <vector>
#include "ClipNode.h"

#include <spine/spine.h>
#include <spine/spine-cocos2dx.h>
using namespace extension;

using namespace std;
using namespace cocos2d;


enum eGameCellType
{
	NoneCell,
	SimpleCell,
	LockCell,
	IceCell,
	SuperIceCell,
	MegaIceCell,
	StoneCell,
    CageCell,
    CageIceCell,
    Chocolate
};

enum eGameElementShift
{
	LeftShift = 0,
	RightShift,
	DownShift,
	UpShift
};

struct sPortal
{
    CCSprite* sprite;
    int x;
    int y;
    int idx;
    bool enter;
};

class GameScene : public cocos2d::CCLayer
{
public:

    virtual ~GameScene();
    
	virtual bool init(int levNum);

	static cocos2d::CCScene* scene(int levNum);

	void menuCloseCallback(CCObject* pSender);

	virtual void update(float delta);

	static GameScene* create(int nLevel);

	void renderField();
    
    CCSprite * createBlur(CCSprite* sprite, int size, ccColor3B color, GLubyte opacity);

	void registerWithTouchDispatcher();
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);

	bool checkField();
    bool checkFieldLast();
	bool checkFieldFirst();
    bool checkFieldBringDown();
    
	bool checkElement(int i);
	void afterDeleting();
    void afterDeletingForCrystalStripe(sGameObject* currObj);
	void refillObject(CCNode* sender = NULL);
    void refillPortals(CCNode* sender = NULL);
	void refillLockObject(CCNode* sender);
    void nextStripe(CCObject* sender);
    void afterStripeCrystal(CCObject* sender);
    
    void refillObjectForCrystalStripe(CCNode* sender = NULL);
    void refillLockObjectForCrystalStripe(CCNode* sender);

	void shiftFinished(CCNode* sender);
	void destroyFinished(CCNode* sender);
	void refillFinished(CCNode* sender);

	bool checkIntersectTwoElements(int first, int second);

	void cleaningTrash(CCNode* sender);

	void setNumLevel(int nL);

	bool findMatch();

	void loadLevel(const char* levelName);

	void sugarCrash(CCNode* sender);

	void end();

	void winCallback(CCNode* sender);
	void loseCallback(CCNode* sender);

	void timeUpdate(CCNode* sender);
    
    void setTutorial_1();
    void setTutorial_2();
    void setTutorial_2_1();
    void setTutorial_2_2();
    
    void setTutorial_3_1();
    void setTutorial_3_2();
    
    void setTutorial_4_1();
    void setTutorial_4_2();
    
    void setTutorial_5_1();
    void setTutorial_5_2();
    
    void setTutorial_6_1();
    
    
    virtual void changeOrientation();
    
    void updateMenu(CCNode* sender);
    
    LeftDownMenuScene* leftDownMenu;
    
    void lose(CCNode* sender);
    
    
    bool isLoseDialog = false;
    
    bool isEndDialog = false;
    
    void addMovesInApp();
    void addHammer();
    void cancelPayment();
    void updateBoosters();
    
    void updateFacebook();
    
    bool lock;
    
    bool timesLock = false;
    bool timesLeftDownLock = false;
    
    void closeLoading();
    
    void afterGetScores();
    
    void beginBannerEndCallback(CCNode* sender);
    
    void unlockInvite();
    
private:
    
    void lightingBomb(CCPoint begin, CCPoint end, float scale, float delay);
    
    bool endConditionCheck();
    

	eLevelType gameType;

	float timeAfterAction;
	bool isFindMatch;

	void help(CCNode* sender);
    void portalsRender();

	void superIceDestroyed(CCNode* sender);
	void megaIceDestroyed(CCNode* sender);
	void stoneDestroyed(CCNode* sender);
    void chocolateDestroyed(CCNode* sender);
    void iceDestroyed(CCNode* sender);
	void unlock(CCNode* sender);
	void crystalAfterAction(CCNode* sender);
    void reInitFieldCancelled(CCNode* sender);
    bool addChocolate();
    
    void runFish(sGameObject* gameObject, float delta, float delay);
    void runFishEnded(CCNode* sender);
    
    void fishingEnded(CCNode* sender);
    void decrementMovesEnded(CCNode* sender);
    void fallEnded(CCNode* sender);
    
    void runParticle(CCNode* sender);

	void reInitField();

	void checkCellSimple(int i, int j);
    void checkCellLock(int i, int j);

	bool isSimpleCell(eGameCellType ntype);
	bool isLockCell(eGameCellType ntype);
	bool isNoneCell(int i, int j);
    
    bool isSpecialElement(sGameObject* object);

	int findGameObject(int x, int y);

	void addToDeleting(int idx);

	void win(CCNode* sender);
	void movesEnded(CCNode* sender);
	void fieldMoveFinished(CCNode* sender);
    void addingBacground(CCNode* sender);
    
    void visibleBacground(CCNode* sender);
    
    void deletingTeleportingObject(CCNode* sender);
    
	void skipCallback(CCObject* pSender);
    
    void chocolateDeleteElement(CCNode* sender);
    void popupExitWithDelay(CCNode* sender);
    
    CCClippingNode* stencilBackground;

	eGameCellType gameField[MAX_ROW][MAX_COLUMN];
	CCSprite* gameFieldSprites[MAX_ROW][MAX_COLUMN];
	vector<sGameObject*> gameObjects;
	vector<sGameObject*> forDeadObjects;

	int isSelectedGameObject;
	CCPoint beginPosition;
	vector<int> allDeletedChain;
	vector<int> currentMatch;
	eTypeGameObject crystalChangeType;

	eGameElementShift prevShift;

	int firstObject;
	int secondObject;

	bool direct;

	CCSprite* element_back;
	CCSprite* element;
	float widthElement;

	bool cellType;
	
	int idxSelectedObject;

	float xZero;
	float yZero;

	GameMenuLayer* menu;
    EndGameLayer* endGameMenu;
    
	CCSprite* field;
	CCClippingNode* background;
    char bufBackground[255];
    
    CCSprite* backgroundPortrait;
    CCSprite* backgroundlandscape;

    CCNode* batchNode;
	CCNode* fieldLayer;

	vector<CCPoint> cellsCheckedSimple;
    vector<CCPoint> cellsCheckedLock;

	int   numLevel;
	float fieldShiftX;
	float fieldShiftY;

	int iceCount;
    int iceTarget;
	int dropN;

	int columnCount;
	int rowCount;
	int colorNumber;
	bool isWin;
	bool isLose;

	bool isNeedCookie;

	int sweetCount;
    
    vector<CCPoint> crystals;
    int crystalIdx;
    
    void lighting(CCPoint begin, CCPoint end, float delay = 0.0f);
    
    bool isEnd;
    
    CCLayer* tutorialLayer;
    vector<pair<CCPoint, int>> tutorialCell;
    
    bool isTutorial;
    bool isSecondTutorial;
    
    vector<sGameObject*> nextDead;
    CCSequence* helpAction;
    
    float helpTime;
    
    vector<CCParticleSystemQuad*> particles;
    
    bool isSugarCrash;
    
    
    CCSprite* backDown;
    
    CCSprite* dark;
    
    vector<sGameObject*> gameObjectHV;
    vector<sGameObject*> newCreateObjects;
    
    bool isFish;
    bool isRunFish;
    bool firstFind = false;
    
    bool combo;
    
    int notRefill;
    
    vector<int> springsObject;
    vector<int> refillDelay;
    
    bool isBeganTouch;
    bool isCrystalStripe;
    
    CCSprite* backSprite;
    
    bool isChocolateDestroyed;
    
    CCSprite* stencilField;
    
    vector<sPortal> portals;
    
    CCTexture2D* texForField;
    
    int nextCrystalStripe = 0;
    
    vector<int> lastRefillObjects;
    
    bool firstAction = false;
    
    int tutorialNumber = -1;
    
    CCTexture2D* batchTexture;
    
    void scoreAnim(int nScore);
    
    bool isEndStencil = false;
    
    bool isPopupTime = false;
    
    int countReinitField = false;
    
    int beganCountMoves = 0;
    
    float stencilOffsetWidth = 0;
    
    CCLabelTTF* boosterTitle;
    
    bool lastFish = false;
    bool isWasFish = false;
    
    vector<CCSkeletonAnimation*> fishes;
    
    vector<CCPoint> iceCells;
    vector<CCPoint> superIceCells;
    
    bool isFinalAction = false;
    float isTeleportedObject = false;
    
    bool isTestLandscape = false;
    
    bool isNotElementFish = false;
    
    bool isFishNeedRun = false;
    bool crystalCrystal = false;
    bool isSimplified = false;
    bool isBeginBanner = false;
    
    CCAction* beginBannerAction = NULL;
    
    bool isRunChangeOrient = true;
    
    bool isFinalBanner = false;
    
    bool isLoseSpeed = false;
    bool isWinSpeed = false;
    
    CCAction* endAction = NULL;
};

#endif