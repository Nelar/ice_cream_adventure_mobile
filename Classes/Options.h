#ifndef _OPTIONS_
#define _OPTIONS_

#include "FileSystem.h"
#include "cSingleton.h"
#include <vector>
#include "NotificationTypes.h"
#include <ctime>

#define LEVEL_COUNT 120

enum eLevelType
{
	Score = 0,
	Time = 1,
	Ice = 2,
	BringDown = 3,
	CountLevelType = 4
};

struct sLevelData
{
	int countStar;
	int countScore;
	eLevelType levelType;
    int targetScore;
    bool lock;
    int countGames;
    time_t lastGame;
    bool isSimple;
};

struct sRequestData
{
	string id;
    string from;
    string fromName;
    NotificationType type;
    bool accept = false;
};

class Options : public cSingleton<Options>
{
public:
	Options();
	~Options();

	void load();
	void save();
	bool useMusic();
	bool useSounds();
	void setUseMusic(bool music);
	void setUseSounds(bool music);
    
    void setNotification(bool music);
    bool isNotification();
    
    void setFacebookConnection(bool music);
    bool isFacebookConnection();


	int	getCurrentLevel();
	void setCurrentLevel(int nCurrentLevel);
    void restoreCurrentLevel(int nCurrentLevel);


	void setLevelData(int idx, int nCountStar, int nCountScore, eLevelType nLevelType, bool lock = false, int nCountGame = 0, time_t nLastGame = 0, bool nIsSimple = false);
	sLevelData getLevelData(int idx);
    
    void setLifeCount(int nLife);
    int getLifeCount();
    
    void setCrystalCount(int nCrystal);
    int getCrystalCOunt();
    
    void setBombCount(int nCrystal);
    int getBombCount();
    
    void setFishCount(int nCrystal);
    int getFishCount();
    
    void setDonuthCount(int nCrystal);
    int getDonutCount();

    void setHammerCount(int nCrystal);
    int getHammerCount();

    
    void setLastTime(long nTime);
    long getLastTime();
    
    
    
    void setFirstGame();
    bool getFirstGame();
    
    int getLastGameLevel();
    void setLastGameLevel(int nlevel);
    
    bool getFirstPayLife();
    bool getFirstPayCrystal();
    bool getFirstPayBomb();
    bool getFirstPayFish();
    bool getFirstPayHammer();
    bool getFirstPayMove();
    
    void setFirstPayLife();
    void setFirstPayCrystal();
    void setFirstPayBomb();
    void setFirstPayFish();
    void setFirstPayHammer();
    void setFirstPayMove();
    
    void setJubPost();
    bool getJubPost();
    
    void setTutorialPost();
    bool getTutorialPost();
    
    void setUnlockPost();
    bool getUnlockPost();
    
    void setEndPost();
    bool getEndPost();
    
    void setUnclock(int numLevel, bool isLock);
    bool getUnlock(int numLevel);
    
    vector<sRequestData> appRequests;
    
    int launchCount = 0;

private:

    int lastGameLevel = 1;
    bool firstGame;
	bool _useSounds;
	bool _useMusic;
    bool _notif;
    bool _facebook;
    bool _jubPost = false;
    bool _tutorialPost = false;
    bool _unlockPost = false;
    bool _endPost = false;
    
    
	FileSystem* fileSystem;
	sLevelData levels[LEVEL_COUNT];
    
    int currentLevel;
    int currentLife;
    
    int crystalCount;
    int bombCount;
    int fishCount;
    int donutCount;
    int hammerCount;
    long lastTime;
    
    bool firstPayLife = true;
    bool firstPayCrystal  = true;
    bool firstPayBomb = true;
    bool firstPayFish = true;
    bool firstPayHammer = true;
    bool firstPayMove = true;
};

#define OptionsPtr	Options::GetSingletonPtr()

#endif