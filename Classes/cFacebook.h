//
//  cFacebook.h
//  IceCreamAdventure
//
//  Created by Nelar on 01.11.13.
//
//

#ifndef __IceCreamAdventure__cFacebook__
#define __IceCreamAdventure__cFacebook__

#include <iostream>
#include "cSingleton.h"
#include <string>
#include <cocos2d.h>
#include "NotificationTypes.h"

using namespace std;
using namespace cocos2d;

struct Leaderboard
{
    string name;
    string uid;
    bool accept = false;
    vector<int> scores;
};

struct GiftMessage
{
    string from;
    string name;
    NotificationType notif;
    string id;
    bool accept;
};


class cFacebook: public cSingleton<cFacebook>
{
public:
    cFacebook();
    ~cFacebook(){};
    
    void login();
    void getUserData();
    string& getAvatar();
    
    void processIncomingURL(char * targetURL);
    void processIncomingRequest(char *  targetURL);
    void processIncomingFeed(char *  targetURL);
    
    void sendRequest(string message, string title, string jsonData, vector<string> to, bool onlyMobileUsers = false);
    void inviteFriends();
    
    void requestWritePermissions();
    void sendScore(const int nScore);
    void getScores();
    void getFriendScores();
    void getFriendScore(const char *uid, const char* name);
    void sendOG();
    void sendRequest(const int nScore);
    void sendFilteredRequest(const int nScore);
    void sendBrag(const int nScore);
    
    string& getWorkDirectory();
    
    void completeLevel(const int nScore);
    void boughtItem(string item);
    void completeStage(string item);
    void completeTutorial(string item);
    void setPointsInLevel(int points, int level);
    void unlockNewLevel(int level);
    void requestHelp(string name);
    
    bool sessionIsOpened();
    
    bool beginGamePost();
    bool endTutorial();
    bool unlockStage();
    bool jubileePost();
    bool endStage(string stage);
    
    void askLife(string fromId);
    vector<int> myScores;
    vector<Leaderboard> friendsScores;
    string name;
    unsigned long long fbid;
    
    void sendLife(string fromId);
    
    void checkNotification();
    
    bool isLoad = false;
    bool isLife = false;
    bool isBooster = false;
    
    bool isLogin = false;
    
    vector<GiftMessage> messages;
    void checkMessages();
    void acceptMessage();
private:
    
    void createFBSession();
    
    void onNotificationRecieved(const char *, const char *, const char *);
    
    string avatarFileName;
    bool createAvatarFromFBID(unsigned long long uIDs);
    bool createAvatarFromFBIDForFriend(string fuid);
    int scoresSize;
    
    string workingDirectory;
    bool bHaveRequestedPublishPermissions = false;
};


#define FacebookPtr	cFacebook::GetSingletonPtr()

#endif /* defined(__IceCreamAdventure__cFacebook__) */
