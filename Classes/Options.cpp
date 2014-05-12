#include "Options.h"
#include "cGlobal.h"
#include "cocos2d.h"
#include "nMMP.h"
#include "cFacebook.h"


using namespace cocos2d;

Options::Options()
{
	fileSystem = new FileSystem();
	_useSounds = true;
	_useMusic = true;
	currentLevel = 30;
    firstGame = true;
	for (int i = 0; i < LEVEL_COUNT; i++)
	{
		levels[i].countScore = 0;
        levels[i].targetScore = 0;
		levels[i].countStar = 0;
		levels[i].levelType = Score;
        levels[i].lock = false;
        levels[i].countGames = 0;
        levels[i].lastGame = 0;
        levels[i].isSimple = false;
	}
    levels[24].lock = true;
    levels[36].lock = true;
    levels[48].lock = true;
    levels[60].lock = true;
    levels[72].lock = true;
}

Options::~Options()
{
    
}

int	Options::getCurrentLevel()
{
	return currentLevel;
}

void Options::restoreCurrentLevel(int nCurrentLevel)
{
    currentLevel = nCurrentLevel;
}

void Options::setCurrentLevel(int nCurrentLevel)
{
	currentLevel = nCurrentLevel;
    FacebookPtr->unlockNewLevel(currentLevel);
}

void Options::setLevelData(int idx, int nCountStar, int nCountScore, eLevelType nLevelType, bool lock, int nCountGame, time_t nLastGame, bool nIsSimple)
{
	levels[idx].countStar = nCountStar;
	levels[idx].countScore = nCountScore;
	levels[idx].levelType = nLevelType;
    levels[idx].lock = lock;
    if (nCountGame != 0)
        levels[idx].countGames = nCountGame;
    if (nLastGame != 0)
        levels[idx].lastGame = nLastGame;
    if (nIsSimple)
        levels[idx].isSimple = nIsSimple;
}

sLevelData Options::getLevelData(int idx)
{
	return levels[idx];
}

void Options::setLifeCount(int nLife)
{
    currentLife = nLife;
}

int Options::getLifeCount()
{
    return currentLife;
}

void Options::setCrystalCount(int nCrystal)
{
    crystalCount = nCrystal;
}

int Options::getCrystalCOunt()
{
    return crystalCount;
}

void Options::setBombCount(int nCrystal)
{
    bombCount = nCrystal;
}

int Options::getBombCount()
{
    return bombCount;
}

void Options::setFishCount(int nCrystal)
{
    fishCount = nCrystal;
}

int Options::getFishCount()
{
    return fishCount;
}

void Options::setDonuthCount(int nCrystal)
{
    donutCount = nCrystal;
}

int Options::getDonutCount()
{
    return donutCount;
}

void Options::setLastTime(long nTime)
{
    lastTime = nTime;
}

long Options::getLastTime()
{
    return lastTime;
}

void Options::setFirstGame()
{
    firstGame = false;
}

bool Options::getFirstGame()
{
    return firstGame;
}

void Options::setHammerCount(int nCrystal)
{
    hammerCount = nCrystal;
}

int Options::getHammerCount()
{
    return hammerCount;
}

int Options::getLastGameLevel()
{
    return lastGameLevel;
}

void Options::setLastGameLevel(int nlevel)
{
    lastGameLevel = nlevel;
}

bool Options::getFirstPayLife()
{
    return firstPayLife;
}

bool Options::getFirstPayCrystal()
{
    return firstPayCrystal;
}

bool Options::getFirstPayBomb()
{
    return firstPayBomb;
}

bool Options::getFirstPayFish()
{
    return firstPayFish;
}

bool Options::getFirstPayHammer()
{
    return firstPayHammer;
}

bool Options::getFirstPayMove()
{
    return firstPayMove;
}

void Options::setFirstPayLife()
{
    firstPayLife = false;
}

void Options::setFirstPayCrystal()
{
    firstPayCrystal = false;
}

void Options::setFirstPayBomb()
{
    firstPayBomb = false;
}

void Options::setFirstPayFish()
{
    firstPayFish = false;
}

void Options::setFirstPayHammer()
{
    firstPayHammer = false;
}

void Options::setFirstPayMove()
{
    firstPayMove = false;
}

void Options::setNotification(bool music)
{
    _notif = music;
}

bool Options::isNotification()
{
    return _notif;
}

void Options::setFacebookConnection(bool music)
{
    _facebook = music;
}

bool Options::isFacebookConnection()
{
    return _facebook;
}

void Options::setJubPost()
{
    _jubPost = true;
}

bool Options::getJubPost()
{
    return _jubPost;
}

void Options::setTutorialPost()
{
    _tutorialPost = true;
}

bool Options::getTutorialPost()
{
    return  _tutorialPost;
}

void Options::setUnlockPost()
{
    _unlockPost = true;
}

bool Options::getUnlockPost()
{
    return _unlockPost;
}

void Options::setEndPost()
{
    _endPost = true;
}

bool Options::getEndPost()
{
    return _endPost;
}

void Options::setUnclock(int numLevel, bool isLock)
{
    levels[numLevel].lock = isLock;
}

bool Options::getUnlock(int numLevel)
{
    return levels[numLevel].lock;
}

void Options::load()
{
    _useSounds = CCUserDefault::sharedUserDefault()->getBoolForKey("sound", true);
    _useMusic = CCUserDefault::sharedUserDefault()->getBoolForKey("music", true);
    _notif = CCUserDefault::sharedUserDefault()->getBoolForKey("notif", false);
    _facebook = CCUserDefault::sharedUserDefault()->getBoolForKey("facebook", false);
    currentLevel = CCUserDefault::sharedUserDefault()->getIntegerForKey("currentLevel", 1);
    currentLife = CCUserDefault::sharedUserDefault()->getIntegerForKey("currentLife", 5);
    crystalCount = CCUserDefault::sharedUserDefault()->getIntegerForKey("crystalCount", 0);
    bombCount = CCUserDefault::sharedUserDefault()->getIntegerForKey("bombCount", 0);
    fishCount = CCUserDefault::sharedUserDefault()->getIntegerForKey("fishCount", 0);
    donutCount = CCUserDefault::sharedUserDefault()->getIntegerForKey("donutCount", 0);
    lastTime = CCUserDefault::sharedUserDefault()->getFloatForKey("lastTime", 0);
    firstGame = CCUserDefault::sharedUserDefault()->getBoolForKey("firstGame", true);
    hammerCount = CCUserDefault::sharedUserDefault()->getIntegerForKey("hammerCount", 0);
    lastGameLevel = CCUserDefault::sharedUserDefault()->getIntegerForKey("lastGameLevel", 1);
    launchCount = CCUserDefault::sharedUserDefault()->getIntegerForKey("launchCount", 0);
    
    firstPayMove = CCUserDefault::sharedUserDefault()->getBoolForKey("payMove", true);
    firstPayLife = CCUserDefault::sharedUserDefault()->getBoolForKey("payLife", true);
    firstPayHammer = CCUserDefault::sharedUserDefault()->getBoolForKey("payHammer", true);
    firstPayFish = CCUserDefault::sharedUserDefault()->getBoolForKey("payFish", true);
    firstPayCrystal = CCUserDefault::sharedUserDefault()->getBoolForKey("payCrystal", true);
    firstPayBomb = CCUserDefault::sharedUserDefault()->getBoolForKey("payBomb", true);
    _jubPost = CCUserDefault::sharedUserDefault()->getBoolForKey("jubPost", false);
    _tutorialPost = CCUserDefault::sharedUserDefault()->getBoolForKey("tutorialPost", false);
    _unlockPost = CCUserDefault::sharedUserDefault()->getBoolForKey("unlockPost", false);
    _endPost = CCUserDefault::sharedUserDefault()->getBoolForKey("endPost", false);
    
    int countRequest = CCUserDefault::sharedUserDefault()->getIntegerForKey("requestCount", 0);
    appRequests.clear();
    for (int i = 0; i < countRequest; i++)
    {
        sRequestData request;
        char buf[255];
        sprintf(buf, "requestId_%d", i);
        request.id = CCUserDefault::sharedUserDefault()->getStringForKey(buf, "");
        sprintf(buf, "requestFrom_%d", i);
        request.from = CCUserDefault::sharedUserDefault()->getStringForKey(buf, "");
        sprintf(buf, "requestFromName_%d", i);
        request.fromName = CCUserDefault::sharedUserDefault()->getStringForKey(buf, "");
        sprintf(buf, "requestType_%d", i);
        request.type = (NotificationType)CCUserDefault::sharedUserDefault()->getIntegerForKey(buf, 0);
        sprintf(buf, "requestAccept_%d", i);
        request.accept = CCUserDefault::sharedUserDefault()->getBoolForKey(buf, 0);
        appRequests.push_back(request);
    }
    
    for (int i = 0; i < LEVEL_COUNT; i++)
	{
        char buf[255];
        sprintf(buf, "countStar_%d", i);
		levels[i].countStar = CCUserDefault::sharedUserDefault()->getIntegerForKey(buf, 0);
        sprintf(buf, "countScore_%d", i);
		levels[i].countScore = CCUserDefault::sharedUserDefault()->getIntegerForKey(buf, 0);
        sprintf(buf, "levelType_%d", i);
		levels[i].levelType = (eLevelType)CCUserDefault::sharedUserDefault()->getIntegerForKey(buf, 0);
        
        sprintf(buf, "lock_%d", i);
        
        bool isStageUnlocked = false;
        
        if (i == 24  || i == 36 || i == 48 || i == 60 || i == 72)
            isStageUnlocked = CCUserDefault::sharedUserDefault()->getBoolForKey(buf, true);
        else
            isStageUnlocked = CCUserDefault::sharedUserDefault()->getBoolForKey(buf, false);

        levels[i].lock = isStageUnlocked;
        
        sprintf(buf, "countGames_%d", i);
        levels[i].countGames = CCUserDefault::sharedUserDefault()->getIntegerForKey(buf, 0);
        
        sprintf(buf, "lastTime_%d", i);
        string lastGame = CCUserDefault::sharedUserDefault()->getStringForKey(buf, "0");
        levels[i].lastGame = atol(lastGame.c_str());
        
        sprintf(buf, "isSimple_%d", i);
        levels[i].isSimple = CCUserDefault::sharedUserDefault()->getBoolForKey(buf, false);
	}
    
    levels[0].levelType = Score;
    levels[0].targetScore = 100;
    
    levels[1].levelType = Score;
    levels[1].targetScore = 1000;

    
    levels[2].levelType = Score;
    levels[2].targetScore = 500;

    
    levels[3].levelType = Score;
    levels[3].targetScore = 2000;

    
    levels[4].levelType = Score;
    levels[4].targetScore = 600;

    
    levels[5].levelType = Ice;
    levels[5].targetScore = 4500;

    
    levels[6].levelType = Ice;
    levels[6].targetScore = 1000;

    
    levels[7].levelType = Ice;
    levels[7].targetScore = 8000;

    
    levels[8].levelType = Ice;
    levels[8].targetScore = 15000;
    
    levels[9].levelType = Ice;
    levels[9].targetScore = 35000;
    
    levels[10].levelType = BringDown;
    levels[10].targetScore = 500;
    
    levels[11].levelType = BringDown;
    levels[11].targetScore = 10000;
    
    levels[12].levelType = Ice;
    levels[12].targetScore = 10000;
    
    levels[13].levelType = BringDown;
    levels[13].targetScore = 30000;
    
    levels[14].levelType = BringDown;
    levels[14].targetScore = 5000;
    
    levels[15].levelType = Ice;
    levels[15].targetScore = 15000;

    
    levels[16].levelType = Ice;
    levels[16].targetScore = 35000;
    
    levels[17].levelType = Ice;
    levels[17].targetScore = 30000;
    
    levels[18].levelType = BringDown;
    levels[18].targetScore = 30000;
    
    levels[19].levelType = Time;
    levels[19].targetScore = 5000;
    
    levels[20].levelType = Ice;
    levels[20].targetScore = 35000;
    
    levels[21].levelType = BringDown;
    levels[21].targetScore = 3000;

    levels[22].levelType = Ice;
    levels[22].targetScore = 15000;

    
    levels[23].levelType = Ice;
    levels[23].targetScore = 15000;

    
    levels[24].levelType = BringDown;
    levels[24].targetScore = 8000;

    
    levels[25].levelType = Time;
    levels[25].targetScore = 15000;
    
    levels[26].levelType = Ice;
    levels[26].targetScore = 20000;
    
    levels[27].levelType = Ice;
    levels[27].targetScore = 20000;
    
    levels[28].levelType = Ice;
    levels[28].targetScore = 30000;

    levels[29].levelType = BringDown;
    levels[29].targetScore = 30000;
    
    levels[30].levelType = Ice;
    levels[30].targetScore = 20000;
    
    levels[31].levelType = Ice;
    levels[31].targetScore = 10000;
    
    levels[32].levelType = BringDown;
    levels[32].targetScore = 3000;
    
    levels[33].levelType = Ice;
    levels[33].targetScore = 20000;
    
    levels[34].levelType = Ice;
    levels[34].targetScore = 20000;
    
    levels[35].levelType = Ice;
    levels[35].targetScore = 10000;
    
    levels[36].levelType = Ice;
    levels[36].targetScore = 20000;
    
    levels[37].levelType = Ice;
    levels[37].targetScore = 25000;

    levels[38].levelType = BringDown;
    levels[38].targetScore = 2000;
    
    levels[39].levelType = Ice;
    levels[39].targetScore = 15000;
    
    levels[40].levelType = Ice;
    levels[40].targetScore = 40000;
    
    levels[41].levelType = Score;
    levels[41].targetScore = 70000;
    
    levels[42].levelType = BringDown;
    levels[42].targetScore = 1500;
    
    levels[43].levelType = Ice;
    levels[43].targetScore = 10000;
    
    levels[44].levelType = Ice;
    levels[44].targetScore = 10000;
    
    levels[45].levelType = Ice;
    levels[45].targetScore = 25000;
    
    levels[46].levelType = BringDown;
    levels[46].targetScore = 2000;
    
    levels[47].levelType = Score;
    levels[47].targetScore = 15000;
    
    levels[48].levelType = Ice;
    levels[48].targetScore = 30000;
    
    levels[49].levelType = BringDown;
    levels[49].targetScore = 12000;
    
    levels[50].levelType = BringDown;
    levels[50].targetScore = 40000;

    levels[51].levelType = BringDown;
    levels[51].targetScore = 20000;

    levels[52].levelType = Ice;
    levels[52].targetScore = 15000;

    levels[53].levelType = Ice;
    levels[53].targetScore = 15000;
    
    levels[54].levelType = Ice;
    levels[54].targetScore = 20000;
    
    levels[55].levelType = BringDown;
    levels[55].targetScore = 10000;

    levels[56].levelType = BringDown;
    levels[56].targetScore = 15000;

    levels[57].levelType = Ice;
    levels[57].targetScore = 20000;

    levels[58].levelType = BringDown;
    levels[58].targetScore = 5000;
    
    levels[59].levelType = Ice;
    levels[59].targetScore = 10000;
    
    levels[60].levelType = Time;
    levels[60].targetScore = 8000;

    levels[61].levelType = BringDown;
    levels[61].targetScore = 50000;

    levels[62].levelType = Ice;
    levels[62].targetScore = 10000;
    
    levels[63].levelType = BringDown;
    levels[63].targetScore = 15000;
    
    levels[64].levelType = Ice;
    levels[64].targetScore = 40000;

    levels[65].levelType = Time;
    levels[65].targetScore = 20000;

    levels[66].levelType = Ice;
    levels[66].targetScore = 40000;

    levels[67].levelType = BringDown;
    levels[67].targetScore = 5000;

    levels[68].levelType = Ice;
    levels[68].targetScore = 40000;
    
    levels[69].levelType = Ice;
    levels[69].targetScore = 20000;
    
    levels[70].levelType = Ice;
    levels[70].targetScore = 20000;
    
    levels[71].levelType = BringDown;
    levels[71].targetScore = 8000;
    
    levels[72].levelType = Ice;
    levels[72].targetScore = 40000;
    
    levels[73].levelType = Ice;
    levels[73].targetScore = 40000;
    
    levels[74].levelType = Ice;
    levels[74].targetScore = 60000;
    
    levels[75].levelType = BringDown;
    levels[75].targetScore = 13000;
    
    levels[76].levelType = Ice;
    levels[76].targetScore = 40000;
    
    levels[77].levelType = BringDown;
    levels[77].targetScore = 40000;
    
    levels[78].levelType = Ice;
    levels[78].targetScore = 90000;
    
    levels[79].levelType = Time;
    levels[79].targetScore = 10000;
    
    levels[80].levelType = Ice;
    levels[80].targetScore = 40000;
    
    levels[81].levelType = Ice;
    levels[81].targetScore = 45000;
    
    levels[82].levelType = Ice;
    levels[82].targetScore = 45000;
    
    levels[83].levelType = Ice;
    levels[83].targetScore = 30000;

    
    levels[106].levelType = Ice;
    levels[106].targetScore = 50000;
    
    levels[107].levelType = Ice;
    levels[107].targetScore = 15000;
    
    levels[108].levelType = Ice;
    levels[108].targetScore = 20000;
    
    levels[109].levelType = Ice;
    levels[109].targetScore = 15000;
    
    levels[110].levelType = Ice;
    levels[110].targetScore = 20000;
    
    levels[111].levelType = Ice;
    levels[111].targetScore = 25000;
}

void Options::save()
{
    CCUserDefault::sharedUserDefault()->setBoolForKey("sound", _useSounds);
    CCUserDefault::sharedUserDefault()->setBoolForKey("music", _useMusic);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("currentLevel", currentLevel);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("currentLife", currentLife);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("crystalCount", crystalCount);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("bombCount", bombCount);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("fishCount", fishCount);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("donutCount", donutCount);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("lastTime", lastTime);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("firstGame", firstGame);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("hammerCount", hammerCount);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("lastGameLevel", lastGameLevel);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("launchCount", launchCount);

    
    CCUserDefault::sharedUserDefault()->setBoolForKey("payMove", firstPayMove);
    CCUserDefault::sharedUserDefault()->setBoolForKey("payLife", firstPayLife);
    CCUserDefault::sharedUserDefault()->setBoolForKey("payHammer", firstPayHammer);
    CCUserDefault::sharedUserDefault()->setBoolForKey("payFish", firstPayFish);
    CCUserDefault::sharedUserDefault()->setBoolForKey("payCrystal", firstPayCrystal);
    CCUserDefault::sharedUserDefault()->setBoolForKey("payBomb", firstPayBomb);
    CCUserDefault::sharedUserDefault()->setBoolForKey("notif", _notif);
    CCUserDefault::sharedUserDefault()->setBoolForKey("facebook", _facebook);
    CCUserDefault::sharedUserDefault()->setBoolForKey("jubPost", _jubPost);
    CCUserDefault::sharedUserDefault()->setBoolForKey("tutorialPost", _tutorialPost);
    CCUserDefault::sharedUserDefault()->setBoolForKey("unlockPost", _unlockPost);
    CCUserDefault::sharedUserDefault()->setBoolForKey("endPost", _endPost);
    
    CCUserDefault::sharedUserDefault()->setIntegerForKey("requestCount", appRequests.size());
    
    for (int i = 0; i < appRequests.size(); i++)
    {
        char buf[255];
        sprintf(buf, "requestId_%d", i);
        CCUserDefault::sharedUserDefault()->setStringForKey(buf, appRequests[i].id);
        sprintf(buf, "requestFrom_%d", i);
        CCUserDefault::sharedUserDefault()->setStringForKey(buf, appRequests[i].from);
        sprintf(buf, "requestFromName_%d", i);
        CCUserDefault::sharedUserDefault()->setStringForKey(buf, appRequests[i].fromName);
        sprintf(buf, "requestType_%d", i);
        CCUserDefault::sharedUserDefault()->setIntegerForKey(buf, (NotificationType)appRequests[i].type);
        sprintf(buf, "requestAccept_%d", i);
        CCUserDefault::sharedUserDefault()->setBoolForKey(buf, appRequests[i].accept);
    }

    
    for (int i = 0; i < LEVEL_COUNT; i++)
	{
        char buf[255];
        sprintf(buf, "countStar_%d", i);
		CCUserDefault::sharedUserDefault()->setIntegerForKey(buf, levels[i].countStar);
        sprintf(buf, "countScore_%d", i);
        CCUserDefault::sharedUserDefault()->setIntegerForKey(buf, levels[i].countScore);
        sprintf(buf, "levelType_%d", i);
        CCUserDefault::sharedUserDefault()->setIntegerForKey(buf, (int)levels[i].levelType);
        sprintf(buf, "lock_%d", i);
        CCUserDefault::sharedUserDefault()->setBoolForKey(buf, levels[i].lock);
        sprintf(buf, "countGames_%d", i);
        CCUserDefault::sharedUserDefault()->setIntegerForKey(buf, levels[i].countGames);
        sprintf(buf, "lastTime_%d", i);
        char bufLastGame[255];
        sprintf(bufLastGame, "%ld", levels[i].lastGame);
        CCUserDefault::sharedUserDefault()->setStringForKey(buf, bufLastGame);
        sprintf(buf, "isSimple_%d", i);
        CCUserDefault::sharedUserDefault()->setBoolForKey(buf, levels[i].isSimple);
	}

    CCUserDefault::sharedUserDefault()->flush();
}

bool Options::useMusic()
{
	return _useMusic;
}

bool Options::useSounds()
{
	return _useSounds;
}

void Options::setUseMusic(bool music)
{
	_useMusic = music;
}
void Options::setUseSounds(bool music)
{
	_useSounds = music;
}
