#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "cocos2d.h"
#include <vector>
#include "ClipNode.h"

using namespace std;
using namespace cocos2d;

enum eColorGameObject
{
	Blue = 0,
	Green = 1,
	Orange = 2,
	Puprle = 3,
	Red = 4,
	Yellow = 5,
	Colorurless = 6
};

enum eTypeGameObject
{
	Simple,
	Vertical,
	Horizontal,
	Bomb,
	Crystal,
	Cookie,
    Fish,
};

struct sGameObject: public CCObject
{
	sGameObject();
	sGameObject(eTypeGameObject nType, eColorGameObject nColor, int nX, int nY, int nXZero, int nYZero);	
	eTypeGameObject type;
	eColorGameObject color;
    ClipNode* node;
	CCSprite* sprite;

	int x;
	int y;
	int xZero;
	int yZero;
	bool isSelected;

	int crystalState;

	bool prepareToDie;

	bool isLock;
    
    bool isStrawberry;
    
    float delayDestroy;
    float delayStripe;

    bool isMegaBomb;
	bool isMegaBombVertical;
	bool isMegaBombHorizontal;
	bool isZombieBomb;
    bool isZombieStripe = false;

    
    bool isScore;
    vector<CCSprite*> stripes;

	void changeCristalSprite(CCNode* sender);	

	void changeType(eTypeGameObject nType);
	void changeColor(eColorGameObject nColor);
    void changeColorCancelled(CCNode* sender);
    void updateCurrentPosition(CCNode* sender);
    void cleaningBang(CCNode* sender);

	bool moveLeft();
	bool moveRight();
	bool moveUp();
	bool moveDown();
    
    bool moveToCurrentPosition();
    

	bool moveLeftDown();
	bool moveRightDown();

	void destroy(float delay = 0.01f);

	void lock();
	void unlock(CCNode* sender);
    void unvisibleObject(CCNode* sender);
    
    void lighting(CCPoint begin, CCPoint end, float scale, float delay);
    void bombBoom(CCNode* sender);
    
    bool isTime;
    void setTime();
    
    bool isNext = false;
    eTypeGameObject nextType;
    
    void setNext(CCObject* sender);
    bool isFished = false;
    
    CCNode* parentNext;
};

#endif
