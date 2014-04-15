#ifndef __MOREGAMES_SCENE_H__
#define __MOREGAMES_SCENE_H__

#include "cocos2d.h"
#include <vector>
#include <GUI/CCScrollView/CCScrollView.h>
#include "NotificationTypes.h"

using namespace std;
using namespace cocos2d;
using namespace extension;

struct sMoreGames
{
    sMoreGames():
    rating(0), name(""), url(""), price(), stars(0), genre(""), compatibility(""), iconUrl(""), description("") {};
    
    sMoreGames(int nrating, string nname, string nurl, string nprice, int nstars, string ngenre, string ncompatibility, string niconUrl, string ndescription):
    rating(nrating), name(nname), url(nurl), price(nprice), stars(nstars), genre(ngenre), compatibility(ncompatibility), iconUrl(niconUrl), description(ndescription) {};
    
    int rating;
    string name;
    string url;
    string price;
    string description;
    int stars;
    string genre;
    string compatibility;
    string iconUrl;
    CCSprite* sprite;
};

class MoreGamesLayer : public cocos2d::CCLayer
{
public:
    
    virtual ~MoreGamesLayer();
	virtual bool init();
    
	CREATE_FUNC(MoreGamesLayer);
    
    virtual void changeOrientation();
    
    void showMessageboard();
    void hideMessageboard();
    
    void deleteMessageboard(CCNode* node);
    
private:
    
    CCNode* createMessageboard();
    CCNode* createMessage(sMoreGames game);
    
    void closeMessageboardCallback(CCObject* pSender);
    void messageCallback(CCObject* pSender);
    
    CCNode* messageboard;
    CCLabelTTF* title;
    CCLabelTTF* copyright;
    CCMenu* menu;
    CCScrollView* scrollView;
    
    bool isMessageBoard;
    bool lock;
    float userNodeSize;
    float multiplier;
};

#endif