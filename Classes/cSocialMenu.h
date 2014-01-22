#ifndef __SOCIAL_SCENE_H__
#define __SOCIAL_SCENE_H__

#include "cocos2d.h"
#include <vector>
#include <GUI/CCScrollView/CCScrollView.h>
#include "NotificationTypes.h"

using namespace std;
using namespace cocos2d;
using namespace extension;


struct ScoreboardCell
{
    string name;
    string uid;
    int score;
    
    const bool operator > (const ScoreboardCell& rv) const {
        return (score > rv.score);
    }
    const bool operator < (const ScoreboardCell& rv) const {
        return (score < rv.score);
    }
    const bool operator >= (const ScoreboardCell& rv) const {
        return (score >= rv.score);
    }
    const bool operator <= (const ScoreboardCell& rv) const {
        return (score <= rv.score);
    }
    const bool operator == (const ScoreboardCell& rv) const {
        return (score == rv.score);
    }
    
    static bool comp(const ScoreboardCell& f1, const ScoreboardCell& f2)
    {
        return f1.score > f2.score;
    }
};

class SocialLayer : public cocos2d::CCLayer
{
public:
    
    virtual ~SocialLayer();
	virtual bool init();
    
	CREATE_FUNC(SocialLayer);
    
    virtual void changeOrientation();
    
    void showScoreboard(int level);
    void hideScoreBoard();
    
    void showMessageboard();
    void hideMessageboard();
    
    void showAskMessageboard();
    void hideAskMessageboard();
    
    void connection();
    
    void update(CCNode*);
    
    bool isMessageBoard = false;    
private:
    CCLayerColor* createUserNode(int place, string avatarFileName, string name, int score, string fid);
    CCNode* createScoreLayer(int level);
    
    CCNode* createMessageboard();
    CCNode* createMessage(string fromId, string from, NotificationType notif, int idxRequest);
    
    CCNode* createMessageboardForAsk();
    CCNode* createAskMessage(string fromId, string from, int idxRequest);
    
    void scrollTo(int place);
    
    //callbacks
    void sendLifeCallback(CCObject* pSender);
    void meCallback(CCObject* pSender);
    void numOneCallback(CCObject* pSender);
    void facebookCallback(CCObject* pSender);
    void scoreboardDelete(CCNode* node);
    void messageboardDelete(CCNode* node);
    
    void checkCallback(CCObject* pSender);
    void acceptCallback(CCObject* pSender);
    void closeMessageboardCallback(CCObject* pSender);
    void messageCallback(CCObject* pSender);
    //////////
    
    bool messageAsk = false;
    
    bool lock;
    float userNodeSize;
    float multiplier;
    int countUserIsScoreboard;
    int userPosition;
    ccColor3B color;
    
    CCNode* scoreboard;
    CCNode* messageboard;
    
    CCMenuItemSprite* checkAll;
    vector<CCMenuItemSprite*> messagesGreen;
    bool flagCheckAll = false;
    
    CCScrollView* scrollView;
    CCSprite* connectionPlate;
    
    vector<CCMenuItemSprite*> sendButtons;
    
    bool isConnection = false;
    
    float timeDt = 0.0f;
    
    vector<ScoreboardCell> users;
    
    int currentLevel = 0;
};

#endif