//
//  nMMP.h
//  IceCreamAdventure
//
//  Created by Nelar on 14.03.14.
//
//

#ifndef __IceCreamAdventure__nMMP__
#define __IceCreamAdventure__nMMP__

#include <iostream>
#include "FileSystem.h"
#include "cSingleton.h"
#include "cocos2d.h"
#include <HttpClient.h>
#include <vector>

using namespace std;
using namespace cocos2d;
using namespace extension;

class MMP : public cSingleton<MMP>, public CCObject
{
public:
    MMP(){};
    ~MMP(){};
    
    void mmpTracking();
    void startSession();
    void endedSession();
    void sessionLoaded();
    void userIdentified(string fbId);
    void purchaseStarted();
    void purchaseCanceled();
    void purchaseConfirmed();
    void purchaseMade(string sku, string currency, float price, bool is_valid);
    void tutorialStepStarted(int num, string name);
    void tutorialStepCompleted(int num, string name);
    void levelStarted(int num, string name);
    void levelCompleted(int num, string name);
    void facebookConnected(string fbId, string name);
    void contentShared();
    void friendsInvited();
private:
    void mmpAnalytics(string json);
    void trackingServerResponse(CCHttpClient * client, CCHttpResponse * response);
    void analyticsServerResponse(CCHttpClient * client, CCHttpResponse * response);
    string meta();
    time_t startTime;
};

#define MMPPtr	MMP::GetSingletonPtr()

#endif /* defined(__IceCreamAdventure__nMMP__) */
