#include "GameScene.h"
#include <cmath>
#include "cGlobal.h"
#include "SimpleAudioEngine.h"
#include "tinyxml/tinyxml.h"
#include "IceMotionStreak.h"

#include "kazmath/GL/matrix.h"
#include "shaders/CCGLProgram.h"
#include "shaders/CCShaderCache.h"
#include "CCDirector.h"
#include "support/CCPointExtension.h"
#include "draw_nodes/CCDrawingPrimitives.h"
#include "GameMapLayer.h"
#include "nMMP.h"
#include "cFacebook.h"

using namespace CocosDenshion;
using namespace cocos2d;

#define TUTORIAL_DELAY 0.5f


GameScene* GameScene::create(int nLevel)
{
	GameScene *pRet = new GameScene();
	if (pRet && pRet->init(nLevel))
	{ 
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

CCScene* GameScene::scene(int levNum)
{
    CCScene * scene = NULL;
    do
    {
        scene = CCScene::create();
        CC_BREAK_IF(! scene);

        GameScene *layer = GameScene::create(levNum);
        CC_BREAK_IF(! layer);
        scene->addChild(layer);
    } while (0);

    return scene;
}

void GameScene::loadLevel(const char* levelName)
{
	TiXmlDocument doc(levelName);
	if(!doc.LoadFile())
		return;

	TiXmlElement* root = doc.RootElement();
	columnCount = atoi(root->Attribute("width"));
	rowCount = atoi(root->Attribute("height"));
    int backNum = atoi(root->Attribute("background"));
    
	sprintf(bufBackground, "backgrounds/%s.png", root->Attribute("background"));
    
	if (!strcmp(root->Attribute("type"), "score"))
		gameType = Score;
	else if (!strcmp(root->Attribute("type"), "time"))
		gameType = Time;
	else if (!strcmp(root->Attribute("type"), "ice"))
		gameType = Ice;
	else if (!strcmp(root->Attribute("type"), "bringDown"))
		gameType = BringDown;

	menu->setLevelType(gameType);
    
    
    if (backNum != GlobalsPtr->currentBackground)
    {
        if (CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("levelBackground"))
            CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFrameByName("levelBackground");
        
        CCSprite* levelBack = CCSprite::create(bufBackground);
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(levelBack->displayFrame(), "levelBackground");
        GlobalsPtr->currentBackground = backNum;
    }

	backDown = CCSprite::createWithSpriteFrameName("levelBackground");
	backDown->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 2.0f, CCDirector::sharedDirector()->getWinSize().height / 2.0f));
    backDown->setBlendFunc((ccBlendFunc){GL_ONE, GL_ZERO});
    
	this->addChild(backDown, 1);
    if (LANDSCAPE)
    {
        if (WINSIZE.width == 1136)
        {
            backDown->setScaleX(1.12f);
        }
    }
    else
    {
        if (WINSIZE.height == 1136)
        {
            backDown->setScaleY(1.12f);
        }
    }
    
	xZero = (CCDirector::sharedDirector()->getWinSize().width - CELL_WIDTH*columnCount)/2.0f +
		+ CELL_WIDTH/2.0f;
	yZero = CCDirector::sharedDirector()->getWinSize().height -
		(CCDirector::sharedDirector()->getWinSize().height - CELL_HEIGHT*rowCount)/2.0f;
	
	fieldLayer = CCNode::create();
	fieldLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    
	this->addChild(fieldLayer, 2);
    
    batchNode = CCNode::create();//CCSpriteBatchNode::createWithTexture(batchTexture);
    batchNode->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
	fieldLayer->addChild(batchNode, 3);
	
	if (gameType == Time)
	{
		menu->setTime(atoi(root->Attribute("time")));
	}
	else
	{
		menu->setCountMoves(atoi(root->Attribute("movesCount")));
        beganCountMoves = atoi(root->Attribute("movesCount"));
	}
	menu->setTargetScore(atoi(root->Attribute("targetScore")));
	colorNumber = atoi(root->Attribute("colorNumber"));
    
    menu->secondStar = -1;
    if (root->Attribute("secondStar") != NULL)
        menu->secondStar = atoi(root->Attribute("secondStar"));
    else
        menu->secondStar = menu->getTargetScore()*2.0f;
    
    menu->thirdStar = -1;
    if (root->Attribute("thirdStar") != NULL)
        menu->thirdStar = atoi(root->Attribute("thirdStar"));
    else
        menu->thirdStar = menu->getTargetScore()*3.0f;
    
    menu->updateProgressBar();
    
    menu->setCurrentScore(0);
    

    vector<sGameObject*> colorObjects;
	TiXmlElement* element = root->FirstChildElement();
    
    
    
    char buf[255];
    sprintf(buf, "%d", menu->getCurrentLevel());

    if (menu->getCurrentLevel() > 105)
    {
        if (menu->getCurrentLevel() == 106)
            MMPPtr->levelStarted(30, "30E");
        else if (menu->getCurrentLevel() == 107)
            MMPPtr->levelStarted(24, "24E");
        else if (menu->getCurrentLevel() == 108)
            MMPPtr->levelStarted(36, "36E");
        else if (menu->getCurrentLevel() == 109)
            MMPPtr->levelStarted(48, "48E");
        else if (menu->getCurrentLevel() == 110)
            MMPPtr->levelStarted(60, "60E");
        else if (menu->getCurrentLevel() == 111)
            MMPPtr->levelStarted(72, "72E");
    }
    else
    {
        MMPPtr->levelStarted(menu->getCurrentLevel(), string(buf));
    }
    
    if (menu->getCurrentLevel() >= 8)
    {
        for (int j = 0; j < columnCount; j++)
        {
            for (int i = 0; i < rowCount; i++)
            {
                if (!strcmp(element->Attribute("t"), "s"))
                    gameField[i][j] = SimpleCell;
                else if (!strcmp(element->Attribute("t"), "n"))
                    gameField[i][j] = NoneCell;
                else if (!strcmp(element->Attribute("t"), "l"))
                    gameField[i][j] = LockCell;
                else if (!strcmp(element->Attribute("t"), "i"))
                    gameField[i][j] = IceCell;
                else if (!strcmp(element->Attribute("t"), "si"))
                    gameField[i][j] = SuperIceCell;
                else if (!strcmp(element->Attribute("t"), "mi"))
                    gameField[i][j] = MegaIceCell;
                else if (!strcmp(element->Attribute("t"), "st"))
                    gameField[i][j] = StoneCell;
                else if (!strcmp(element->Attribute("t"), "gr"))
                    gameField[i][j] = CageCell;
                else if (!strcmp(element->Attribute("t"), "gri"))
                    gameField[i][j] = CageIceCell;
                else if (!strcmp(element->Attribute("t"), "ch"))
                    gameField[i][j] = Chocolate;
                else if (!strcmp(element->Attribute("t"), "chi"))
                {
                    iceCells.push_back(ccp(i,j));
                    iceCount++;
                    gameField[i][j] = Chocolate;
                }
                
                
                gameFieldSprites[i][j] = NULL;
                
                if (isSimpleCell(gameField[i][j]) || gameField[i][j] == CageCell || gameField[i][j] == CageIceCell)
                {
                    eColorGameObject color = (eColorGameObject)(rand()%colorNumber);
                    sGameObject* gameObj = new sGameObject(Simple, color, i, j, xZero, yZero);
                    batchNode->addChild(gameObj->node, 3);
                    gameObj->sprite->setVisible(true);
                    if (element->Attribute("color") != NULL)
                    {
                        if (!strcmp(element->Attribute("color"), "r"))
                            gameObj->changeColor(Red);
                        if (!strcmp(element->Attribute("color"), "g"))
                            gameObj->changeColor(Green);
                        if (!strcmp(element->Attribute("color"), "b"))
                            gameObj->changeColor(Blue);
                        if (!strcmp(element->Attribute("color"), "y"))
                            gameObj->changeColor(Yellow);
                        if (!strcmp(element->Attribute("color"), "o"))
                            gameObj->changeColor(Orange);
                        if (!strcmp(element->Attribute("color"), "p"))
                            gameObj->changeColor(Puprle);
                        colorObjects.push_back(gameObj);
                    }
                    if (element->Attribute("oT") != NULL)
                    {
                        if (!strcmp(element->Attribute("oT"), "cookie"))
                        {
                            gameObj->changeColor(Colorurless);
                            if(menu->isStrawberry)
                                gameObj->isStrawberry = true;
                            else
                                gameObj->isStrawberry = false;
                            gameObj->changeType(Cookie);
                        }
                        if (!strcmp(element->Attribute("oT"), "h"))
                        {
                            gameObj->changeType(Horizontal);
                        }
                        if (!strcmp(element->Attribute("oT"), "v"))
                        {
                            gameObj->changeType(Vertical);
                        }
                        if (!strcmp(element->Attribute("oT"), "b"))
                        {
                            gameObj->changeType(Bomb);
                        }
                        if (!strcmp(element->Attribute("oT"), "c"))
                        {
                            gameObj->changeColor(Colorurless);
                            gameObj->changeType(Crystal);
                        }
                        colorObjects.push_back(gameObj);
                    }
                    
                    if (element->Attribute("index") != NULL)
                    {
                        sPortal portal;
                        portal.x = i;
                        portal.y = j;
                        portal.idx = atoi(element->Attribute("index"));
                        portal.enter = portal.idx%2;
                        portals.push_back(portal);
                    }
                    gameObjects.push_back(gameObj);
                }
                
                element = element->NextSiblingElement();
            }
        }
    }
    else
    {
        for (int i = 0; i < rowCount; i++)
        {
            for (int j = 0; j < columnCount; j++)
            {
                if (!strcmp(element->Attribute("t"), "s"))
                    gameField[i][j] = SimpleCell;
                else if (!strcmp(element->Attribute("t"), "n"))
                    gameField[i][j] = NoneCell;
                else if (!strcmp(element->Attribute("t"), "l"))
                    gameField[i][j] = LockCell;
                else if (!strcmp(element->Attribute("t"), "i"))
                    gameField[i][j] = IceCell;
                else if (!strcmp(element->Attribute("t"), "si"))
                    gameField[i][j] = SuperIceCell;
                else if (!strcmp(element->Attribute("t"), "mi"))
                    gameField[i][j] = MegaIceCell;
                else if (!strcmp(element->Attribute("t"), "st"))
                    gameField[i][j] = StoneCell;
                else if (!strcmp(element->Attribute("t"), "gr"))
                    gameField[i][j] = CageCell;
                else if (!strcmp(element->Attribute("t"), "gri"))
                    gameField[i][j] = CageIceCell;
                else if (!strcmp(element->Attribute("t"), "ch"))
                    gameField[i][j] = Chocolate;
                else if (!strcmp(element->Attribute("t"), "chi"))
                {
                    iceCells.push_back(ccp(i,j));
                    iceCount++;
                    gameField[i][j] = Chocolate;
                }
                
                gameFieldSprites[i][j] = NULL;
                
                if (isSimpleCell(gameField[i][j]))
                {
                    eColorGameObject color = (eColorGameObject)(rand()%colorNumber);
                    sGameObject* gameObj = new sGameObject(Simple, color, i, j, xZero, yZero);
                    batchNode->addChild(gameObj->node, 3);
                    gameObj->sprite->setVisible(true);
                    if (element->Attribute("color") != NULL)
                    {
                        if (!strcmp(element->Attribute("color"), "r"))
                            gameObj->changeColor(Red);
                        if (!strcmp(element->Attribute("color"), "g"))
                            gameObj->changeColor(Green);
                        if (!strcmp(element->Attribute("color"), "b"))
                            gameObj->changeColor(Blue);
                        if (!strcmp(element->Attribute("color"), "y"))
                            gameObj->changeColor(Yellow);
                        if (!strcmp(element->Attribute("color"), "o"))
                            gameObj->changeColor(Orange);
                        if (!strcmp(element->Attribute("color"), "p"))
                            gameObj->changeColor(Puprle);
                        colorObjects.push_back(gameObj);
                    }
                    if (element->Attribute("oT") != NULL)
                    {
                        if (!strcmp(element->Attribute("oT"), "cookie"))
                        {
                            gameObj->changeColor(Colorurless);
                            if(menu->isStrawberry)
                                gameObj->isStrawberry = true;
                            else
                                gameObj->isStrawberry = false;
                            gameObj->changeType(Cookie);
                        }
                        if (!strcmp(element->Attribute("oT"), "h"))
                        {
                            gameObj->changeType(Horizontal);
                        }
                        if (!strcmp(element->Attribute("oT"), "v"))
                        {
                            gameObj->changeType(Vertical);
                        }
                        if (!strcmp(element->Attribute("oT"), "b"))
                        {
                            gameObj->changeType(Bomb);
                        }
                        if (!strcmp(element->Attribute("oT"), "c"))
                        {
                            gameObj->changeColor(Colorurless);
                            gameObj->changeType(Crystal);
                        }
                        colorObjects.push_back(gameObj);
                    }
                    
                    if (element->Attribute("index") != NULL)
                    {
                        sPortal portal;
                        portal.x = i;
                        portal.y = j;
                        portal.idx = atoi(element->Attribute("index"));
                        portal.enter = portal.idx%2;
                        portals.push_back(portal);
                    }
                    gameObjects.push_back(gameObj);
                }
                
                element = element->NextSiblingElement();
            }
        }

    }

	renderField();
    
	bool isCheckField = true;

	while (isCheckField)
	{
		if (checkFieldFirst())
		{
			for(int i = 0; i < allDeletedChain.size(); i++)
			{
                bool changeFlag = false;
                for(int j = 0; j < colorObjects.size(); j++)
                {
                    if (gameObjects[allDeletedChain[i]] == colorObjects[j])
                        changeFlag = true;
                }
                if (gameObjects[allDeletedChain[i]]->type != Simple)
                    changeFlag = true;
                if (!changeFlag)
                    gameObjects[allDeletedChain[i]]->changeColor((eColorGameObject)(rand()%colorNumber));
			}
			allDeletedChain.clear();
		}
		else
		{
			isCheckField = false;
		}
	}
    if (!findMatch())
    {
        isCheckField = false;
        while (isCheckField)
        {
            if (checkFieldFirst())
            {
                for(int i = 0; i < allDeletedChain.size(); i++)
                {
                    bool changeFlag = false;
                    for(int j = 0; j < colorObjects.size(); j++)
                    {
                        if (gameObjects[allDeletedChain[i]] == colorObjects[j])
                            changeFlag = true;
                    }
                    if (gameObjects[allDeletedChain[i]]->type != Simple)
                        changeFlag = true;
                    if (!changeFlag)
                        gameObjects[allDeletedChain[i]]->changeColor((eColorGameObject)(rand()%colorNumber));
                }
                allDeletedChain.clear();
            }
            else
            {
                isCheckField = false;
            }
        }
    }
    
    
    if (gameType == Ice)
	{
		menu->bannerIce("updateArt/target.png", 1.0f, ccWHITE);
        isBeginBanner = true;
	}
	else if (gameType == Score)
	{
		sprintf(buf, CCLocalizedString("BANNER_TEXT_SCORE"), menu->getTargetScore(), menu->getCountMoves());
		menu->banner("updateArt/target.png", buf, 1.0f, ccWHITE);
        isBeginBanner = true;

	}
	else if (gameType == BringDown)
	{
		menu->bannerBringDown("updateArt/target.png", 1.0f, ccWHITE);
		menu->setBringDownTarget(atoi(root->Attribute("bringDownCount")));
		menu->setBringDownCurrent(0);
        isBeginBanner = true;
	}
	else if (gameType == Time)
	{
		sprintf(buf, CCLocalizedString("BANNER_TEXT_TIME"), menu->getTargetScore(), atoi(root->Attribute("time")));
		menu->banner("updateArt/target.png", buf, 1.0f, ccWHITE);
        isBeginBanner = true;
	}
    beginBannerAction = this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.0f), CCCallFuncN::create(this, callfuncN_selector(GameScene::beginBannerEndCallback))));
    
    if (WINSIZE.width == 640)
    {
        if (columnCount == 10)
            fieldLayer->setScale(0.95f);
    }
    
    if (GlobalsPtr->booster_2)
    {
        int idxBoost = rand() % gameObjects.size();
        while (gameObjects[idxBoost]->type != Simple || gameField[gameObjects[idxBoost]->x][gameObjects[idxBoost]->y] == CageCell || gameField[gameObjects[idxBoost]->x][gameObjects[idxBoost]->y] == CageIceCell)
            idxBoost = rand() % gameObjects.size();
        if (rand()%2)
            gameObjects[idxBoost]->changeType(Bomb);
        else
            gameObjects[idxBoost]->changeType(Horizontal);
    }
    
    if (GlobalsPtr->booster_1)
    {
        int idxBoost = rand() % gameObjects.size();
        while (gameObjects[idxBoost]->type != Simple || gameField[gameObjects[idxBoost]->x][gameObjects[idxBoost]->y] == CageCell || gameField[gameObjects[idxBoost]->x][gameObjects[idxBoost]->y] == CageIceCell)
            idxBoost = rand() % gameObjects.size();
        gameObjects[idxBoost]->changeType(Crystal);
    }
    
    if (GlobalsPtr->booster_3 && menu->type != Score)
    {
        int idxBoost = rand() % gameObjects.size();
        while (gameObjects[idxBoost]->type != Simple || gameField[gameObjects[idxBoost]->x][gameObjects[idxBoost]->y] == CageCell || gameField[gameObjects[idxBoost]->x][gameObjects[idxBoost]->y] == CageIceCell)
            idxBoost = rand() % gameObjects.size();
        gameObjects[idxBoost]->changeType(Fish);
    }
    OptionsPtr->save();
}

void GameScene::beginBannerEndCallback(CCNode* sender)
{
    isBeginBanner = false;
    beginBannerAction = NULL;
}

GameScene::~GameScene()
{
    for (int i = 0; i < particles.size(); i++)
        particles[i]->removeFromParentAndCleanup(true);
    particles.clear();
    
    this->removeAllChildrenWithCleanup(true);
    
    this->stopAllActions();
    this->unscheduleAllSelectors();
//  CCSpriteFrameCache::sharedSpriteFrameCache()->removeUnusedSpriteFrames();
    CCTextureCache::sharedTextureCache()->removeUnusedTextures();
}

bool GameScene::init(int levNum)
{
    
    //CCDirector::sharedDirector()->setAnimationInterval(1.0f / 30.0f);
    GlobalsPtr->iceCreamScene = Game;
    
    if (LANDSCAPE)
        isTestLandscape = true;
    
    isChocolateDestroyed = false;
	columnCount = 0;
	rowCount = 0;
	timeAfterAction = 0.0f;
	isFindMatch = false;
	lock = false;
	cellType = false;
	idxSelectedObject = -1;
	numLevel = levNum;
	iceCount = 0;
	isWin = false;
	isNeedCookie = false;
	sweetCount = 0;
    isEnd = false;
    isSecondTutorial = false;
    helpTime = 0.0f;
    isSugarCrash = false;
    isFish = false;
    combo = false;
    notRefill = -2;
    isBeganTouch = false;
    isCrystalStripe = false;
    
    isRunFish = false;
    lock = true;
    
    iceCells.clear();
    superIceCells.clear();
    
    if (!IPAD)
        batchTexture = CCTextureCache::sharedTextureCache()->addImage("common.png");
    else
        batchTexture = CCTextureCache::sharedTextureCache()->addPVRImage("common.pvr.ccz");
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("common.plist", batchTexture);
    
    CCSprite* yellow = CCSprite::create("updateArt/yellow.png");
    
    CCSprite* yellow_vertical = CCSprite::create("updateArt/vertical.png");
    CCSprite* yellow_vertical_1 = CCSprite::create("updateArt/vertical_1.png");
    CCSprite* yellow_vertical_2 = CCSprite::create("updateArt/vertical_2.png");
    CCSprite* yellow_vertical_3 = CCSprite::create("updateArt/vertical_3.png");
    CCSprite* yellow_vertical_4 = CCSprite::create("updateArt/vertical_4.png");
    CCSprite* yellow_vertical_5 = CCSprite::create("updateArt/vertical_5.png");
    CCSprite* yellow_vertical_6 = CCSprite::create("updateArt/vertical_6.png");
    
    CCSprite* yellow_horizontal = CCSprite::create("updateArt/horizontal.png");
    CCSprite* yellow_horizontal_1 = CCSprite::create("updateArt/horizontal_1.png");
    CCSprite* yellow_horizontal_2 = CCSprite::create("updateArt/horizontal_2.png");
    CCSprite* yellow_horizontal_3 = CCSprite::create("updateArt/horizontal_3.png");
    CCSprite* yellow_horizontal_4 = CCSprite::create("updateArt/horizontal_4.png");
    CCSprite* yellow_horizontal_5 = CCSprite::create("updateArt/horizontal_5.png");
    CCSprite* yellow_horizontal_6 = CCSprite::create("updateArt/horizontal_6.png");
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow->displayFrame(), "yellow");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_vertical->displayFrame(), "yellow_vertical");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_vertical_1->displayFrame(), "yellow_vertical_1");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_vertical_2->displayFrame(), "yellow_vertical_2");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_vertical_3->displayFrame(), "yellow_vertical_3");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_vertical_4->displayFrame(), "yellow_vertical_4");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_vertical_5->displayFrame(), "yellow_vertical_5");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_vertical_6->displayFrame(), "yellow_vertical_6");
    
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_horizontal->displayFrame(), "yellow_horizontal");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_horizontal_1->displayFrame(), "yellow_horizontal_1");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_horizontal_2->displayFrame(), "yellow_horizontal_2");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_horizontal_3->displayFrame(), "yellow_horizontal_3");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_horizontal_4->displayFrame(), "yellow_horizontal_4");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_horizontal_5->displayFrame(), "yellow_horizontal_5");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(yellow_horizontal_6->displayFrame(), "yellow_horizontal_6");
    
    
    CCTextureCache::reloadAllTextures();
    
    batchTexture->setAntiAliasTexParameters();

	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sound/level_1_loop.mp3", true);
    
    if (IPHONE_5 || IPHONE_4)
    {
        if (!LANDSCAPE)
        {
            this->setScale(1.095f);
        }
    }
    

	crystalChangeType = Vertical;

	menu = GameMenuLayer::create(Ice);
	menu->setLevel(levNum);
	this->addChild(menu, 21);
    menu->addPopap();
    
    endGameMenu = EndGameLayer::create();
	this->addChild(endGameMenu, 23);
    endGameMenu->addSocialLayer();
    endGameMenu->setVisible(false);

	leftDownMenu = LeftDownMenuScene::create();
    leftDownMenu->setBackScene(MapLayer);
	this->addChild(leftDownMenu, 22);

	allDeletedChain.clear();
    
    if (!CCLayer::init())
		return false;
	
	isSelectedGameObject = -1;

	this->setTouchEnabled(true);

	char buf[255];
    if (menu->getCurrentLevel() >= 8)
        sprintf(buf, "levels/%d.txt", numLevel);
    else
        sprintf(buf, "levels/%d.xml", numLevel);
    

    if (!OptionsPtr->getLevelData(numLevel-1).isSimple)
    {
        int currLevel = OptionsPtr->getCurrentLevel();
        int countGames = OptionsPtr->getLevelData(numLevel-1).countGames;
        time_t timeLast = (time(0) - OptionsPtr->getLevelData(numLevel-1).lastGame);
        if ((time(0) - OptionsPtr->getLevelData(numLevel-1).lastGame) > 86400)
        {
            if (OptionsPtr->getLevelData(numLevel-1).countGames >= 4)
            {
                if (currLevel == numLevel)
                {
                    FacebookPtr->askBooster();
                    isSimplified = true;
                }
            }
        }
    }
    else
    {
        isSimplified = true;
    }
    
    OptionsPtr->setLevelData(numLevel-1, OptionsPtr->getLevelData(numLevel-1).countStar, OptionsPtr->getLevelData(numLevel-1).countScore, OptionsPtr->getLevelData(numLevel-1).levelType, false, OptionsPtr->getLevelData(numLevel-1).countGames+1, time(0), isSimplified);
    OptionsPtr->save();

    
	loadLevel(CCFileUtils::sharedFileUtils()->fullPathForFilename(buf).c_str());
    
    helpAction = NULL;
    
    if (WINSIZE.height < WINSIZE.width)
        fieldLayer->setPosition(CELL_WIDTH*2.0f, -CELL_HEIGHT/4.0f);
    else
        fieldLayer->setPosition(0.0f, 0.0f);

	fieldLayer->setPosition(fieldLayer->getPositionX() + CCDirector::sharedDirector()->getWinSize().width, fieldLayer->getPositionY());
	fieldLayer->runAction(CCSequence::create(CCDelayTime::create(1.5f),
		CCEaseOut::create(CCMoveBy::create(0.5f, ccp(-CCDirector::sharedDirector()->getWinSize().width, fieldLayer->getPositionY())), 2.5f),
        NULL));
    
    this->runAction(CCSequence::create(CCDelayTime::create(1.5f),
                                       CCCallFuncN::create(this, callfuncN_selector(GameScene::fieldMoveFinished)), NULL));
	
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.0f), CCCallFuncN::create(this, callfuncN_selector(GameScene::addingBacground))));
    
//  gameObjects[findGameObject(5, 7)]->changeType(Fish);
    
    
    this->scheduleUpdate();
    
    dark = CCSprite::createWithSpriteFrameName("game/cell.png");
    dark->setScale(1000);
    this->addChild(dark, leftDownMenu->getZOrder() - 1);
    dark->setOpacity(0);
    dark->setColor(ccBLACK);
    
    updateMenu(this);
    
    menu->createSnow();
    
    portalsRender();
    
    if (IPHONE_5 || IPHONE_4)
    {
        if (!LANDSCAPE)
        {
            backDown->setScaleX(backDown->getScaleX()*0.913f);
            backDown->setScaleY(backDown->getScaleY()*0.913f);
            if (false)
                leftDownMenu->setPosition(ccp(leftDownMenu->getPositionX() - 25, leftDownMenu->getPositionY() + 20));
            leftDownMenu->setScale(0.913f);
            menu->setScale(0.913f);
            endGameMenu->setScale(0.913f);
        }
    }

    
    return true;
}

void GameScene::portalsRender()
{
    for (int i = 0; i < portals.size(); i++)
    {
        CCAnimation *animation = CCAnimation::create();
        animation->setDelayPerUnit(0.03f);
        animation->setRestoreOriginalFrame(false);
        animation->setLoops(1);
        
        if (portals[i].enter)
        {
            portals[i].sprite = CCSprite::create("game/portal/portal-enter (1).png");
            gameFieldSprites[portals[i].x][portals[i].y]->addChild(portals[i].sprite, 10);
            portals[i].sprite->setPosition(ccp(CELL_WIDTH/2.0f, portals[i].sprite->getContentSize().height/5.0f));
            for (int j = 1; j < 24; j++)
            {
                char buf[255];
                sprintf(buf, "game/portal/portal-enter (%d).png", j);
                animation->addSpriteFrame(CCSprite::create(buf)->displayFrame());
            }
            portals[i].sprite->runAction(CCRepeatForever::create(CCAnimate::create(animation)));
        }
        else
        {
            portals[i].sprite = CCSprite::create("game/portal/portal-exit (1).png");
            gameFieldSprites[portals[i].x][portals[i].y]->addChild(portals[i].sprite, 10);
            portals[i].sprite->setPosition(ccp(CELL_WIDTH/2.0f, CELL_HEIGHT - portals[i].sprite->getContentSize().height/5.0f));
            for (int j = 1; j < 24; j++)
            {
                char buf[255];
                sprintf(buf, "game/portal/portal-exit (%d).png", j);
                animation->addSpriteFrame(CCSprite::create(buf)->displayFrame());
            }
            portals[i].sprite->runAction(CCRepeatForever::create(CCAnimate::create(animation)));
        }
    }
}

void GameScene::changeOrientation(void)
{
    this->setContentSize(WINSIZE);
    fieldLayer->stopAllActions();
    
    if (LANDSCAPE)
    {
        if (WINSIZE.width == 1136)
        {
            backDown->setScaleX(1.12f);
            backDown->setScaleY(1.0f);
        }
    }
    else
    {
        if (WINSIZE.height == 1136)
        {
            backDown->setScaleY(1.12f);
            backDown->setScaleX(1.0f);
        }
    }

    leftDownMenu->changeOrientation();
    menu->changeOrientation();
    endGameMenu->changeOrientation();
    
    fieldLayer->setContentSize(WINSIZE);
    
    backDown->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width / 2.0f, CCDirector::sharedDirector()->getWinSize().height / 2.0f));
    field->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, CCDirector::sharedDirector()->getWinSize().height/2.0f));
    
    xZero = (CCDirector::sharedDirector()->getWinSize().width - CELL_WIDTH*columnCount)/2.0f +
    + CELL_WIDTH/2.0f;
    
    yZero = CCDirector::sharedDirector()->getWinSize().height -
    (CCDirector::sharedDirector()->getWinSize().height - CELL_HEIGHT*rowCount)/2.0f;
    
    if (WINSIZE.height < WINSIZE.width)
        fieldLayer->setPosition(CELL_WIDTH*2.0f, -CELL_HEIGHT/2.0f);
    else
        fieldLayer->setPosition(0.0f, 0.0f);

    
    for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < columnCount; j++)
		{
            if (gameFieldSprites[i][j])
            {
                gameFieldSprites[i][j]->setPosition(ccp(j*CELL_WIDTH + xZero,
                                                        yZero - i*CELL_HEIGHT));
                if (gameFieldSprites[i][j]->getUserData())
                {
                    ((CCSprite*)gameFieldSprites[i][j]->getUserData())->setPosition(gameFieldSprites[i][j]->getPosition());
                }
            }
        }
	}
    
    for (int i = 0; i < gameObjects.size(); i++)
    {
        if (!isRunChangeOrient)
            gameObjects[i]->sprite->stopAllActions();
        gameObjects[i]->sprite->setPosition(ccp(gameObjects[i]->y*CELL_WIDTH + xZero, yZero - gameObjects[i]->x*CELL_HEIGHT));
        gameObjects[i]->xZero = xZero;
        gameObjects[i]->yZero = yZero;
        gameObjects[i]->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, WINSIZE.height));
    }
    
    for (int i = 0; i < fishes.size(); i++)
    {
        if (IPHONE_4 || IPHONE_5)
        {
            if (LANDSCAPE)
                fishes[i]->setPosition(ccp(fishes[i]->getPositionX() + CELL_WIDTH*6.0f,
                                           fishes[i]->getPositionY() - CELL_HEIGHT*4.0f));
            else
                fishes[i]->setPosition(ccp(fishes[i]->getPositionX() - CELL_WIDTH*6.0f,
                                           fishes[i]->getPositionY() + CELL_HEIGHT*4.0f));

        }
        else
        {
            if (LANDSCAPE)
                fishes[i]->setPosition(ccp(fishes[i]->getPositionX() + CELL_WIDTH*4.0f,
                                           fishes[i]->getPositionY() - CELL_HEIGHT*2.0f));
            else
                fishes[i]->setPosition(ccp(fishes[i]->getPositionX() - CELL_WIDTH*4.0f,
                                           fishes[i]->getPositionY() + CELL_HEIGHT*2.0f));
        }
    }
    
    if (tutorialNumber > 0)
    {
        if (tutorialNumber == 1)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_1();
        }
        else if (tutorialNumber == 2)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_2();
        }
        else if (tutorialNumber == 3)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_2_1();
        }
        else if (tutorialNumber == 4)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_2_2();
        }
        else if (tutorialNumber == 5)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_3_1();
        }
        else if (tutorialNumber == 6)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_3_2();
        }
        else if (tutorialNumber == 7)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_4_1();
        }
        else if (tutorialNumber == 8)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_5_1();
        }
        else if (tutorialNumber == 9)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_5_2();
        }
        else if (tutorialNumber == 10)
        {
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            setTutorial_6_1();
        }
    }
    
    if (menu->getCurrentLevel() <= 6)
        menu->bannerSprite->setVisible(false);
    
    if (isBeginBanner && (menu->getCurrentLevel() > 6))
    {
        menu->bannerSprite->setVisible(false);
        char buf[255];
        if (gameType == Ice)
        {
            menu->bannerIce("updateArt/target.png", 1.0f, ccWHITE);
            isBeginBanner = true;
        }
        else if (gameType == Score)
        {
            sprintf(buf, CCLocalizedString("BANNER_TEXT_SCORE"), menu->getTargetScore(), menu->getCountMoves());
            menu->banner("updateArt/target.png", buf, 1.0f, ccWHITE);
            isBeginBanner = true;
            
        }
        else if (gameType == BringDown)
        {
            menu->bannerBringDown("updateArt/target.png", 1.0f, ccWHITE);
            menu->setBringDownCurrent(0);
            isBeginBanner = true;
        }
        else if (gameType == Time)
        {
            sprintf(buf, CCLocalizedString("BANNER_TEXT_TIME"), menu->getTargetScore(), 60);
            menu->banner("updateArt/target.png", buf, 1.0f, ccWHITE);
            isBeginBanner = true;
        }

    }
    if (beginBannerAction)
    {
        this->stopAction(beginBannerAction);
        beginBannerAction = NULL;
    }
    beginBannerAction = this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.0f), CCCallFuncN::create(this, callfuncN_selector(GameScene::beginBannerEndCallback))));

    
    if (menu->isTextBanner || isFinalBanner)
    {
        menu->bannerSprite->setVisible(false);
    }
    
    if (IPHONE_5 || IPHONE_4)
    {
        if (LANDSCAPE)
        {
            this->setScale(1.0f);
            if (isTestLandscape)
                leftDownMenu->setPosition(ccp(leftDownMenu->getPositionX() + 25, leftDownMenu->getPositionY() - 20));
            leftDownMenu->setScale(1.0f);
            menu->setScale(1.0f);
        }
        else
        {
            this->setScale(1.095f);
            backDown->setScaleX(backDown->getScaleX()*0.913f);
            backDown->setScaleY(backDown->getScaleY()*0.913f);
            if (isTestLandscape)
                leftDownMenu->setPosition(ccp(leftDownMenu->getPositionX() - 25, leftDownMenu->getPositionY() + 20));
            leftDownMenu->setScale(0.913f);
            menu->setScale(0.913f);
        }
    }
    timesLock = false;
}

void GameScene::setTutorial_1()
{
    tutorialNumber = 1;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(0, 2), 0));
    tutorialCell.push_back(pair<CCPoint, int>(ccp(1, 2), 1));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(WINSIZE);
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(WINSIZE);
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell3 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell4 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    cell3->setScaleY(1.1f);
    cell4->setScaleY(1.1f);
    
    cell1->setPosition(gameFieldSprites[0][2]->getPosition());
    cell2->setPosition(gameFieldSprites[1][2]->getPosition());
    cell3->setPosition(gameFieldSprites[2][2]->getPosition());
    cell4->setPosition(gameFieldSprites[3][2]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    stencil->addChild(cell3);
    stencil->addChild(cell4);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    stencil->setScale(fieldLayer->getScale());
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/5.0f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    


    
    CCMenu* menu = CCMenu::create();
    tutorialLayer->addChild(menu);
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_1", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    skip->setOpacity(0);
    skip->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 255)));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setRotation(90);
    up->setPosition(ccp(gameFieldSprites[0][2]->getPosition().x - up->getContentSize().width,
                        gameFieldSprites[0][2]->getPosition().y));
    up->setOpacity(0);
    up->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0.0f, -50.0f)), CCMoveBy::create(0.3f, ccp(0.0f, 50.0f)))));
    tutorialLayer->addChild(up);
    
    CCSprite* down = CCSprite::createWithSpriteFrameName("game/right.png");
    down->setRotation(-90);
    down->setPosition(ccp(gameFieldSprites[0][2]->getPosition().x + down->getContentSize().width,
                          gameFieldSprites[1][2]->getPosition().y));
    down->setOpacity(0);
    down->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    down->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0.0f, 50.0f)), CCMoveBy::create(0.3f, ccp(0.0f, -50.0f)))));
    tutorialLayer->addChild(down);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        cell3->setPosition(ccp(cell3->getPositionX() + CELL_WIDTH*2.0f, cell3->getPositionY() - CELL_HEIGHT/2.0f));
        cell4->setPosition(ccp(cell4->getPositionX() + CELL_WIDTH*2.0f, cell4->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
        down->setPosition(ccp(down->getPositionX() + CELL_WIDTH*2.0f, down->getPositionY() - CELL_HEIGHT/2.0f));
    }
    char buf[255];
    sprintf(buf, "%d", this->menu->getCurrentLevel());
    MMPPtr->tutorialStepStarted(this->menu->getCurrentLevel(), string(buf));
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::setTutorial_2()
{
    tutorialNumber = 2;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(3, 3), 3));
    tutorialCell.push_back(pair<CCPoint, int>(ccp(3, 4), 2));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(ccp(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell3 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell4 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    cell3->setScaleY(1.1f);
    cell4->setScaleY(1.1f);
    
    cell1->setPosition(gameFieldSprites[3][3]->getPosition());
    cell2->setPosition(gameFieldSprites[3][4]->getPosition());
    cell3->setPosition(gameFieldSprites[3][5]->getPosition());
    cell4->setPosition(gameFieldSprites[3][6]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    stencil->addChild(cell3);
    stencil->addChild(cell4);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCFadeTo::create(0.5f, 200));
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/1.5f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCFadeTo::create(0.5f, 200));
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    
    CCMenu* menu = CCMenu::create();
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_2", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCFadeTo::create(0.5f, 200));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    tutorialLayer->addChild(menu);
    skip->setOpacity(0);
    skip->runAction(CCFadeTo::create(0.5f, 200));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setPosition(ccp(gameFieldSprites[3][3]->getPosition().x,
                        gameFieldSprites[3][3]->getPosition().y + up->getContentSize().height));
    up->setOpacity(0);
    up->runAction(CCFadeTo::create(0.5f, 200));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(50.0f, 0.0f)), CCMoveBy::create(0.3f, ccp(-50.0f, 0.0f)))));
    tutorialLayer->addChild(up);
    
    CCSprite* down = CCSprite::createWithSpriteFrameName("game/right.png");
    down->setPosition(ccp(gameFieldSprites[3][4]->getPosition().x,
                          gameFieldSprites[3][4]->getPosition().y - down->getContentSize().height));
    down->setOpacity(0);
    down->setFlipX(true);
    down->runAction(CCFadeTo::create(0.5f, 200));
    down->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(-50.0f, 0.0f)), CCMoveBy::create(0.3f, ccp(50.0f, 0.0f)))));
    tutorialLayer->addChild(down);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        cell3->setPosition(ccp(cell3->getPositionX() + CELL_WIDTH*2.0f, cell3->getPositionY() - CELL_HEIGHT/2.0f));
        cell4->setPosition(ccp(cell4->getPositionX() + CELL_WIDTH*2.0f, cell4->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
        down->setPosition(ccp(down->getPositionX() + CELL_WIDTH*2.0f, down->getPositionY() - CELL_HEIGHT/2.0f));
    }
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::setTutorial_2_1()
{
    tutorialNumber = 3;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(2, 3), 2));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(ccp(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell3 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell4 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell5 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    cell3->setScaleY(1.1f);
    cell4->setScaleY(1.1f);
    cell5->setScaleY(1.1f);
    
    cell1->setPosition(gameFieldSprites[1][2]->getPosition());
    cell2->setPosition(gameFieldSprites[2][2]->getPosition());
    cell3->setPosition(gameFieldSprites[3][2]->getPosition());
    cell4->setPosition(gameFieldSprites[4][2]->getPosition());
    cell5->setPosition(gameFieldSprites[2][3]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    stencil->addChild(cell3);
    stencil->addChild(cell4);
    stencil->addChild(cell5);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/3.0f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    

    CCMenu* menu = CCMenu::create();
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_3", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    tutorialLayer->addChild(menu);
    skip->setOpacity(0);
    skip->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setRotation(180);
    up->setPosition(ccp(gameFieldSprites[2][3]->getPosition().x - up->getContentSize().width,
                        gameFieldSprites[2][3]->getPosition().y));
    up->setOpacity(0);
    up->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(50.0f, 0.0f)), CCMoveBy::create(0.3f, ccp(-50.0f, 0.0f)))));
    tutorialLayer->addChild(up);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        cell3->setPosition(ccp(cell3->getPositionX() + CELL_WIDTH*2.0f, cell3->getPositionY() - CELL_HEIGHT/2.0f));
        cell4->setPosition(ccp(cell4->getPositionX() + CELL_WIDTH*2.0f, cell4->getPositionY() - CELL_HEIGHT/2.0f));
        cell5->setPosition(ccp(cell5->getPositionX() + CELL_WIDTH*2.0f, cell5->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
    }
    char buf[255];
    sprintf(buf, "%d", this->menu->getCurrentLevel());
    MMPPtr->tutorialStepStarted(this->menu->getCurrentLevel(), string(buf));
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::setTutorial_2_2()
{
    tutorialNumber = 4;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(4, 2), 0));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(ccp(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell3 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell4 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    cell3->setScaleY(1.1f);
    cell4->setScaleY(1.1f);
    
    
    cell1->setPosition(gameFieldSprites[4][2]->getPosition());
    cell2->setPosition(gameFieldSprites[5][2]->getPosition());
    cell3->setPosition(gameFieldSprites[6][2]->getPosition());
    cell4->setPosition(gameFieldSprites[7][2]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    stencil->addChild(cell3);
    stencil->addChild(cell4);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCFadeTo::create(0.5f, 200));
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/1.3f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCFadeTo::create(0.5f, 200));
    
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    

    CCMenu* menu = CCMenu::create();
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_4", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCFadeTo::create(0.5f, 200));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    tutorialLayer->addChild(menu);
    skip->setOpacity(0);
    skip->runAction(CCFadeTo::create(0.5f, 200));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setRotation(90.0f);
    up->setPosition(ccp(gameFieldSprites[4][2]->getPosition().x,
                        gameFieldSprites[4][2]->getPosition().y - up->getContentSize().height));
    up->setOpacity(0);
    up->runAction(CCFadeTo::create(0.5f, 200));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0.0f, 50.0f)), CCMoveBy::create(0.3f, ccp(0.0f, -50.0f)))));
    tutorialLayer->addChild(up);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        cell3->setPosition(ccp(cell3->getPositionX() + CELL_WIDTH*2.0f, cell3->getPositionY() - CELL_HEIGHT/2.0f));
        cell4->setPosition(ccp(cell4->getPositionX() + CELL_WIDTH*2.0f, cell4->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
    }
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::setTutorial_3_1()
{
    return;
    tutorialNumber = 5;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(2, 2), 0));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(ccp(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell3 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell4 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell5 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell6 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    cell3->setScaleY(1.1f);
    cell4->setScaleY(1.1f);
    cell5->setScaleY(1.1f);
    cell6->setScaleY(1.1f);
    
    
    cell1->setPosition(gameFieldSprites[2][0]->getPosition());
    cell2->setPosition(gameFieldSprites[2][1]->getPosition());
    cell3->setPosition(gameFieldSprites[2][2]->getPosition());
    cell4->setPosition(gameFieldSprites[2][3]->getPosition());
    cell5->setPosition(gameFieldSprites[1][2]->getPosition());
    cell6->setPosition(gameFieldSprites[3][2]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    stencil->addChild(cell3);
    stencil->addChild(cell4);
    stencil->addChild(cell5);
    stencil->addChild(cell6);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/3.0f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    

    CCMenu* menu = CCMenu::create();
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_5", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    tutorialLayer->addChild(menu);
    skip->setOpacity(0);
    skip->runAction(CCFadeTo::create(0.5f, 200));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setPosition(ccp(gameFieldSprites[2][2]->getPosition().x,
                        gameFieldSprites[2][2]->getPosition().y));
    up->setOpacity(0);
    up->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(50.0f, 0.0f)), CCMoveBy::create(0.3f, ccp(-50.0f, 0.0f)))));
    tutorialLayer->addChild(up);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        cell3->setPosition(ccp(cell3->getPositionX() + CELL_WIDTH*2.0f, cell3->getPositionY() - CELL_HEIGHT/2.0f));
        cell4->setPosition(ccp(cell4->getPositionX() + CELL_WIDTH*2.0f, cell4->getPositionY() - CELL_HEIGHT/2.0f));
        cell5->setPosition(ccp(cell5->getPositionX() + CELL_WIDTH*2.0f, cell5->getPositionY() - CELL_HEIGHT/2.0f));
        cell6->setPosition(ccp(cell6->getPositionX() + CELL_WIDTH*2.0f, cell6->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
    }
    char buf[255];
    sprintf(buf, "%d", this->menu->getCurrentLevel());
    MMPPtr->tutorialStepStarted(this->menu->getCurrentLevel(), string(buf));
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::setTutorial_3_2()
{
    return;
    tutorialNumber = 6;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(3, 2), 0));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(ccp(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell3 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell4 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    cell3->setScaleY(1.1f);
    cell4->setScaleY(1.1f);
    
    
    cell1->setPosition(gameFieldSprites[3][2]->getPosition());
    cell2->setPosition(gameFieldSprites[4][1]->getPosition());
    cell3->setPosition(gameFieldSprites[4][3]->getPosition());
    cell4->setPosition(gameFieldSprites[4][2]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    stencil->addChild(cell3);
    stencil->addChild(cell4);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCFadeTo::create(0.5f, 200));
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/1.3f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCFadeTo::create(0.5f, 200));
    
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    

    CCMenu* menu = CCMenu::create();
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_6", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCFadeTo::create(0.5f, 200));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    tutorialLayer->addChild(menu);
    skip->setOpacity(0);
    skip->runAction(CCFadeTo::create(0.5f, 200));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setRotation(90.0f);
    up->setPosition(ccp(gameFieldSprites[3][2]->getPosition().x,
                        gameFieldSprites[3][2]->getPosition().y - up->getContentSize().height));
    up->setOpacity(0);
    up->runAction(CCFadeTo::create(0.5f, 200));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0.0f, 50.0f)), CCMoveBy::create(0.3f, ccp(0.0f, -50.0f)))));
    tutorialLayer->addChild(up);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        cell3->setPosition(ccp(cell3->getPositionX() + CELL_WIDTH*2.0f, cell3->getPositionY() - CELL_HEIGHT/2.0f));
        cell4->setPosition(ccp(cell4->getPositionX() + CELL_WIDTH*2.0f, cell4->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
    }
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::setTutorial_4_1()
{
    tutorialNumber = 7;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(3, 2), 0));
    tutorialCell.push_back(pair<CCPoint, int>(ccp(4, 2), 1));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(ccp(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    
    cell1->setPosition(gameFieldSprites[3][2]->getPosition());
    cell2->setPosition(gameFieldSprites[4][2]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/3.0f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    

    CCMenu* menu = CCMenu::create();
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_7", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    tutorialLayer->addChild(menu);
    skip->setOpacity(0);
    skip->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setRotation(90.0f);
    up->setPosition(ccp(gameFieldSprites[3][2]->getPosition().x,
                        gameFieldSprites[3][2]->getPosition().y));
    up->setOpacity(0);
    up->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0.0f, 50.0f)), CCMoveBy::create(0.3f, ccp(0.0f, -50.0f)))));
    tutorialLayer->addChild(up);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
    }
    char buf[255];
    sprintf(buf, "%d", this->menu->getCurrentLevel());
    MMPPtr->tutorialStepStarted(this->menu->getCurrentLevel(), string(buf));
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::setTutorial_5_1()
{
    tutorialNumber = 8;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(3, 2), 0));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(ccp(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell3 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell4 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell5 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell6 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    cell3->setScaleY(1.1f);
    cell4->setScaleY(1.1f);
    cell5->setScaleY(1.1f);
    cell6->setScaleY(1.1f);
    
    cell1->setPosition(gameFieldSprites[3][2]->getPosition());
    cell2->setPosition(gameFieldSprites[4][1]->getPosition());
    cell3->setPosition(gameFieldSprites[4][2]->getPosition());
    cell4->setPosition(gameFieldSprites[4][4]->getPosition());
    cell5->setPosition(gameFieldSprites[4][0]->getPosition());
    cell6->setPosition(gameFieldSprites[4][3]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    stencil->addChild(cell3);
    stencil->addChild(cell4);
    stencil->addChild(cell5);
    stencil->addChild(cell6);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/3.0f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    

    CCMenu* menu = CCMenu::create();
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_8", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    tutorialLayer->addChild(menu);
    skip->setOpacity(0);
    skip->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setRotation(90.0f);
    up->setPosition(ccp(gameFieldSprites[3][2]->getPosition().x,
                        gameFieldSprites[3][2]->getPosition().y));
    up->setOpacity(0);
    up->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0.0f, 50.0f)), CCMoveBy::create(0.3f, ccp(0.0f, -50.0f)))));
    tutorialLayer->addChild(up);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        cell3->setPosition(ccp(cell3->getPositionX() + CELL_WIDTH*2.0f, cell3->getPositionY() - CELL_HEIGHT/2.0f));
        cell4->setPosition(ccp(cell4->getPositionX() + CELL_WIDTH*2.0f, cell4->getPositionY() - CELL_HEIGHT/2.0f));
        cell5->setPosition(ccp(cell5->getPositionX() + CELL_WIDTH*2.0f, cell5->getPositionY() - CELL_HEIGHT/2.0f));
        cell6->setPosition(ccp(cell6->getPositionX() + CELL_WIDTH*2.0f, cell6->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
    }
    char buf[255];
    sprintf(buf, "%d", this->menu->getCurrentLevel());
    MMPPtr->tutorialStepStarted(this->menu->getCurrentLevel(), string(buf));
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::setTutorial_5_2()
{
    tutorialNumber = 9;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(4, 2), 0));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(ccp(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    
    
    cell1->setPosition(gameFieldSprites[4][2]->getPosition());
    cell2->setPosition(gameFieldSprites[5][2]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCFadeTo::create(0.5f, 200));
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/1.3f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCFadeTo::create(0.5f, 200));
    
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    

    CCMenu* menu = CCMenu::create();
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_9", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCFadeTo::create(0.5f, 200));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    tutorialLayer->addChild(menu);
    skip->setOpacity(0);
    skip->runAction(CCFadeTo::create(0.5f, 200));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setRotation(90.0f);
    up->setPosition(ccp(gameFieldSprites[4][2]->getPosition().x,
                        gameFieldSprites[4][2]->getPosition().y - up->getContentSize().height));
    up->setOpacity(0);
    up->runAction(CCFadeTo::create(0.5f, 200));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0.0f, 50.0f)), CCMoveBy::create(0.3f, ccp(0.0f, -50.0f)))));
    tutorialLayer->addChild(up);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
    }
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::setTutorial_4_2()
{
    
}

void GameScene::setTutorial_6_1()
{
    tutorialNumber = 10;
    isTutorial = true;
    tutorialCell.push_back(pair<CCPoint, int>(ccp(0, 2), 0));
    tutorialCell.push_back(pair<CCPoint, int>(ccp(1, 2), 1));
    tutorialLayer = CCLayer::create();
    tutorialLayer->setContentSize(CCSize(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCClippingNode* darkLayer = CCClippingNode::create();
    darkLayer->setInverted(true);
    darkLayer->setAlphaThreshold(0.05f);
    CCNode* stencil = CCNode::create();
    stencil->setContentSize(ccp(CCDirector::sharedDirector()->getWinSize().width, CCDirector::sharedDirector()->getWinSize().height));
    CCSprite* cell1 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell2 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell3 = CCSprite::createWithSpriteFrameName("game/cell.png");
    CCSprite* cell4 = CCSprite::createWithSpriteFrameName("game/cell.png");
    cell1->setScaleY(1.1f);
    cell2->setScaleY(1.1f);
    cell3->setScaleY(1.1f);
    cell4->setScaleY(1.1f);
    
    cell1->setPosition(gameFieldSprites[0][2]->getPosition());
    cell2->setPosition(gameFieldSprites[1][2]->getPosition());
    cell3->setPosition(gameFieldSprites[2][2]->getPosition());
    cell4->setPosition(gameFieldSprites[3][2]->getPosition());
    stencil->addChild(cell1);
    stencil->addChild(cell2);
    stencil->addChild(cell3);
    stencil->addChild(cell4);
    darkLayer->setStencil(stencil);
    CCSprite* content = CCSprite::createWithSpriteFrameName("game/cell.png");
    content->setScale(80.0f);
    content->setColor(ccBLACK);
    content->setOpacity(0);
    darkLayer->addChild(content);
    tutorialLayer->addChild(darkLayer);
    content->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    this->addChild(tutorialLayer, 30);
    CCSprite* tutorialPopup = CCSprite::createWithSpriteFrameName("game/tutorialPopup.png");
    tutorialLayer->addChild(tutorialPopup);
    tutorialPopup->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f,
                                   CCDirector::sharedDirector()->getWinSize().height/5.0f));
    tutorialPopup->setOpacity(0);
    tutorialPopup->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    
    CCSprite* spriteNormal = CCSprite::createWithSpriteFrameName("common/redButton.png");
	CCSprite* spriteSelected = CCSprite::createWithSpriteFrameName("common/redButton.png");
    spriteSelected->setColor(ccGRAY);
    
    CCMenuItemSprite* skip = CCMenuItemSprite::create(spriteNormal, spriteSelected, this, menu_selector(GameScene::skipCallback));
    
    CCLabelTTF* labelTTF = CCLabelTTF::create(CCLocalizedString("SKIP", NULL), FONT_COMMON, FONT_SIZE_48);
    labelTTF->setColor(ccWHITE);
    labelTTF->enableShadow(CCSize(5*MULTIPLIER, -5*MULTIPLIER), 255, 8.0f*MULTIPLIER);
    skip->addChild(labelTTF);
    labelTTF->setPosition(ccp(labelTTF->getParent()->getContentSize().width/2.0f, labelTTF->getParent()->getContentSize().height/2.0f));
    

    CCMenu* menu = CCMenu::create();
    menu->addChild(skip);
    int fontSize = 0;
    if (IPAD)
        fontSize = 50;
    else
        fontSize = 25;
    CCLabelTTF* label = CCLabelTTF::create(CCLocalizedString("TUTORIAL_10", NULL), FONT_COMMON, FONT_SIZE_48);
    label->setPosition(ccp(tutorialPopup->getContentSize().width/1.6f, tutorialPopup->getContentSize().height/2.0f));
    tutorialPopup->addChild(label);
    ccColor3B color;
    color.r = 0xba;
	color.g = 0x29;
	color.b = 0x91;
    label->setColor(color);
    label->setOpacity(0);
    label->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    skip->setPosition(-CCDirector::sharedDirector()->getWinSize().width/2.0f + skip->getContentSize().width/1.9f, -CCDirector::sharedDirector()->getWinSize().height/2.0f + skip->getContentSize().height/1.9f);
    tutorialLayer->addChild(menu);
    skip->setOpacity(0);
    skip->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    
    CCSprite* up = CCSprite::createWithSpriteFrameName("game/right.png");
    up->setRotation(90);
    up->setPosition(ccp(gameFieldSprites[0][2]->getPosition().x - up->getContentSize().width,
                        gameFieldSprites[0][2]->getPosition().y));
    up->setOpacity(0);
    up->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    up->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0.0f, -50.0f)), CCMoveBy::create(0.3f, ccp(0.0f, 50.0f)))));
    tutorialLayer->addChild(up);
    
    CCSprite* down = CCSprite::createWithSpriteFrameName("game/right.png");
    down->setRotation(-90);
    down->setPosition(ccp(gameFieldSprites[0][2]->getPosition().x + down->getContentSize().width,
                          gameFieldSprites[1][2]->getPosition().y));
    down->setOpacity(0);
    down->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(TUTORIAL_DELAY), CCFadeTo::create(0.5f, 200)));
    down->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(CCMoveBy::create(0.3f, ccp(0.0f, 50.0f)), CCMoveBy::create(0.3f, ccp(0.0f, -50.0f)))));
    tutorialLayer->addChild(down);
    
    if (LANDSCAPE)
    {
        cell1->setPosition(ccp(cell1->getPositionX() + CELL_WIDTH*2.0f, cell1->getPositionY() - CELL_HEIGHT/2.0f));
        cell2->setPosition(ccp(cell2->getPositionX() + CELL_WIDTH*2.0f, cell2->getPositionY() - CELL_HEIGHT/2.0f));
        cell3->setPosition(ccp(cell3->getPositionX() + CELL_WIDTH*2.0f, cell3->getPositionY() - CELL_HEIGHT/2.0f));
        cell4->setPosition(ccp(cell4->getPositionX() + CELL_WIDTH*2.0f, cell4->getPositionY() - CELL_HEIGHT/2.0f));
        up->setPosition(ccp(up->getPositionX() + CELL_WIDTH*2.0f, up->getPositionY() - CELL_HEIGHT/2.0f));
        down->setPosition(ccp(down->getPositionX() + CELL_WIDTH*2.0f, down->getPositionY() - CELL_HEIGHT/2.0f));
    }
    char buf[255];
    sprintf(buf, "%d", this->menu->getCurrentLevel());
    MMPPtr->tutorialStepStarted(this->menu->getCurrentLevel(), string(buf));
    
    if (IPHONE_4 || IPHONE_5)
    {
        if (LANDSCAPE)
        {
            tutorialLayer->setScale(1.0f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX(), darkLayer->getPositionY() + 10));
        }
        else
        {
            tutorialLayer->setScale(0.913f);
            darkLayer->setPosition(ccp(darkLayer->getPositionX() - 10, darkLayer->getPositionY() + 10));
        }
    }
}

void GameScene::closeLoading()
{
    if (endGameMenu)
    {
        if (endGameMenu->isVisible())
            endGameMenu->closeLoading();
        else 
            menu->closeLoading();
    }
    else
        menu->closeLoading();
}

void GameScene::skipCallback(CCObject* pSender)
{
    isTutorial = false;
    isSecondTutorial = true;
    tutorialCell.clear();
    tutorialLayer->removeFromParentAndCleanup(true);
    tutorialNumber = -1;
}

void GameScene::timeUpdate(CCNode* sender)
{
    if (isEnd)
        return;
    
    isEndDialog = true;
    if (menu->getCurrentScore() >= menu->getTargetScore())
    {
        this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameScene::win)));
        isEnd = true;
    }
    else
    {
        if (!isLoseDialog)
        {
            this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(GameScene::popupExitWithDelay))));
        }
        else
        {
            isEnd = true;
            this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameScene::lose)));
        }
    }
}

void GameScene::fieldMoveFinished(CCNode* sender)
{
    isRunChangeOrient = false;
}

void GameScene::addingBacground(CCNode* sender)
{
    if (menu->getCurrentLevel()== 1)
        setTutorial_1();
    if (menu->getCurrentLevel() == 2)
        setTutorial_2_1();
    if (menu->getCurrentLevel() == 3)
        setTutorial_3_1();
    if (menu->getCurrentLevel() == 4)
        setTutorial_4_1();
    if (menu->getCurrentLevel() == 5)
        setTutorial_5_1();
    if (menu->getCurrentLevel() == 6)
        setTutorial_6_1();
	if (gameType == Time)
		menu->startTime();
    lock = false;
}

void GameScene::reInitField()
{
    ccColor3B color;
    color.r = 0x8e;
    color.g = 0x2d;
    color.b = 0x1a;
    menu->banner("updateArt/target.png", CCLocalizedString("BANNER_NO_MATCH"), 0.5f, ccWHITE);
    lock = true;
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCCallFuncN::create(this, callfuncN_selector(GameScene::reInitFieldCancelled))));
}

void GameScene::reInitFieldCancelled(CCNode* sender)
{
    lock = false;
    for (int i = 0; i < rowCount; i++)
		for (int j = 0; j < columnCount; j++)
			if (isSimpleCell(gameField[i][j]))
                if (findGameObject(i, j) >= 0)
                    if (gameObjects[findGameObject(i, j)]->type == Simple)
                        gameObjects[findGameObject(i, j)]->changeColor((eColorGameObject)(rand()%colorNumber));
    
	bool isCheckField = true;
    
	while (isCheckField)
	{
		if (checkFieldFirst())
		{
			for(int i = 0; i < allDeletedChain.size(); i++)
			{
                if (gameObjects[allDeletedChain[i]]->type == Simple)
                    gameObjects[allDeletedChain[i]]->changeColor((eColorGameObject)(rand()%colorNumber));
			}
			allDeletedChain.clear();
		}
		else
		{
			isCheckField = false;
		}
	}
}

bool GameScene::findMatch()
{
    for (int i = 0; i < particles.size(); i++)
    {
        particles[i]->stopSystem();
        particles[i]->removeFromParentAndCleanup(true);
    }
    particles.clear();
    currentMatch.clear();
    
    if (isFinalAction)
        return true;
    
    bool findFlag = false;
	for (int i = 0; i < rowCount; i++)
	{
        if (findFlag)
            break;
		for (int j = 0; j < columnCount; j++)
		{
            if (findGameObject(i, j) >= 0 && gameObjects[findGameObject(i, j)]->type != Cookie)
			if (isSimpleCell(gameField[i][j]))
			{
                if (gameObjects[findGameObject(i, j)]->type != Simple && gameObjects[findGameObject(i, j)]->type != Cookie && gameObjects[findGameObject(i, j)]->type != Fish)
                {
                    if (findGameObject(i - 1, j) >= 0 && gameObjects[findGameObject(i - 1, j)]->type != Simple && gameObjects[findGameObject(i - 1, j)]->type != Cookie && gameObjects[findGameObject(i - 1, j)]->type != Fish && gameField[i - 1][j] != CageCell && gameField[i - 1][j] != CageIceCell)
                    {
                        currentMatch.push_back(findGameObject(i - 1, j));
						currentMatch.push_back(findGameObject(i, j));
						findFlag = true;
                        break;
                    }
                    if (findGameObject(i + 1, j) >= 0 && gameObjects[findGameObject(i + 1, j)]->type != Simple && gameObjects[findGameObject(i + 1, j)]->type != Cookie && gameObjects[findGameObject(i + 1, j)]->type != Fish && gameField[i + 1][j] != CageCell && gameField[i + 1][j] != CageIceCell)
                    {
                        currentMatch.push_back(findGameObject(i + 1, j));
						currentMatch.push_back(findGameObject(i, j));
						findFlag = true;
                        break;
                    }
                    if (findGameObject(i, j - 1) >= 0 && gameObjects[findGameObject(i, j - 1)]->type != Simple && gameObjects[findGameObject(i , j - 1)]->type != Cookie && gameObjects[findGameObject(i , j - 1)]->type != Fish && gameField[i][j - 1] != CageCell && gameField[i][j - 1] != CageIceCell)
                    {
                        currentMatch.push_back(findGameObject(i, j - 1));
						currentMatch.push_back(findGameObject(i, j));
						findFlag = true;
                        break;
                    }
                    
                    if (findGameObject(i, j + 1) >= 0 && gameObjects[findGameObject(i, j + 1)]->type != Simple && gameObjects[findGameObject(i , j + 1)]->type != Cookie && gameObjects[findGameObject(i , j + 1)]->type != Fish && gameField[i][j + 1] != CageCell && gameField[i][j + 1] != CageIceCell)
                    {
                        currentMatch.push_back(findGameObject(i, j + 1));
						currentMatch.push_back(findGameObject(i, j));
						findFlag = true;
                        break;
                    }
                }
				if (findGameObject(i - 2, j) >= 0 && gameObjects[findGameObject(i - 2, j)]->color == gameObjects[findGameObject(i, j)]->color
                    && gameObjects[findGameObject(i - 2, j)]->type != Cookie)
				{
					if (findGameObject(i - 1, j + 1) >= 0 && findGameObject(i - 1, j) >= 0 && gameObjects[findGameObject(i - 1, j + 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i - 1, j + 1)]->type != Cookie)
					{
                        if (gameField[i - 2][j] != CageCell && gameField[i - 1][j + 1] != CageCell && gameField[i - 2][j] != CageIceCell && gameField[i - 1][j + 1] != CageIceCell
                            && gameField[i - 1][j] != CageCell && gameField[i - 1][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i - 2, j));
                            currentMatch.push_back(findGameObject(i - 1, j + 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
                    }
					if (findGameObject(i - 1, j - 1) >= 0 && findGameObject(i - 1, j) >= 0 && gameObjects[findGameObject(i - 1, j - 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i - 1, j - 1)]->type != Cookie)
					{
                        if (gameField[i - 2][j] != CageCell && gameField[i - 1][j - 1] != CageCell && gameField[i - 2][j] != CageIceCell && gameField[i - 1][j - 1] != CageIceCell
                            && gameField[i - 1][j] != CageCell && gameField[i - 1][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i - 2, j));
                            currentMatch.push_back(findGameObject(i - 1, j - 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
				}

				if (findGameObject(i + 2, j) >= 0 && gameObjects[findGameObject(i + 2, j)]->color == gameObjects[findGameObject(i, j)]->color
                    && gameObjects[findGameObject(i + 2, j)]->type != Cookie)
				{
					if (findGameObject(i + 1, j + 1) >= 0 && findGameObject(i + 1, j) >= 0 && gameObjects[findGameObject(i + 1, j + 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i + 1, j + 1)]->type != Cookie)
					{
                        if (gameField[i + 2][j] != CageCell && gameField[i + 1][j + 1] != CageCell && gameField[i + 2][j] != CageIceCell && gameField[i + 1][j + 1] != CageIceCell
                            && gameField[i + 1][j] != CageCell && gameField[i + 1][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i + 2, j));
                            currentMatch.push_back(findGameObject(i + 1, j + 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					if (findGameObject(i + 1, j - 1) >= 0 && findGameObject(i + 1, j) >= 0 && gameObjects[findGameObject(i + 1, j - 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i + 1, j - 1)]->type != Cookie)
					{
                        if (gameField[i + 2][j] != CageCell && gameField[i + 1][j - 1] != CageCell && gameField[i + 2][j] != CageIceCell && gameField[i + 1][j - 1] != CageIceCell
                            && gameField[i + 1][j] != CageCell && gameField[i + 1][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i + 2, j));
                            currentMatch.push_back(findGameObject(i + 1, j - 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
				}

				if (findGameObject(i, j + 2) >= 0 && gameObjects[findGameObject(i, j + 2)]->color == gameObjects[findGameObject(i, j)]->color
                    && gameObjects[findGameObject(i, j + 2)]->type != Cookie)
				{
					if (findGameObject(i + 1, j + 1) >= 0 && findGameObject(i, j + 1) >= 0 && gameObjects[findGameObject(i + 1, j + 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i + 1, j + 1)]->type != Cookie)
					{
                        if (gameField[i][j + 2] != CageCell && gameField[i + 1][j + 1] != CageCell && gameField[i][j + 2] != CageIceCell && gameField[i + 1][j + 1] != CageIceCell
                            && gameField[i][j + 1] != CageCell && gameField[i][j + 1] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i, j + 2));
                            currentMatch.push_back(findGameObject(i + 1, j + 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					if (findGameObject(i - 1, j + 1) >= 0 && findGameObject(i, j + 1) >= 0 && gameObjects[findGameObject(i - 1, j + 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i - 1, j + 1)]->type != Cookie)
					{
                        if (gameField[i][j + 2] != CageCell && gameField[i - 1][j + 1] != CageCell && gameField[i][j + 2] != CageIceCell && gameField[i - 1][j + 1] != CageIceCell
                            && gameField[i][j + 1] != CageCell && gameField[i][j + 1] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i, j + 2));
                            currentMatch.push_back(findGameObject(i - 1, j + 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
				}

				if (findGameObject(i, j - 2) >= 0 && gameObjects[findGameObject(i, j - 2)]->color == gameObjects[findGameObject(i, j)]->color
                    && gameObjects[findGameObject(i, j - 2)]->type != Cookie)
				{
					if (findGameObject(i + 1, j - 1) >= 0 && findGameObject(i, j - 1) >= 0 && gameObjects[findGameObject(i + 1, j - 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i + 1, j - 1)]->type != Cookie)
					{
                        if (gameField[i][j - 2] != CageCell && gameField[i + 1][j - 1] != CageCell && gameField[i][j - 2] != CageIceCell && gameField[i + 1][j - 1] != CageIceCell
                            && gameField[i][j - 1] != CageCell && gameField[i][j - 1] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i, j - 2));
                            currentMatch.push_back(findGameObject(i + 1, j - 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					if (findGameObject(i - 1, j - 1) >= 0 && findGameObject(i, j - 1) >= 0 && gameObjects[findGameObject(i - 1, j - 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i - 1, j - 1)]->type != Cookie)
					{
                        if (gameField[i][j - 2] != CageCell && gameField[i - 1][j - 1] != CageCell && gameField[i][j - 2] != CageIceCell && gameField[i - 1][j - 1] != CageIceCell
                            && gameField[i][j - 1] != CageCell && gameField[i][j - 1] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i, j - 2));
                            currentMatch.push_back(findGameObject(i - 1, j - 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
				}


				if (findGameObject(i - 1, j) >= 0 && gameObjects[findGameObject(i - 1, j)]->color == gameObjects[findGameObject(i, j)]->color
                    && gameObjects[findGameObject(i - 1, j)]->type != Cookie)
				{
					if (findGameObject(i - 2, j - 1) >= 0 && findGameObject(i - 2, j) >= 0 && gameObjects[findGameObject(i - 2, j - 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i - 2, j - 1)]->type != Cookie)
					{
                        if (gameField[i - 2][j - 1] != CageCell && gameField[i - 1][j] != CageCell && gameField[i - 2][j - 1] != CageIceCell && gameField[i - 1][j] != CageIceCell
                            && gameField[i - 2][j] != CageCell && gameField[i - 2][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i - 2, j - 1));
                            currentMatch.push_back(findGameObject(i - 1, j));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					else if (findGameObject(i - 2, j + 1) >= 0 && findGameObject(i - 2, j) >= 0 && gameObjects[findGameObject(i - 2, j + 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i - 2, j + 1)]->type != Cookie)
					{
                        if (gameField[i - 2][j + 1] != CageCell && gameField[i - 1][j] != CageCell && gameField[i - 2][j + 1] != CageIceCell && gameField[i - 1][j] != CageIceCell
                            && gameField[i - 2][j] != CageCell && gameField[i - 2][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i - 2, j + 1));
                            currentMatch.push_back(findGameObject(i - 1, j));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					else if (findGameObject(i - 3, j) >= 0 && findGameObject(i - 2, j) >= 0 && gameObjects[findGameObject(i - 3, j)]->color == gameObjects[findGameObject(i, j)]->color
                             && gameObjects[findGameObject(i - 3, j)]->type != Cookie)
					{
                        if (gameField[i - 3][j] != CageCell && gameField[i - 1][j] != CageCell && gameField[i - 3][j] != CageIceCell && gameField[i - 1][j] != CageIceCell
                            && gameField[i - 2][j] != CageCell && gameField[i - 2][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i - 3, j));
                            currentMatch.push_back(findGameObject(i - 1, j));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}					
				}				

				if (findGameObject(i + 1, j) >= 0 && gameObjects[findGameObject(i + 1, j)]->color == gameObjects[findGameObject(i, j)]->color
                    && gameObjects[findGameObject(i + 1, j)]->type != Cookie)
				{					
					if (findGameObject(i + 2, j - 1) >= 0 && findGameObject(i + 2, j) >= 0 && gameObjects[findGameObject(i + 2, j - 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i + 2, j - 1)]->type != Cookie)
					{
                        if (gameField[i + 2][j - 1] != CageCell && gameField[i + 1][j] != CageCell && gameField[i + 2][j - 1] != CageIceCell && gameField[i + 1][j] != CageIceCell
                            && gameField[i + 2][j] != CageCell && gameField[i + 2][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i + 2, j - 1));
                            currentMatch.push_back(findGameObject(i + 1, j));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					else if (findGameObject(i + 2, j + 1) >= 0 && findGameObject(i + 2, j) >= 0 && gameObjects[findGameObject(i + 2, j + 1)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i + 2, j + 1)]->type != Cookie)
					{
                        if (gameField[i + 2][j + 1] != CageCell && gameField[i + 1][j] != CageCell && gameField[i + 2][j + 1] != CageIceCell && gameField[i + 1][j] != CageIceCell
                            && gameField[i + 2][j] != CageCell && gameField[i + 2][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i + 2, j + 1));
                            currentMatch.push_back(findGameObject(i + 1, j));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					else if (findGameObject(i + 3, j) >= 0 && findGameObject(i + 2, j) >= 0 && gameObjects[findGameObject(i + 3, j)]->color == gameObjects[findGameObject(i, j)]->color
                             && gameObjects[findGameObject(i + 3, j)]->type != Cookie)
					{
                        if (gameField[i + 3][j] != CageCell && gameField[i + 1][j] != CageCell && gameField[i + 3][j] != CageIceCell && gameField[i + 1][j] != CageIceCell
                            && gameField[i + 2][j] != CageCell && gameField[i + 2][j] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i + 3, j));
                            currentMatch.push_back(findGameObject(i + 1, j));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}	
				}	

				if (findGameObject(i, j - 1) >= 0 && gameObjects[findGameObject(i, j - 1)]->color == gameObjects[findGameObject(i, j)]->color
                    && gameObjects[findGameObject(i, j - 1)]->type != Cookie)
				{
					if (findGameObject(i - 1, j - 2) >= 0 && findGameObject(i, j - 2) >= 0 && gameObjects[findGameObject(i - 1, j - 2)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i - 1, j - 2)]->type != Cookie)
					{
                        if (gameField[i - 1][j - 2] != CageCell && gameField[i][j - 1] != CageCell && gameField[i - 1][j - 2] != CageIceCell && gameField[i][j - 1] != CageIceCell
                            && gameField[i][j - 2] != CageCell && gameField[i][j - 2] != CageIceCell )
                        {
                            currentMatch.push_back(findGameObject(i - 1, j - 2));
                            currentMatch.push_back(findGameObject(i, j - 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					else if (findGameObject(i + 1, j - 2) >= 0  && findGameObject(i, j - 2) >= 0 && gameObjects[findGameObject(i + 1, j - 2)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i + 1, j - 2)]->type != Cookie)
					{
                        if (gameField[i + 1][j - 2] != CageCell && gameField[i][j - 1] != CageCell && gameField[i + 1][j - 2] != CageIceCell && gameField[i][j - 1] != CageIceCell
                            && gameField[i][j - 2] != CageCell && gameField[i][j - 2] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i + 1, j - 2));
                            currentMatch.push_back(findGameObject(i, j - 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					else if (findGameObject(i, j - 3) >= 0  && findGameObject(i, j - 2) >= 0 && gameObjects[findGameObject(i, j - 3)]->color == gameObjects[findGameObject(i, j)]->color
                             && gameObjects[findGameObject(i, j - 3)]->type != Cookie)
					{
                        if (gameField[i][j - 3] != CageCell && gameField[i][j - 1] != CageCell && gameField[i][j - 3] != CageIceCell && gameField[i][j - 1] != CageIceCell
                            && gameField[i][j - 2] != CageCell && gameField[i][j - 2] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i, j - 3));
                            currentMatch.push_back(findGameObject(i, j - 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}					
				}

				if (findGameObject(i, j + 1) >= 0 && gameObjects[findGameObject(i, j + 1)]->color == gameObjects[findGameObject(i, j)]->color
                    && gameObjects[findGameObject(i, j + 1)]->type != Cookie)
				{
					if (findGameObject(i - 1, j + 2) >= 0  && findGameObject(i, j + 2) >= 0 && gameObjects[findGameObject(i - 1, j + 2)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i - 1, j + 2)]->type != Cookie)
					{
                        if (gameField[i - 1][j + 2] != CageCell && gameField[i][j + 1] != CageCell && gameField[i - 1][j + 2] != CageIceCell && gameField[i][j + 1] != CageIceCell
                            && gameField[i][j + 2] != CageCell && gameField[i][j + 2] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i - 1, j + 2));
                            currentMatch.push_back(findGameObject(i, j + 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					else if (findGameObject(i + 1, j + 2) >= 0 && findGameObject(i, j + 2) >= 0 && gameObjects[findGameObject(i + 1, j + 2)]->color == gameObjects[findGameObject(i, j)]->color && gameObjects[findGameObject(i + 1, j + 2)]->type != Cookie)
					{
                        if (gameField[i + 1][j + 2] != CageCell && gameField[i][j + 1] != CageCell && gameField[i + 1][j + 2] != CageIceCell && gameField[i][j + 1] != CageIceCell
                            && gameField[i][j + 2] != CageCell && gameField[i][j + 2] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i + 1, j + 2));
                            currentMatch.push_back(findGameObject(i, j + 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
					else if (findGameObject(i, j + 3) >= 0  && findGameObject(i, j + 2) >= 0 && gameObjects[findGameObject(i, j + 3)]->color == gameObjects[findGameObject(i, j)]->color
                             && gameObjects[findGameObject(i, j + 3)]->type != Cookie)
					{
                        if (gameField[i][j + 3] != CageCell && gameField[i][j + 1] != CageCell && gameField[i][j + 3] != CageIceCell && gameField[i][j + 1] != CageIceCell
                            && gameField[i][j + 2] != CageCell && gameField[i][j + 2] != CageIceCell)
                        {
                            currentMatch.push_back(findGameObject(i, j + 3));
                            currentMatch.push_back(findGameObject(i, j + 1));
                            currentMatch.push_back(findGameObject(i, j));
                            findFlag = true;
                            break;
                        }
					}
				}
			}
		}
	}
    
    if (findFlag && firstFind)
    {
        firstFind = false;
        for (int i = 0; i < currentMatch.size(); i++)
        {
            gameObjects[currentMatch[i]]->sprite->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.5f, ELEMENT_SCALE+(ELEMENT_SCALE/10.0f)),
                                                                                             CCScaleTo::create(0.5f, ELEMENT_SCALE)));
        }
        
        currentMatch.clear();
    }
    firstFind = true;
	return findFlag;
}

void GameScene::setNumLevel(int nL)
{
	numLevel = nL;
}

bool GameScene::isSimpleCell(eGameCellType ntype)
{
	if (ntype == SimpleCell || ntype == IceCell || ntype == SuperIceCell || ntype == CageCell || ntype == CageIceCell)
		return true;

	return false;
}

bool GameScene::isLockCell(eGameCellType ntype)
{
	if (ntype == LockCell || ntype == MegaIceCell || ntype == StoneCell || ntype == CageCell || ntype == CageIceCell || ntype == Chocolate)
		return true;

	return false;
}

bool GameScene::isNoneCell(int i, int j)
{
	if (i < 0 || i > rowCount-1)
		return true;
	if (j < 0 || j > columnCount-1)
		return true;

	if (gameField[i][j] == NoneCell || gameField[i][j] == LockCell)
		return true;

	return false;
}

bool GameScene::checkElement(int idx)
{
	bool result = false;

	sGameObject* currObj = gameObjects[idx];

	eColorGameObject currentColor = gameObjects[idx]->color;
	
	int x = gameObjects[idx]->x;
	int y = gameObjects[idx]->y;

	vector<int> horizontalChain;
	vector<int> verticalChain;

	horizontalChain.clear();
	verticalChain.clear();

	for (int i = x; i < rowCount; i++)
	{
		if (!isSimpleCell(gameField[i][y]))
			break;

		if (findGameObject(i, y) < 0)
			break;

		if (gameObjects[findGameObject(i, y)]->color == currentColor)
		{
			if (i != x)
				horizontalChain.push_back(findGameObject(i, y));
		}
		else
			break;
	}	

	for (int i = x; i >= 0; i--)
	{
		if (!isSimpleCell(gameField[i][y]))
			break;

		if (findGameObject(i, y) < 0)
			break;

		if (gameObjects[findGameObject(i, y)]->color == currentColor)
		{
			if (i != x)
				horizontalChain.push_back(findGameObject(i, y));	
		}
		else
			break;
	}	

	for (int i = y; i < columnCount; i++)
	{
		if (!isSimpleCell(gameField[x][i]))
			break;

		if (findGameObject(x, i) < 0)
			break;

		if (gameObjects[findGameObject(x, i)]->color == currentColor)
		{
			if (i != y)
				verticalChain.push_back(findGameObject(x, i));	
		}
		else
			break;
	}

	for (int i = y; i >= 0; i--)
	{
		if (!isSimpleCell(gameField[x][i]))
			break;
		
		if (findGameObject(x, i) < 0)
			break;

		if (gameObjects[findGameObject(x, i)]->color == currentColor)
		{
			if (i != y)
				verticalChain.push_back(findGameObject(x, i));
		}
		else
			break;
	}	

	if (horizontalChain.size() < 2)
	{
		horizontalChain.clear();
	}
	else
	{
		result = true;
		for (int i = 0; i < horizontalChain.size(); i++)
            if (horizontalChain[i] != idx)
                addToDeleting(horizontalChain[i]);
	}

	if (verticalChain.size() < 2)
	{
		verticalChain.clear();
	}
	else		
	{
		result = true;
		for (int i = 0; i < verticalChain.size(); i++)
            if (verticalChain[i] != idx)
                addToDeleting(verticalChain[i]);
	}

	if (result)
	{
        if (horizontalChain.size() > 3 || verticalChain.size() > 3)
		{
            if (gameObjects[idx]->type !=Simple)
            {
				addToDeleting(idx);
                gameObjects[idx]->isScore = true;
                gameObjects[idx]->isNext = true;
                gameObjects[idx]->nextType = Crystal;
            }
            else
            {
                gameObjects[idx]->changeType(Crystal);
            }
            
            checkCellLock(gameObjects[idx]->x - 1, gameObjects[idx]->y);
            checkCellLock(gameObjects[idx]->x + 1, gameObjects[idx]->y);
            checkCellLock(gameObjects[idx]->x, gameObjects[idx]->y - 1);
            checkCellLock(gameObjects[idx]->x, gameObjects[idx]->y + 1);
            checkCellSimple(gameObjects[idx]->x, gameObjects[idx]->y);
            
            char buf[255];
            sprintf(buf, "%d", 200);
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            gameFieldSprites[gameObjects[idx]->x][gameObjects[idx]->y]->getParent()->addChild(label,10);
            label->setPosition(gameFieldSprites[gameObjects[idx]->x][gameObjects[idx]->y]->getPosition());
            ccColor3B col;
            col.r = 0xb6;
            col.g = 0xff;
            col.b = 0x38;
            label->setColor(col);
            
            label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
            menu->setCurrentScore(menu->getCurrentScore() + 200*(sweetCount+1));
            
		}
		else if (horizontalChain.size() >=2 && verticalChain.size() >=2)
		{			
			if (gameObjects[idx]->type !=Simple)
            {
				addToDeleting(idx);
                gameObjects[idx]->isScore = true;
                gameObjects[idx]->isNext = true;
                gameObjects[idx]->nextType = Bomb;
            }
			else
            {
				gameObjects[idx]->changeType(Bomb);
                checkCellLock(gameObjects[idx]->x - 1, gameObjects[idx]->y);
                checkCellLock(gameObjects[idx]->x + 1, gameObjects[idx]->y);
                checkCellLock(gameObjects[idx]->x, gameObjects[idx]->y - 1);
                checkCellLock(gameObjects[idx]->x, gameObjects[idx]->y + 1);
                checkCellSimple(gameObjects[idx]->x, gameObjects[idx]->y);
                
                char buf[255];
                sprintf(buf, "%d", 200);
                CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
                gameFieldSprites[gameObjects[idx]->x][gameObjects[idx]->y]->getParent()->addChild(label,10);
                label->setPosition(gameFieldSprites[gameObjects[idx]->x][gameObjects[idx]->y]->getPosition());

                ccColor3B col;
                col.r = 0xb6;
                col.g = 0xff;
                col.b = 0x38;
                label->setColor(col);
                
                label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
                menu->setCurrentScore(menu->getCurrentScore() + 200*(sweetCount+1));
            }
		}
		else if (horizontalChain.size() == 3)
		{
			if (gameObjects[idx]->type !=Simple)
            {
				addToDeleting(idx);
                gameObjects[idx]->isScore = true;
                gameObjects[idx]->isNext = true;
                gameObjects[idx]->nextType = Horizontal;
            }
			else
            {

                gameObjects[idx]->changeType(Horizontal);
                checkCellLock(gameObjects[idx]->x - 1, gameObjects[idx]->y);
                checkCellLock(gameObjects[idx]->x + 1, gameObjects[idx]->y);
                checkCellLock(gameObjects[idx]->x, gameObjects[idx]->y - 1);
                checkCellLock(gameObjects[idx]->x, gameObjects[idx]->y + 1);
                
                checkCellSimple(gameObjects[idx]->x, gameObjects[idx]->y);
                
                char buf[255];
                sprintf(buf, "%d", 120);
                CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
                gameFieldSprites[gameObjects[idx]->x][gameObjects[idx]->y]->getParent()->addChild(label,10);
                label->setPosition(gameFieldSprites[gameObjects[idx]->x][gameObjects[idx]->y]->getPosition());

                ccColor3B col;
                col.r = 0xb6;
                col.g = 0xff;
                col.b = 0x38;
                label->setColor(col);
                
                label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
                menu->setCurrentScore(menu->getCurrentScore() + 120*(sweetCount+1));
            }
		}
		else if (verticalChain.size() == 3)
		{
			if (gameObjects[idx]->type !=Simple)
            {
				addToDeleting(idx);
                gameObjects[idx]->isScore = true;
                gameObjects[idx]->isNext = true;
                gameObjects[idx]->nextType = Vertical;
            }
			else
            {
                gameObjects[idx]->changeType(Vertical);
                
                checkCellLock(gameObjects[idx]->x - 1, gameObjects[idx]->y);
                checkCellLock(gameObjects[idx]->x + 1, gameObjects[idx]->y);
                checkCellLock(gameObjects[idx]->x, gameObjects[idx]->y - 1);
                checkCellLock(gameObjects[idx]->x, gameObjects[idx]->y + 1);
                
                checkCellSimple(gameObjects[idx]->x, gameObjects[idx]->y);
                
                char buf[255];
                sprintf(buf, "%d", 120);
                CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
                gameFieldSprites[gameObjects[idx]->x][gameObjects[idx]->y]->getParent()->addChild(label,10);
                label->setPosition(gameFieldSprites[gameObjects[idx]->x][gameObjects[idx]->y]->getPosition());

                ccColor3B col;
                col.r = 0xb6;
                col.g = 0xff;
                col.b = 0x38;
                label->setColor(col);
                
                label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
                menu->setCurrentScore(menu->getCurrentScore() + 120*(sweetCount+1));
            }
		}
		else
        {
			addToDeleting(idx);
            gameObjects[idx]->isScore = true;
        }
	}

	return result;
}

void GameScene::addToDeleting(int idx)
{
	if (idx < 0)
		return;

	if (gameObjects[idx]->type == Cookie)
		return;

	for (int i = 0; i < allDeletedChain.size(); i++)
	{
		if (allDeletedChain[i] == idx)
			return;
	}
	allDeletedChain.push_back(idx);
}

void GameScene::runFish(sGameObject* gameObject, float delta, float delay)
{
    CCSkeletonAnimation* fish;
    if (gameObject->color == Red)
        fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishRed.atlas");
    if (gameObject->color == Blue)
        fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishBlue.atlas");
    if (gameObject->color == Green)
        fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishGreen.atlas");
    if (gameObject->color == Yellow)
        fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishYellow.atlas");
    if (gameObject->color == Orange)
        fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishOrange.atlas");
    if (gameObject->color == Puprle)
        fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishViolet.atlas");
    if (gameObject->color == Colorurless)
        fish = CCSkeletonAnimation::createWithFile("penguin/penguin.json", "penguin/fishRed.atlas");
    
    fish->setAnimation("animation-copy2", true);
    
    if (IPAD)
        fish->setScale(0.3f);
    else
        fish->setScale(0.15f);
    
    isFish = true;
    
    this->addChild(fish, 15);
    gameObject->isScore = true;
    gameObject->isFished = true;
    if (LANDSCAPE)
        fish->setPosition(ccp(gameObject->sprite->getPositionX() + fish->getContentSize().width*fish->getScale()/2.0f + CCDirector::sharedDirector()->getWinSize().width + CELL_WIDTH*2.0f,
                          gameObject->sprite->getPositionY() + fish->getContentSize().height*fish->getScale()/2.0f - CELL_HEIGHT/2.0f));
    else
        fish->setPosition(ccp(gameObject->sprite->getPositionX() + fish->getContentSize().width*fish->getScale()/2.0f + CCDirector::sharedDirector()->getWinSize().width,
                              gameObject->sprite->getPositionY() + fish->getContentSize().height*fish->getScale()/2.0f));

    
    fish->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delay), CCMoveBy::create(delta, ccp(-WINSIZE.width, 0))));
    
    fishes.push_back(fish);
    for (int i = 0; i < gameObjects.size(); i++)
    {
        if (gameObject == gameObjects[i])
        {
            fish->setTag(i);
        }
    }
    fish->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(delay + delta), CCCallFuncN::create(this, callfuncN_selector(GameScene::runFishEnded))));
}

void GameScene::runFishEnded(CCNode* sender)
{
    for (int i = 0; i < fishes.size(); i++)
    {
        if (fishes[i] == sender)
            fishes.erase(fishes.begin() + i--);
    }
    addToDeleting(sender->getTag());
    afterDeleting();
    sender->removeFromParentAndCleanup(true);
}

bool GameScene::isSpecialElement(sGameObject* object)
{
    if (object->type == Vertical || object->type == Horizontal || object->type == Bomb || object->type == Crystal)
        return true;
    return false;
}

void GameScene::updateBoosters()
{
    endGameMenu->updateBoosters();
}

void GameScene::afterDeletingForCrystalStripe(sGameObject* currObj)
{
	timeAfterAction = 0.0f;
	isFindMatch = false;
    float timeDestroying = 0.1f;
    int checedSpecCount = allDeletedChain.size();
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.1f), CCCallFuncN::create(this, callfuncN_selector(GameScene::visibleBacground))));
    
    sGameObject* deadman = NULL;
    bool isMegaBomb2 = false;
    int countSpecial = 0;
    
    for (int i = 0; i < allDeletedChain.size(); i++)
    {
        gameObjects[allDeletedChain[i]]->delayDestroy = 0.0f;
    }
    
    for (int i = 0; i < allDeletedChain.size(); i++)
    {
        
        if (gameObjects[allDeletedChain[i]]->type == Fish)
        {
            isFishNeedRun = true;
        }
        if (gameObjects[allDeletedChain[i]]->type == Simple)
        {
            if (timeDestroying < gameObjects[allDeletedChain[i]]->delayDestroy)
                timeDestroying = gameObjects[allDeletedChain[i]]->delayDestroy;
            
            if (i < checedSpecCount)
            {
                checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
                checkCellLock(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y);
                checkCellLock(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y);
                checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y - 1);
                checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y + 1);
            }
            
            if (gameObjects[allDeletedChain[i]]->isScore)
            {
                char buf[255];
                sprintf(buf, "%d", SIMPLE_SCORE*(sweetCount+1));
                CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
                gameObjects[allDeletedChain[i]]->sprite->getParent()->getParent()->addChild(label,10);
                label->setPosition(gameObjects[allDeletedChain[i]]->sprite->getPosition());

                if (gameObjects[allDeletedChain[i]]->color == Green)
                {
                    ccColor3B color;
                    color.r = 0xb6;
                    color.g = 0xff;
                    color.b = 0x38;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Red)
                {
                    ccColor3B color;
                    color.r = 0xff;
                    color.g = 0x22;
                    color.b = 0xa1;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Yellow)
                {
                    ccColor3B color;
                    color.r = 0xff;
                    color.g = 0xff;
                    color.b = 0x47;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Blue)
                {
                    ccColor3B color;
                    color.r = 0x00;
                    color.g = 0xf7;
                    color.b = 0xf9;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Orange)
                {
                    ccColor3B color;
                    color.r = 0xff;
                    color.g = 0x76;
                    color.b = 0x24;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Puprle)
                {
                    ccColor3B color;
                    color.r = 0xaa;
                    color.g = 0x35;
                    color.b = 0x1c;
                    label->setColor(color);
                }
                
                label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
                
                menu->setCurrentScore(menu->getCurrentScore() + SIMPLE_SCORE*(sweetCount+1));
            }
        }
        if (gameObjects[allDeletedChain[i]]->type == Vertical)
        {
            if (timeDestroying < 0.7f)
                timeDestroying = 0.7f;
            timesLock = true;
            
            for (int j = 0; j < rowCount; j++)
            {
                float delayDestroy = abs(j - gameObjects[allDeletedChain[i]]->x)*0.04f;
                checkCellSimple(j,gameObjects[allDeletedChain[i]]->y);
                checkCellLock(j,gameObjects[allDeletedChain[i]]->y);
                
                if (j == gameObjects[allDeletedChain[i]]->x)
                    continue;
                
                if (findGameObject(j, gameObjects[allDeletedChain[i]]->y) >= 0)
                {
                    if (isSpecialElement(gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]) &&
                             gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)] != gameObjects[allDeletedChain[i]])
                    {
                        bool tempFlag = true;
                        for (int n = 0; n < nextDead.size(); n++)
                        {
                            if (gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)] == nextDead[n])
                                tempFlag = false;
                        }
                        if (tempFlag)
                        {
                            gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->delayDestroy = 0.0f;
                            addToDeleting(findGameObject(j, gameObjects[allDeletedChain[i]]->y));

                        }
                    }
                    else
                    {
                        addToDeleting(findGameObject(j, gameObjects[allDeletedChain[i]]->y));
                        gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->isScore = true;
                        gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy;
                    }
                }
            }
        }
        else if (gameObjects[allDeletedChain[i]]->type == Horizontal)
        {
            if (timeDestroying < 0.7f)
                timeDestroying = 0.7f;
            timesLock = true;
            
            for (int j = 0; j < columnCount; j++)
            {
                refillDelay.push_back(j);
                float delayDestroy = abs(j - gameObjects[allDeletedChain[i]]->y)*0.04f;
                
                

                checkCellLock(gameObjects[allDeletedChain[i]]->x, j);
                checkCellSimple(gameObjects[allDeletedChain[i]]->x, j);
                
                if (j == gameObjects[allDeletedChain[i]]->y)
                    continue;
                
                if (findGameObject(gameObjects[allDeletedChain[i]]->x, j) >= 0)
                {
                    if (isSpecialElement(gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]) &&
                             gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)] != gameObjects[allDeletedChain[i]])
                    {
                        bool tempFlag = true;
                        for (int n = 0; n < nextDead.size(); n++)
                        {
                            if (gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)] == nextDead[n])
                                tempFlag = false;
                        }
                        if (tempFlag)
                        {
                            gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->delayDestroy = 0.0f;
                            addToDeleting(findGameObject(gameObjects[allDeletedChain[i]]->x, j));
                        }
                    }
                    else
                    {
                        addToDeleting(findGameObject(gameObjects[allDeletedChain[i]]->x, j));
                        gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->isScore = true;
                        gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy;
                    }
                }
            }
            
            for (int j = gameObjects[allDeletedChain[i]]->y; j < columnCount; j++)
            {
                if (findGameObject(gameObjects[allDeletedChain[i]]->x, j) >=0)
                {
                    if (gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->color == gameObjects[allDeletedChain[i]]->color)
                    {
                        for (int n = 0; n < refillDelay.size(); n++)
                        {
                            if (refillDelay[n] == j)
                            {
                                refillDelay.erase(refillDelay.begin() + n);
                                n--;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            for (int j = gameObjects[allDeletedChain[i]]->y; j >=0 ; j--)
            {
                if (findGameObject(gameObjects[allDeletedChain[i]]->x, j) >=0)
                {
                    if (gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->color == gameObjects[allDeletedChain[i]]->color)
                    {
                        for (int n = 0; n < refillDelay.size(); n++)
                        {
                            if (refillDelay[n] == j)
                            {
                                refillDelay.erase(refillDelay.begin() + n);
                                n--;
                            }
                        }
                    }
                }
                else
                {
                    break;
                }
            }
            int a = 0;
        }
        else if (gameObjects[allDeletedChain[i]]->type == Bomb)
        {
            timesLock = true;
            if (timeDestroying < 0.5f)
                timeDestroying = 0.5f;
            bool isColor = false;
            if (gameObjects[allDeletedChain[i]]->isZombieBomb || gameObjects[allDeletedChain[i]]->isMegaBomb)
                isColor = true;
            if (gameObjects[allDeletedChain[i]]->x > 0)
            {
                int center = findGameObject(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y);
                    checkCellLock(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y);
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y);
                
                if (center >= 0)
                {
                    gameObjects[center]->isScore = true;
                    addToDeleting(center);
                }
            }
            if (gameObjects[allDeletedChain[i]]->x < rowCount - 1)
            {
                int center =  findGameObject(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y);
                
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y);
                    checkCellLock(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y);
                
                if (center >= 0)
                {
                    addToDeleting(center);
                    gameObjects[center]->isScore = true;
                }
            }
            
            if (gameObjects[allDeletedChain[i]]->y > 0)
            {
                int center =  findGameObject(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y - 1);
                
                    checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y - 1);
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y - 1);
                
                if (center >= 0)
                {
                    gameObjects[center]->isScore = true;
                    addToDeleting(center);
                }
            }
            
            if (gameObjects[allDeletedChain[i]]->y < columnCount - 1)
            {
                int center =  findGameObject(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y + 1);
                
                    checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y + 1);
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y + 1);
                
                if (center >= 0)
                {
                    gameObjects[center]->isScore = true;
                    addToDeleting(center);
                }
            }
            
            if (gameObjects[allDeletedChain[i]]->x > 0 && gameObjects[allDeletedChain[i]]->y > 0)
            {
                int center =  findGameObject(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y - 1);
                
                    checkCellLock(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y - 1);
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y - 1);
                
                if (center >= 0)
                {
                    gameObjects[center]->isScore = true;
                    addToDeleting(center);
                }
            }
            
            if (gameObjects[allDeletedChain[i]]->x > 0 && gameObjects[allDeletedChain[i]]->y < columnCount - 1)
            {
                int center =  findGameObject(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y + 1);
                
                    checkCellLock(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y + 1);
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y + 1);
                
                if (center >= 0)
                {
                    gameObjects[center]->isScore = true;
                    addToDeleting(center);
                }
            }
            
            if (gameObjects[allDeletedChain[i]]->x < rowCount - 1 && gameObjects[allDeletedChain[i]]->y > 0)
            {
                int center =  findGameObject(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y - 1);
                
                    checkCellLock(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y - 1);
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y - 1);
                
                if (center >= 0)
                {
                    gameObjects[center]->isScore = true;
                    addToDeleting(center);
                }
            }
            
            if (gameObjects[allDeletedChain[i]]->x < rowCount - 1 && gameObjects[allDeletedChain[i]]->y < columnCount - 1)
            {
                int center =  findGameObject(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y + 1);
                
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y + 1);
                    checkCellLock(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y + 1);
                
                if (center >= 0)
                {
                    gameObjects[center]->isScore = true;
                    addToDeleting(center);
                }
            }
        }
        else
        {
                checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
                checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
        }
    }
    
    int countDead = 0;
    
    for(int i = 0; i < nextDead.size(); i++)
    {
        for (int j = 0; j < allDeletedChain.size();)
        {
            if (nextDead[i] == gameObjects[allDeletedChain[j]] && currObj != nextDead[i])
                allDeletedChain.erase(allDeletedChain.begin() + j);
            else
                j++;
        }
    }
  
    bool isMegaBomb = false;
    float maxDelay = 0.0f;
    for (int i = 0; i < allDeletedChain.size(); i++)
    {
        if (gameObjects[allDeletedChain[i]]->isTime && !isFinalAction)
            menu->time+=6;
        
        if (gameObjects[allDeletedChain[i]]->delayDestroy > maxDelay)
            maxDelay = gameObjects[allDeletedChain[i]]->delayDestroy;
        countDead++;
        gameObjects[allDeletedChain[i]]->sprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(gameObjects[allDeletedChain[i]]->delayDestroy), CCCallFuncN::create(this, callfuncN_selector(GameScene::runParticle))));
        
        gameObjects[allDeletedChain[i]]->prepareToDie = true;
        gameObjects[allDeletedChain[i]]->destroy();
        checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
        checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
        forDeadObjects.push_back(gameObjects[allDeletedChain[i]]);
    }
    
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->prepareToDie)
		{
			gameObjects[i] = NULL;
			gameObjects.erase(gameObjects.begin() + i);
			i--;
		}
	}
    
	cellsCheckedSimple.clear();
	cellsCheckedLock.clear();
    
	sweetCount++;
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(timeDestroying + maxDelay), CCCallFuncN::create(this, callfuncN_selector(GameScene::refillObjectForCrystalStripe))));
    
	allDeletedChain.clear();
}

void GameScene::visibleBacground(CCNode* sender)
{
//   if (background)
//        background->setVisible(false);
}

void GameScene::afterDeleting()
{
	timeAfterAction = 0.0f;
	isFindMatch = false;
    float timeDestroying = 0.1f;
    int checedSpecCount = allDeletedChain.size();
    
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.1f), CCCallFuncN::create(this, callfuncN_selector(GameScene::visibleBacground))));
    
    sGameObject* deadman = NULL;
    bool isMegaBomb2 = false;
    int countSpecial = 0;

    if (!crystalCrystal)
    for (int i = 0; i < allDeletedChain.size(); i++)
    {
        gameObjects[allDeletedChain[i]]->delayDestroy = 0.0f;
    }
    
    crystalCrystal = false;
    
    for (int i = 0; i < allDeletedChain.size(); i++)
    {
        bool flagNext = false;
        for(int n = 0; n < nextDead.size(); n++)
        {
            if (nextDead[n] == gameObjects[allDeletedChain[i]] )
                flagNext = true;
        }
        if (flagNext)
            continue;
        if (gameObjects[allDeletedChain[i]]->type == Fish)
        {
            isFishNeedRun = true;
        }
        if (gameObjects[allDeletedChain[i]]->type == Simple)
        {
            if (timeDestroying < gameObjects[allDeletedChain[i]]->delayDestroy)
                timeDestroying = gameObjects[allDeletedChain[i]]->delayDestroy;
            
            if (i < checedSpecCount)
            {
                checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
                checkCellLock(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y);
                checkCellLock(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y);
                checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y - 1);
                checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y + 1);
            }
            
            if (gameObjects[allDeletedChain[i]]->isScore)
            {
                char buf[255];
                sprintf(buf, "%d", SIMPLE_SCORE*(sweetCount+1));
                CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
                gameObjects[allDeletedChain[i]]->sprite->getParent()->getParent()->addChild(label,10);
                label->setPosition(gameObjects[allDeletedChain[i]]->sprite->getPosition());

                if (gameObjects[allDeletedChain[i]]->color == Green)
                {
                    ccColor3B color;
                    color.r = 0xb6;
                    color.g = 0xff;
                    color.b = 0x38;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Red)
                {
                    ccColor3B color;
                    color.r = 0xff;
                    color.g = 0x22;
                    color.b = 0xa1;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Yellow)
                {
                    ccColor3B color;
                    color.r = 0xff;
                    color.g = 0xff;
                    color.b = 0x47;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Blue)
                {
                    ccColor3B color;
                    color.r = 0x00;
                    color.g = 0xf7;
                    color.b = 0xf9;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Orange)
                {
                    ccColor3B color;
                    color.r = 0xff;
                    color.g = 0x76;
                    color.b = 0x24;
                    label->setColor(color);
                }
                if (gameObjects[allDeletedChain[i]]->color == Puprle)
                {
                    ccColor3B color;
                    color.r = 0xaa;
                    color.g = 0x35;
                    color.b = 0x1c;
                    label->setColor(color);
                }
                label->setVisible(false);
                label->runAction(CCSequence::create(CCDelayTime::create(gameObjects[allDeletedChain[i]]->delayDestroy), CCShow::create(), CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create(), NULL));
                
                menu->setCurrentScore(menu->getCurrentScore() + SIMPLE_SCORE*(sweetCount+1));
            }
        }
        if (gameObjects[allDeletedChain[i]]->type == Vertical)
        {
            if (timeDestroying < 0.5f)
                timeDestroying = 0.5f;
            
            if (gameObjects[allDeletedChain[i]]->delayDestroy == 0.0f)
                gameObjects[allDeletedChain[i]]->delayDestroy = countSpecial*0.5f;
            countSpecial++;
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1,gameObjects[allDeletedChain[i]]->y);
            checkCellLock(gameObjects[allDeletedChain[i]]->x - 1,gameObjects[allDeletedChain[i]]->y);
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1,gameObjects[allDeletedChain[i]]->y);
            checkCellLock(gameObjects[allDeletedChain[i]]->x + 1,gameObjects[allDeletedChain[i]]->y);
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y - 1);
            checkCellLock(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y - 1);
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y + 1);
            checkCellLock(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y + 1);

            
            for (int j = 0; j < rowCount; j++)
            {
                float delayDestroy = abs(j - gameObjects[allDeletedChain[i]]->x)*0.04f;
                checkCellSimple(j,gameObjects[allDeletedChain[i]]->y);
                checkCellLock(j,gameObjects[allDeletedChain[i]]->y);
                if (j == gameObjects[allDeletedChain[i]]->x)
                    continue;
                if (findGameObject(j, gameObjects[allDeletedChain[i]]->y) >= 0)
                {
                    checkCellSimple(j, gameObjects[allDeletedChain[i]]->y);
                    checkCellLock(j, gameObjects[allDeletedChain[i]]->y);
                    if (!gameObjectHV.empty())
                    {
                        if (isSpecialElement(gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]) &&
                            gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)] != gameObjects[allDeletedChain[i]]
                            &&gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)] != gameObjectHV[0]
                            &&gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)] != gameObjectHV[1])
                        {
                            if (gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->type != Vertical)
                            {
                                addToDeleting(findGameObject(j, gameObjects[allDeletedChain[i]]->y));
                                gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->delayDestroy = delayDestroy;
                                if (gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->type == Crystal)
                                {
                                    for (int n = 0; n < rowCount; n++)
                                        for (int k = 0; k < columnCount; k++)
                                            if (isSimpleCell(gameField[n][k]))
                                                if (findGameObject(n, k) >= 0)
                                                    if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                    {
                                                        addToDeleting(findGameObject(n, k));
                                                        lighting(gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->sprite->getPosition(),
                                                                 gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                    }
                                }
                            }
                            else
                                nextDead.push_back(gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]);
                        }
                        else
                        {
                            addToDeleting(findGameObject(j, gameObjects[allDeletedChain[i]]->y));
                            gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->isScore = true;
                            gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->delayDestroy = delayDestroy;
                        }
                    }
                    else if (isSpecialElement(gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]) &&
                        gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)] != gameObjects[allDeletedChain[i]])
                    {
                        if (gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->type != Vertical)
                        {
                            addToDeleting(findGameObject(j, gameObjects[allDeletedChain[i]]->y));
                            gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + delayDestroy;
                            if (gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else
                            nextDead.push_back(gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]);
                    }
                    else
                    {
                        addToDeleting(findGameObject(j, gameObjects[allDeletedChain[i]]->y));
                        gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->isScore = true;
                        gameObjects[findGameObject(j, gameObjects[allDeletedChain[i]]->y)]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + delayDestroy;
                    }
                }
            }
            timesLock = true;
        }
        else if (gameObjects[allDeletedChain[i]]->type == Horizontal)
        {
            if (timeDestroying < 0.5f)
                timeDestroying = 0.5f;
            
            timesLock = true;
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1,gameObjects[allDeletedChain[i]]->y);
            checkCellLock(gameObjects[allDeletedChain[i]]->x - 1,gameObjects[allDeletedChain[i]]->y);
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1,gameObjects[allDeletedChain[i]]->y);
            checkCellLock(gameObjects[allDeletedChain[i]]->x + 1,gameObjects[allDeletedChain[i]]->y);
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y - 1);
            checkCellLock(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y - 1);
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y + 1);
            checkCellLock(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y + 1);

            
            if (gameObjects[allDeletedChain[i]]->delayDestroy == 0.0f)
                gameObjects[allDeletedChain[i]]->delayDestroy = countSpecial*0.5f;
            countSpecial++;
            
            for (int j = 0; j < columnCount; j++)
            {
                float delayDestroy = abs(j - gameObjects[allDeletedChain[i]]->y)*0.04f;
                checkCellLock(gameObjects[allDeletedChain[i]]->x, j);
                checkCellSimple(gameObjects[allDeletedChain[i]]->x, j);
                
                if (j == gameObjects[allDeletedChain[i]]->y)
                    continue;
                refillDelay.push_back(j);
                
                if (findGameObject(gameObjects[allDeletedChain[i]]->x, j) >= 0)
                {
                    if (!gameObjectHV.empty())
                    {
                        if (isSpecialElement(gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]) &&
                            gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)] != gameObjects[allDeletedChain[i]]
                            &&gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)] != gameObjectHV[0]
                            &&gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)] != gameObjectHV[1])
                        {
                            if (gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->type != Horizontal)
                            {
                                addToDeleting(findGameObject(gameObjects[allDeletedChain[i]]->x, j));
                                gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->delayDestroy = delayDestroy;
                                
                                if (gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->type == Crystal)
                                {
                                    for (int n = 0; n < rowCount; n++)
                                        for (int k = 0; k < columnCount; k++)
                                            if (isSimpleCell(gameField[n][k]))
                                                if (findGameObject(n, k) >= 0)
                                                    if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                    {
                                                        addToDeleting(findGameObject(n, k));
                                                        lighting(gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->sprite->getPosition(),
                                                                 gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                    }
                                }
                            }
                            else
                                nextDead.push_back(gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]);
                        }
                        else
                        {
                            addToDeleting(findGameObject(gameObjects[allDeletedChain[i]]->x, j));
                            gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->isScore = true;
                            gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->delayDestroy = delayDestroy;
                        }
                    }
                    else if (isSpecialElement(gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]) &&
                        gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)] != gameObjects[allDeletedChain[i]])
                    {
                        if (gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->type != Horizontal)
                        {
                            addToDeleting(findGameObject(gameObjects[allDeletedChain[i]]->x, j));
                            gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + delayDestroy;
                            
                            if (gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else
                            nextDead.push_back(gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]);
                    }
                    else
                    {
                        addToDeleting(findGameObject(gameObjects[allDeletedChain[i]]->x, j));
                        gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->isScore = true;
                        gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + delayDestroy;
                    }
                }
            }
            
            for (int j = gameObjects[allDeletedChain[i]]->y; j < columnCount; j++)
            {
                if (findGameObject(gameObjects[allDeletedChain[i]]->x, j) >=0)
                {
                    if (gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->color == gameObjects[allDeletedChain[i]]->color)
                    {
                        for (int n = 0; n < refillDelay.size(); n++)
                        {
                            if (refillDelay[n] == j)
                            {
                                refillDelay.erase(refillDelay.begin() + n);
                                n--;
                            }
                        }
                    }
                    else
                    {
                        break;
                    }
                }
            }
            for (int j = gameObjects[allDeletedChain[i]]->y; j >=0 ; j--)
            {
                if (findGameObject(gameObjects[allDeletedChain[i]]->x, j) >=0)
                {
                    if (gameObjects[findGameObject(gameObjects[allDeletedChain[i]]->x, j)]->color == gameObjects[allDeletedChain[i]]->color)
                    {
                        for (int n = 0; n < refillDelay.size(); n++)
                        {
                            if (refillDelay[n] == j)
                            {
                                refillDelay.erase(refillDelay.begin() + n);
                                n--;
                            }
                        }
                    }
                }
                else
                {
                    break;
                }
            }
            int a = 0;
        }
        else if (gameObjects[allDeletedChain[i]]->type == Bomb)
        {
            timesLock = true;
            if (gameObjects[allDeletedChain[i]]->delayDestroy == 0.0f)
                gameObjects[allDeletedChain[i]]->delayDestroy = countSpecial*0.5f;
            countSpecial++;
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1,gameObjects[allDeletedChain[i]]->y);
            checkCellLock(gameObjects[allDeletedChain[i]]->x - 1,gameObjects[allDeletedChain[i]]->y);
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1,gameObjects[allDeletedChain[i]]->y);
            checkCellLock(gameObjects[allDeletedChain[i]]->x + 1,gameObjects[allDeletedChain[i]]->y);
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y - 1);
            checkCellLock(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y - 1);
            
            checkCellSimple(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y + 1);
            checkCellLock(gameObjects[allDeletedChain[i]]->x,gameObjects[allDeletedChain[i]]->y + 1);
            
            if (gameObjects[allDeletedChain[i]]->isMegaBombHorizontal)
            {
                isMegaBomb2 = true;
                timeDestroying = 0.1f;
                notRefill = gameObjects[allDeletedChain[i]]->y;

                for (int n = 0; n < columnCount; n++)
                {
                    float delayDestroy = 0.1f + abs(n - gameObjects[allDeletedChain[i]]->y)*0.15f;

                    int left = findGameObject(gameObjects[allDeletedChain[i]]->x - 1, n);
                    int center =  findGameObject(gameObjects[allDeletedChain[i]]->x, n);
                    int right = findGameObject(gameObjects[allDeletedChain[i]]->x + 1, n);
                    
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1, n);
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x, n);
                    checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1, n);
                    checkCellLock(gameObjects[allDeletedChain[i]]->x - 1, n);
                    checkCellLock(gameObjects[allDeletedChain[i]]->x, n);
                    checkCellLock(gameObjects[allDeletedChain[i]]->x + 1, n);
                    
                    if (left >= 0)
                    {
                        if (isSpecialElement(gameObjects[left]) &&
                            gameObjects[left] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[left]->delayDestroy = delayDestroy;
                            addToDeleting(left);
                            
                            if (gameObjects[left]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[left]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else
                        {
                            addToDeleting(left);
                            gameObjects[left]->isScore = true;
                            gameObjects[left]->delayDestroy = delayDestroy;
                        }
                    }
                    
                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = delayDestroy;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else
                        {
                            addToDeleting(center);
                            gameObjects[center]->isScore = true;
                            gameObjects[center]->delayDestroy = delayDestroy;
                        }
                    }
                    
                    if (right >= 0)
                    {
                        if (isSpecialElement(gameObjects[right]) && gameObjects[right] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[right]->delayDestroy = delayDestroy;
                            addToDeleting(right);
                            
                            if (gameObjects[right]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[right]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else
                        {
                            gameObjects[right]->isScore = true;
                            addToDeleting(right);
                            gameObjects[right]->delayDestroy = delayDestroy;
                        }
                    }
                }
            }
            else if (gameObjects[allDeletedChain[i]]->isMegaBombVertical)
            {
                isMegaBomb2 = true;
                timeDestroying = 0.1f;
                notRefill = -2;
                for (int n = 0; n < rowCount; n++)
                {
                    float delayDestroy = 0.1f + abs(n - gameObjects[allDeletedChain[i]]->x)*0.15f;
                    int left = findGameObject(n, gameObjects[allDeletedChain[i]]->y - 1);
                    int center =  findGameObject(n, gameObjects[allDeletedChain[i]]->y);
                    int right = findGameObject(n, gameObjects[allDeletedChain[i]]->y + 1);

                        checkCellSimple(n, gameObjects[allDeletedChain[i]]->y - 1);
                        checkCellSimple(n, gameObjects[allDeletedChain[i]]->y);
                        checkCellSimple(n, gameObjects[allDeletedChain[i]]->y + 1);
                        checkCellLock(n, gameObjects[allDeletedChain[i]]->y - 1);
                        checkCellLock(n, gameObjects[allDeletedChain[i]]->y);
                        checkCellLock(n, gameObjects[allDeletedChain[i]]->y + 1);
                    
                    if (left >= 0)
                    {
                        if (isSpecialElement(gameObjects[left]) &&
                            gameObjects[left] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[left]->delayDestroy = delayDestroy;
                            addToDeleting(left);
                            
                            if (gameObjects[left]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[left]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else
                        {
                            addToDeleting(left);
                            gameObjects[left]->isScore = true;
                            gameObjects[left]->delayDestroy = delayDestroy;
                        }
                    }
                    
                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = delayDestroy;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else
                        {
                            addToDeleting(center);
                            gameObjects[center]->isScore = true;
                            gameObjects[center]->delayDestroy = delayDestroy;
                        }
                    }
                    
                    if (right >= 0)
                    {
                        if (isSpecialElement(gameObjects[right]) &&
                            gameObjects[right] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[right]->delayDestroy = delayDestroy;
                            addToDeleting(right);
                            
                            if (gameObjects[right]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[right]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else
                        {
                            gameObjects[right]->isScore = true;
                            addToDeleting(right);
                            gameObjects[right]->delayDestroy = delayDestroy;
                        }
                    }
                }
            }
            else
            {
                if (timeDestroying < 0.5f)
                    timeDestroying = 0.5f;
                bool isColor = false;
                float offsetTime = 0.15f;
                if (gameObjects[allDeletedChain[i]]->isZombieBomb || gameObjects[allDeletedChain[i]]->isMegaBomb)
                    isColor = true;
                
                if (gameObjects[allDeletedChain[i]]->x > 0)
                {
                    int center = findGameObject(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y);

                        checkCellLock(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y);
                        checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y);
                    
                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                            gameObjects[center]->isNext = false;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else if (!isColor && gameObjects[center]->color == gameObjects[allDeletedChain[i]]->color &&
                                 gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            isColor = true;
                            gameObjects[center]->isZombieBomb = true;
                            gameObjects[center]->changeType(Bomb);
                            nextDead.push_back(gameObjects[center]);
                        }
                        else
                        {
                            gameObjects[center]->isScore = true;
                            addToDeleting(center);
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                        }
                    }
                }
                if (gameObjects[allDeletedChain[i]]->x < rowCount - 1)
                {
                    int center =  findGameObject(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y);
                    
                        checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y);
                        checkCellLock(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y);
                    
                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                            gameObjects[center]->isNext = false;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else if (!isColor && gameObjects[center]->color == gameObjects[allDeletedChain[i]]->color &&
                                 gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            isColor = true;
                            gameObjects[center]->isZombieBomb = true;
                            gameObjects[center]->changeType(Bomb);
                            nextDead.push_back(gameObjects[center]);
                        }
                        else
                        {
                            addToDeleting(center);
                            gameObjects[center]->isScore = true;
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;

                        }
                    }
                }
                
                if (gameObjects[allDeletedChain[i]]->y > 0)
                {
                    int center =  findGameObject(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y - 1);
                    
                        checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y - 1);
                        checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y - 1);
                    
                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                            gameObjects[center]->isNext = false;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else if (!isColor && gameObjects[center]->color == gameObjects[allDeletedChain[i]]->color &&
                                 gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            isColor = true;
                            gameObjects[center]->isZombieBomb = true;
                            gameObjects[center]->changeType(Bomb);
                            nextDead.push_back(gameObjects[center]);
                        }
                        else
                        {
                            gameObjects[center]->isScore = true;
                            addToDeleting(center);
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;

                        }
                    }
                }
                
                if (gameObjects[allDeletedChain[i]]->y < columnCount - 1)
                {
                    int center =  findGameObject(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y + 1);
                    
                        checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y + 1);
                        checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y + 1);
                    
                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                            gameObjects[center]->isNext = false;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else if (!isColor && gameObjects[center]->color == gameObjects[allDeletedChain[i]]->color &&
                                 gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            isColor = true;
                            gameObjects[center]->isZombieBomb = true;
                            gameObjects[center]->changeType(Bomb);
                            nextDead.push_back(gameObjects[center]);
                        }
                        else
                        {
                            gameObjects[center]->isScore = true;
                            addToDeleting(center);
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                        }
                    }
                }
                
                if (gameObjects[allDeletedChain[i]]->x > 0 && gameObjects[allDeletedChain[i]]->y > 0)
                {
                    int center =  findGameObject(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y - 1);
                    
                        checkCellLock(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y - 1);
                        checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y - 1);
                    
                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                            gameObjects[center]->isNext = false;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else if (!isColor && gameObjects[center]->color == gameObjects[allDeletedChain[i]]->color &&
                                 gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            isColor = true;
                            gameObjects[center]->isZombieBomb = true;
                            gameObjects[center]->changeType(Bomb);
                            nextDead.push_back(gameObjects[center]);
                        }
                        else
                        {
                            gameObjects[center]->isScore = true;
                            addToDeleting(center);
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;

                        }
                    }
                }
                
                if (gameObjects[allDeletedChain[i]]->x > 0 && gameObjects[allDeletedChain[i]]->y < columnCount - 1)
                {
                    int center =  findGameObject(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y + 1);
                    
                        checkCellLock(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y + 1);
                        checkCellSimple(gameObjects[allDeletedChain[i]]->x - 1, gameObjects[allDeletedChain[i]]->y + 1);
                    
                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                            gameObjects[center]->isNext = false;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else if (!isColor && gameObjects[center]->color == gameObjects[allDeletedChain[i]]->color &&
                                 gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            isColor = true;
                            gameObjects[center]->isZombieBomb = true;
                            gameObjects[center]->changeType(Bomb);
                            nextDead.push_back(gameObjects[center]);
                        }
                        else
                        {
                            gameObjects[center]->isScore = true;
                            addToDeleting(center);
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;

                        }
                    }
                }
                
                if (gameObjects[allDeletedChain[i]]->x < rowCount - 1 && gameObjects[allDeletedChain[i]]->y > 0)
                {
                    int center =  findGameObject(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y - 1);
                    
                        checkCellLock(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y - 1);
                        checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y - 1);

                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                            gameObjects[center]->isNext = false;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else if (!isColor && gameObjects[center]->color == gameObjects[allDeletedChain[i]]->color &&
                                 gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            isColor = true;
                            gameObjects[center]->isZombieBomb = true;
                            gameObjects[center]->changeType(Bomb);
                            nextDead.push_back(gameObjects[center]);
                        }
                        else
                        {
                            gameObjects[center]->isScore = true;
                            addToDeleting(center);
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;

                        }
                    }
                }
                
                if (gameObjects[allDeletedChain[i]]->x < rowCount - 1 && gameObjects[allDeletedChain[i]]->y < columnCount - 1)
                {
                    int center =  findGameObject(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y + 1);
                    
                        checkCellSimple(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y + 1);
                        checkCellLock(gameObjects[allDeletedChain[i]]->x + 1, gameObjects[allDeletedChain[i]]->y + 1);
                    
                    if (center >= 0)
                    {
                        if (isSpecialElement(gameObjects[center]) &&
                            gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                            gameObjects[center]->isNext = false;
                            addToDeleting(center);
                            
                            if (gameObjects[center]->type == Crystal)
                            {
                                for (int n = 0; n < rowCount; n++)
                                    for (int k = 0; k < columnCount; k++)
                                        if (isSimpleCell(gameField[n][k]))
                                            if (findGameObject(n, k) >= 0)
                                                if (gameObjects[findGameObject(n, k)]->color == gameObjects[allDeletedChain[i]]->color)
                                                {
                                                    addToDeleting(findGameObject(n, k));
                                                    lighting(gameObjects[center]->sprite->getPosition(),
                                                             gameObjects[findGameObject(n, k)]->sprite->getPosition());
                                                }
                            }
                        }
                        else if (!isColor && gameObjects[center]->color == gameObjects[allDeletedChain[i]]->color &&
                                 gameObjects[center] != gameObjects[allDeletedChain[i]])
                        {
                            isColor = true;
                            gameObjects[center]->isZombieBomb = true;
                            gameObjects[center]->changeType(Bomb);
                            nextDead.push_back(gameObjects[center]);
                        }
                        else
                        {
                            gameObjects[center]->isScore = true;
                            addToDeleting(center);
                            gameObjects[center]->delayDestroy = gameObjects[allDeletedChain[i]]->delayDestroy + offsetTime;
                        }
                    }
                }
            }
        }
        else
        {
//            if (timeDestroying < 0.5f)
 //               timeDestroying = 0.5f;
            if (i < checedSpecCount)
            {
                checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
                checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
            }
        }
    }

    if (!gameObjectHV.empty())
    {
        gameObjectHV[0]->delayDestroy = 0.0f;
        gameObjectHV[1]->delayDestroy = 0.0f;
    }

    int countDead = 0;
    
    if (!gameObjectHV.empty())
    {
        for(int i = 0; i < nextDead.size(); i++)
        {
            if (nextDead[i] == gameObjectHV[0] || nextDead[i] == gameObjectHV[0])
            {
                nextDead.erase(nextDead.begin() + i);
                i--;
            }
        }
        gameObjectHV.clear();
    }
    
    for(int i = 0; i < nextDead.size(); i++)
    {
        for (int j = 0; j < allDeletedChain.size();)
        {
            if (nextDead[i] == gameObjects[allDeletedChain[j]] && nextDead[i] != deadman)
                allDeletedChain.erase(allDeletedChain.begin() + j);
            else
                j++;
        }
    }
    
    bool isMegaBomb = false;
    float maxDelay = 0.0f;
    for (int i = 0; i < allDeletedChain.size(); i++)
    {
        if (gameObjects[allDeletedChain[i]]->isTime  && !isFinalAction)
            menu->time+=6;
        
        if (gameObjects[allDeletedChain[i]]->delayDestroy > maxDelay)
            maxDelay = gameObjects[allDeletedChain[i]]->delayDestroy;
        countDead++;
        gameObjects[allDeletedChain[i]]->sprite->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(gameObjects[allDeletedChain[i]]->delayDestroy), CCCallFuncN::create(this, callfuncN_selector(GameScene::runParticle))));
        
        if (gameObjects[allDeletedChain[i]]->isMegaBomb)
        {
            gameObjects[allDeletedChain[i]]->destroy();
            gameObjects[allDeletedChain[i]]->isMegaBomb = false;
            gameObjects[allDeletedChain[i]]->isMegaBombHorizontal = true;
            gameObjects[allDeletedChain[i]]->isMegaBombVertical = false;
            nextDead.push_back(gameObjects[allDeletedChain[i]]);
            sGameObject* tempGO = nextDead[0];
            nextDead[0] = nextDead[nextDead.size() - 1];
            nextDead[nextDead.size() - 1] = tempGO;
            isMegaBomb = true;
        }
        else if (gameObjects[allDeletedChain[i]]->isMegaBombHorizontal)
        {
            gameObjects[allDeletedChain[i]]->destroy();
            gameObjects[allDeletedChain[i]]->isMegaBomb = false;
            gameObjects[allDeletedChain[i]]->isMegaBombHorizontal = false;
            gameObjects[allDeletedChain[i]]->isMegaBombVertical = true;
            nextDead.push_back(gameObjects[allDeletedChain[i]]);
            sGameObject* tempGO = nextDead[0];
            nextDead[0] = nextDead[nextDead.size() - 1];
            nextDead[nextDead.size() - 1] = tempGO;
            isMegaBomb2 = true;
        }
        else if (gameObjects[allDeletedChain[i]]->isZombieStripe)
        {
            gameObjects[allDeletedChain[i]]->destroy();
            gameObjects[allDeletedChain[i]]->isZombieStripe = false;
            checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
            checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
            forDeadObjects.push_back(gameObjects[allDeletedChain[i]]);
        }
        else
        {
            if (!gameObjects[allDeletedChain[i]]->isNext)
            {
                gameObjects[allDeletedChain[i]]->prepareToDie = true;
                if (!gameObjects[allDeletedChain[i]]->isFished)
                    forDeadObjects.push_back(gameObjects[allDeletedChain[i]]);
            }
            gameObjects[allDeletedChain[i]]->destroy();
            checkCellLock(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
            checkCellSimple(gameObjects[allDeletedChain[i]]->x, gameObjects[allDeletedChain[i]]->y);
        }
    }

    if (!isRunFish)
    {
        for (int i = 0; i < gameObjects.size(); i++)
        {
            if (gameObjects[i]->prepareToDie)
            {
                gameObjects[i] = NULL;
                gameObjects.erase(gameObjects.begin() + i);
                i--;
            }
        }
    }

	cellsCheckedSimple.clear();
	cellsCheckedLock.clear();

	sweetCount++;
    
    for (int i = 0; i < newCreateObjects.size(); i++)
    {
        for (int j = 0; j < gameObjects.size(); j++)
        {
            if (newCreateObjects[i] == gameObjects[j])
            {
                lastRefillObjects.push_back(j);
            }
        }
    }
    
    newCreateObjects.clear();

    if (isMegaBomb || isMegaBomb)
    {
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(timeDestroying/* + maxDelay*/), CCCallFuncN::create(this, callfuncN_selector(GameScene::destroyFinished))));
    }
    else if (isMegaBomb2)
    {
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(timeDestroying), CCCallFuncN::create(this, callfuncN_selector(GameScene::refillObject))));
    }
    else
    {
        if (!isRunFish)
            this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(timeDestroying/* + maxDelay*/), CCCallFuncN::create(this, callfuncN_selector(GameScene::refillObject))));
    }

	allDeletedChain.clear();
}

void GameScene::runParticle(CCNode* sender)
{
    CCParticleSystemQuad* explosion;// = CCParticleExplosion::createWithTotalParticles(4);
    if (IPAD)
        explosion = CCParticleExplosion::createWithTotalParticles(8);
    else
        explosion = CCParticleExplosion::createWithTotalParticles(4);
    this->addChild(explosion, 10);
    particles.push_back(explosion);
    explosion->setPosition(sender->getPositionX(),
                           sender->getPositionY());
    explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/snow.png"));
    explosion->setSpeed(100.0f);
    explosion->setSpeedVar(10.0f);
    explosion->setScale(3.0f);
    explosion->setGravity(ccp(0.0f, -100.0f));
    explosion->setEndSpin(800.0f);
    explosion->setBlendAdditive(true);
    ccBlendFunc blend;
    blend.dst = GL_ONE;
    blend.src = GL_ONE;
    explosion->setBlendFunc(blend);
    explosion->setEndColor(ccc4f(0, 255, 255, 255));
    explosion->setEndColorVar(ccc4f(0, 0, 0, 0));
    explosion->setStartColor(ccc4f(229, 229, 229, 229));
    explosion->setStartColorVar(ccc4f(0, 0, 0, 25));
    explosion->setEndSize(0.0f);
    explosion->setLife(3.0f);
}

void GameScene::end()
{
	menu->end();
	fieldLayer->runAction(CCSequence::create(CCDelayTime::create(0.5f),
		CCEaseOut::create(CCMoveBy::create(0.5f, ccp(CCDirector::sharedDirector()->getWinSize().width, fieldLayer->getPositionY())), 2.5f),	NULL));
	leftDownMenu->hide();
    isEndStencil = true;
}

void GameScene::winCallback(CCNode* sender)
{
    isWinSpeed = false;
    isLoseSpeed = false;
    endGameMenu->setVisible(true);
    lock = true;
    fieldLayer->setVisible(false);
    menu->setVisible(false);
    if (IPHONE_5 || IPHONE_4)
    {
        if (LANDSCAPE)
        {
            this->setScale(1.0f);
            if (WINSIZE.width == 1136)
            {
                backDown->setScaleX(1.12f);
                backDown->setScaleY(1.0f);
            }
            else
                backDown->setScale(1.1f);
        }
        else
        {
            this->setScale(1.0f);
            if (WINSIZE.height == 1136)
            {
                backDown->setScaleY(1.12f);
                backDown->setScaleX(1.0f);
            }
            else
                backDown->setScale(1.1f);

        }
        
    }
    endGameMenu->popupWin(menu->getCountStar(), menu->currentScore, menu->currentLevel);
}

void GameScene::loseCallback(CCNode* sender)
{
    isWinSpeed = false;
    isLoseSpeed = false;
    endGameMenu->setVisible(true);
    lock = true;
    fieldLayer->setVisible(false);
    menu->setVisible(false);
    if (IPHONE_5 || IPHONE_4)
    {
        if (LANDSCAPE)
        {
            this->setScale(1.0f);
            if (WINSIZE.width == 1136)
            {
                backDown->setScaleX(1.12f);
                backDown->setScaleY(1.0f);
            }
            else
                backDown->setScale(1.1f);
        }
        else
        {
            this->setScale(1.0f);
            if (WINSIZE.height == 1136)
            {
                backDown->setScaleY(1.12f);
                backDown->setScaleX(1.0f);
            }
            else
                backDown->setScale(1.1f);
        }
        
    }
    endGameMenu->popupLose(menu->currentScore, menu->type, menu->currentLevel);
}

void GameScene::movesEnded(CCNode* sender)
{
	lock = true;
	isWin = true;
	isLose = true;
    if (gameType != Time)
    {
        bool flag = false;
        for (int i = 0; i < gameObjects.size(); i++)
        {
            if (gameObjects[i]->type == Vertical || gameObjects[i]->type == Horizontal
                || gameObjects[i]->type == Bomb || gameObjects[i]->type == Crystal)
            {
                flag = true;
                break;
            }
        }
        if (flag)
        {
            menu->sugarCrash();
            this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f),
                                                         CCCallFuncN::create(this, callfuncN_selector(GameScene::sugarCrash))));
        }
        else
        {
            afterDeleting();
        }
    }
    else
        afterDeleting();
}


void GameScene::sugarCrash(CCNode* sender)
{
    isSugarCrash = true;
    for (int i = 0; i < gameObjects.size(); i++)
    {
        if (gameObjects[i]->type == Vertical || gameObjects[i]->type == Horizontal || gameObjects[i]->type == Bomb || gameObjects[i]->type == Fish)
        {
            menu->sugarCrash();
            isSugarCrash = false;
            addToDeleting(i);
        }
        else if (gameObjects[i]->type == Crystal)
        {
            eColorGameObject colorDestiny = (eColorGameObject)(rand()%colorNumber);
            for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < columnCount; j++)
				{
					if (isSimpleCell(gameField[i][j]))
						if (findGameObject(i, j) >= 0)
							if (gameObjects[findGameObject(i, j)]->color == colorDestiny)
							{
								addToDeleting(findGameObject(i, j));
                                lighting(gameObjects[allDeletedChain[i]]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
							}
				}
			}
            addToDeleting(i);
        }
    }
	afterDeleting();
}

void GameScene::addMovesInApp()
{
    leftDownMenu->exitPress = false;
    leftDownMenu->setVisible(true);
    menu->addMovesInApp();
    isFinalAction = false;
}

void GameScene::addHammer()
{
    menu->setTouchEnabled(true);
    menu->addHammer();
}

void GameScene::cancelPayment()
{
    if (!endGameMenu->isVisible())
        menu->inappPopupCancel(NULL);
}

void GameScene::win(CCNode* sender)
{
    char buf[255];
    sprintf(buf, "%d",menu->getCurrentLevel());
    
    if (menu->getCurrentLevel() > 105)
    {
        if (menu->getCurrentLevel() == 106)
            MMPPtr->levelCompleted(30, "30E");
        else if (menu->getCurrentLevel() == 107)
            MMPPtr->levelCompleted(24, "24E");
        else if (menu->getCurrentLevel() == 108)
            MMPPtr->levelCompleted(36, "36E");
        else if (menu->getCurrentLevel() == 109)
            MMPPtr->levelCompleted(48, "48E");
        else if (menu->getCurrentLevel() == 110)
            MMPPtr->levelCompleted(60, "60E");
        else if (menu->getCurrentLevel() == 111)
            MMPPtr->levelCompleted(72, "72E");
    }
    else
    {
        MMPPtr->levelCompleted(menu->getCurrentLevel(), string(buf));
    }
    
    SimpleAudioEngine::sharedEngine()->playEffect("sound/level_completed.mp3");
	lock = true;
	end();
    ccColor3B color;
    color.r = 0x8e;
    color.g = 0x2d;
    color.b = 0x1a;
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    isWinSpeed = true;
    if (menu->isBannerLock)
    {
        isFinalBanner = true;
        menu->banner("updateArt/target.png", CCLocalizedString("BANNER_WIN"), 0.0f, ccWHITE);
        endAction = this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.0f),
                                                         CCCallFuncN::create(this, callfuncN_selector(GameScene::winCallback))));
    }
    else
    {
        isFinalBanner = true;
        menu->banner("updateArt/target.png", CCLocalizedString("BANNER_WIN"), 0.0f, ccWHITE);
        endAction = this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.0f),
                                                         CCCallFuncN::create(this, callfuncN_selector(GameScene::winCallback))));
    }
}

void GameScene::lose(CCNode* sender)
{
    isLoseSpeed = true;
    SimpleAudioEngine::sharedEngine()->playEffect("sound/level_failed.mp3");
	lock = true;
	end();
    ccColor3B color;
    color.r = 0x8e;
    color.g = 0x2d;
    color.b = 0x1a;
    SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    isFinalBanner = true;
	menu->banner("updateArt/target.png", CCLocalizedString("BANNER_LOSE"), 0.0f, ccWHITE);
	endAction = this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(3.0f),
		CCCallFuncN::create(this, callfuncN_selector(GameScene::loseCallback))));
}

void GameScene::cleaningTrash(CCNode* sender)
{
	for (int i = 0; i < forDeadObjects.size(); i++)
	{
        if (forDeadObjects[i]->sprite)
            if (forDeadObjects[i]->sprite->getParent())
            {
                forDeadObjects[i]->sprite->removeFromParentAndCleanup(true);
                forDeadObjects[i]->sprite = NULL;
            }
	}
	forDeadObjects.clear();
}

void GameScene::checkCellSimple(int i, int j)
{
    if (i < 0 || i > (rowCount - 1) || j < 0 || j > (columnCount - 1))
        return;
	for (int n = 0; n < cellsCheckedSimple.size(); n++)
	{
		if (cellsCheckedSimple[n].x == i && cellsCheckedSimple[n].y == j)
			return;
	}
    
	cellsCheckedSimple.push_back(ccp(i, j));
    
	if (isSimpleCell(gameField[i][j]))
	{
		if (gameField[i][j] == IceCell)
		{
            for (int k = 0; k < iceCells.size(); k++)
                if (iceCells[k].x == i && iceCells[k].y == j)
                    iceCells.erase(iceCells.begin() + k--);
            
            SimpleAudioEngine::sharedEngine()->playEffect("sound/ice_crashed.mp3");
            CCSprite* iceBreak = CCSprite::createWithSpriteFrameName("game/ice_break.png");
            ((CCSprite*)gameFieldSprites[i][j]->getChildByTag(1))->addChild(iceBreak);
            iceBreak->setPosition(ccp(((CCSprite*)gameFieldSprites[i][j]->getChildByTag(1))->getContentSize().width/2.0f, ((CCSprite*)gameFieldSprites[i][j]->getChildByTag(1))->getContentSize().height/2.0f));
            iceBreak->setScale(0.01f);
            iceBreak->runAction(CCScaleTo::create(0.2f, 1.0f));
            ((CCSprite*)gameFieldSprites[i][j]->getChildByTag(1))->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(GameScene::iceDestroyed)), NULL));
			iceCount--;
			gameField[i][j] = SimpleCell;
            
            CCParticleSystemQuad* explosion;// = CCParticleExplosion::createWithTotalParticles(5);
            if (IPAD)
                explosion = CCParticleExplosion::createWithTotalParticles(10);
            else
                explosion = CCParticleExplosion::createWithTotalParticles(5);
            
            gameFieldSprites[i][j]->getParent()->getParent()->getParent()->addChild(explosion, 10);
            if (LANDSCAPE)
                explosion->setPosition(ccp(gameFieldSprites[i][j]->getPositionX() + CELL_WIDTH*2.0f, gameFieldSprites[i][j]->getPositionY() -CELL_HEIGHT/2.0f));
            else
                explosion->setPosition(gameFieldSprites[i][j]->getPosition());
            explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/ice.png"));
            explosion->setSpeed(100.0f);
            explosion->setSpeedVar(10.0f);
            explosion->setScale(3.0f);
            if (!IPAD)
                explosion->setScale(1.5f);
            explosion->setGravity(ccp(0.0f, -200.0f));
            explosion->setEndSpin(800.0f);
            explosion->setBlendAdditive(true);
            ccBlendFunc blend;
            blend.dst = GL_ONE;
            blend.src = GL_ONE;
            explosion->setBlendFunc(blend);
            
            explosion->setEndColor(ccc4f(0, 255, 255, 255));
            explosion->setEndColorVar(ccc4f(0, 0, 0, 0));
            explosion->setStartColor(ccc4f(229, 229, 229, 229));
            explosion->setStartColorVar(ccc4f(0, 0, 0, 25));
            explosion->setEndSize(0.0f);
            explosion->setLife(3.0f);
            
            char buf[255];
            sprintf(buf, "%d", ICE_SCORE*(sweetCount+1));
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            gameFieldSprites[i][j]->getParent()->addChild(label,10);
            label->setPosition(ccp(gameFieldSprites[i][j]->getPositionX(), gameFieldSprites[i][j]->getPositionY() - 100));
            label->setScale(0.3f);
            ccColor3B col;
            col.r = 0xb6;
            col.g = 0xff;
            col.b = 0x38;
            label->setColor(col);
            
            label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
            
            menu->setCurrentScore(menu->getCurrentScore() + ICE_SCORE*(sweetCount+1));
		}
        else if (gameField[i][j] == CageCell)
		{
            SimpleAudioEngine::sharedEngine()->playEffect("sound/ice_crashed.mp3");
            ((CCSprite*)gameFieldSprites[i][j]->getUserData())->removeFromParentAndCleanup(true);
            gameFieldSprites[i][j]->setUserData(NULL);
            
            char buf[255];
            sprintf(buf, "%d", CAGE_SCORE*(sweetCount+1));
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            gameFieldSprites[i][j]->getParent()->getParent()->getParent()->addChild(label,10);
            label->setPosition(ccp(gameFieldSprites[i][j]->getPositionX(), gameFieldSprites[i][j]->getPositionY() - 100));
            label->setScale(0.3f);
            ccColor3B col;
            col.r = 0xb6;
            col.g = 0xff;
            col.b = 0x38;
            label->setColor(col);
            
            CCParticleSystemQuad* explosion = CCParticleExplosion::createWithTotalParticles(5);
            gameFieldSprites[i][j]->getParent()->addChild(explosion, 10);
            explosion->setPosition(gameFieldSprites[i][j]->getPosition());
            explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/reshetka.png"));
            explosion->setSpeed(100.0f);
            explosion->setSpeedVar(10.0f);
            explosion->setScale(6.0f);
            if (!IPAD)
                explosion->setScale(1.5f);
            explosion->setGravity(ccp(0.0f, -100.0f));
            explosion->setEndSpin(800.0f);
            explosion->setBlendAdditive(true);
            ccBlendFunc blend;
            blend.dst = GL_ONE;
            blend.src = GL_ONE;
            explosion->setBlendFunc(blend);
            
            explosion->setEndColor(ccc4f(0, 255, 255, 255));
            explosion->setEndColorVar(ccc4f(0, 0, 0, 0));
            explosion->setStartColor(ccc4f(229, 229, 229, 229));
            explosion->setStartColorVar(ccc4f(0, 0, 0, 25));
            explosion->setEndSize(0.0f);
            explosion->setLife(3.0f);
            
            label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
            
            menu->setCurrentScore(menu->getCurrentScore() + CAGE_SCORE*(sweetCount+1));
            
			gameField[i][j] = SimpleCell;
        }
        else if (gameField[i][j] == CageIceCell)
		{
            SimpleAudioEngine::sharedEngine()->playEffect("sound/ice_crashed.mp3");
            ((CCSprite*)gameFieldSprites[i][j]->getUserData())->removeFromParentAndCleanup(true);
            gameFieldSprites[i][j]->setUserData(NULL);
            
            char buf[255];
            sprintf(buf, "%d", CAGE_SCORE*(sweetCount+1));
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            gameFieldSprites[i][j]->getParent()->getParent()->getParent()->addChild(label,10);
            label->setPosition(ccp(gameFieldSprites[i][j]->getPositionX(), gameFieldSprites[i][j]->getPositionY() - 100));
            label->setScale(0.3f);
            ccColor3B col;
            col.r = 0xb6;
            col.g = 0xff;
            col.b = 0x38;
            label->setColor(col);
            
            CCParticleSystemQuad* explosion = CCParticleExplosion::createWithTotalParticles(5);
            gameFieldSprites[i][j]->getParent()->addChild(explosion, 10);
            explosion->setPosition(gameFieldSprites[i][j]->getPosition());
            explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/reshetka.png"));
            explosion->setSpeed(100.0f);
            explosion->setSpeedVar(10.0f);
            explosion->setScale(6.0f);
            if (!IPAD)
                explosion->setScale(1.5f);
            explosion->setGravity(ccp(0.0f, -100.0f));
            explosion->setEndSpin(800.0f);
            explosion->setBlendAdditive(true);
            ccBlendFunc blend;
            blend.dst = GL_ONE;
            blend.src = GL_ONE;
            explosion->setBlendFunc(blend);
            
            explosion->setEndColor(ccc4f(0, 255, 255, 255));
            explosion->setEndColorVar(ccc4f(0, 0, 0, 0));
            explosion->setStartColor(ccc4f(229, 229, 229, 229));
            explosion->setStartColorVar(ccc4f(0, 0, 0, 25));
            explosion->setEndSize(0.0f);
            explosion->setLife(3.0f);
            
            label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
            
            menu->setCurrentScore(menu->getCurrentScore() + CAGE_SCORE*(sweetCount+1));
            
			gameField[i][j] = IceCell;
        }
		else if (gameField[i][j] == SuperIceCell)
		{
            for (int k = 0; k < superIceCells.size(); k++)
            {
                if (superIceCells[k].x == i && superIceCells[k].y == j)
                {
                    iceCells.push_back(ccp(i,j));
                    superIceCells.erase(superIceCells.begin() + k--);
                }
            }
            
            SimpleAudioEngine::sharedEngine()->playEffect("sound/ice_crashed.mp3");
			((CCSprite*)gameFieldSprites[i][j]->getChildByTag(1))->runAction(
                                                                             CCSequence::create(
                                                                                                CCCallFuncN::create(this, callfuncN_selector(GameScene::superIceDestroyed)), NULL));
            CCParticleSystemQuad* explosion;// = CCParticleExplosion::createWithTotalParticles(5);
            if (IPAD)
                explosion = CCParticleExplosion::createWithTotalParticles(10);
            else
                explosion = CCParticleExplosion::createWithTotalParticles(5);
            
            gameFieldSprites[i][j]->getParent()->getParent()->getParent()->addChild(explosion, 10);
            if (LANDSCAPE)
                explosion->setPosition(ccp(gameFieldSprites[i][j]->getPositionX() + CELL_WIDTH*2.0f, gameFieldSprites[i][j]->getPositionY() -CELL_HEIGHT/2.0f));
            else
                explosion->setPosition(gameFieldSprites[i][j]->getPosition());
            explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/ice.png"));
            explosion->setSpeed(100.0f);
            explosion->setSpeedVar(10.0f);
            explosion->setScale(3.0f);
            if (!IPAD)
                explosion->setScale(1.5f);
            explosion->setGravity(ccp(0.0f, -200.0f));
            explosion->setEndSpin(800.0f);
            explosion->setBlendAdditive(true);
            ccBlendFunc blend;
            blend.dst = GL_ONE;
            blend.src = GL_ONE;
            explosion->setBlendFunc(blend);
            
            explosion->setEndColor(ccc4f(0, 255, 255, 255));
            explosion->setEndColorVar(ccc4f(0, 0, 0, 0));
            explosion->setStartColor(ccc4f(229, 229, 229, 229));
            explosion->setStartColorVar(ccc4f(0, 0, 0, 25));
            explosion->setEndSize(0.0f);
            explosion->setLife(3.0f);
            
            char buf[255];
            sprintf(buf, "%d", MEGA_SCORE*(sweetCount+1));
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            gameFieldSprites[i][j]->getParent()->addChild(label,10);
            label->setPosition(ccp(gameFieldSprites[i][j]->getPositionX(), gameFieldSprites[i][j]->getPositionY() - 100));
            label->setScale(0.3f);
            ccColor3B col;
            col.r = 0xb6;
            col.g = 0xff;
            col.b = 0x38;
            label->setColor(col);
            
            label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
            menu->setCurrentScore(menu->getCurrentScore() + MEGA_SCORE*(sweetCount+1));
            
			gameField[i][j] = IceCell;
		}
	}
}

void GameScene::updateFacebook()
{
    endGameMenu->updateFacebook();
}


void GameScene::checkCellLock(int i, int j)
{
    if (i < 0 || i > (rowCount - 1) || j < 0 || j > (columnCount - 1))
        return;
	for (int n = 0; n < cellsCheckedLock.size(); n++)
	{
		if (cellsCheckedLock[n].x == i && cellsCheckedLock[n].y == j)
			return;
	}
    
	cellsCheckedLock.push_back(ccp(i, j));
    
    if (isLockCell(gameField[i][j]))
	{
		if (gameField[i][j] == MegaIceCell)
		{
            superIceCells.push_back(ccp(i, j));
            SimpleAudioEngine::sharedEngine()->playEffect("sound/ice_crashed.mp3");
			((CCSprite*)gameFieldSprites[i][j]->getChildByTag(1))->runAction(
                                                                             CCSequence::create(
                                                                                                CCCallFuncN::create(this, callfuncN_selector(GameScene::megaIceDestroyed)), NULL));
            
            
            CCParticleSystemQuad* explosion = CCParticleExplosion::createWithTotalParticles(5);
            gameFieldSprites[i][j]->getParent()->getParent()->getParent()->addChild(explosion, 10);
            if (LANDSCAPE)
                explosion->setPosition(ccp(gameFieldSprites[i][j]->getPositionX() + CELL_WIDTH*2.0f, gameFieldSprites[i][j]->getPositionY() -CELL_HEIGHT/2.0f));
            else
                explosion->setPosition(gameFieldSprites[i][j]->getPosition());
            explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/ice.png"));
            explosion->setSpeed(100.0f);
            explosion->setSpeedVar(10.0f);
            explosion->setScale(3.0f);
            if (!IPAD)
                explosion->setScale(1.5f);
            explosion->setGravity(ccp(0.0f, -200.0f));
            explosion->setEndSpin(800.0f);
            explosion->setBlendAdditive(true);
            ccBlendFunc blend;
            blend.dst = GL_ONE;
            blend.src = GL_ONE;
            explosion->setBlendFunc(blend);
            
            explosion->setEndColor(ccc4f(0, 255, 255, 255));
            explosion->setEndColorVar(ccc4f(0, 0, 0, 0));
            explosion->setStartColor(ccc4f(229, 229, 229, 229));
            explosion->setStartColorVar(ccc4f(0, 0, 0, 25));
            explosion->setEndSize(0.0f);
            explosion->setLife(3.0f);
            
			gameField[i][j] = SuperIceCell;
            
			eColorGameObject color = (eColorGameObject)(rand()%colorNumber);
			sGameObject* gameObj = new sGameObject(Simple, color, i, j, xZero, yZero);
			batchNode->addChild(gameObj->node, 2);
			gameObjects.push_back(gameObj);
			gameObj->sprite->setOpacity(0);
			gameObj->sprite->runAction(CCSequence::create(CCFadeIn::create(0.1f), NULL));
            
            newCreateObjects.push_back(gameObj);
            
            char buf[255];
            sprintf(buf, "%d", MEGA_SCORE);
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            gameFieldSprites[i][j]->getParent()->addChild(label,10);
            label->setPosition(gameFieldSprites[i][j]->getPosition());
            ccColor3B col;
            col.r = 0xb6;
            col.g = 0xff;
            col.b = 0x38;
            label->setColor(col);
            
            label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
            menu->setCurrentScore(menu->getCurrentScore() + MEGA_SCORE*(sweetCount+1));
		}
		if (gameField[i][j] == StoneCell)
		{
            SimpleAudioEngine::sharedEngine()->playEffect("sound/hammer_impact.mp3");
			((CCSprite*)gameFieldSprites[i][j]->getChildByTag(3))->runAction(
                                                                             CCSequence::create(
                                                                                                CCScaleTo::create(0.1f, 1.1f),
                                                                                                CCScaleTo::create(0.1f, 1.0f),
                                                                                                CCScaleTo::create(0.1f, 1.1f),
                                                                                                CCScaleTo::create(0.1f, 1.0f),
                                                                                                CCScaleTo::create(0.1f, 1.1f),
                                                                                                CCScaleTo::create(0.1f, 1.0f),
                                                                                                CCCallFuncN::create(this, callfuncN_selector(GameScene::stoneDestroyed)), NULL));
            
            CCParticleSystemQuad* explosion = CCParticleExplosion::createWithTotalParticles(5);
            gameFieldSprites[i][j]->getParent()->getParent()->getParent()->addChild(explosion, 10);
            if (LANDSCAPE)
                explosion->setPosition(ccp(gameFieldSprites[i][j]->getPositionX() + CELL_WIDTH*2.0f, gameFieldSprites[i][j]->getPositionY() -CELL_HEIGHT/2.0f));
            else
                explosion->setPosition(gameFieldSprites[i][j]->getPosition());
            explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/stone.png"));
            explosion->setSpeed(100.0f);
            explosion->setSpeedVar(10.0f);
            explosion->setScale(3.0f);
            if (!IPAD)
                explosion->setScale(1.5f);
            explosion->setGravity(ccp(0.0f, -200.0f));
            explosion->setEndSpin(800.0f);
            explosion->setBlendAdditive(true);
            ccBlendFunc blend;
            blend.dst = GL_ONE;
            blend.src = GL_ONE;
            explosion->setBlendFunc(blend);
            
            explosion->setEndColor(ccc4f(0, 255, 255, 255));
            explosion->setEndColorVar(ccc4f(0, 0, 0, 0));
            explosion->setStartColor(ccc4f(229, 229, 229, 229));
            explosion->setStartColorVar(ccc4f(0, 0, 0, 25));
            explosion->setEndSize(0.0f);
            explosion->setLife(3.0f);
            
            char buf[255];
            sprintf(buf, "%d", STONE_SCORE);
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            gameFieldSprites[i][j]->getParent()->addChild(label,10);
            label->setPosition(gameFieldSprites[i][j]->getPosition());
            ccColor3B col;
            col.r = 0xb6;
            col.g = 0xff;
            col.b = 0x38;
            label->setColor(col);
            
            label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
            menu->setCurrentScore(menu->getCurrentScore() + STONE_SCORE*(sweetCount+1));
			gameField[i][j] = SimpleCell;
		}
        if (gameField[i][j] == Chocolate)
		{
            SimpleAudioEngine::sharedEngine()->playEffect("sound/hammer_impact.mp3");
			((CCSprite*)gameFieldSprites[i][j]->getChildByTag(4))->runAction(CCSequence::create(
                                                                                                CCFadeOut::create(0.2f), CCHide::create(),
                                                                                                CCCallFuncN::create(this, callfuncN_selector(GameScene::chocolateDestroyed)), NULL));
            
            CCParticleSystemQuad* explosion = CCParticleExplosion::createWithTotalParticles(10);
            gameFieldSprites[i][j]->getParent()->getParent()->addChild(explosion, 10);
            if (LANDSCAPE)
                explosion->setPosition(ccp(gameFieldSprites[i][j]->getPositionX() + CELL_WIDTH*2.0f, gameFieldSprites[i][j]->getPositionY() -CELL_HEIGHT/2.0f));
            else
                explosion->setPosition(gameFieldSprites[i][j]->getPosition());
            explosion->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/chokolad.png"));
            explosion->setSpeed(100.0f);
            explosion->setSpeedVar(10.0f);
            explosion->setScale(5.0f);
            if (!IPAD)
                explosion->setScale(2.5f);
            explosion->setGravity(ccp(0.0f, -200.0f));
            explosion->setEndSpin(800.0f);
            explosion->setBlendAdditive(true);
            ccBlendFunc blend;
            blend.dst = GL_ONE_MINUS_SRC_ALPHA;
            blend.src = GL_SRC_ALPHA;
            explosion->setBlendFunc(blend);
            
            explosion->setEndColor(ccc4f(255, 255, 255, 255));
            explosion->setEndColorVar(ccc4f(0, 0, 0, 0));
            explosion->setStartColor(ccc4f(255, 255, 255, 255));
            explosion->setStartColorVar(ccc4f(0, 0, 0, 0));
            explosion->setEndSize(0.0f);
            explosion->setLife(3.0f);
            
            char buf[255];
            sprintf(buf, "%d", CHOCOLATE_SCORE);
            CCLabelTTF* label = CCLabelTTF::create(buf, FONT_COMMON, FONT_SIZE_86);
            gameFieldSprites[i][j]->getParent()->addChild(label,10);
            label->setPosition(gameFieldSprites[i][j]->getPosition());
            ccColor3B col;
            col.r = 0xb6;
            col.g = 0xff;
            col.b = 0x38;
            label->setColor(col);
            
            label->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCMoveBy::create(0.7f, ccp(0.0f, 50.0f)), CCFadeOut::create(2.0f)), CCHide::create()));
            menu->setCurrentScore(menu->getCurrentScore() + CHOCOLATE_SCORE*(sweetCount+1));
            
            
            bool flagIce = false;
            
            for(int k = 0; k < superIceCells.size(); k++)
            {
                if (i == superIceCells[k].x && j == superIceCells[k].y)
                {
                    gameField[i][j] = SuperIceCell;
                    CCSprite* ice = CCSprite::createWithSpriteFrameName("game/superIce.png");
                    ice->setPosition(ccp(gameFieldSprites[i][j]->getContentSize().width/2.0f, gameFieldSprites[i][j]->getContentSize().height/2.0f));
                    gameFieldSprites[i][j]->addChild(ice, 10);
                    ice->setOpacity(0);
                    ice->runAction(CCFadeIn::create(0.2f));
                    ice->setTag(1);
                    flagIce = true;
                    
                    if (ice)
                    {
                        if (IPAD_MINI)
                        {
                            ice->setScale(0.93f);
                        }
                        else if (IPHONE_4 || IPHONE_5)
                        {
                            ice->setScale(0.85f);
                        }
                    }

                    break;
                }
            }
            
            if (!flagIce)
            {
                for(int k = 0; k < iceCells.size(); k++)
                {
                    if (i == iceCells[k].x && j == iceCells[k].y)
                    {
                        gameField[i][j] = IceCell;
                        CCSprite* ice = CCSprite::createWithSpriteFrameName("game/ice.png");
                        ice->setPosition(ccp(gameFieldSprites[i][j]->getContentSize().width/2.0f, gameFieldSprites[i][j]->getContentSize().height/2.0f));
                        gameFieldSprites[i][j]->addChild(ice, 10);
                        ice->setOpacity(0);
                        ice->runAction(CCFadeIn::create(0.2f));
                        ice->setTag(1);
                        flagIce = true;
                        
                        if (ice)
                        {
                            if (IPAD_MINI)
                            {
                                ice->setScale(0.93f);
                            }
                            else if (IPHONE_4 || IPHONE_5)
                            {
                                ice->setScale(0.85f);
                            }
                        }

                        break;
                    }
                }
            }
            
            if (!flagIce)
                gameField[i][j] = SimpleCell;
			         
            isChocolateDestroyed = true;
		}
	}
}

void GameScene::chocolateDestroyed(CCNode* sender)
{
    ((CCSprite*)sender)->removeFromParentAndCleanup(true);
}

void GameScene::iceDestroyed(CCNode* sender)
{
    ((CCSprite*)sender)->removeFromParentAndCleanup(true);
}

void GameScene::stoneDestroyed(CCNode* sender)
{
	((CCSprite*)sender)->removeFromParentAndCleanup(true);
}

void GameScene::megaIceDestroyed(CCNode* sender)
{
 	((CCSprite*)sender)->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/superIce.png")->displayFrame());
}

void GameScene::superIceDestroyed(CCNode* sender)
{
	((CCSprite*)sender)->setDisplayFrame(CCSprite::createWithSpriteFrameName("game/ice.png")->displayFrame());
    CCSprite* iceBreak = CCSprite::createWithSpriteFrameName("game/ice_break.png");
    ((CCSprite*)sender)->addChild(iceBreak);
    iceBreak->setPosition(ccp(((CCSprite*)sender)->getContentSize().width/2.0f, ((CCSprite*)sender)->getContentSize().height/2.0f));
    iceBreak->setScale(0.01f);
    iceBreak->runAction(CCSequence::create(CCScaleTo::create(0.2f, 1.0f), CCDelayTime::create(0.2f), CCHide::create(), NULL));
}

void GameScene::refillObject(CCNode* sender)
{
	int maxCountEmptyCell = 0;
    float deltaMove = 0.3f;
    float afterDeltaMove = 0.0f;
    float accel = 0.37f;
    
    float multi = 1.37f;
    
    bool isTeleport = false;
    
	for (int i = 0; i < columnCount; i++)
	{
        bool isLastBerry = false;
        if (i == notRefill || i == notRefill - 1 || i == notRefill + 1)
            continue;
        
		int countEmptyCell = 0;
		bool flagAfter = false;
        
        afterDeltaMove = 0.0f;
        
        for (int n = 0; n < refillDelay.size(); n++)
        {
            if (refillDelay[n] == i)
                afterDeltaMove = 0.2f;
        }
        
        CCPoint lockPortal = ccp(-1, -1);
        
        if (!portals.empty())
        {
            for (int j = 0; j < rowCount; j++)
            {
                for (int k = 0; k < portals.size(); k++)
                {
                    if (!portals[k].enter)
                    {
                        if (portals[k].x == j && portals[k].y == i)
                        {
                            lockPortal.x = j;
                            lockPortal.y = i;
                        }
                    }
                }
            }
        }
        
		for (int j = rowCount - 1; j >= 0; j--)
		{
			if (isSimpleCell(gameField[j][i]) && gameField[j][i] != CageCell && gameField[j][i] != CageIceCell)
			{
				int currObj = findGameObject(j, i);
				if (currObj >= 0)
				{
					int countShift = 0;
					int countEmpty = 0;
                    if (gameObjects[currObj]->type == Cookie)
                    {
                        isLastBerry = true;
                        for(int n = j + 1; n < rowCount; n++)
                        {
                            if (isLockCell(gameField[n][i]))
                            {
                                isLastBerry = false;
                                break;
                            }
                            
                            if (lockPortal.x == n && lockPortal.y == i)
                            {
                                isLastBerry = false;
                                break;
                            }
                            
                            if (findGameObject(n,i) >= 0)
                            {
                                isLastBerry = false;
                                break;
                            }
                        }
                    }
                    
					for(int n = j + 1; n < rowCount; n++)
					{
						if (gameField[n][i] == NoneCell)
						{
							countEmpty++;
							continue;
						}
						if (isLockCell(gameField[n][i]))
							break;
                        
                        if (lockPortal.x == n && lockPortal.y == i)
                        {
                            break;
                        }
                        
                        
						if (findGameObject(n,i) < 0)
						{
							if (countEmpty > 0)
							{
								countShift+= countEmpty;
								countEmpty = 0;
							}

							countShift++;
						}
					}
                    if (isLastBerry)
                    {
                        countShift++;
                    }
                    
                    if (countShift == 1)
						deltaMove = 0.35f;
                    else if (countShift == 2)
						deltaMove = 0.4f;
					else if (countShift == 3)
						deltaMove = 0.5f;
					else if (countShift > 3 && countShift <= 6)
                        deltaMove = 0.6f;
                    else
                        deltaMove = 0.7f;
                    
                    deltaMove*=multi;

					if (countShift > 0)
					{
                        lastRefillObjects.push_back(currObj);
                        if (isLastBerry)
                        {
                            int n = rowCount-1;
                            for (n = rowCount-1; n >= 0; n--)
                                if (gameField[n][i] != NoneCell && gameField[n][i] != LockCell)
                                    break;
                            
                            if (LANDSCAPE)
                            {
                                gameObjects[currObj]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[n][i]->getPositionY() - CELL_HEIGHT, WINSIZE.width, WINSIZE.height - (gameFieldSprites[n][i]->getPositionY() - CELL_HEIGHT)));
                            }
                            else
                            {
                                gameObjects[currObj]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[n][i]->getPositionY() - CELL_HEIGHT/2.0f, WINSIZE.width, WINSIZE.height - (gameFieldSprites[n][i]->getPositionY()  - CELL_HEIGHT/2.0f)));
                            }
                            gameObjects[currObj]->sprite->runAction(CCSequence::create(CCDelayTime::create(afterDeltaMove), CCEaseOut::create(CCMoveBy::create(deltaMove, ccp(0.0f, - CELL_HEIGHT*countShift)), accel), CCMoveBy::create(0.1f, ccp(0.0f, 5.0f)), CCMoveBy::create(0.1f, ccp(0.0f, -5.0f)), NULL));
                        }
                        else
                        {
                            gameObjects[currObj]->sprite->runAction(CCSequence::create(CCDelayTime::create(afterDeltaMove), CCEaseOut::create(CCMoveBy::create(deltaMove, ccp(0.0f, - CELL_HEIGHT*countShift)), accel), CCMoveBy::create(0.1f, ccp(0.0f, 5.0f)), CCMoveBy::create(0.1f, ccp(0.0f, -5.0f)),NULL));
                        }
						gameObjects[currObj]->x += countShift;
                        flagAfter = true;
                        isLastBerry = false;

					}
				}
				else
				{
					countEmptyCell++;
				}
			}
			else if (gameField[j][i] == NoneCell)
			{
                    countEmptyCell++;
			}
		}

		if (countEmptyCell > maxCountEmptyCell)
			maxCountEmptyCell = countEmptyCell;
        
        int noneCellCount = 0;
        
		eColorGameObject prevColor = Colorurless;
        int firstj = -1;
        int jIdx = rowCount - 1;
        for (int j = 0; j < rowCount; j++)
		{
            if (isLockCell(gameField[j][i]))
            {
                jIdx = j;
                break;
            }
            if ((lockPortal.x == j && lockPortal.y == i))
            {
                jIdx = j - 1;
                break;
            }
        }
        
        
		for (int j = jIdx; j >= 0; j--)
		{
			if (isLockCell(gameField[j][i]))
				continue;
            
			if (isSimpleCell(gameField[j][i])  && gameField[j][i] != CageCell && gameField[j][i] != CageIceCell)
			{
                if (firstj < 0)
                    firstj = j;
				int currObj = findGameObject(j, i);
				if (currObj < 0)
				{
                    int currentEmptyCell = countEmptyCell;
                    
					eColorGameObject color = (eColorGameObject)(rand()%colorNumber);
                    
                    bool isColor = false;
					
                    int randomBreak = 0;
                    if (!isSimplified)
                    {
                        while (!isColor && randomBreak < 1000)
                        {
                            color = (eColorGameObject)(rand()%colorNumber);
                            isColor = true;
                            if (findGameObject(j - 1, i) >= 0)
                                if (color == gameObjects[findGameObject(j - 1, i)]->color)
                                    isColor = false;
                            
                            if (findGameObject(j, i + 1) >= 0)
                                if (color == gameObjects[findGameObject(j, i + 1)]->color)
                                    isColor = false;
                            
                            if (findGameObject(j, i - 1) >= 0)
                                if (color == gameObjects[findGameObject(j, i - 1)]->color)
                                    isColor = false;
                            
                            if (color == prevColor)
                                isColor = false;
                            
                            randomBreak++;
                        }
                    }
                    else
                    {
                        color = (eColorGameObject)(rand()%colorNumber);
                        
                        if (rand()%100 > 60)
                        {
                            int randColor = rand() % 3;

                            if (randColor == 0)
                                if (findGameObject(j - 1, i) >= 0)
                                    color = gameObjects[findGameObject(j - 1, i)]->color;
                            
                            if (randColor == 1)
                                if (findGameObject(j, i + 1) >= 0)
                                    color = gameObjects[findGameObject(j, i + 1)]->color;
                            
                            if (randColor == 2)
                                if (findGameObject(j, i - 1) >= 0)
                                    color = gameObjects[findGameObject(j, i - 1)]->color;
                        }
                    }
					
					prevColor = color;
					sGameObject* gameObj = new sGameObject(Simple, color, j, i, xZero, yZero);
					if (isNeedCookie)
					{
						isNeedCookie = false;
						gameObj->changeColor(Colorurless);
                        if(menu->isStrawberry)
                            gameObj->isStrawberry = true;
                        else
                            gameObj->isStrawberry = false;
                        gameObj->changeType(Cookie);
					}
					batchNode->addChild(gameObj->node, 2);
                    
					gameObjects.push_back(gameObj);
                    int rowNormal = 0;
                    
                    for (rowNormal = 0; rowNormal < rowCount; rowNormal++)
                            if (gameField[rowNormal][i] != NoneCell && gameField[rowNormal][i] != LockCell)
                                break;
                    
					gameObj->sprite->setPosition(ccp(gameObj->sprite->getPositionX(), gameObj->sprite->getPositionY() + CELL_HEIGHT*(currentEmptyCell + noneCellCount)));
                    
                    if (IPHONE_5 || IPHONE_4)
                    {
                        if (LANDSCAPE)
                            gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[rowNormal][i]->getPositionY()));
                        else
                            gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[rowNormal][i]->getPositionY() + CELL_HEIGHT));
                    }
                    else
                    {
                        if (LANDSCAPE)
                            gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[rowNormal][i]->getPositionY()));
                        else
                            gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[rowNormal][i]->getPositionY() + CELL_HEIGHT/2.0f));
                    }
                    
                    if (gameType == Time  && !(rand()%25) && !menu->isThirdStar)
                    {
                        gameObj->setTime();
                    }
                    
                    if (currentEmptyCell + noneCellCount == 1)
						deltaMove = 0.35f;
                    else if (currentEmptyCell + noneCellCount == 2)
						deltaMove = 0.4f;
					else if (currentEmptyCell + noneCellCount == 3)
						deltaMove = 0.5f;
					else if (currentEmptyCell + noneCellCount > 3 && currentEmptyCell + noneCellCount <= 6)
                        deltaMove = 0.6f;
                    else
                        deltaMove = 0.7f;
                    
                    deltaMove*=multi;
                    
                    lastRefillObjects.push_back(gameObjects.size() - 1);

					gameObj->sprite->runAction(CCSequence::create(CCDelayTime::create(afterDeltaMove), CCEaseOut::create(CCMoveBy::create(deltaMove, ccp(0.0f, - CELL_HEIGHT*(currentEmptyCell + noneCellCount))), accel),
						CCMoveBy::create(0.1f, ccp(0.0f, 5.0f)), CCMoveBy::create(0.1f, ccp(0.0f, -5.0f)),NULL));
                    flagAfter = true;
				}
			}
		}
	}

    if (maxCountEmptyCell == 1)
        deltaMove = 0.35f;
    else if (maxCountEmptyCell == 2)
        deltaMove = 0.4f;
    else if (maxCountEmptyCell == 3)
        deltaMove = 0.5f;
    else if (maxCountEmptyCell > 3 && maxCountEmptyCell <= 6)
        deltaMove = 0.6f;
    else
        deltaMove = 0.7f;
    deltaMove = deltaMove + afterDeltaMove;
    
    deltaMove*=multi;
	SimpleAudioEngine::sharedEngine()->playEffect("sound/item_fall.mp3");
    refillDelay.clear();
    
    if (!portals.empty())
    {
        for (int k = 0; k < portals.size(); k++)
        {
            if (!portals[k].enter)
            {
                if (!isLockCell(gameField[portals[k].x][portals[k].y]) && findGameObject(portals[k].x, portals[k].y) < 0)
                {
                    int portalEnter = -1;
                    
                    for (int i = 0; i < portals.size(); i++)
                    {
                        if (portals[i].idx == portals[k].idx - 1)
                        {
                            portalEnter = i;
                            CCLOG("Portal:%d", portalEnter);
                        }
                    }
                    
                    if (portalEnter >= 0 && !isLockCell(gameField[portals[portalEnter].x][portals[portalEnter].y]) &&
                        findGameObject(portals[portalEnter].x, portals[portalEnter].y) >= 0)
                    {
                        isTeleport = true;
                    }
                }
            }
        }
    }
    
    /*
    if (checkFieldBringDown())
    {
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(deltaMove-0.2f), CCCallFuncN::create( this, callfuncN_selector(GameScene::afterDeleting))));
        return;
    }
     */
    
    if (isTeleport)
    {
        refillPortals(NULL);
    }
    else
    {
        if (!nextDead.empty())
        {
            this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(deltaMove-0.1f), CCCallFuncN::create( this, callfuncN_selector(GameScene::destroyFinished))));
        }
        else
        {
            this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(deltaMove-0.1f), CCCallFuncN::create( this, callfuncN_selector(GameScene::refillLockObject))));
        }
    }
}

void GameScene::refillPortals(CCNode* sender)
{
    float deltaMove = 0.0f;
    float accel = 0.40f;
    
    for (int k = 0; k < portals.size(); k++)
    {
        if (!portals[k].enter)
        {
            int countObjectRefill = 0;
            int j = portals[k].y;
            for (int i = portals[k].x; i < rowCount; i++)
            {
                if (isLockCell(gameField[i][j]))
                    break;
                bool isLastNone = false;
                if (isNoneCell(i,j))
                {
                    isLastNone = true;
                    for (int n = i; n < rowCount; n++)
                    {
                        if (findGameObject(n, j) >= 0)
                        {
                            isLastNone = false;
                        }
                    }
                }
                if (isLastNone)
                    break;

                if (findGameObject(i, j) >= 0)
                    break;
                countObjectRefill++;
            }
            
            int portalEnter = -1;
            
            for (int i = 0; i < portals.size(); i++)
            {
                if (portals[i].idx == portals[k].idx - 1)
                {
                    portalEnter = i;
                }
            }
            
            if (isLockCell(gameField[portals[portalEnter].x][portals[portalEnter].y]) &&
                findGameObject(portals[portalEnter].x, portals[portalEnter].y) < 0)
                continue;
            
            j = portals[portalEnter].y;
            
            int countShift = 0;
            
            int countObjectRefillSave = countObjectRefill;
            for (int i = portals[portalEnter].x; i >= 0; i--)
            {
                countShift++;

                if (isLockCell(gameField[i][j]))
                    break;
                
                if (findGameObject(i, j) < 0)
                    break;
                
                if (isNoneCell(i, j))
                    countShift++;
                
                isTeleportedObject = true;
                
                if (countObjectRefill > 0)
                {
                    countObjectRefill--;
					if (countShift > 0)
					{
                        if (countShift == 1)
                            deltaMove = 0.35f;
                        else if (countShift == 2)
                            deltaMove = 0.4f;
                        else if (countShift == 3)
                            deltaMove = 0.5f;
                        else if (countShift > 3 && countShift <= 6)
                            deltaMove = 0.6f;
                        else
                            deltaMove = 0.7f;
                        
                        deltaMove*=1.37f;
                        
						gameObjects[findGameObject(i, j)]->sprite->
                        runAction(CCSequence::create(CCEaseOut::create(CCMoveBy::create(deltaMove, ccp(0.0f, - CELL_HEIGHT*countShift)), accel), CCCallFuncN::create(this, callfuncN_selector(GameScene::deletingTeleportingObject)), NULL));
                        
                        int currlevel = menu->getCurrentLevel();

                        if (IPHONE_5 || IPHONE_4)
                        {
                            if (LANDSCAPE)
                            {
                                gameObjects[findGameObject(i, j)]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT, WINSIZE.width, WINSIZE.height - (gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT)));
                            }
                            else
                            {
                                if (currlevel == 77)
                                    gameObjects[findGameObject(i, j)]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/4.0f, WINSIZE.width, WINSIZE.height - (gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/2.0f)));
                                if (currlevel == 83)
                                    gameObjects[findGameObject(i, j)]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/1.2f, WINSIZE.width, WINSIZE.height - (gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/2.0f)));
                                else
                                    gameObjects[findGameObject(i, j)]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/2.0f, WINSIZE.width, WINSIZE.height - (gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/2.0f)));
                            }

                        }
                        else
                        {
                            if (LANDSCAPE)
                            {
                                gameObjects[findGameObject(i, j)]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT, WINSIZE.width, WINSIZE.height - (gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT)));
                            }
                            else
                            {
                                if (currlevel == 77)
                                    gameObjects[findGameObject(i, j)]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/4.0f, WINSIZE.width, WINSIZE.height - (gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/2.0f)));
                                if (currlevel == 83)
                                    gameObjects[findGameObject(i, j)]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/2.0f, WINSIZE.width, WINSIZE.height - (gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/2.0f)));
                                else
                                    gameObjects[findGameObject(i, j)]->node->setClippingRegion(CCRect(0.0f, gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/2.0f, WINSIZE.width, WINSIZE.height - (gameFieldSprites[portals[portalEnter].x][ j]->getPositionY() - CELL_HEIGHT/2.0f)));
                            }

                        }
                        newCreateObjects.push_back(gameObjects[findGameObject(i, j)]);
                        
                        
                        eColorGameObject colorTelObject = gameObjects[findGameObject(i, j)]->color;
                        if (colorTelObject == Colorurless)
                            colorTelObject = Red;
                        sGameObject* gameObj = new sGameObject(Simple, colorTelObject, portals[k].x + countObjectRefill, portals[k].y, xZero, yZero);
                        
                        if (IPHONE_5 || IPHONE_4)
                        {
                            if (LANDSCAPE)
                            {
                                gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[portals[k].x][portals[k].y]->getPositionY()));
                            }
                            else
                            {
                                if (currlevel == 77)
                                    gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[portals[k].x][portals[k].y]->getPositionY() + CELL_HEIGHT/2.5f));
                                else
                                    gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[portals[k].x][portals[k].y]->getPositionY() + CELL_HEIGHT));
                            }
                        }
                        else
                        {
                            if (LANDSCAPE)
                            {
                                gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[portals[k].x][portals[k].y]->getPositionY()));
                            }
                            else
                            {
                                gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[portals[k].x][portals[k].y]->getPositionY() + CELL_HEIGHT/2.0f));
                            }
                        }
                        
                        if (gameObjects[findGameObject(i, j)]->type != Simple)
                            gameObj->changeType(gameObjects[findGameObject(i, j)]->type);
                        batchNode->addChild(gameObj->node, 2);
                        gameObjects.push_back(gameObj);
                        gameObj->sprite->setPosition(ccp(gameObj->sprite->getPositionX(), gameObj->sprite->getPositionY() + CELL_HEIGHT*(countObjectRefill + countShift)));
                        
                        newCreateObjects.push_back(gameObj);
                        
                        if ((countObjectRefill + countShift) == 1)
                            deltaMove = 0.35f;
                        else if ((countObjectRefill + countShift) == 2)
                            deltaMove = 0.4f;
                        else if ((countObjectRefill + countShift) == 3)
                            deltaMove = 0.5f;
                        else if ((countObjectRefill + countShift) > 3 && (countObjectRefill +countShift) <= 6)
                            deltaMove = 0.6f;
                        else
                            deltaMove = 0.7f;
                        
                        deltaMove*=1.37f;
                         gameObj->sprite->runAction(CCSequence::create(CCEaseOut::create(CCMoveBy::create(deltaMove, ccp(0.0f, - CELL_HEIGHT*(countObjectRefill + countShift))), accel), NULL));
                        
                        gameObjects.erase(gameObjects.begin() + findGameObject(i, j));
					}
                }
                /*else
                {
                    if (countShift == 1)
                        deltaMove = 0.35f;
                    else if (countShift == 2)
						deltaMove = 0.4f;
					else if (countShift == 3)
						deltaMove = 0.5f;
					else if (countShift > 3 && countShift <= 6)
                        deltaMove = 0.6f;
                    else
                        deltaMove = 0.7f;
                    
                    deltaMove*=1.37f;
                    
					if (countShift > 0)
					{
						gameObjects[findGameObject(i, j)]->sprite->
                        runAction(CCSequence::create(CCEaseOut::create(CCMoveBy::create(deltaMove, ccp(0.0f, - CELL_HEIGHT*countShift)), accel), CCCallFuncN::create(this, callfuncN_selector(GameScene::deletingTeleportingObject)), NULL));
                        
                        sGameObject* gameObj = new sGameObject(Simple, gameObjects[findGameObject(i, j)]->color, portals[k].x + countObjectRefill, portals[k].y, xZero, yZero);
                        if (gameObjects[findGameObject(i, j)]->type != Simple)
                            gameObj->changeType(gameObjects[findGameObject(i, j)]->type);
                        batchNode->addChild(gameObj->sprite, 2);
                        gameObjects.push_back(gameObj);
                        gameObj->sprite->setPosition(ccp(gameObj->sprite->getPositionX(), gameObj->sprite->getPositionY() + CELL_HEIGHT*countObjectRefill));
                        
                        gameObj->sprite->runAction(CCSequence::create(CCEaseOut::create(CCMoveBy::create(deltaMove, ccp(0.0f, - CELL_HEIGHT*countObjectRefill)), accel), NULL));
                        
                        gameObjects.erase(gameObjects.begin() + findGameObject(i, j));
					}
                }*/
            }
        }
    }
    refillObject();
}

void GameScene::deletingTeleportingObject(CCNode* sender)
{
    ((CCSprite*)sender)->removeFromParentAndCleanup(true);
}

void GameScene::fallEnded(CCNode* sender)
{
    sender->setVisible(true);
}

void GameScene::refillLockObject(CCNode* sender)
{
	if (dropN > 8)
		dropN = 1;
	if (dropN < 1)
		dropN = 1;
	char buf[255];
	sprintf(buf, "sound/drop_%d.mp3", dropN++);
	SimpleAudioEngine::sharedEngine()->playEffect(buf);
	bool isRefillLock = false;
	for (int i = 0; i < columnCount; i++)
	{
		bool isLockStripe = false;
		for (int j = 0; j < rowCount; j++)
		{
            bool isTeleport = false;
            
            if (!portals.empty())
            {
                for (int k = 0; k < portals.size(); k++)
                {
                    if (!portals[k].enter && portals[k].x == j && portals[k].y == i)
                    {
                        if (findGameObject(portals[k].x, portals[k].y) < 0)
                        {
                            int portalEnter = -1;
                            
                            for (int i = 0; i < portals.size(); i++)
                            {
                                if (portals[i].idx == portals[k].idx - 1)
                                {
                                    portalEnter = i;
                                    CCLOG("Portal:%d", portalEnter);
                                }
                            }
                            
                            if (isLockCell(gameField[portals[portalEnter].x][portals[portalEnter].y]) ||
                                findGameObject(portals[portalEnter].x, portals[portalEnter].y) < 0)
                            {
                                isTeleport = true;
                                break;
                            }
                        }
                    }
                }
            }
            
			if (isLockCell(gameField[j][i]) || isTeleport)
			{
				isLockStripe = true;
				continue;
			}
            
			if (isLockStripe)
			{
				if (isSimpleCell(gameField[j][i]) && gameField[j][i] != CageCell && gameField[j][i] != CageIceCell)
				{
					int currObj = findGameObject(j, i);
					if (currObj < 0)
					{
						int leftShiftObject = findGameObject(j - 1, i - 1);
						int rightShiftObject = findGameObject(j - 1, i + 1);

                        if (gameField[j][i - 1] != CageCell && gameField[j][i - 1] != CageIceCell && gameField[j][i - 1] != LockCell &&
                            gameField[j][i - 1] != MegaIceCell && gameField[j][i - 1] != StoneCell)
                        {
                            if (leftShiftObject >= 0 && gameField[j - 1][i - 1] != CageCell && gameField[j - 1][i - 1] != CageIceCell)
                            {
                                gameObjects[leftShiftObject]->moveRightDown();
                                lastRefillObjects.push_back(leftShiftObject);
                                isRefillLock = true;
                                break;
                            }
                            else if (gameField[j][i + 1] != CageCell && gameField[j][i + 1] != CageIceCell && gameField[j][i + 1] != LockCell &&
                                     gameField[j][i + 1] != MegaIceCell && gameField[j][i + 1] != StoneCell)
                            {
                                if (rightShiftObject >= 0 && gameField[j - 1][i + 1]  != CageCell && gameField[j - 1][i + 1]  != CageIceCell)
                                {
                                    gameObjects[rightShiftObject]->moveLeftDown();
                                    lastRefillObjects.push_back(rightShiftObject);
                                    isRefillLock = true;
                                    break;
                                }
                            }
                        }
                        else if (gameField[j][i + 1] != CageCell && gameField[j][i + 1] != CageIceCell && gameField[j][i + 1] != LockCell &&
                                 gameField[j][i + 1] != MegaIceCell && gameField[j][i + 1] != StoneCell)
                        {
                            if (rightShiftObject >= 0 && gameField[j - 1][i + 1]  != CageCell && gameField[j - 1][i + 1]  != CageIceCell)
                            {
                                gameObjects[rightShiftObject]->moveLeftDown();
                                lastRefillObjects.push_back(rightShiftObject);
                                isRefillLock = true;
                                break;
                                
                            }
                        }
                    }
				}
			}
		}
	}

	if (isRefillLock)
	{
		refillFinished(NULL);
	}
	else
	{
        timesLock = false;
		this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(MOVE_DELTA/2.0f-0.1f), CCCallFuncN::create( this,
			callfuncN_selector(GameScene::destroyFinished))));
	}
}

void GameScene::refillFinished(CCNode* sender)
{
	refillObject();
	if (checkField())
	{
        afterDeleting();
	}
    else if (checkFieldLast())
    {
        afterDeleting();
    }
	else
	{
		timeAfterAction = 0.0f;
		isFindMatch = false;
	}
}

void GameScene::refillObjectForCrystalStripe(CCNode* sender)
{
//    if (background)
//        background->setVisible(true);
    int maxCountEmptyCell = 0;
    float deltaMove = 0.3f;
    float afterDeltaMove = 0.0f;
	for (int i = 0; i < columnCount; i++)
	{
        if (i == notRefill || i == notRefill - 1 || i == notRefill + 1)
            continue;
		int countEmptyCell = 0;
		bool flagAfter = false;
		for (int j = rowCount - 1; j >= 0; j--)
		{
			if (isSimpleCell(gameField[j][i]) && gameField[j][i] != CageCell && gameField[j][i] != CageIceCell)
			{
				int currObj = findGameObject(j, i);
				if (currObj >= 0)
				{
					int countShift = 0;
					int countEmpty = 0;
					for(int n = j + 1; n < rowCount; n++)
					{
						if (gameField[n][i] == NoneCell)
						{
							countEmpty++;
							continue;
						}
						if (isLockCell(gameField[n][i]))
							break;
                        
						if (findGameObject(n,i) < 0)
						{
							if (countEmpty > 0)
							{
								countShift+= countEmpty;
								countEmpty = 0;
							}
                            
							countShift++;
						}
					}
                    if (countShift == 1)
						deltaMove = 0.25f;
                    else if (countShift == 2)
						deltaMove = 0.3f;
					else if (countShift == 3)
						deltaMove = 0.35f;
					else if (countShift > 3 && countShift <= 6)
                        deltaMove = 0.5f;
                    else
                        deltaMove = 0.6f;
                    
					if (countShift > 0)
					{
						gameObjects[currObj]->sprite->runAction(CCSequence::create(CCDelayTime::create(afterDeltaMove), CCEaseOut::create(CCMoveBy::create(deltaMove, ccp(0.0f, - CELL_HEIGHT*countShift)), 0.5f),
                                                     CCMoveBy::create(0.1f, ccp(0.0f, 10.0f)), CCMoveBy::create(0.1f, ccp(0.0f, -10.0f)),NULL));
						gameObjects[currObj]->x += countShift;
                        
                        lastRefillObjects.push_back(currObj);
                        flagAfter = true;
					}
				}
				else
				{
					countEmptyCell++;
				}
			}
			else if (gameField[j][i] == NoneCell)
			{
				countEmptyCell++;
			}
		}
        
		if (countEmptyCell > maxCountEmptyCell)
			maxCountEmptyCell = countEmptyCell;
        
		int noneCellCount = 0;
		
		eColorGameObject prevColor = Colorurless;
		for (int j = 0; j < rowCount; j++)
		{
			if (isLockCell(gameField[j][i]))
				break;
            
			if (isSimpleCell(gameField[j][i])  && gameField[j][i] != CageCell  && gameField[j][i] != CageIceCell)
			{
				int currObj = findGameObject(j, i);
				if (currObj < 0)
				{
					eColorGameObject color = (eColorGameObject)(rand()%colorNumber);
					
					while (color == prevColor)
						color = (eColorGameObject)(rand()%colorNumber);
                    
					prevColor = color;
					sGameObject* gameObj = new sGameObject(Simple, color, j, i, xZero, yZero);
					if (isNeedCookie)
					{
						isNeedCookie = false;
						gameObj->changeColor(Colorurless);
                        if(menu->isStrawberry)
                            gameObj->isStrawberry = true;
                        else
                            gameObj->isStrawberry = false;
                        gameObj->changeType(Cookie);
					}
					fieldLayer->addChild(gameObj->node, 2);
					gameObjects.push_back(gameObj);
					
                    
                    int rowNormal = 0;
                    
                    for (rowNormal = 0; rowNormal < rowCount; rowNormal++)
                        if (gameField[rowNormal][i] != NoneCell && gameField[rowNormal][i] != LockCell)
                            break;
                    
					gameObj->sprite->setPosition(ccp(gameObj->sprite->getPositionX(), gameObj->sprite->getPositionY() + CELL_HEIGHT*(countEmptyCell + noneCellCount)));
                    if (IPHONE_5 || IPHONE_4)
                    {
                        if (LANDSCAPE)
                            gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[rowNormal][i]->getPositionY() + CELL_HEIGHT/2.0f));
                        else
                            gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[rowNormal][i]->getPositionY() + CELL_HEIGHT));
                    }
                    else
                    {
                        if (LANDSCAPE)
                            gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[rowNormal][i]->getPositionY()));
                        else
                            gameObj->node->setClippingRegion(CCRect(0.0f, 0.0f, WINSIZE.width, gameFieldSprites[rowNormal][i]->getPositionY() + CELL_HEIGHT/2.0f));
                    }
                    if (countEmptyCell + noneCellCount == 1)
						deltaMove = 0.25f;
                    else if (countEmptyCell + noneCellCount == 2)
						deltaMove = 0.3f;
					else if (countEmptyCell + noneCellCount == 3)
						deltaMove = 0.35f;
					else if (countEmptyCell + noneCellCount > 3 && countEmptyCell + noneCellCount <= 6)
                        deltaMove = 0.5f;
                    else
                        deltaMove = 0.6f;
                    
					gameObj->sprite->runAction(CCSequence::create(CCDelayTime::create(afterDeltaMove), CCEaseOut::create(CCMoveBy::create(deltaMove, ccp(0.0f, - CELL_HEIGHT*(countEmptyCell + noneCellCount))), 0.5f),
                                                                  CCMoveBy::create(0.1f, ccp(0.0f, 10.0f)), CCMoveBy::create(0.1f, ccp(0.0f, -10.0f)),NULL));
                    lastRefillObjects.push_back(gameObjects.size() - 1);
                    flagAfter = true;
				}
			}
		}
	}
    
    if (maxCountEmptyCell == 1)
        deltaMove = 0.25f;
    else if (maxCountEmptyCell == 2)
        deltaMove = 0.3f;
    else if (maxCountEmptyCell == 3)
        deltaMove = 0.35f;
    else if (maxCountEmptyCell > 3 && maxCountEmptyCell <= 6)
        deltaMove = 0.5f;
    else
        deltaMove = 0.6f;
    deltaMove = deltaMove + afterDeltaMove;
	SimpleAudioEngine::sharedEngine()->playEffect("sound/item_fall.mp3");
	this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(deltaMove-0.1f), CCCallFuncN::create( this,
                                                                                                              callfuncN_selector(GameScene::refillLockObjectForCrystalStripe))));
}

void GameScene::refillLockObjectForCrystalStripe(CCNode* sender)
{
    if (dropN > 8)
		dropN = 1;
	if (dropN < 1)
		dropN = 1;
	char buf[255];
	sprintf(buf, "sound/drop_%d.mp3", dropN++);
	SimpleAudioEngine::sharedEngine()->playEffect(buf);
	bool isRefillLock = false;
	for (int i = 0; i < columnCount; i++)
	{
		bool isLockStripe = false;
		for (int j = 0; j < rowCount; j++)
		{
			if (isLockCell(gameField[j][i]))
			{
				isLockStripe = true;
				continue;
			}
			if (isLockStripe)
			{
				if (isSimpleCell(gameField[j][i]) && gameField[j][i] != CageCell && gameField[j][i] != CageIceCell)
				{
					int currObj = findGameObject(j, i);
					if (currObj < 0)
					{
						int leftShiftObject = findGameObject(j - 1, i - 1);
						int rightShiftObject = findGameObject(j - 1, i + 1);
                        
						if (leftShiftObject >= 0 && gameField[j - 1][i - 1] != CageCell && gameField[j - 1][i - 1] != CageIceCell)
						{
							gameObjects[leftShiftObject]->moveRightDown();
                            lastRefillObjects.push_back(leftShiftObject);
							isRefillLock = true;
							break;
						}
						else if (rightShiftObject >= 0 && gameField[j - 1][i + 1]  != CageCell && gameField[j - 1][i + 1]  != CageIceCell)
						{
							gameObjects[rightShiftObject]->moveLeftDown();
                            lastRefillObjects.push_back(rightShiftObject);
							isRefillLock = true;
							break;
                            
						}
					}
				}
			}
		}
	}
    
	if (isRefillLock)
	{
		this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(MOVE_DELTA-0.1f), CCCallFuncN::create( this,
                                                                                                                   callfuncN_selector(GameScene::refillObjectForCrystalStripe))));
	}
}

void GameScene::help(CCNode* sender)
{
}



void GameScene::update(float delta)
{
	helpTime+=delta;
    if (helpTime >= 3.0f)
    {
        if (!lock)
        {
            if (!findMatch())
            {
                if (countReinitField)
                {
                    if (gameType != Time || gameType != Score)
                    {
                        if (menu->getCurrentScore() >= menu->getTargetScore())
                            this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameScene::win)));
                        else
                            this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameScene::lose)));
                    }
                    else
                    {
                        this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameScene::lose)));
                    }
                }
                
                reInitField();
                countReinitField = true;
            }
        }
        helpTime = 1.0f;
    }
}

bool GameScene::addChocolate()
{
    vector<CCPoint> chocolats;
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < columnCount; j++)
        {
            if (gameField[i][j] == Chocolate)
            {
                chocolats.push_back(ccp(i, j));
            }
        }
    }
    
    if (!chocolats.empty())
    {
        for (int i = 0; i < chocolats.size(); i++)
        {
            int x = chocolats[i].x;
            int y = chocolats[i].y;
            if (x > 0 && (gameField[x-1][y] == SimpleCell || gameField[x-1][y] == IceCell || gameField[x-1][y] == SuperIceCell)
                && (((findGameObject(x-1, y) >= 0 && gameObjects[findGameObject(x-1, y)]->type != Cookie))))
            {
                x = x - 1;
                y = y;
                gameField[x][y] = Chocolate;
                CCSprite* ice = CCSprite::createWithSpriteFrameName("game/chokolad.png");
				ice->setPosition(ccp(gameFieldSprites[x][y]->getContentSize().width/2.0f, gameFieldSprites[x][y]->getContentSize().height/2.0f));
				ice->setOpacity(0);
				ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f), CCFadeIn::create(0.1f)));
                
                if (IPAD_MINI)
                    ice->setScale(0.93f);
                else if (IPHONE_4 || IPHONE_5)
                    ice->setScale(0.85f);
                
				gameFieldSprites[x][y]->addChild(ice, 10);
                ice->setTag(4);
                
                if (gameFieldSprites[x][y]->getChildByTag(1))
                    gameFieldSprites[x][y]->getChildByTag(1)->removeFromParentAndCleanup(true);
                
                if (findGameObject(x, y) < 0)
                    return true;
                
                for (int j = 0; j < gameObjects[findGameObject(x, y)]->sprite->getChildrenCount(); j++)
                    ((CCSprite*)gameObjects[findGameObject(x, y)]->sprite->getChildren()->objectAtIndex(j))->removeFromParentAndCleanup(true);
                
                gameObjects[findGameObject(x, y)]->sprite->runAction(CCSequence::create(CCFadeOut::create(0.2f), CCHide::create(), CCCallFuncN::create(this, callfuncN_selector(GameScene::chocolateDeleteElement)), NULL));
                
                gameObjects.erase(gameObjects.begin() + findGameObject(x, y));
                return true;
            }
            
            if (x < rowCount - 1 && (gameField[x+1][y] == SimpleCell || gameField[x+1][y] == IceCell || gameField[x+1][y] == SuperIceCell)
                && (((findGameObject(x+1, y) >= 0 && gameObjects[findGameObject(x+1, y)]->type != Cookie))))
            {
                x = x + 1;
                y = y;
                gameField[x][y] = Chocolate;
                CCSprite* ice = CCSprite::createWithSpriteFrameName("game/chokolad.png");
				ice->setPosition(ccp(gameFieldSprites[x][y]->getContentSize().width/2.0f, gameFieldSprites[x][y]->getContentSize().height/2.0f));
				ice->setOpacity(0);
                if (IPAD_MINI)
                    ice->setScale(0.93f);
                else if (IPHONE_4 || IPHONE_5)
                    ice->setScale(0.85f);
                
				ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f), CCFadeIn::create(0.1f)));
				gameFieldSprites[x][y]->addChild(ice, 10);
                ice->setTag(4);
                
                if (gameFieldSprites[x][y]->getChildByTag(1))
                    gameFieldSprites[x][y]->getChildByTag(1)->removeFromParentAndCleanup(true);
                
                if (findGameObject(x, y) < 0)
                    return true;
                    
                for (int j = 0; j < gameObjects[findGameObject(x, y)]->sprite->getChildrenCount(); j++)
                    ((CCSprite*)gameObjects[findGameObject(x, y)]->sprite->getChildren()->objectAtIndex(j))->removeFromParentAndCleanup(true);
                
                gameObjects[findGameObject(x, y)]->sprite->runAction(CCSequence::create(CCFadeOut::create(0.2f), CCHide::create(), CCCallFuncN::create(this, callfuncN_selector(GameScene::chocolateDeleteElement)), NULL));
                
                gameObjects.erase(gameObjects.begin() + findGameObject(x, y));
                return true;
            }
            
            if (y > 0 && (gameField[x][y - 1] == SimpleCell || gameField[x][y - 1] == IceCell || gameField[x][y - 1] == SuperIceCell)
                && (((findGameObject(x, y - 1) >= 0 && gameObjects[findGameObject(x, y - 1)]->type != Cookie))))
            {
                x = x;
                y = y - 1;
                gameField[x][y] = Chocolate;
                CCSprite* ice = CCSprite::createWithSpriteFrameName("game/chokolad.png");
				ice->setPosition(ccp(gameFieldSprites[x][y]->getContentSize().width/2.0f, gameFieldSprites[x][y]->getContentSize().height/2.0f));
				ice->setOpacity(0);
                if (IPAD_MINI)
                    ice->setScale(0.93f);
                else if (IPHONE_4 || IPHONE_5)
                    ice->setScale(0.85f);
				ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f), CCFadeIn::create(0.1f)));
				gameFieldSprites[x][y]->addChild(ice, 10);
                ice->setTag(4);
                
                if (gameFieldSprites[x][y]->getChildByTag(1))
                    gameFieldSprites[x][y]->getChildByTag(1)->removeFromParentAndCleanup(true);
                
                if (findGameObject(x, y) < 0)
                    return true;
                
                for (int j = 0; j < gameObjects[findGameObject(x, y)]->sprite->getChildrenCount(); j++)
                    ((CCSprite*)gameObjects[findGameObject(x, y)]->sprite->getChildren()->objectAtIndex(j))->removeFromParentAndCleanup(true);
                
                gameObjects[findGameObject(x, y)]->sprite->runAction(CCSequence::create(CCFadeOut::create(0.2f), CCHide::create(), CCCallFuncN::create(this, callfuncN_selector(GameScene::chocolateDeleteElement)), NULL));
                
                gameObjects.erase(gameObjects.begin() + findGameObject(x, y));
                return true;
            }
            
            if (y < columnCount - 1 && (gameField[x][y + 1] == SimpleCell || gameField[x][y + 1] == IceCell || gameField[x][y + 1] == SuperIceCell)
                && (((findGameObject(x, y + 1) >= 0 && gameObjects[findGameObject(x, y + 1)]->type != Cookie))))
            {
                x = x;
                y = y + 1;
                gameField[x][y] = Chocolate;
                CCSprite* ice = CCSprite::createWithSpriteFrameName("game/chokolad.png");
				ice->setPosition(ccp(gameFieldSprites[x][y]->getContentSize().width/2.0f, gameFieldSprites[x][y]->getContentSize().height/2.0f));
				ice->setOpacity(0);
                if (IPAD_MINI)
                    ice->setScale(0.93f);
                else if (IPHONE_4 || IPHONE_5)
                    ice->setScale(0.85f);
				ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f), CCFadeIn::create(0.1f)));
				gameFieldSprites[x][y]->addChild(ice, 10);
                ice->setTag(4);
                
                if (gameFieldSprites[x][y]->getChildByTag(1))
                    gameFieldSprites[x][y]->getChildByTag(1)->removeFromParentAndCleanup(true);
                
                if (findGameObject(x, y) < 0)
                    return true;

                for (int j = 0; j < gameObjects[findGameObject(x, y)]->sprite->getChildrenCount(); j++)
                    ((CCSprite*)gameObjects[findGameObject(x, y)]->sprite->getChildren()->objectAtIndex(j))->removeFromParentAndCleanup(true);
                
                gameObjects[findGameObject(x, y)]->sprite->runAction(CCSequence::create(CCFadeOut::create(0.2f), CCHide::create(), CCCallFuncN::create(this, callfuncN_selector(GameScene::chocolateDeleteElement)), NULL));
                
                gameObjects.erase(gameObjects.begin() + findGameObject(x, y));
                return true;
            }
        }
    }
    
    return false;
}
                                                                     
void GameScene::chocolateDeleteElement(CCNode* sender)
{
    ((CCSprite*)sender)->removeFromParentAndCleanup(true);
}

void GameScene::nextStripe(CCObject* sender)
{
    sGameObject* currObj = ((sGameObject*)sender);
    int currNext = ++nextCrystalStripe;
    
    if (findGameObject(((sGameObject*)sender)->x, ((sGameObject*)sender)->y) >= 0)
    {
        if (gameObjects[findGameObject(((sGameObject*)sender)->x, ((sGameObject*)sender)->y)]->type == Horizontal ||
            gameObjects[findGameObject(((sGameObject*)sender)->x, ((sGameObject*)sender)->y)]->type == Vertical ||
            gameObjects[findGameObject(((sGameObject*)sender)->x, ((sGameObject*)sender)->y)]->type == Bomb)
        {
            addToDeleting(findGameObject(((sGameObject*)sender)->x, ((sGameObject*)sender)->y));
            afterDeletingForCrystalStripe(currObj);
        }
    }
    
    if (currNext >= nextDead.size())
        currNext = -1;
    
    if (currNext >= 0)
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.5f), CCCallFuncO::create(this, callfuncO_selector(GameScene::nextStripe), nextDead[currNext])));
    else
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncO::create(this, callfuncO_selector(GameScene::afterStripeCrystal), NULL)));
}

void GameScene::afterStripeCrystal(CCObject* sender)
{
    isCrystalStripe = false;
    nextDead.clear();
    destroyFinished(NULL);
    nextCrystalStripe = 0;
}

void GameScene::destroyFinished(CCNode* sender)
{
    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.1f), CCCallFuncN::create(this, callfuncN_selector(GameScene::visibleBacground))));
    firstAction = true;
    if (isCrystalStripe && !nextDead.empty())
    {
        float timeNext = 0.0f;
        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.0f), CCCallFuncO::create(this, callfuncO_selector(GameScene::nextStripe), nextDead[nextCrystalStripe])));
        return;
    }
    if (!nextDead.empty())
    {
        for (int i = 0; i < nextDead.size(); i++)
        {
            if (isSpecialElement(nextDead[i]))
            {
                bool flag = false;
                for (int n = 0; n < gameObjects.size(); n++)
                {
                    if (gameObjects[n] == nextDead[i])
                        flag = true;
                }
                if (flag)
                {
                    addToDeleting(findGameObject(nextDead[i]->x, nextDead[i]->y));
                    int colorsCount[Colorurless];
                    for (int n = 0; n < gameObjects.size(); n++)
                        colorsCount[gameObjects[n]->color]++;
                    
                    eColorGameObject maxIdx = (eColorGameObject)0;
                    for (int n = 0; n < Colorurless; n++)
                        if (colorsCount[n] > colorsCount[maxIdx])
                            maxIdx = (eColorGameObject)n;
                        
                    if (nextDead[i]->type == Crystal)
                    {
                        for (int n = 0; n < rowCount; n++)
                        {
                            for (int j = 0; j < columnCount; j++)
                            {
                                    if (findGameObject(n, j) >= 0)
                                    {
                                        if (gameObjects[findGameObject(n, j)]->color == maxIdx)
                                        {
                                            addToDeleting(findGameObject(n, j));
                                            CCPoint o1 = nextDead[i]->sprite->getPosition();
                                            CCPoint o2 = gameObjects[findGameObject(n, j)]->sprite->getPosition();
                                            lighting(o1, o2);
                                        }
                                    }
                            }
                        }
                    }
                    
                    nextDead.erase(nextDead.begin());
                    afterDeleting();
                    return;
                }
            }
        }
    }

	if (checkField())
	{
        if (isFishNeedRun)
        {
            runFish(gameObjects[rand()%gameObjects.size()], 2.5f, 0.3f);
            runFish(gameObjects[rand()%gameObjects.size()], 2.5f, 0.6f);
            runFish(gameObjects[rand()%gameObjects.size()], 2.5f, 0.9f);
            runFish(gameObjects[rand()%gameObjects.size()], 2.5f, 1.2f);
            menu->isBannerLock = true;
            sweetCount = 5;
            lock = true;
            this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(4.0f), CCCallFuncN::create(this, callfuncN_selector(GameScene::fishingEnded))));
            isRunFish = true;
            isFishNeedRun = false;
            return;
        }
        else
            afterDeleting();
	}
	else if (checkFieldLast())
    {
        afterDeleting();
    }
    else
	{
        timesLock = false;
        if (isFishNeedRun)
        {
            runFish(gameObjects[rand()%gameObjects.size()], 2.5f, 0.3f);
            runFish(gameObjects[rand()%gameObjects.size()], 2.5f, 0.6f);
            runFish(gameObjects[rand()%gameObjects.size()], 2.5f, 0.9f);
            runFish(gameObjects[rand()%gameObjects.size()], 2.5f, 1.2f);
            menu->isBannerLock = true;
            sweetCount = 5;
            lock = true;
            this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(4.0f), CCCallFuncN::create(this, callfuncN_selector(GameScene::fishingEnded))));
            isRunFish = true;
            isFishNeedRun = false;
            return;
        }
        timesLock = false;
        countReinitField = false;
        cleaningTrash(NULL);
        
        if (!isChocolateDestroyed)
        {
            addChocolate();
            isChocolateDestroyed = true;
        }
        
        helpTime = 2.0f;
        if (menu->getCurrentLevel() == 1 && !isSecondTutorial)
        {
            isSecondTutorial = true;
            setTutorial_2();
        }
        if (menu->getCurrentLevel() == 2 && !isSecondTutorial)
        {
            isSecondTutorial = true;
            setTutorial_2_2();
        }
        if (menu->getCurrentLevel() == 3 && !isSecondTutorial)
        {
            isSecondTutorial = true;
            setTutorial_3_2();
        }
        if (menu->getCurrentLevel() == 5 && !isSecondTutorial)
        {
            isSecondTutorial = true;
            setTutorial_5_2();
        }
        
        lock = false;
        if (isEnd)
            return;
        
		if (sweetCount == 3)
		{
			menu->sweet();
		}
		else if (sweetCount == 4)
		{
			menu->tasty();
		}
		else if (sweetCount == 5)
		{
			menu->divine();
		}
		else if (sweetCount >= 6)
		{
			menu->delicious();
		}
		sweetCount = 0;
        
        endConditionCheck();
        
		timeAfterAction = 0.0f;
		isFindMatch = false;
        
        firstFind = false;
	}
}

bool GameScene::endConditionCheck()
{
    if (gameType != Time)
    {
        bool countMoves = (menu->getCountMoves() <= 0);
        bool additionalCondition = false;
        
        bool winCondition = false;
        switch (gameType) {
            case Score:
                additionalCondition = false;
                winCondition = menu->getCurrentScore() >= menu->getTargetScore();
                break;
            case Ice:
                additionalCondition = (iceCount <= 0);
                winCondition = (menu->getCurrentScore() >= menu->getTargetScore())&&(iceCount <=0);
                break;
            case BringDown:
                additionalCondition = (menu->getBringDownCurrent() >= menu->getBringDownTarget());
                winCondition = (menu->getCurrentScore() >= menu->getTargetScore())&&(menu->getBringDownCurrent() >= menu->getBringDownTarget());
                break;
        }
        if ((countMoves || additionalCondition)&&!isRunFish)
        {
            isFinalAction = true;
            
            leftDownMenu->hide();
            
            bool isSugarCrash = true;
            
            for (int i = 0; i < gameObjects.size(); i++)
                if (gameObjects[i]->type == Vertical || gameObjects[i]->type == Horizontal || gameObjects[i]->type == Bomb || gameObjects[i]->type == Crystal || gameObjects[i]->type == Fish)
                {
                    isSugarCrash = false;
                }
            
            if (!isSugarCrash && winCondition)
            {
                sugarCrash(NULL);
            }
            else if (!countMoves && !isNotElementFish)
            {
                //isNotElementFish = true;
                menu->booster_1_Button->setEnabled(false);
                menu->booster_2_Button->setEnabled(false);
                menu->booster_3_Button->setEnabled(false);
                menu->isLastAction = true;

                if (!isRunFish)
                {
                    isRunFish = true;
                    int count = 0;
                    float time = 0.0f;
                    int movesFish = menu->getCountMoves();
                    if (movesFish > 0)
                    {
                        SimpleAudioEngine::sharedEngine()->playEffect("sound/penguins_loop.mp3");
                        int countObjectCur = 0;
                        float stepCount = 0;
                        for (int n = 0; n < columnCount; n++)
                        {
                            for (int k = 0; k < rowCount; k++)
                            {
                                if (findGameObject(k, n) >= 0)
                                    countObjectCur++;
                            }
                        }
                        stepCount = (float)countObjectCur/(float)movesFish;
                        float stepCountCurr = 0;
                        vector<int> fgObject;
                        for (int n = 0; n < columnCount; n++)
                        {
                            for (int k = 0; k < rowCount; k++)
                            {
                                if (findGameObject(k, n) >= 0)
                                {
                                    stepCountCurr+=1.0f;
                                    if (stepCountCurr >= stepCount)
                                    {
                                        stepCountCurr -= stepCount;
                                        runFish(gameObjects[findGameObject(k, n)], 2.4f, time);
                                        fgObject.push_back(findGameObject(k, n));
                                        time += 0.1f;
                                        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(time), CCCallFuncN::create(this, callfuncN_selector(GameScene::decrementMovesEnded))));
                                        count++;
                                        movesFish--;
                                        if ((count > gameObjects.size() - 1) || movesFish <= 0)
                                        {
                                            this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(time + 2.4f), CCCallFuncN::create(this, callfuncN_selector(GameScene::fishingEnded))));
                                            return true;
                                        }
                                    }
                                }
                            }
                        }
                        if (movesFish > 0)
                        {
                            for (;;)
                            {
                                int idxFish = rand()%(gameObjects.size());
                                bool flag = true;
                                
                                for (int o = 0; o < fgObject.size(); o++)
                                    if (fgObject[o] == idxFish)
                                        flag = false;
                                
                                if (flag)
                                {
                                    runFish(gameObjects[idxFish], 2.4f, time);
                                    fgObject.push_back(idxFish);
                                    time += 0.1f;
                                    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(time), CCCallFuncN::create(this, callfuncN_selector(GameScene::decrementMovesEnded))));
                                    count++;
                                    movesFish--;
                                    if ((count > gameObjects.size() - 1) || movesFish <= 0)
                                    {
                                        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(time + 2.4f), CCCallFuncN::create(this, callfuncN_selector(GameScene::fishingEnded))));
                                        return true;
                                    }
                                }
                            }
                        }
                        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(time + 4.5f), CCCallFuncN::create(this, callfuncN_selector(GameScene::fishingEnded))));
                    }
                }
            }
            else
            {
                isEndDialog = true;
                if (winCondition)
                {
                    this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameScene::win)));
                    isEnd = true;
                }
                else
                {
                    if (!isLoseDialog)
                    {
                        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(GameScene::popupExitWithDelay))));
                    }
                    else
                    {
                        isEnd = true;
                        this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameScene::lose)));
                    }
                }
            }
        }
    }
   
    else
    {
        if (!menu->isTime())
        {
            isEndDialog = true;
            if (menu->getCurrentScore() >= menu->getTargetScore())
            {
                this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameScene::win)));
                isEnd = true;
            }
            else
            {
                if (!isLoseDialog)
                {
                    this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(1.0f), CCCallFuncN::create(this, callfuncN_selector(GameScene::popupExitWithDelay))));
                }
                else
                {
                    isEnd = true;
                    this->runAction(CCCallFuncN::create(this, callfuncN_selector(GameScene::lose)));
                }
            }
        }
    }
}

void GameScene::afterGetScores()
{
    endGameMenu->afterGetScores();
}

void GameScene::unlockInvite()
{
    leftDownMenu->setLock(false);
    leftDownMenu->setVisible(true);
}

void GameScene::updateMenu(CCNode* sender)
{
    if (leftDownMenu->isInvite)
    {
        leftDownMenu->setLock(true);
        leftDownMenu->setVisible(false);
        leftDownMenu->isInvite = false;
        menu->inviteFriend();
    }
    if (leftDownMenu->isLock())
    {
        timesLeftDownLock = true;
        leftDownMenu->setLock(true);
        dark->setVisible(true);
        dark->runAction(CCFadeTo::create(0.5f, 155));
        menu->booster_1_Button->setEnabled(false);
        menu->booster_2_Button->setEnabled(false);
        menu->booster_3_Button->setEnabled(false);
        menu->snow->setEnabled(false);
        
        if (leftDownMenu->exitPress)
        {
            if (firstAction)
            {
                leftDownMenu->exitPress = false;
                leftDownMenu->setVisible(false);
                menu->popupExit(iceTarget, iceCount);
            }
            else
            {
                endGameMenu->setVisible(true);
                endGameMenu->closeAfterLoading(NULL);
                GlobalsPtr->booster_1 = false;
                GlobalsPtr->booster_2 = false;
                GlobalsPtr->booster_3 = false;
            }
        }
    }
    else if (!leftDownMenu->isLock())
    {
        timesLeftDownLock = false;
        leftDownMenu->setLock(false);
        dark->runAction(CCFadeTo::create(0.5f, 0));
        if (!menu->isDialog && !menu->isLastAction)
        {
            menu->booster_1_Button->setEnabled(true);
            menu->booster_2_Button->setEnabled(true);
            menu->booster_3_Button->setEnabled(true);
            menu->snow->setEnabled(true);
        }
    }
    this->runAction(CCSequence::create(CCDelayTime::create(0.3f), CCCallFuncN::create(this, callfuncN_selector(GameScene::updateMenu)), NULL));
}

void GameScene::popupExitWithDelay(CCNode* sender)
{
    menu->booster_1_Button->setEnabled(false);
    menu->booster_2_Button->setEnabled(false);
    menu->booster_3_Button->setEnabled(false);
    menu->popupExit(iceTarget, iceCount);
}

void GameScene::decrementMovesEnded(CCNode* sender)
{
    if (menu->getCountMoves() > 0)
        menu->setCountMoves(menu->getCountMoves() - 1);
}

void GameScene::fishingEnded(CCNode* sender)
{
    fishes.clear();
    isRunFish = false;
    afterDeleting();
    menu->isBannerLock = false;
    isFish = false;
    if(isNotElementFish)
        menu->setCountMoves(0);
}

bool GameScene::checkFieldFirst()
{
	vector<int> checkedElements;
	for (int idx = 0; idx < gameObjects.size(); idx++)
	{
		bool result = false;
		sGameObject* currObj = gameObjects[idx];

		eColorGameObject currentColor = gameObjects[idx]->color;

		int x = gameObjects[idx]->x;
		int y = gameObjects[idx]->y;

		vector<int> horizontalChain;
		vector<int> verticalChain;

		horizontalChain.clear();
		verticalChain.clear();

		for (int i = x; i < rowCount; i++)
		{
			if (!isSimpleCell(gameField[i][y]))
				break;

			bool flagCheck = false;
			for (int n = 0; n < checkedElements.size(); n++)
			{
				if (checkedElements[n] == findGameObject(i, y))
					flagCheck = true;
			}
			if (flagCheck)
				break;

			int tempEl = findGameObject(i, y);

			if (tempEl >= 0)
			{
				if (gameObjects[findGameObject(i, y)]->color == currentColor)
				{
					if (i != x)
						horizontalChain.push_back(findGameObject(i, y));
				}
				else
					break;
			}
			else
				break;
		}	

		for (int i = x; i >= 0; i--)
		{
			if (!isSimpleCell(gameField[i][y]))
				break;

			bool flagCheck = false;
			for (int n = 0; n < checkedElements.size(); n++)
			{
				if (checkedElements[n] == findGameObject(i, y))
					flagCheck = true;
			}
			if (flagCheck)
				break;

			int tempEl = findGameObject(i, y);

			if (tempEl > 0)
			{
				if (gameObjects[findGameObject(i, y)]->color == currentColor)
				{
					if (i != x)
						horizontalChain.push_back(findGameObject(i, y));	
				}
				else
					break;
			}			
			else
				break;
		}

		for (int i = y; i < columnCount; i++)
		{
			if (!isSimpleCell(gameField[x][i]))
				break;

			bool flagCheck = false;
			for (int n = 0; n < checkedElements.size(); n++)
			{
				if (checkedElements[n] == findGameObject(x, i))
					flagCheck = true;
			}
			if (flagCheck)
				break;

			int tempEl = findGameObject(x, i);

			if (tempEl >= 0)
			{
				if (gameObjects[findGameObject(x, i)]->color == currentColor)
				{
					if (i != y)
						verticalChain.push_back(findGameObject(x, i));	
				}
				else
					break;
			}			
			else
				break;
		}

		for (int i = y; i >= 0; i--)
		{
			if (!isSimpleCell(gameField[x][i]))
				break;

			bool flagCheck = false;
			for (int n = 0; n < checkedElements.size(); n++)
			{
				if (checkedElements[n] == findGameObject(x, i))
					flagCheck = true;
			}
			if (flagCheck)
				break;

			int tempEl = findGameObject(x, i);

			if (tempEl >= 0)
			{
				if (gameObjects[findGameObject(x, i)]->color == currentColor)
				{			
					if (i != y)
						verticalChain.push_back(findGameObject(x, i));
				}
				else
					break;
			}			
			else
				break;
		}	

		if (horizontalChain.size() < 2)
		{
			horizontalChain.clear();
		}
		else
		{
			result = true;
			for (int i = 0; i < horizontalChain.size(); i++)
				addToDeleting(horizontalChain[i]);
		}

		if (verticalChain.size() < 2)
		{
			verticalChain.clear();
		}
		else
		{
			result = true;
			for (int i = 0; i < verticalChain.size(); i++)
				addToDeleting(verticalChain[i]);
		}

		if (result)
		{		
			addToDeleting(idx);
		}
	}

	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->type == Cookie)
			if (gameObjects[i]->x == (rowCount - 1))
				allDeletedChain.push_back(i);
	}

	return !allDeletedChain.empty();
}

bool GameScene::checkFieldLast()
{
    lock = true;
	vector<int> checkedElements;
    vector<int> superCreations;
    bool flagBreak = false;
    

    for (int i = 0; i < gameObjects.size(); i++)
    {
        checkElement(i);
    }
    
	return !allDeletedChain.empty();
}

bool GameScene::checkFieldBringDown()
{
    bool result = false;
    for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->type == Cookie)
        {
			if (gameObjects[i]->x == (rowCount - 1))
			{
                result = true;
				allDeletedChain.push_back(i);
				menu->setBringDownCurrent(menu->getBringDownCurrent() + 1);
                int countCookie = 0;
                for (int n = 0; n < gameObjects.size(); n++)
                {
                    if (gameObjects[n]->type == Cookie && gameObjects[i] != gameObjects[n])
                        countCookie++;
                }
				if (menu->getBringDownCurrent() + countCookie < menu->getBringDownTarget())
					isNeedCookie = true;
			}
            else
            {
                bool flag = false;
                for (int n = gameObjects[i]->x + 1; n < rowCount; n++)
                {
                    if (!isNoneCell(n, gameObjects[i]->y))
                        flag = true;
                }
                if (!flag)
                {
                    result = true;
                    allDeletedChain.push_back(i);
                    menu->setBringDownCurrent(menu->getBringDownCurrent() + 1);
                    int countCookie = 0;
                    for (int n = 0; n < gameObjects.size(); n++)
                    {
                        if (gameObjects[n]->type == Cookie)
                            countCookie++;
                    }
                    if (menu->getBringDownCurrent() + countCookie < menu->getBringDownTarget())
                        isNeedCookie = true;
                }
            }
        }
	}
    return result;
}

bool GameScene::checkField()
{
	lock = true;
	vector<int> checkedElements;
    vector<int> superCreations;
    bool flagBreak = false;
    
    for (int i = 0; i < lastRefillObjects.size(); i++)
    {
        bool isRepeat = false;
        
        for (int j = i; j >= 0; j--)
            if (j != i)
                if (lastRefillObjects[i] == lastRefillObjects[j])
                    isRepeat = true;
        
        if (isRepeat)
        {
            lastRefillObjects.erase(lastRefillObjects.begin() + i);
            i--;
        }
    }
    
    for (int i = 0; i < lastRefillObjects.size(); i++)
    {
        bool flagCheck = true;
        for (int j = 0; j < allDeletedChain.size(); j++)
        {
            if (allDeletedChain[j] == lastRefillObjects[i])
                flagCheck = false;
        }
        if (flagCheck)
            checkElement(lastRefillObjects[i]);
    }
    

    if (isTeleportedObject)
    {
        isTeleportedObject = false;
        for (int i = 0; i < gameObjects.size(); i++)
        {
            bool flagCheck = true;
            for (int j = 0; j < allDeletedChain.size(); j++)
            {
                if (allDeletedChain[j] == i)
                    flagCheck = false;
            }
            if (flagCheck)
                checkElement(i);
        }
    }
    
    lastRefillObjects.clear();
    
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->type == Cookie)
        {
			if (gameObjects[i]->x == (rowCount - 1))
			{
				allDeletedChain.push_back(i);
				menu->setBringDownCurrent(menu->getBringDownCurrent() + 1);
                int countCookie = 0;
                for (int n = 0; n < gameObjects.size(); n++)
                {
                    if (gameObjects[n]->type == Cookie && gameObjects[i] != gameObjects[n])
                        countCookie++;
                }
				if (menu->getBringDownCurrent() + countCookie < menu->getBringDownTarget())
					isNeedCookie = true;
			}
            else
            {
                bool flag = false;
                for (int n = gameObjects[i]->x + 1; n < rowCount; n++)
                {
                    if (!isNoneCell(n, gameObjects[i]->y))
                        flag = true;
                }
                if (!flag)
                {
                    allDeletedChain.push_back(i);
                    menu->setBringDownCurrent(menu->getBringDownCurrent() + 1);
                    int countCookie = 0;
                    for (int n = 0; n < gameObjects.size(); n++)
                    {
                        if (gameObjects[n]->type == Cookie)
                            countCookie++;
                    }
                    if (menu->getBringDownCurrent() + countCookie < menu->getBringDownTarget())
                        isNeedCookie = true;
                }
            }
        }
	}
    
    if (gameType == BringDown && !isNeedCookie && menu->getBringDownCurrent()< menu->getBringDownTarget())
    {
        bool flag = false;
        int countCookie = 0;
        for (int i = 0; i < gameObjects.size(); i++)
        {
            if (gameObjects[i]->type == Cookie)
            {
                flag = true;
                countCookie++;
            }
        }
        
        if (menu->getBringDownCurrent() + countCookie >= menu->getBringDownTarget())
            flag = true;
        if (!flag)
            isNeedCookie = true;
        
        if (((beganCountMoves - menu->getCountMoves()) >= (beganCountMoves / menu->getBringDownTarget())*(menu->getBringDownCurrent() + countCookie)) &&
            (menu->getBringDownCurrent() + countCookie < menu->getBringDownTarget()))
        {
            isNeedCookie = true;
        }
    }
    
    for (int i = 0; i < superCreations.size(); i++)
    {
        for (int j = 0; j < allDeletedChain.size();)
        {
            if (allDeletedChain[j] == superCreations[i])
            {
                allDeletedChain.erase(allDeletedChain.begin() + j);
            }
            else
            {
                j++;
            }
        }
    }
    
    superCreations.clear();
    
    if (crystalChangeType == Horizontal)
    {
        for (int i = 0; i < gameObjects.size(); i++)
        {
            if (gameObjects[i]->type == Horizontal)
                addToDeleting(i);
        }
        crystalChangeType = Simple;
    }
    
    if (crystalChangeType == Vertical)
    {
        for (int i = 0; i < gameObjects.size(); i++)
        {
            if (gameObjects[i]->type == Vertical)
                addToDeleting(i);
        }
        crystalChangeType = Simple;
    }

	return !allDeletedChain.empty();
}

void GameScene::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, 1);
}

void GameScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
    if (endGameMenu != NULL)
        endGameMenu->ccTouchesBegan(NULL, NULL);
    
    if (menu)
    {
        menu->touch();
        if (isFinalBanner)
        {
            if (isWinSpeed)
            {
                this->stopAction(endAction);
                this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f),
                                                                     CCCallFuncN::create(this, callfuncN_selector(GameScene::winCallback))));
            }
            else if (isLoseSpeed)
            {
                this->stopAction(endAction);
                this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(0.3f),
                                                                 CCCallFuncN::create(this, callfuncN_selector(GameScene::loseCallback))));
            }
        }
    }
    
	if (!menu->getCountMoves())
		return;

    if (menu->isLock())
		return;

	if (lock)
		return;
    
    if (isFish)
		return;
    
    if (!menu->isTime())
        return;

	CCTouch* touch = (CCTouch*)( pTouches->anyObject());
	CCPoint location = touch->getLocation();
    
    if (LANDSCAPE)
    {
        location.x = location.x - CELL_WIDTH*2.0f;
        location.y = location.y + CELL_HEIGHT/2.0f;
    }
    
    location = this->convertToNodeSpace(location);
    beginPosition = location;

    
	for (int i  = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->isLock)
			return;

		if (gameObjects[i]->sprite->boundingBox().containsPoint(location))
		{
            if (isTutorial)
            {
                bool flag = false;
                for (int j = 0; j < tutorialCell.size(); j++)
                {
                    if (gameObjects[i] == gameObjects[findGameObject(tutorialCell[j].first.x, tutorialCell[j].first.y)])
                    {
                        flag = true;
                    }
                }
                if (!flag)
                    return;
            }
            
            if (gameField[gameObjects[i]->x][gameObjects[i]->y] == CageCell || gameField[gameObjects[i]->x][gameObjects[i]->y] == CageIceCell)
                return;
            
            if (menu->isClear && gameObjects[i]->type != Cookie)
            {
                menu->isClear = false;
                gameObjects[i]->isScore = true;
                if (gameObjects[i]->type == Crystal)
                {
                    for (int ix = 0; ix < rowCount; ix++)
                    {
                        for (int j = 0; j < columnCount; j++)
                        {
                            if (isSimpleCell(gameField[ix][j]))
                                if (findGameObject(ix, j) >= 0)
                                    if (gameObjects[findGameObject(ix, j)]->color == (eColorGameObject)(rand()%6))
                                    {
                                        gameObjects[findGameObject(ix, j)]->isScore = true;
                                        addToDeleting(findGameObject(ix, j));
                                        lighting(gameObjects[i]->sprite->getPosition(),
                                                 gameObjects[findGameObject(ix, j)]->sprite->getPosition());
                                        lighting(gameObjects[i]->sprite->getPosition(),
                                                 gameObjects[findGameObject(ix, j)]->sprite->getPosition());
                                    }
                        }
                    }
                }
                addToDeleting(i);
                afterDeleting();
                return;
            }
			if (idxSelectedObject < 0)
			{
				if (!gameObjects[i]->isSelected)
				{
					gameObjects[i]->sprite->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.3f, 1.0f, 0.7f), CCScaleTo::create(0.3f, ELEMENT_SCALE)));
					gameObjects[i]->isSelected = true;
					idxSelectedObject = i;
				}
				else {
					gameObjects[i]->sprite->runAction(CCScaleTo::create(0.3f, ELEMENT_SCALE));
					gameObjects[i]->sprite->setRotation(0.0f);
					gameObjects[i]->isSelected = false;
					idxSelectedObject = -1;
				}
			}
			else
			{
				if (i != idxSelectedObject)
				{
					gameObjects[idxSelectedObject]->sprite->runAction(CCScaleTo::create(0.3f, ELEMENT_SCALE));
					gameObjects[idxSelectedObject]->sprite->setRotation(0.0f);
					gameObjects[idxSelectedObject]->isSelected = false;
 
                    beginPosition = gameObjects[idxSelectedObject]->sprite->getPosition();
                    location = gameObjects[i]->sprite->getPosition();
                    int moveFlag = false;
                    if (abs(beginPosition.x - location.x) > abs(beginPosition.y - location.y))
                    {
                        if (abs(beginPosition.x - location.x) > 20)
                        {
                            if (beginPosition.x > location.x)
                            {
                                if (gameObjects[isSelectedGameObject]->y > 0)
                                {
                                    if (isSimpleCell(gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y - 1])
                                        && gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y - 1] != CageCell
                                        && gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y - 1] != CageIceCell)
                                    {
                                        firstObject = isSelectedGameObject;
                                        secondObject = findGameObject(gameObjects[isSelectedGameObject]->x, gameObjects[isSelectedGameObject]->y - 1);
                                        
                                        if (firstObject < 0 || secondObject < 0)
                                            return;
                                        
                                        if (gameObjects[firstObject]->isLock || gameObjects[secondObject]->isLock)
                                            return;
                                        
                                        if (isTutorial)
                                        {
                                            for (int l = 0; l < tutorialCell.size(); l++)
                                            {
                                                if (gameObjects[firstObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                                {
                                                    if (tutorialCell[l].second != 2)
                                                        return;
                                                }
                                                if (gameObjects[secondObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                                {
                                                    if (tutorialCell[l].second != 3)
                                                        return;
                                                }

                                            }
                                        }
                                        
                                        gameObjects[firstObject]->moveLeft();
                                        gameObjects[secondObject]->moveRight();
                                        moveFlag = true;
                                        prevShift = LeftShift;
                                    }
                                }
                            }
                            else
                            {
                                if (gameObjects[isSelectedGameObject]->y  < columnCount - 1)
                                {
                                    if (isSimpleCell(gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y + 1])
                                        && gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y + 1] != CageCell
                                        && gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y + 1] != CageIceCell)
                                    {
                                        firstObject = isSelectedGameObject;
                                        secondObject = findGameObject(gameObjects[isSelectedGameObject]->x, gameObjects[isSelectedGameObject]->y + 1);
                                        
                                        if (firstObject < 0 || secondObject < 0)
                                            return;
                                        
                                        if (gameObjects[firstObject]->isLock || gameObjects[secondObject]->isLock)
                                            return;
                                        
                                        if (isTutorial)
                                        {
                                            for (int l = 0; l < tutorialCell.size(); l++)
                                            {
                                                if (gameObjects[firstObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                                {
                                                    if (tutorialCell[l].second != 3)
                                                        return;
                                                }
                                                if (gameObjects[secondObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                                {
                                                    if (tutorialCell[l].second != 2)
                                                        return;
                                                }
                                                
                                            }
                                        }
                                        
                                        gameObjects[firstObject]->moveRight();
                                        gameObjects[secondObject]->moveLeft();
                                        moveFlag = true;
                                        prevShift = RightShift;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (abs(beginPosition.y - location.y) > 20)
                        {
                            if (beginPosition.y > location.y)
                            {
                                if (gameObjects[isSelectedGameObject]->x < rowCount - 1)
                                {
                                    if (isSimpleCell(gameField[gameObjects[isSelectedGameObject]->x + 1][gameObjects[isSelectedGameObject]->y])
                                        && gameField[gameObjects[isSelectedGameObject]->x + 1][gameObjects[isSelectedGameObject]->y] != CageCell
                                        && gameField[gameObjects[isSelectedGameObject]->x + 1][gameObjects[isSelectedGameObject]->y] != CageIceCell)
                                    {
                                        firstObject = isSelectedGameObject;
                                        secondObject = findGameObject(gameObjects[isSelectedGameObject]->x + 1, gameObjects[isSelectedGameObject]->y);
                                        
                                        if (firstObject < 0 || secondObject < 0)
                                            return;
                                        
                                        if (gameObjects[firstObject]->isLock || gameObjects[secondObject]->isLock)
                                            return;
                                        
                                        if (isTutorial)
                                        {
                                            for (int l = 0; l < tutorialCell.size(); l++)
                                            {
                                                if (gameObjects[firstObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                                {
                                                    if (tutorialCell[l].second != 0)
                                                        return;
                                                }
                                                if (gameObjects[secondObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                                {
                                                    if (tutorialCell[l].second != 1)
                                                        return;
                                                }
                                                
                                            }
                                        }
                                        
                                        gameObjects[firstObject]->moveDown();
                                        gameObjects[secondObject]->moveUp();
                                        moveFlag = true;
                                        prevShift = DownShift;
                                    }
                                }				
                            }
                            else
                            {
                                if (gameObjects[isSelectedGameObject]->x > 0)
                                {
                                    if (isSimpleCell(gameField[gameObjects[isSelectedGameObject]->x - 1][gameObjects[isSelectedGameObject]->y])
                                        && gameField[gameObjects[isSelectedGameObject]->x - 1][gameObjects[isSelectedGameObject]->y] != CageCell
                                        && gameField[gameObjects[isSelectedGameObject]->x - 1][gameObjects[isSelectedGameObject]->y] != CageIceCell)
                                    {
                                        firstObject = isSelectedGameObject;
                                        secondObject = findGameObject(gameObjects[isSelectedGameObject]->x - 1, gameObjects[isSelectedGameObject]->y);
                                        
                                        if (firstObject < 0 || secondObject < 0)
                                            return;
                                        
                                        if (gameObjects[firstObject]->isLock || gameObjects[secondObject]->isLock)
                                            return;
                                        
                                        if (isTutorial)
                                        {
                                            for (int l = 0; l< tutorialCell.size(); l++)
                                            {
                                                if (gameObjects[firstObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                                {
                                                    if (tutorialCell[l].second != 1)
                                                        return;
                                                }
                                                if (gameObjects[secondObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                                {
                                                    if (tutorialCell[l].second != 0)
                                                        return;
                                                }
                                                
                                            }
                                        }
                                        
                                        gameObjects[firstObject]->moveUp();
                                        gameObjects[secondObject]->moveDown();
                                        moveFlag = true;
                                        prevShift = UpShift;
                                    }
                                }
                            }
                        }
                    }
                    
                    if (moveFlag)
                    {
                        if (isTutorial)
                        {
                            isTutorial = false;
                            tutorialCell.clear();
                            tutorialLayer->removeFromParentAndCleanup(true);
                            char buf[255];
                            sprintf(buf, "%d", menu->getCurrentLevel());
                            MMPPtr->tutorialStepCompleted(menu->getCurrentLevel(), string(buf));
                        }
                        lock = true;
                        this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(MOVE_DELTA*1.5f), CCCallFuncN::create( this, 
                                                                                                                                   callfuncN_selector(GameScene::shiftFinished))));
                    }
					idxSelectedObject = -1;
					gameObjects[i]->sprite->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.3f, 1.0f, 0.7f), CCScaleTo::create(0.3f, ELEMENT_SCALE)));
					if (gameObjects[i]->type == Simple)
						gameObjects[i]->sprite->runAction(CCSequence::createWithTwoActions(CCRotateBy::create(0.2f, 5.0f), CCRotateBy::create(0.2f, -5.0f)));
					gameObjects[i]->isSelected = true;
					idxSelectedObject = i;
				}
				else
				{
					gameObjects[i]->sprite->runAction(CCScaleTo::create(0.3f, ELEMENT_SCALE));
					gameObjects[i]->sprite->setRotation(0.0f);
					gameObjects[i]->isSelected = false;
					idxSelectedObject = -1;
				}
			}
			isSelectedGameObject = idxSelectedObject;
		}
	}
}

void GameScene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
	if (menu->isLock())
		return;
	if (!menu->getCountMoves())
		return;
	if (lock)
		return;
    if (leftDownMenu->isLock())
        return;
	if (isSelectedGameObject < 0)
		return;
    
    if (isFish)
		return;
    
    if (!menu->isTime())
        return;
    
    
	CCTouch* touch = (CCTouch*)( pTouches->anyObject());
	CCPoint location = touch->getLocation();
    
    if (LANDSCAPE)
    {
        location.x = location.x - CELL_WIDTH*2.0f;
        location.y = location.y + CELL_HEIGHT/2.0f;
    }
    
    location = this->convertToNodeSpace(location);

    
    if (!(abs(location.x - beginPosition.x) > CELL_HEIGHT/3.0f ||
          abs(location.y - beginPosition.y) > CELL_HEIGHT/3.0f))
        return;
    
    
	int moveFlag = false;
	if (abs(beginPosition.x - location.x) > abs(beginPosition.y - location.y))
	{
		if (abs(beginPosition.x - location.x) > 20)
		{
			if (beginPosition.x > location.x)
			{
				if (gameObjects[isSelectedGameObject]->y > 0)
				{
					if (isSimpleCell(gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y - 1])
                        && gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y - 1] != CageCell
                        && gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y - 1] != CageIceCell)
					{
						firstObject = isSelectedGameObject;
						secondObject = findGameObject(gameObjects[isSelectedGameObject]->x, gameObjects[isSelectedGameObject]->y - 1);
                        
						if (firstObject < 0 || secondObject < 0)
							return;
                        
						if (gameObjects[firstObject]->isLock || gameObjects[secondObject]->isLock)
							return;
                        
                        if (isTutorial)
                        {
                            for (int l = 0; l < tutorialCell.size(); l++)
                            {
                                if (gameObjects[firstObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                {
                                    if (tutorialCell[l].second != 2)
                                        return;
                                }
                                if (gameObjects[secondObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                {
                                    if (tutorialCell[l].second != 3)
                                        return;
                                }
                                
                            }
                        }
                        
						gameObjects[firstObject]->moveLeft();
						gameObjects[secondObject]->moveRight();
						moveFlag = true;
						prevShift = LeftShift;
					}
				}
			}
			else
			{
				if (gameObjects[isSelectedGameObject]->y  < columnCount - 1)
				{
					if (isSimpleCell(gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y + 1])
                        && gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y + 1] != CageCell
                        && gameField[gameObjects[isSelectedGameObject]->x][gameObjects[isSelectedGameObject]->y + 1] != CageIceCell)
					{
						firstObject = isSelectedGameObject;
						secondObject = findGameObject(gameObjects[isSelectedGameObject]->x, gameObjects[isSelectedGameObject]->y + 1);
                        
						if (firstObject < 0 || secondObject < 0)
							return;
                        
						if (gameObjects[firstObject]->isLock || gameObjects[secondObject]->isLock)
							return;
                        
                        if (isTutorial)
                        {
                            for (int l = 0; l < tutorialCell.size(); l++)
                            {
                                if (gameObjects[firstObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                {
                                    if (tutorialCell[l].second != 3)
                                        return;
                                }
                                if (gameObjects[secondObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                {
                                    if (tutorialCell[l].second != 2)
                                        return;
                                }
                                
                            }
                        }
                        
						gameObjects[firstObject]->moveRight();
						gameObjects[secondObject]->moveLeft();
						moveFlag = true;
						prevShift = RightShift;
					}
				}
			}
		}
	}
	else
	{
		if (abs(beginPosition.y - location.y) > 20)
		{
			if (beginPosition.y > location.y)
			{
				if (gameObjects[isSelectedGameObject]->x < rowCount - 1)
				{
					if (isSimpleCell(gameField[gameObjects[isSelectedGameObject]->x + 1][gameObjects[isSelectedGameObject]->y])
                        && gameField[gameObjects[isSelectedGameObject]->x + 1][gameObjects[isSelectedGameObject]->y] != CageCell
                        && gameField[gameObjects[isSelectedGameObject]->x + 1][gameObjects[isSelectedGameObject]->y] != CageIceCell)
					{
						firstObject = isSelectedGameObject;
						secondObject = findGameObject(gameObjects[isSelectedGameObject]->x + 1, gameObjects[isSelectedGameObject]->y);
                        
						if (firstObject < 0 || secondObject < 0)
							return;
                        
						if (gameObjects[firstObject]->isLock || gameObjects[secondObject]->isLock)
							return;
                        
                        if (isTutorial)
                        {
                            for (int l = 0; l < tutorialCell.size(); l++)
                            {
                                if (gameObjects[firstObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                {
                                    if (tutorialCell[l].second != 0)
                                        return;
                                }
                                if (gameObjects[secondObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                {
                                    if (tutorialCell[l].second != 1)
                                        return;
                                }
                                
                            }
                        }
                        
						gameObjects[firstObject]->moveDown();
						gameObjects[secondObject]->moveUp();
						moveFlag = true;
						prevShift = DownShift;
					}
				}
			}
			else
			{
				if (gameObjects[isSelectedGameObject]->x > 0)
				{
					if (isSimpleCell(gameField[gameObjects[isSelectedGameObject]->x - 1][gameObjects[isSelectedGameObject]->y])
                        && gameField[gameObjects[isSelectedGameObject]->x - 1][gameObjects[isSelectedGameObject]->y] != CageCell
                        && gameField[gameObjects[isSelectedGameObject]->x - 1][gameObjects[isSelectedGameObject]->y] != CageIceCell)
					{
						firstObject = isSelectedGameObject;
						secondObject = findGameObject(gameObjects[isSelectedGameObject]->x - 1, gameObjects[isSelectedGameObject]->y);
                        
						if (firstObject < 0 || secondObject < 0)
							return;
                        
						if (gameObjects[firstObject]->isLock || gameObjects[secondObject]->isLock)
							return;
                        
                        if (isTutorial)
                        {
                            for (int l = 0; l < tutorialCell.size(); l++)
                            {
                                if (gameObjects[firstObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                {
                                    if (tutorialCell[l].second != 1)
                                        return;
                                }
                                if (gameObjects[secondObject] == gameObjects[findGameObject(tutorialCell[l].first.x, tutorialCell[l].first.y)])
                                {
                                    if (tutorialCell[l].second != 0)
                                        return;
                                }
                                
                            }
                        }
                        
						gameObjects[firstObject]->moveUp();
						gameObjects[secondObject]->moveDown();
						moveFlag = true;
						prevShift = UpShift;
					}
				}
			}
		}
	}
    
	if (moveFlag)
	{
        isBeganTouch = false;
        if (isTutorial)
        {
            isTutorial = false;
            tutorialCell.clear();
            tutorialLayer->removeFromParentAndCleanup(true);
            tutorialNumber = -1;
        }
		lock = true;
		this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(MOVE_DELTA), CCCallFuncN::create( this,
                                                                                                              callfuncN_selector(GameScene::shiftFinished))));
        isSelectedGameObject = -1;
        idxSelectedObject = -1;
	}
}

void GameScene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
    if (isSelectedGameObject > 0)
    {
        gameObjects[isSelectedGameObject]->sprite->runAction(CCScaleTo::create(0.3f, ELEMENT_SCALE));
        gameObjects[isSelectedGameObject]->sprite->setRotation(0.0f);
        gameObjects[isSelectedGameObject]->isSelected = false;
        isSelectedGameObject = -1;
        idxSelectedObject = -1;
    }
    return;
}

void GameScene::lighting(CCPoint begin, CCPoint end, float delay)
{
    if (LANDSCAPE)
    {
        begin.x += CELL_WIDTH*2.0f;
        begin.y -= CELL_HEIGHT/2.0f;
        end.x += CELL_WIDTH*2.0f;
        end.y -= CELL_HEIGHT/2.0f;
    }
    CCNode* grandParent = this;
    

    CCParticleSystem* meteorRight = CCParticleMeteor::create();
    meteorRight->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/fire.png"));
    meteorRight->setPosition(begin);
    meteorRight->setScale(1.3f);
    meteorRight->setGravity(ccp(0.0f, -100.0f));
    meteorRight->setLife(2.0f);
    grandParent->addChild(meteorRight, 100);
    meteorRight->runAction(CCSequence::create(CCDelayTime::create(delay),CCMoveTo::create(0.4f, end), CCDelayTime::create(0.1f), CCScaleTo::create(0.2f, 0.01f), CCHide::create(), NULL));
    /*if (LANDSCAPE)
    {
        begin.x += CELL_WIDTH*2.0f;
        begin.y -= CELL_HEIGHT/2.0f;
    }
    CCAnimation *animation = CCAnimation::create();
    
    CCSprite* light = CCSprite::create("lite.png");
    CCSprite* lightFlip = CCSprite::create("liteFlip.png");
    
    animation->addSpriteFrame(light->displayFrame());
    animation->addSpriteFrame(lightFlip->displayFrame());
    
    animation->setDelayPerUnit(0.05f);
    animation->setRestoreOriginalFrame(true);
    animation->setLoops(10);
    
    CCAnimate *action = CCAnimate::create(animation);
    light->runAction(action);
    float scaleX = (begin.getDistance(end)/light->getContentSize().width);
    light->setAnchorPoint(ccp(0.5f, 0.5f));
    light->setPosition(begin);
    CCPoint vector1 = ccp(1.0f, 0.0f);
    CCPoint vector2 = ccp(end.x - begin.x, end.y - begin.y);
    vector2 = vector2.normalize();
    float angle = (vector1.x*vector2.x + vector1.y*vector2.y)/
        sqrt(vector1.x*vector1.x + vector1.y*vector1.y)*sqrt(vector2.x*vector2.x + vector2.y*vector2.y);
    angle = acos(angle)*57.29577951f;
    
    if (end.y > begin.y)
        light->setRotation(-angle);
    else
        light->setRotation(angle);
    
    
    light->setScaleX(0.01f);
    light->setOpacity(255);
    //light->runAction(CCSequence::create(/*CCSpawn::createWithTwoActions(*//*CCScaleTo::create(0.2f, scaleX*2.0f, 0.8f),*//* CCMoveTo::create(0.2f, end)),*/
    //                                   CCDelayTime::create(0.2f), CCHide::create(), NULL));
    //this->addChild(light,10);
}

void GameScene::lightingBomb(CCPoint begin, CCPoint end, float bScale, float delay)
{
    if (LANDSCAPE)
    {
        begin.x += CELL_WIDTH*2.0f;
        begin.y -= CELL_HEIGHT/2.0f;
        end.x += CELL_WIDTH*2.0f;
        end.y -= CELL_HEIGHT/2.0f;
    }
    CCNode* grandParent = this;
    
    CCParticleSystem* meteorRight = CCParticleMeteor::create();
    meteorRight->setTexture(CCTextureCache::sharedTextureCache()->addImage("particle/fire.png"));
    meteorRight->setPosition(begin);
    meteorRight->setScale(bScale);
    meteorRight->setGravity(ccp(0.0f, -100.0f));
    meteorRight->setLife(1.0f);
    grandParent->addChild(meteorRight, 100);
    meteorRight->runAction(CCSequence::create(CCDelayTime::create(delay + ((float)(rand()%20))/100.0f), CCMoveTo::create(0.2f, end), CCDelayTime::create(0.1f), CCScaleTo::create(0.2f, 0.01f), CCHide::create(), NULL));
}


bool GameScene::checkIntersectTwoElements(int first, int second)
{
	crystalChangeType = Simple;
    
    if (gameObjects[first]->type == Cookie || gameObjects[second]->type == Cookie)
        return false;
    if (gameObjects[first]->type == Fish || gameObjects[second]->type == Fish)
        return false;
    
	if ((gameObjects[first]->type != Simple && gameObjects[second]->type != Simple) || gameObjects[first]->type == Crystal || gameObjects[second]->type == Crystal)
	{
		if ((gameObjects[first]->type == Horizontal && gameObjects[second]->type == Horizontal) ||
			(gameObjects[first]->type == Vertical && gameObjects[second]->type == Vertical) ||
			(gameObjects[first]->type == Horizontal && gameObjects[second]->type == Vertical) ||
			(gameObjects[first]->type == Vertical && gameObjects[second]->type == Horizontal))
		{
            if (gameObjects[first]->x == gameObjects[second]->x)
            {
                gameObjects[second]->changeType(Horizontal);
                gameObjects[first]->changeType(Vertical);
                addToDeleting(first);
                addToDeleting(second);
                gameObjectHV.push_back(gameObjects[first]);
                gameObjectHV.push_back(gameObjects[second]);

            }
            else
            {
                gameObjects[second]->changeType(Vertical);
                gameObjects[first]->changeType(Horizontal);
                addToDeleting(first);
                addToDeleting(second);
                gameObjectHV.push_back(gameObjects[first]);
                gameObjectHV.push_back(gameObjects[second]);
            }
        }

		if ((gameObjects[first]->type == Horizontal && gameObjects[second]->type == Bomb) ||
			(gameObjects[first]->type == Bomb && gameObjects[second]->type == Horizontal) ||
			(gameObjects[first]->type == Vertical && gameObjects[second]->type == Bomb) ||
			(gameObjects[first]->type == Bomb && gameObjects[second]->type == Vertical))
		{
			/*if (gameObjects[first]->type == Bomb)
			{
				gameObjects[second]->isMegaBomb = true;
                gameObjects[second]->changeType(Bomb);
				gameObjects[first]->changeType(Simple);
				addToDeleting(second);
			}*/
			//else if (gameObjects[second]->type == Bomb)
			//{
                ///gameObjects[first]->sprite->
                gameObjects[first]->sprite->setScale(ELEMENT_SCALE);
				gameObjects[first]->isMegaBomb = true;
                gameObjects[first]->changeType(Bomb);
				gameObjects[second]->changeType(Simple);
				addToDeleting(first);
			//}
		}
        
        if ((gameObjects[first]->type == Bomb && gameObjects[second]->type == Bomb))
		{
            gameObjects[first]->changeType(Simple);
            gameObjects[second]->changeType(Simple);
            addToDeleting(first);
            addToDeleting(second);
            
            sGameObject* bombObj = gameObjects[first];
            int xObj = bombObj->x;
            int yObj = bombObj->y;
            float bombScale = 2.5f;
            float delay = 0.01f;
            
            if (findGameObject(xObj-2, yObj+2) >= 0)
                addToDeleting(findGameObject(xObj-2, yObj+2));
            if (findGameObject(xObj-2, yObj+1) >= 0)
                addToDeleting(findGameObject(xObj-2, yObj+1));
            if (findGameObject(xObj-2, yObj) >= 0)
                addToDeleting(findGameObject(xObj-2, yObj));
            if (findGameObject(xObj-2, yObj-1) >= 0)
                addToDeleting(findGameObject(xObj-2, yObj-1));
            if (findGameObject(xObj-2, yObj-2) >= 0)
                addToDeleting(findGameObject(xObj-2, yObj-2));
            
            if (findGameObject(xObj-1, yObj+2) >= 0)
                addToDeleting(findGameObject(xObj-1, yObj+2));
            if (findGameObject(xObj-1, yObj+1) >= 0)
                addToDeleting(findGameObject(xObj-1, yObj+1));
            if (findGameObject(xObj-1, yObj) >= 0)
                addToDeleting(findGameObject(xObj-2, yObj));
            if (findGameObject(xObj-1, yObj-1) >= 0)
                addToDeleting(findGameObject(xObj-1, yObj-1));
            if (findGameObject(xObj-1, yObj-2) >= 0)
                addToDeleting(findGameObject(xObj-1, yObj-2));
            
            if (findGameObject(xObj, yObj+2) >= 0)
                addToDeleting(findGameObject(xObj, yObj+2));
            if (findGameObject(xObj, yObj+1) >= 0)
                addToDeleting(findGameObject(xObj, yObj+1));
            if (findGameObject(xObj, yObj) >= 0)
                addToDeleting(findGameObject(xObj, yObj));
            if (findGameObject(xObj, yObj-1) >= 0)
                addToDeleting(findGameObject(xObj, yObj-1));
            if (findGameObject(xObj, yObj-2) >= 0)
                addToDeleting(findGameObject(xObj, yObj-2));
            
            if (findGameObject(xObj+2, yObj+2) >= 0)
                addToDeleting(findGameObject(xObj+2, yObj+2));
            if (findGameObject(xObj+2, yObj+1) >= 0)
                addToDeleting(findGameObject(xObj+2, yObj+1));
            if (findGameObject(xObj+2, yObj) >= 0)
                addToDeleting(findGameObject(xObj+2, yObj));
            if (findGameObject(xObj+2, yObj-1) >= 0)
                addToDeleting(findGameObject(xObj+2, yObj-1));
            if (findGameObject(xObj+2, yObj-2) >= 0)
                addToDeleting(findGameObject(xObj+2, yObj-2));
            
            
            
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x + CELL_WIDTH*2.0f, bombObj->sprite->getPosition().y), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x + 3.0f*(CELL_WIDTH*2.0f)/4.0f, bombObj->sprite->getPosition().y + (CELL_HEIGHT*2.0f)/2.2f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x + 3.0f*(CELL_WIDTH*2.0f)/4.0f, bombObj->sprite->getPosition().y - (CELL_HEIGHT*2.0f)/2.2f), bombScale, delay);
            
            
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x - CELL_WIDTH*2.0f, bombObj->sprite->getPosition().y), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x - 3.0f*(CELL_WIDTH*2.0f)/4.0f, bombObj->sprite->getPosition().y + (CELL_HEIGHT*2.0f)/2.2f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x - 3.0f*(CELL_WIDTH*2.0f)/4.0f, bombObj->sprite->getPosition().y - (CELL_HEIGHT*2.0f)/2.2f), bombScale, delay);
            
            
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x, bombObj->sprite->getPosition().y + CELL_HEIGHT*2.0f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x + (CELL_WIDTH*2.0f)/2.2f, bombObj->sprite->getPosition().y + 3.0f*(CELL_HEIGHT*2.0f)/4.0f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x - (CELL_WIDTH*2.0f)/2.2f, bombObj->sprite->getPosition().y + 3.0f*(CELL_HEIGHT*2.0f)/4.0f), bombScale, delay);
            
            
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x, bombObj->sprite->getPosition().y - CELL_HEIGHT*2.0f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x + (CELL_WIDTH*2.0f)/2.2f, bombObj->sprite->getPosition().y - 3.0f*(CELL_HEIGHT*2.0f)/4.0f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                     ccp(bombObj->sprite->getPosition().x - (CELL_WIDTH*2.0f)/2.2f, bombObj->sprite->getPosition().y - 3.0f*(CELL_HEIGHT*2.0f)/4.0f), bombScale, delay);
            
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x + CELL_WIDTH*2.0f, bombObj->sprite->getPosition().y), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x + 3.0f*(CELL_WIDTH*2.0f)/4.0f, bombObj->sprite->getPosition().y + (CELL_HEIGHT*2.0f)/2.2f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x + 3.0f*(CELL_WIDTH*2.0f)/4.0f, bombObj->sprite->getPosition().y - (CELL_HEIGHT*2.0f)/2.2f), bombScale, delay);
            
            
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x - CELL_WIDTH*2.0f, bombObj->sprite->getPosition().y), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x - 3.0f*(CELL_WIDTH*2.0f)/4.0f, bombObj->sprite->getPosition().y + (CELL_HEIGHT*2.0f)/2.2f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x - 3.0f*(CELL_WIDTH*2.0f)/4.0f, bombObj->sprite->getPosition().y - (CELL_HEIGHT*2.0f)/2.2f), bombScale, delay);
            
            
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x, bombObj->sprite->getPosition().y + CELL_HEIGHT*2.0f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x + (CELL_WIDTH*2.0f)/2.2f, bombObj->sprite->getPosition().y + 3.0f*(CELL_HEIGHT*2.0f)/4.0f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x - (CELL_WIDTH*2.0f)/2.2f, bombObj->sprite->getPosition().y + 3.0f*(CELL_HEIGHT*2.0f)/4.0f), bombScale, delay);
            
            
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x, bombObj->sprite->getPosition().y - CELL_HEIGHT*2.0f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x + (CELL_WIDTH*2.0f)/2.2f, bombObj->sprite->getPosition().y - 3.0f*(CELL_HEIGHT*2.0f)/4.0f), bombScale, delay);
            lightingBomb(bombObj->sprite->getPosition(),
                         ccp(bombObj->sprite->getPosition().x - (CELL_WIDTH*2.0f)/2.2f, bombObj->sprite->getPosition().y - 3.0f*(CELL_HEIGHT*2.0f)/4.0f), bombScale, delay);
		}
        
		if ((gameObjects[first]->type == Crystal && gameObjects[second]->type == Crystal))
		{
            float delayCrystal = 0.0f;
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < columnCount; j++)
				{
					if (isSimpleCell(gameField[i][j]))
                    {
						if (findGameObject(i, j) >= 0)
                        {
                            gameObjects[findGameObject(i, j)]->isScore = true;
                            gameObjects[findGameObject(i, j)]->delayDestroy = delayCrystal + 0.3f;
                            gameObjects[findGameObject(i, j)]->delayStripe = delayCrystal + 0.3f;
                            addToDeleting(findGameObject(i, j));
                            lighting(gameObjects[first]->sprite->getPosition(),
                                     gameObjects[findGameObject(i, j)]->sprite->getPosition(),
                                     delayCrystal);
                            lighting(gameObjects[first]->sprite->getPosition(),
                                     gameObjects[findGameObject(i, j)]->sprite->getPosition(),
                                     delayCrystal);
                            delayCrystal+=0.03f;
                        }
                    }
                }
			}
            addToDeleting(first);
			addToDeleting(second);
            crystalCrystal = true;
		}


		if ((gameObjects[first]->type == Crystal && gameObjects[second]->type == Simple))
		{
			for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < columnCount; j++)
				{
					if (isSimpleCell(gameField[i][j]))
						if (findGameObject(i, j) >= 0)
							if (gameObjects[findGameObject(i, j)]->color == gameObjects[second]->color)
							{
                                gameObjects[findGameObject(i, j)]->isScore = true;
								addToDeleting(findGameObject(i, j));
                                lighting(gameObjects[first]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
                                lighting(gameObjects[first]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
							}
				}
			}
			addToDeleting(first);
			addToDeleting(second);
		}		

		if ((gameObjects[first]->type == Simple && gameObjects[second]->type == Crystal))
		{
			for (int i = 0; i < rowCount; i++)
				for (int j = 0; j < columnCount; j++)
					if (isSimpleCell(gameField[i][j]))
						if (findGameObject(i, j) >= 0)
							if (gameObjects[findGameObject(i, j)]->color == gameObjects[first]->color)
							{
                                gameObjects[findGameObject(i, j)]->isScore = true;
								addToDeleting(findGameObject(i, j));
                                lighting(gameObjects[second]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
                                lighting(gameObjects[second]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());

							}
			addToDeleting(first);
			addToDeleting(second);
		}

		if ((gameObjects[first]->type == Horizontal && gameObjects[second]->type == Crystal))
		{
            nextDead.clear();
			for (int i = 0; i < rowCount; i++)
				for (int j = 0; j < columnCount; j++)
					if (isSimpleCell(gameField[i][j]))
						if (findGameObject(i, j)>=0)
							if (gameObjects[findGameObject(i, j)]->color == gameObjects[first]->color)
							{
                                if (rand()%2)
                                    gameObjects[findGameObject(i, j)]->changeType(Horizontal);
                                else
                                    gameObjects[findGameObject(i, j)]->changeType(Vertical);
                                //addToDeleting(findGameObject(i, j));
                                nextDead.push_back(gameObjects[findGameObject(i, j)]);
                                lighting(gameObjects[second]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
                                lighting(gameObjects[second]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
							}
            addToDeleting(second);
//            afterDeleting();
            isCrystalStripe = true;
		}

		if ((gameObjects[second]->type == Horizontal && gameObjects[first]->type == Crystal))
		{
            nextDead.clear();
			for (int i = 0; i < rowCount; i++)
				for (int j = 0; j < columnCount; j++)
					if (isSimpleCell(gameField[i][j]))
						if (findGameObject(i, j)>=0)
							if (gameObjects[findGameObject(i, j)]->color == gameObjects[second]->color)
							{
                                if (rand()%2)
                                    gameObjects[findGameObject(i, j)]->changeType(Horizontal);
                                else
                                    gameObjects[findGameObject(i, j)]->changeType(Vertical);
                                
                                //addToDeleting(findGameObject(i, j));
                                nextDead.push_back(gameObjects[findGameObject(i, j)]);
                                lighting(gameObjects[first]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());

                                lighting(gameObjects[first]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
}
            addToDeleting(first);
//            afterDeleting();
            isCrystalStripe = true;
		}

		if ((gameObjects[first]->type == Vertical && gameObjects[second]->type == Crystal))
		{
            nextDead.clear();
			for (int i = 0; i < rowCount; i++)
				for (int j = 0; j < columnCount; j++)
					if (isSimpleCell(gameField[i][j]))
						if (findGameObject(i, j)>=0)
							if (gameObjects[findGameObject(i, j)]->color == gameObjects[first]->color)
							{
                                if (rand()%2)
                                    gameObjects[findGameObject(i, j)]->changeType(Vertical);
                                else
                                    gameObjects[findGameObject(i, j)]->changeType(Horizontal);
								
                                //addToDeleting(findGameObject(i, j));
                                nextDead.push_back(gameObjects[findGameObject(i, j)]);
                                lighting(gameObjects[second]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
                                lighting(gameObjects[second]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
							}
            addToDeleting(second);
//            afterDeleting();
            isCrystalStripe = true;
		}

		if ((gameObjects[second]->type == Vertical && gameObjects[first]->type == Crystal))
		{
            nextDead.clear();
			for (int i = 0; i < rowCount; i++)
				for (int j = 0; j < columnCount; j++)
					if (isSimpleCell(gameField[i][j]))
						if (findGameObject(i, j)>=0)
							if (gameObjects[findGameObject(i, j)]->color == gameObjects[second]->color)
							{
                                if (rand()%2)
                                    gameObjects[findGameObject(i, j)]->changeType(Vertical);
                                else
                                    gameObjects[findGameObject(i, j)]->changeType(Horizontal);

                                //addToDeleting(findGameObject(i, j));
                                nextDead.push_back(gameObjects[findGameObject(i, j)]);
                                lighting(gameObjects[first]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());
                                lighting(gameObjects[first]->sprite->getPosition(),
                                         gameObjects[findGameObject(i, j)]->sprite->getPosition());

							}
            addToDeleting(first);
//            afterDeleting();
            isCrystalStripe = true;
		}
	
		if ((gameObjects[first]->type == Bomb && gameObjects[second]->type == Crystal))
		{
            addToDeleting(second);
            for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < columnCount; j++)
				{
					if (isSimpleCell(gameField[i][j]))
						if (findGameObject(i, j) >= 0)
							if (gameObjects[findGameObject(i, j)]->color == gameObjects[first]->color)
							{
                                if (gameObjects[findGameObject(i, j)] != gameObjects[first])
                                {
                                    addToDeleting(findGameObject(i, j));
                                    lighting(gameObjects[first]->sprite->getPosition(),
                                             gameObjects[findGameObject(i, j)]->sprite->getPosition());
                                    lighting(gameObjects[first]->sprite->getPosition(),
                                             gameObjects[findGameObject(i, j)]->sprite->getPosition());

                                }
                            }
				}
			}
            
            gameObjects[first]->sprite->stopAllActions();
            gameObjects[first]->sprite->setScaleX(ELEMENT_SCALE);
            gameObjects[first]->sprite->setScaleY(ELEMENT_SCALE);
            gameObjects[first]->changeType(Crystal);
            nextDead.push_back(gameObjects[first]);
		}

		if ((gameObjects[second]->type == Bomb && gameObjects[first]->type == Crystal))
		{
            addToDeleting(first);
            for (int i = 0; i < rowCount; i++)
			{
				for (int j = 0; j < columnCount; j++)
				{
					if (isSimpleCell(gameField[i][j]))
						if (findGameObject(i, j) >= 0)
							if (gameObjects[findGameObject(i, j)]->color == gameObjects[second]->color)
							{
                                if (gameObjects[findGameObject(i, j)] != gameObjects[second])
                                {
                                    addToDeleting(findGameObject(i, j));
                                    lighting(gameObjects[second]->sprite->getPosition(),
                                             gameObjects[findGameObject(i, j)]->sprite->getPosition());
                                }
                            }
				}
			}
            
            gameObjects[second]->sprite->stopAllActions();
            gameObjects[second]->sprite->setScaleX(ELEMENT_SCALE);
            gameObjects[second]->sprite->setScaleY(ELEMENT_SCALE);
            gameObjects[second]->changeType(Crystal);
            nextDead.push_back(gameObjects[second]);
        }
		return true;
	}
	return false;
}

void GameScene::crystalAfterAction(CCNode* sender)
{	
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->type == crystalChangeType)
		{				
			addToDeleting(i);
		}
	}
	afterDeleting();
}

void GameScene::shiftFinished(CCNode* sender)
{
    for (int i = 0; i < gameObjects.size();i ++)
    {
        if (gameObjects[i]->isSelected)
        {
            gameObjects[i]->sprite->runAction(CCScaleTo::create(0.3f, ELEMENT_SCALE));
            gameObjects[i]->sprite->setRotation(0.0f);
            gameObjects[i]->isSelected = false;
        }
    }
	if (idxSelectedObject > 0)
	{
		gameObjects[idxSelectedObject]->sprite->runAction(CCScaleTo::create(0.3f, ELEMENT_SCALE));
		gameObjects[idxSelectedObject]->sprite->setRotation(0.0f);
		gameObjects[idxSelectedObject]->isSelected = false;
		idxSelectedObject = -1;
	}
	if (checkIntersectTwoElements(firstObject, secondObject))
	{
        isChocolateDestroyed = false;
        SimpleAudioEngine::sharedEngine()->playEffect("sound/transformation.mp3");
		menu->setCountMoves(menu->getCountMoves() - 1);
		afterDeleting();
    }
	else if (checkElement(firstObject) && gameObjects[firstObject]->type != Cookie)
	{
        isChocolateDestroyed = false;
		checkElement(secondObject);
		menu->setCountMoves(menu->getCountMoves() - 1);
		afterDeleting();
	}
	else if (checkElement(secondObject) && gameObjects[secondObject]->type != Cookie)
	{
        isChocolateDestroyed = false;
		checkElement(firstObject);
		menu->setCountMoves(menu->getCountMoves() - 1);
		afterDeleting();		
	}
	else
	{
        SimpleAudioEngine::sharedEngine()->playEffect("sound/dumb.mp3");
        for (int i = 0; i < gameObjects.size(); i++)
            gameObjects[i]->unlock(NULL);
		this->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(MOVE_DELTA), CCCallFuncN::create(this, callfuncN_selector(GameScene::unlock))));
//      lock = false;
		switch (prevShift)
		{
		case LeftShift:
			gameObjects[firstObject]->moveRight();
			gameObjects[secondObject]->moveLeft();
			break;
		case RightShift:
			gameObjects[firstObject]->moveLeft();
			gameObjects[secondObject]->moveRight();
			break;
		case DownShift:
			gameObjects[firstObject]->moveUp();
			gameObjects[secondObject]->moveDown();
			break;
		case UpShift:
			gameObjects[firstObject]->moveDown();
			gameObjects[secondObject]->moveUp();
			break;
		}
	}
}

void GameScene::unlock(CCNode* sender)
{
	lock = false;
}

void GameScene::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
	if (isSelectedGameObject > 0)
    {
        gameObjects[isSelectedGameObject]->sprite->runAction(CCScaleTo::create(0.3f, ELEMENT_SCALE));
        gameObjects[isSelectedGameObject]->sprite->setRotation(0.0f);
        gameObjects[isSelectedGameObject]->isSelected = false;
        isSelectedGameObject = -1;
        idxSelectedObject = -1;
    }
    return;
}

int GameScene::findGameObject(int x, int y)
{
	for (int i = 0; i < gameObjects.size(); i++)
		if (gameObjects[i]->x == x && gameObjects[i]->y == y)
			return i;

	return -1;
}

CCSprite* GameScene::createBlur(CCSprite* sprite, int size, ccColor3B color, GLubyte opacity)
{
    CCRenderTexture* rt = CCRenderTexture::create(sprite->getTexture()->getContentSize().width + size * 2, sprite->getTexture()->getContentSize().height+size * 2);
    
    CCPoint originalPos = sprite->getPosition();
    
    ccColor3B originalColor = sprite->getColor();
    
    GLubyte originalOpacity = sprite->getOpacity();
    
    bool originalVisibility = sprite->isVisible();
    
    sprite->setColor(color);
    
    sprite->setOpacity(opacity);
    
    sprite->setVisible(true);
    
    ccBlendFunc originalBlend = sprite->getBlendFunc();
    
    ccBlendFunc bf = {GL_SRC_ALPHA, GL_ONE};
    
    sprite->setBlendFunc(bf);
    
    CCPoint bottomLeft = ccp(
                             sprite->getTexture()->getContentSize().width * sprite->getAnchorPoint().x + size,
                             sprite->getTexture()->getContentSize().height * sprite->getAnchorPoint().y + size);
    
    CCPoint positionOffset= ccp(
                                - sprite->getTexture()->getContentSize().width / 2,
                                - sprite->getTexture()->getContentSize().height / 2);
    
    CCPoint position = ccpSub(originalPos, positionOffset);
    
    rt->begin();
    
    for (int i=0; i<360; i+= 15) // you should optimize that for your needs
    {
        sprite->setPosition(ccp(bottomLeft.x + sin(CC_DEGREES_TO_RADIANS(i))*size, bottomLeft.y + cos(CC_DEGREES_TO_RADIANS(i))*size));
        sprite->visit();
    }
    rt->end();
    
    sprite->setPosition(originalPos);
    sprite->setColor(originalColor);
    sprite->setBlendFunc(originalBlend);
    sprite->setVisible(originalVisibility);
    sprite->setOpacity(originalOpacity);
    
    rt->setPosition(position);
    rt->getSprite()->getTexture()->setAntiAliasTexParameters();
    
    CCImage* image = rt->newCCImage();
	CCTexture2D* tex = new CCTexture2D();
	tex->initWithImage(image);
    rt->release();
    image->release();
    
	CCSprite* returnSprite = CCSprite::createWithTexture(tex,
                                                         CCRect(0, 0, sprite->getTexture()->getContentSize().width + size * 2, sprite->getTexture()->getContentSize().height+size * 2));
    returnSprite->setAnchorPoint(ccp(0.5f, 0.5f - (CELL_HEIGHT/2.0f)/WINSIZE.height));
    return returnSprite;
}

void GameScene::renderField()
{
	CCRenderTexture* renderTex = CCRenderTexture::create(CCDirector::sharedDirector()->getWinSize().width,
		CCDirector::sharedDirector()->getWinSize().height);

	renderTex->retain();
	renderTex->begin();
    if (IPHONE_4 || IPHONE_5)
    {
        for (int i = 0; i < rowCount; i++)
        {
            if (cellType)
                cellType = false;
            else
                cellType = true;
            
            for (int j = 0; j < columnCount; j++)
            {
                if (cellType)
                    cellType = false;
                else
                    cellType = true;
                
                if (gameField[i][j] == NoneCell || gameField[i][j] == LockCell)
                    continue;
                
                CCSprite* cell = CCSprite::create("game/cell_landscape.png");
                CCPoint temp = ccp(j*CELL_WIDTH + xZero,
                                   yZero - i*CELL_HEIGHT);
                
                cell->setPosition(ccp(j*CELL_WIDTH + xZero,
                                      yZero - i*CELL_HEIGHT));
                cell->setAnchorPoint(ccp(0.5f, 1.0f));
                
                ccColor3B color;
                color.r = 0x51;
                color.g = 0x7f;
                color.b = 0x95;
                
                bool isVer = true;
                bool isGor = true;
                
                if ( i == rowCount-1 && gameType == BringDown)
                {
                    CCSprite* stencil = CCSprite::createWithSpriteFrameName("game/bringStudy.png");
                    stencil->setAnchorPoint(ccp(0.5f, 0.5f));
                    stencil->setPosition(ccp(CELL_WIDTH/2.0f, 0.0f));
                    stencil->setScale(0.4f);
                    stencil->setVisible(true);
                    cell->addChild(stencil, 5);
                }
                
                if (isNoneCell(i + 1, j) && gameType == BringDown)
                {
                    bool isNone = true;
                    
                    for (int k = i + 1; k < rowCount; k++)
                        if (!isNoneCell(k, j))
                            isNone = false;
                    
                    if (isNone)
                    {
                        CCSprite* stencil = CCSprite::createWithSpriteFrameName("game/bringStudy.png");
                        stencil->setAnchorPoint(ccp(0.5f, 0.5f));
                        stencil->setPosition(ccp(CELL_WIDTH/2.0f, 0.0f));
                        stencil->setScale(0.4f);
                        stencil->setVisible(true);
                        cell->addChild(stencil, 5);
                    }
                }
                
                
                if (isNoneCell(i - 1, j))
                {
                    isGor = false;
                    CCSprite* stencil = CCSprite::create("game/edge_landscape.png");
                    stencil->setRotation(90.0f);
                    stencil->setFlipX(true);
                    stencil->setAnchorPoint(ccp(1.0f, 0.0f));
                    stencil->setPosition(ccp(0.0f, CELL_HEIGHT));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                if (isNoneCell(i + 1, j))
                {
                    CCSprite* stencil = CCSprite::create("game/edge_landscape.png");
                    stencil->setRotation(90.0f);
                    stencil->setAnchorPoint(ccp(0.0f, 0.0f));
                    stencil->setPosition(ccp(0.0f, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                if (isNoneCell(i, j - 1))
                {
                    isVer = false;
                    CCSprite* stencil = CCSprite::create("game/edge_landscape.png");
                    stencil->setScaleY(1.1f);
                    stencil->setFlipX(true);
                    stencil->setAnchorPoint(ccp(1.0f, 0.0f));
                    stencil->setPosition(ccp(0.0f, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                if (isNoneCell(i, j + 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge_landscape.png");
                    stencil->setScaleY(1.1f);
                    stencil->setAnchorPoint(ccp(0.0f, 0.0f));
                    stencil->setPosition(ccp(CELL_WIDTH, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                
                if (isNoneCell(i - 1, j - 1) && isNoneCell(i - 1, j) && isNoneCell(i, j - 1))
                {
                    CCSprite* stencil = CCSprite::create("game/outside_landscape.png");
                    stencil->setAnchorPoint(ccp(1.0f, 0.0f));
                    if (IPHONE_4 || IPHONE_5)
                        stencil->setPosition(ccp(0.0f, cell->getContentSize().height-2));
                    else
                        stencil->setPosition(ccp(0.0f, cell->getContentSize().height));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                
                if (isNoneCell(i - 1, j + 1) && isNoneCell(i - 1, j) && isNoneCell(i, j + 1))
                {
                    CCSprite* stencil = CCSprite::create("game/outside_landscape.png");
                    stencil->setFlipX(true);
                    stencil->setAnchorPoint(ccp(0.0f, 0.0f));
                    stencil->setPosition(ccp(CELL_WIDTH, CELL_HEIGHT));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                
                if (isNoneCell(i, j - 1) && isNoneCell(i + 1, j - 1) && isNoneCell(i + 1, j))
                {
                    CCSprite* stencil = CCSprite::create("game/outside_landscape.png");
                    stencil->setFlipY(true);
                    stencil->setAnchorPoint(ccp(1.0f, 1.0f));
                    stencil->setPosition(ccp(0.0f, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                if (isNoneCell(i, j + 1) && isNoneCell(i + 1, j + 1) && isNoneCell(i + 1, j))
                {
                    CCSprite* stencil = CCSprite::create("game/outside_landscape.png");
                    stencil->setFlipY(true);
                    stencil->setFlipX(true);
                    stencil->setAnchorPoint(ccp(0.0f, 1.0f));
                    stencil->setPosition(ccp(CELL_WIDTH, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                
                if (isNoneCell(i + 1, j) && !isNoneCell(i + 1, j + 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge_landscape.png");
                    CCSprite* content = CCSprite::create("game/inside_landscape.png");
                    content->setFlipX(true);
                    content->setAnchorPoint(ccp(1.0f, 1.0f));
                    content->setPosition(ccp(cell->getContentSize().width - stencil->getContentSize().width, -stencil->getContentSize().width));
                    content->setVisible(true);
                    cell->addChild(content, 11);
                }
                if (isNoneCell(i + 1, j) && !isNoneCell(i + 1, j - 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge_landscape.png");
                    CCSprite* content = CCSprite::create("game/inside_landscape.png");
                    content->setAnchorPoint(ccp(0.0f, 1.0f));
                    content->setPosition(ccp(stencil->getContentSize().width, -stencil->getContentSize().width));
                    content->setVisible(true);
                    cell->addChild(content);
                }
                if (isNoneCell(i - 1, j) && !isNoneCell(i - 1, j - 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge_landscape.png");
                    CCSprite* content = CCSprite::create("game/inside_landscape.png");
                    content->setFlipY(true);
                    content->setAnchorPoint(ccp(0.0f, 0.0f));
                    if (IPHONE_4 || IPHONE_5)
                        content->setPosition(ccp(stencil->getContentSize().width, stencil->getContentSize().width + cell->getContentSize().height - 2));
                    else
                        content->setPosition(ccp(stencil->getContentSize().width, stencil->getContentSize().width + cell->getContentSize().height));
                    content->setVisible(true);
                    cell->addChild(content);
                }
                if (isNoneCell(i - 1, j) && !isNoneCell(i - 1, j + 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge_landscape.png");
                    CCSprite* content = CCSprite::create("game/inside_landscape.png");
                    content->setFlipY(true);
                    content->setFlipX(true);
                    content->setAnchorPoint(ccp(1.0f, 0.0f));
                    if (IPHONE_4 || IPHONE_5)
                        content->setPosition(ccp(cell->getContentSize().width - stencil->getContentSize().width,
                                                 cell->getContentSize().height + stencil->getContentSize().width - 2));
                    else
                        content->setPosition(ccp(cell->getContentSize().width - stencil->getContentSize().width,
                                                 cell->getContentSize().height + stencil->getContentSize().width));
                    
                    content->setVisible(true);
                    cell->addChild(content);
                }
                
                cell->visit();
                cell->setPosition(ccp(cell->getPositionX(), cell->getPositionY()));
                cell->setAnchorPoint(ccp(0.5f, 0.5f));
                fieldLayer->addChild(cell,2);
                cell->setOpacity(0);
                for (int n = 0; n < cell->getChildrenCount(); n++)
                    ((CCSprite*)cell->getChildren()->objectAtIndex(n))->setOpacity(0);
 
                gameFieldSprites[i][j] = cell;
                
                CCSprite* gor = CCSprite::create("game/gorizontal_landscape.png");
                gor->setAnchorPoint(ccp(0.0f, 0.0f));
                gor->setPosition(ccp(0.0f, cell->getContentSize().height));
                cell->addChild(gor, 1);
                //            gor->setColor(color);
                gor->setOpacity(0);
                gor->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.1f), CCFadeTo::create(0.2f, 120)));
                
                if (!isGor)
                    gor->setVisible(false);
                
                CCSprite* ver = CCSprite::create("game/vertical_landscape.png");
                ver->setAnchorPoint(ccp(0.0f, 0.0f));
                ver->setPosition(ccp(0.0f, 0.0f));
                cell->addChild(ver, 1);
                ver->setOpacity(0);
                ver->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.1f), CCFadeTo::create(0.2f, 90)));
                
                if (!isVer)
                    ver->setVisible(false);
                
                CCSprite* ice = NULL;
                if (gameField[i][j] == IceCell)
                {
                    iceCells.push_back(ccp(i,j));
                    ice = CCSprite::createWithSpriteFrameName("game/ice.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    cell->addChild(ice, 10);
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    ice->setTag(1);
                    iceCount++;
                }
                else if (gameField[i][j] == SuperIceCell)
                {
                    superIceCells.push_back(ccp(i,j));
                    ice = CCSprite::createWithSpriteFrameName("game/superIce.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    cell->addChild(ice, 10);
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    ice->setTag(1);
                    iceCount++;
                }
                else if (gameField[i][j] == MegaIceCell)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/megaIce.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    cell->addChild(ice, 12);
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    ice->setTag(1);
                    iceCount++;
                }
                else if (gameField[i][j] == StoneCell)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/stone.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    cell->addChild(ice, 12);
                    ice->setTag(3);
                }
                else if (gameField[i][j] == CageCell)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/reshetka.png");
                    ice->setPosition(cell->getPosition());
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    cell->getParent()->addChild(ice, 10);
                    cell->setUserData((void*)ice);
                    ice->setTag(3);
                }
                else if (gameField[i][j] == CageIceCell)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/reshetka.png");
                    ice->setPosition(cell->getPosition());
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    cell->getParent()->addChild(ice, 10);
                    cell->setUserData((void*)ice);
                    ice->setTag(3);
                    
                    iceCells.push_back(ccp(i,j));
                    CCSprite* ice2 = CCSprite::createWithSpriteFrameName("game/ice.png");
                    ice2->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    cell->addChild(ice2, 10);
                    ice2->setOpacity(0);
                    ice2->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    ice2->setTag(1);
                    
                    if (ice2)
                    {
                        ice2->setScaleX(0.95f);
                        ice2->setScaleY(1.05f);
                        if (IPAD_MINI)
                        {
                            ice2->setScaleX(0.95f);
                            ice2->setScaleY(1.05f);
                        }
                        else if (IPHONE_4 || IPHONE_5)
                        {
                            ice2->setScaleX(0.95f*IPHONE_MULTIPLIER);
                            ice2->setScaleY(1.05f*IPHONE_MULTIPLIER);
                        }
                    }
                    iceCount++;
                }
                else if (gameField[i][j] == Chocolate)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/chokolad.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    cell->addChild(ice, 12);
                    ice->setTag(4);
                }
                
                if (ice)
                {
                    ice->setScaleX(0.95f);
                    ice->setScaleY(1.05f);
                    if (IPAD_MINI)
                    {
                        ice->setScaleX(0.95f);
                        ice->setScaleY(1.05f);
                    }
                    else if (IPHONE_4 || IPHONE_5)
                    {
                        ice->setScaleX(0.95f*IPHONE_MULTIPLIER);
                        ice->setScaleY(1.05f*IPHONE_MULTIPLIER);
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < rowCount; i++)
        {
            if (cellType)
                cellType = false;
            else
                cellType = true;
            
            for (int j = 0; j < columnCount; j++)
            {
                if (cellType)
                    cellType = false;
                else
                    cellType = true;
                
                if (gameField[i][j] == NoneCell || gameField[i][j] == LockCell)
                    continue;
                
                CCSprite* cell = CCSprite::create("game/cell.png");
                
                CCPoint temp = ccp(j*CELL_WIDTH + xZero,
                                   yZero - i*CELL_HEIGHT);
                
                cell->setPosition(ccp(j*CELL_WIDTH + xZero,
                                      yZero - i*CELL_HEIGHT));
                cell->setAnchorPoint(ccp(0.5f, 1.0f));
                
                ccColor3B color;
                color.r = 0x51;
                color.g = 0x7f;
                color.b = 0x95;
                
                bool isVer = true;
                bool isGor = true;
                
                
                if ( i == rowCount-1 && gameType == BringDown)
                {
                    CCSprite* stencil = CCSprite::createWithSpriteFrameName("game/bringStudy.png");
                    stencil->setAnchorPoint(ccp(0.5f, 0.5f));
                    stencil->setPosition(ccp(CELL_WIDTH/2.0f, 0.0f));
                    stencil->setScale(0.4f);
                    stencil->setVisible(true);
                    cell->addChild(stencil, 5);
                }
                
                if (isNoneCell(i + 1, j) && gameType == BringDown)
                {
                    bool isNone = true;
                    
                    for (int k = i + 1; k < rowCount; k++)
                        if (!isNoneCell(k, j))
                            isNone = false;
                    
                    if (isNone)
                    {
                        CCSprite* stencil = CCSprite::createWithSpriteFrameName("game/bringStudy.png");
                        stencil->setAnchorPoint(ccp(0.5f, 0.5f));
                        stencil->setPosition(ccp(CELL_WIDTH/2.0f, 0.0f));
                        stencil->setScale(0.4f);
                        stencil->setVisible(true);
                        cell->addChild(stencil, 5);
                    }
                }
                
                
                if (isNoneCell(i - 1, j))
                {
                    isGor = false;
                    CCSprite* stencil = CCSprite::create("game/edge.png");
                    stencil->setRotation(90.0f);
                    stencil->setFlipX(true);
                    stencil->setAnchorPoint(ccp(1.0f, 0.0f));
                    stencil->setPosition(ccp(0.0f, CELL_HEIGHT));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                if (isNoneCell(i + 1, j))
                {
                    CCSprite* stencil = CCSprite::create("game/edge.png");
                    stencil->setRotation(90.0f);
                    stencil->setAnchorPoint(ccp(0.0f, 0.0f));
                    stencil->setPosition(ccp(0.0f, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                if (isNoneCell(i, j - 1))
                {
                    isVer = false;
                    CCSprite* stencil = CCSprite::create("game/edge.png");
                    stencil->setScaleY(1.1f);
                    stencil->setFlipX(true);
                    stencil->setAnchorPoint(ccp(1.0f, 0.0f));
                    stencil->setPosition(ccp(0.0f, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                if (isNoneCell(i, j + 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge.png");
                    stencil->setScaleY(1.1f);
                    stencil->setAnchorPoint(ccp(0.0f, 0.0f));
                    stencil->setPosition(ccp(CELL_WIDTH, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                
                if (isNoneCell(i - 1, j - 1) && isNoneCell(i - 1, j) && isNoneCell(i, j - 1))
                {
                    CCSprite* stencil = CCSprite::create("game/outside.png");
                    stencil->setAnchorPoint(ccp(1.0f, 0.0f));
                    if (IPHONE_4 || IPHONE_5)
                        stencil->setPosition(ccp(0.0f, cell->getContentSize().height-2));
                    else
                        stencil->setPosition(ccp(0.0f, cell->getContentSize().height));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                
                if (isNoneCell(i - 1, j + 1) && isNoneCell(i - 1, j) && isNoneCell(i, j + 1))
                {
                    CCSprite* stencil = CCSprite::create("game/outside.png");
                    stencil->setFlipX(true);
                    stencil->setAnchorPoint(ccp(0.0f, 0.0f));
                    stencil->setPosition(ccp(CELL_WIDTH, CELL_HEIGHT));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                
                if (isNoneCell(i, j - 1) && isNoneCell(i + 1, j - 1) && isNoneCell(i + 1, j))
                {
                    CCSprite* stencil = CCSprite::create("game/outside.png");
                    stencil->setFlipY(true);
                    stencil->setAnchorPoint(ccp(1.0f, 1.0f));
                    stencil->setPosition(ccp(0.0f, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                if (isNoneCell(i, j + 1) && isNoneCell(i + 1, j + 1) && isNoneCell(i + 1, j))
                {
                    CCSprite* stencil = CCSprite::create("game/outside.png");
                    stencil->setFlipY(true);
                    stencil->setFlipX(true);
                    stencil->setAnchorPoint(ccp(0.0f, 1.0f));
                    stencil->setPosition(ccp(CELL_WIDTH, 0.0f));
                    stencil->setVisible(true);
                    cell->addChild(stencil);
                }
                
                if (isNoneCell(i + 1, j) && !isNoneCell(i + 1, j + 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge.png");
                    CCSprite* content = CCSprite::create("game/inside.png");
                    content->setFlipX(true);
                    content->setAnchorPoint(ccp(1.0f, 1.0f));
                    content->setPosition(ccp(cell->getContentSize().width - stencil->getContentSize().width, -stencil->getContentSize().width));
                    content->setVisible(true);
                    cell->addChild(content, 11);
                }
                if (isNoneCell(i + 1, j) && !isNoneCell(i + 1, j - 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge.png");
                    CCSprite* content = CCSprite::create("game/inside.png");
                    content->setAnchorPoint(ccp(0.0f, 1.0f));
                    content->setPosition(ccp(stencil->getContentSize().width, -stencil->getContentSize().width));
                    content->setVisible(true);
                    cell->addChild(content);
                }
                if (isNoneCell(i - 1, j) && !isNoneCell(i - 1, j - 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge.png");
                    CCSprite* content = CCSprite::create("game/inside.png");
                    content->setFlipY(true);
                    content->setAnchorPoint(ccp(0.0f, 0.0f));
                    if (IPHONE_4 || IPHONE_5)
                        content->setPosition(ccp(stencil->getContentSize().width, stencil->getContentSize().width + cell->getContentSize().height - 2));
                    else
                        content->setPosition(ccp(stencil->getContentSize().width, stencil->getContentSize().width + cell->getContentSize().height));
                    content->setVisible(true);
                    cell->addChild(content);
                }
                if (isNoneCell(i - 1, j) && !isNoneCell(i - 1, j + 1))
                {
                    CCSprite* stencil = CCSprite::create("game/edge.png");
                    CCSprite* content = CCSprite::create("game/inside.png");
                    content->setFlipY(true);
                    content->setFlipX(true);
                    content->setAnchorPoint(ccp(1.0f, 0.0f));
                    if (IPHONE_4 || IPHONE_5)
                        content->setPosition(ccp(cell->getContentSize().width - stencil->getContentSize().width,
                                                 cell->getContentSize().height + stencil->getContentSize().width - 2));
                    else
                        content->setPosition(ccp(cell->getContentSize().width - stencil->getContentSize().width,
                                                 cell->getContentSize().height + stencil->getContentSize().width));
                    
                    content->setVisible(true);
                    cell->addChild(content);
                }
                
                cell->visit();
                cell->setPosition(ccp(cell->getPositionX(), cell->getPositionY()));
                cell->setAnchorPoint(ccp(0.5f, 0.5f));
                fieldLayer->addChild(cell,2);
                cell->setOpacity(0);
                for (int n = 0; n < cell->getChildrenCount(); n++)
                    ((CCSprite*)cell->getChildren()->objectAtIndex(n))->setOpacity(0);
                
                gameFieldSprites[i][j] = cell;
                
                CCSprite* gor = CCSprite::create("game/gorizontal.png");
                gor->setAnchorPoint(ccp(0.0f, 0.0f));
                gor->setPosition(ccp(0.0f, cell->getContentSize().height));
                cell->addChild(gor, 1);
                //            gor->setColor(color);
                gor->setOpacity(0);
                gor->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.1f), CCFadeTo::create(0.2f, 120)));
                
                if (!isGor)
                    gor->setVisible(false);
                
                CCSprite* ver = CCSprite::create("game/vertical.png");
                ver->setAnchorPoint(ccp(0.0f, 0.0f));
                ver->setPosition(ccp(0.0f, 0.0f));
                cell->addChild(ver, 1);
                ver->setOpacity(0);
                ver->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.1f), CCFadeTo::create(0.2f, 90)));
                
                if (!isVer)
                    ver->setVisible(false);
                
                CCSprite* ice = NULL;
                if (gameField[i][j] == IceCell)
                {
                    iceCells.push_back(ccp(i,j));
                    ice = CCSprite::createWithSpriteFrameName("game/ice.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    cell->addChild(ice, 10);
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    ice->setTag(1);
                    iceCount++;
                }
                else if (gameField[i][j] == SuperIceCell)
                {
                    superIceCells.push_back(ccp(i,j));
                    ice = CCSprite::createWithSpriteFrameName("game/superIce.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    cell->addChild(ice, 10);
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    ice->setTag(1);
                    iceCount++;
                }
                else if (gameField[i][j] == MegaIceCell)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/megaIce.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    cell->addChild(ice, 12);
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    ice->setTag(1);
                    iceCount++;
                }
                else if (gameField[i][j] == StoneCell)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/stone.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    cell->addChild(ice, 12);
                    ice->setTag(3);
                }
                else if (gameField[i][j] == CageCell)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/reshetka.png");
                    ice->setPosition(cell->getPosition());
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    cell->getParent()->addChild(ice, 10);
                    cell->setUserData((void*)ice);
                    ice->setTag(3);
                }
                else if (gameField[i][j] == CageIceCell)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/reshetka.png");
                    ice->setPosition(cell->getPosition());
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    cell->getParent()->addChild(ice, 10);
                    cell->setUserData((void*)ice);
                    ice->setTag(3);
                    
                    iceCells.push_back(ccp(i,j));
                    CCSprite* ice2 = CCSprite::createWithSpriteFrameName("game/ice.png");
                    ice2->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    cell->addChild(ice2, 10);
                    ice2->setOpacity(0);
                    ice2->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    ice2->setTag(1);
                    
                    if (ice2)
                    {
                        ice2->setScaleX(0.95f);
                        ice2->setScaleY(1.05f);
                        if (IPAD_MINI)
                        {
                            ice2->setScaleX(0.95f);
                            ice2->setScaleY(1.05f);
                        }
                        else if (IPHONE_4 || IPHONE_5)
                        {
                            ice2->setScaleX(0.95f*IPHONE_MULTIPLIER);
                            ice2->setScaleY(1.05f*IPHONE_MULTIPLIER);
                        }
                    }
                    
                    iceCount++;
                }
                else if (gameField[i][j] == Chocolate)
                {
                    ice = CCSprite::createWithSpriteFrameName("game/chokolad.png");
                    ice->setPosition(ccp(cell->getContentSize().width/2.0f, cell->getContentSize().height/2.0f));
                    ice->setOpacity(0);
                    ice->runAction(CCSequence::createWithTwoActions(CCDelayTime::create(2.0f), CCFadeIn::create(0.2f)));
                    cell->addChild(ice, 12);
                    ice->setTag(4);
                }
                
                if (ice)
                {
                    ice->setScaleX(0.95f);
                    ice->setScaleY(1.05f);
                    if (IPAD_MINI)
                    {
                        ice->setScaleX(0.95f);
                        ice->setScaleY(1.05f);
                    }
                    else if (IPHONE_4 || IPHONE_5)
                    {
                        ice->setScaleX(0.95f*IPHONE_MULTIPLIER);
                        ice->setScaleY(1.05f*IPHONE_MULTIPLIER);
                    }
                }
            }
        }
    }
	renderTex->end();

    CCImage* image = renderTex->newCCImage();
	texForField = new CCTexture2D();
	texForField->initWithImage(image);
    renderTex->release();
    
    image->release();
    
	field = CCSprite::createWithTexture(texForField, CCRect(0, 0, WINSIZE.width, WINSIZE.height));
    field->setAnchorPoint(ccp(0.5f, 0.5f - (CELL_HEIGHT/2.0f)/WINSIZE.height));
    field->setPosition(ccp(CCDirector::sharedDirector()->getWinSize().width/2.0f, CCDirector::sharedDirector()->getWinSize().height/2.0f));
    ccColor3B colorF;
    colorF.r = 37;
    colorF.g = 118;
    colorF.b = 176;
    field->setColor(colorF);
	field->setOpacity(155);
    texForField->release();
    
    fieldLayer->addChild(field, 1);

    
    iceTarget = iceCount;
}

void GameScene::menuCloseCallback(CCObject* pSender)
{    
    CCDirector::sharedDirector()->end();
}


