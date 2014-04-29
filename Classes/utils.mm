//
//  utils.cpp
//  IceCream
//
//  Created by Nelar on 30.10.13.
//
//

#include "utils.h"
#import <AdSupport/ASIdentifierManager.h>
#import "Reachability.h"

#import "Reachability.h"
#include "cocos2d.h"
#include "CCLocalizedString.h"
#import <ifaddrs.h>
#import <arpa/inet.h>
#import "Reachability.h"
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <CoreTelephony/CTCarrier.h>

#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <string>

#import "AppsFlyerTracker.h"
#import <AdSupport/AdSupport.h>
#import <GameKit/GameKit.h>
#include "cocos2d.h"
#include <MainMenuScene.h>


using namespace cocos2d;
using namespace std;

void appsFlyerTrackEvent(const char* event, const char* value)
{
    //[[AppsFlyerTracker sharedTracker] trackEvent:[NSString stringWithUTF8String:event] withValue:[NSString stringWithUTF8String:value]];
}
    
const char* device()
{
    struct utsname systemInfo;
    uname(&systemInfo);
    NSLog(@"[NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding]: %@",[NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding]);
    NSString *platform = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    return [platform UTF8String];
}

const char* iOSversion()
{
    NSString *version = [[UIDevice currentDevice] systemVersion];
    return [version UTF8String];
}

const char* version()
{
    NSString *versionString = [[NSBundle mainBundle] objectForInfoDictionaryKey:(NSString*)kCFBundleVersionKey];
    return [versionString UTF8String];
}

const char* idfa()
{
    NSString *idfaString = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
    return [idfaString UTF8String];
}

const char*  limitAdTracking()
{
    ASIdentifierManager *adIdentManager = [ASIdentifierManager sharedManager];
    if (adIdentManager.advertisingTrackingEnabled)
        return "0";
    else
        return "1";
    return "0";
}

const char* carrier()
{
    CTTelephonyNetworkInfo *phoneInfo = [[CTTelephonyNetworkInfo alloc] init];
    CTCarrier *phoneCarrier = [phoneInfo subscriberCellularProvider];
    NSLog(@"Carrier = %@", [phoneCarrier carrierName]);
    if (phoneCarrier == nil)
        return "";
    else
        return [[phoneCarrier carrierName] UTF8String];
}

const char* idfv()
{
    NSString *idfvString =  [[[UIDevice currentDevice] identifierForVendor] UUIDString];
    return [idfvString UTF8String];
}

void goToLink(const char* link)
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:link]]];
    appsFlyerTrackEvent("install_link", link);
}

void sendlocalNotification(float second)
{
    UILocalNotification *notification = [[UILocalNotification alloc] init];
    notification.fireDate = [[NSDate date] dateByAddingTimeInterval:second];
    notification.alertBody = [NSString stringWithUTF8String:CCLocalizedString("SETTING_NOTIF_TEXT", NULL)];
    [[UIApplication sharedApplication] scheduleLocalNotification:notification];
}

void removeAllNotification()
{
    UIApplication* app = [UIApplication sharedApplication];
    NSArray *notifications = [app scheduledLocalNotifications];
    if ([notifications count] > 0)
        [app presentLocalNotificationNow:notifications[0]];
}

const char* getCountry()
{
    NSLocale *currentLocale = [NSLocale currentLocale];
    NSString *countryCode = [currentLocale objectForKey:NSLocaleCountryCode];
    NSString *countryCodeLow = [countryCode lowercaseString];
    return [countryCodeLow UTF8String];
}

bool getNetworkStatus()
{
    /*Reachability* reachability = [Reachability reachabilityForInternetConnection];
    NetworkStatus remoteHostStatus = [reachability currentReachabilityStatus];
    
    if(remoteHostStatus == NotReachable)
        return false;
    else if (remoteHostStatus == ReachableViaWiFi)
    {
        return true;
    }
    else if (remoteHostStatus == ReachableViaWWAN)
    {
        return true;
    }
    
    return false;*/
    const char *host_name = "www.google.com";
    BOOL _isDataSourceAvailable = NO;
    Boolean success;
    SCNetworkReachabilityRef reachability = SCNetworkReachabilityCreateWithName(NULL,host_name);
    SCNetworkReachabilityFlags flags;
    success = SCNetworkReachabilityGetFlags(reachability, &flags);
    _isDataSourceAvailable = success &&
    (flags & kSCNetworkFlagsReachable) &&
    !(flags & kSCNetworkFlagsConnectionRequired);
    
    CFRelease(reachability);
    return _isDataSourceAvailable;
}

const char* networkStatus()
{
    Reachability* reachability = [Reachability reachabilityForInternetConnection];
    NetworkStatus remoteHostStatus = [reachability currentReachabilityStatus];
    
    if(remoteHostStatus == NotReachable)
        return "no";
    else if (remoteHostStatus == ReachableViaWiFi)
        return "wifi";
    else if (remoteHostStatus == ReachableViaWWAN)
        return "wwan";
    
    return "no";
}

void alertNetwork()
{
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"" message:[NSString stringWithUTF8String:CCLocalizedString("NETWORK_MESSAGE", NULL)]
                                                   delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}


std::string push_enabled()
{
    UIRemoteNotificationType types = [[UIApplication sharedApplication] enabledRemoteNotificationTypes];
    if (types == UIRemoteNotificationTypeNone)
    {
        return "false";
    }
    else
    {
        return "true";
    }
}

std::string getIsFirstTimeRunning()
{
    if ([[NSUserDefaults standardUserDefaults] boolForKey:@"HasLaunchedOnce"])
    {
        return "false";
    }
    else
    {
        [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"HasLaunchedOnce"];
        [[NSUserDefaults standardUserDefaults] synchronize];
        return "true";
    }
}

bool isLoginGC()
{
    return [GKLocalPlayer localPlayer].isAuthenticated;
}

void loginGC()
{
    [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:^(NSError *error){
        if (error ==nil) {
            NSLog(@"GameCenter Success");
            CCUserDefault::sharedUserDefault()->setBoolForKey("gameCenterLogin", true);
            CCUserDefault::sharedUserDefault()->flush();
            for (int i = 0; i < 84; i++)
            {
                char buf[255];
                sprintf(buf, "level_%d", i+1);
                GKLeaderboard *board = [[[GKLeaderboard alloc] initWithPlayerIDs:[NSArray arrayWithObject:[GKLocalPlayer localPlayer].playerID]] autorelease];
                board.category = [NSString stringWithUTF8String:buf];
//                board.identifier = [NSString stringWithUTF8String:buf];
                board.timeScope   = GKLeaderboardTimeScopeAllTime;
                board.playerScope = GKLeaderboardPlayerScopeFriendsOnly;
                board.range       = NSMakeRange(1, 1);
                [board loadScoresWithCompletionHandler:^(NSArray *scores, NSError *error){
                    if(error != nil){
                        NSLog(@"Score not loaded");
                        if (GlobalsPtr->iceCreamScene == Loading)
                            CCDirector::sharedDirector()->replaceScene(MainMenuScene::scene());
                        else if (GlobalsPtr->iceCreamScene == Menu)
                        {
                            MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                            layer->closeLoading();
                        }
                    } else {
                        NSLog(@"Score loaded");
                        char buf[255];
                        sprintf(buf, "level_%d", i+1);
                        
                        if (board.localPlayerScore.value < OptionsPtr->getLevelData(i).countScore)
                        {
                            GKScore *myScoreValue = [[[GKScore alloc] initWithCategory:[NSString stringWithUTF8String:buf]] autorelease];
                            myScoreValue.value = OptionsPtr->getLevelData(i).countScore;
                            myScoreValue.context = OptionsPtr->getLevelData(i).countStar;
                            CCLOG("%s score %d star %d", buf, OptionsPtr->getLevelData(i).countScore, OptionsPtr->getLevelData(i).countStar);
                            [myScoreValue reportScoreWithCompletionHandler:^(NSError *error){
                                if(error != nil){
                                    NSLog(@"Score Submission Failed");
                                } else {
                                    NSLog(@"Score Submitted");
                                }
                            }];
                        }
                        else
                        {
                            CCLOG("%s score %llu star %llu", buf, board.localPlayerScore.value, board.localPlayerScore.context);
                            OptionsPtr->setLevelData(i, board.localPlayerScore.context, board.localPlayerScore.value, OptionsPtr->getLevelData(i).levelType);
                            if (OptionsPtr->getLevelData(i).countScore > 0)
                                if (OptionsPtr->getCurrentLevel() < i +2)
                                    OptionsPtr->restoreCurrentLevel(i + 2);
                        }
                        
                        if (i == 83)
                        {
                            OptionsPtr->save();
                            if (GlobalsPtr->iceCreamScene == Loading)
                                CCDirector::sharedDirector()->replaceScene(MainMenuScene::scene());
                            else if (GlobalsPtr->iceCreamScene == Menu)
                            {
                                MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                                layer->gameCenterButtonHide();
                                layer->closeLoading();
                            }
                        }
                    }
                }];
                
            }
            OptionsPtr->save();

        } else {
            NSLog(@"GameCenter Fail %@", [error localizedDescription]);
            if (GlobalsPtr->iceCreamScene == Loading)
            {
                CCDirector::sharedDirector()->replaceScene(MainMenuScene::scene());
            }
            else if (GlobalsPtr->iceCreamScene == Menu)
            {
                MainMenuScene* layer = ((MainMenuScene*)CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(0));
                layer->closeLoading();
            }
                
        }
        
    }];
}

void getLevelDataFromGC()
{
    if (!getNetworkStatus())
        return;
    
    if (!isLoginGC())
        return;
    for (int i = 0; i < 84; i++)
    {
        char buf[255];
        sprintf(buf, "level_%d", i+1);
        GKLeaderboard *board = [[[GKLeaderboard alloc] initWithPlayerIDs:[NSArray arrayWithObject:[GKLocalPlayer localPlayer].playerID]] autorelease];
        board.category = [NSString stringWithUTF8String:buf];
//        board.identifier = [NSString stringWithUTF8String:buf];
        board.timeScope   = GKLeaderboardTimeScopeAllTime;
        board.playerScope = GKLeaderboardPlayerScopeFriendsOnly;
        board.range       = NSMakeRange(1, 1);
        [board loadScoresWithCompletionHandler:^(NSArray *scores, NSError *error){
            if(error != nil){
                NSLog(@"Score not loaded");
            } else {
                NSLog(@"Score loaded");
                char buf[255];
                sprintf(buf, "level_%d", i+1);

                if (board.localPlayerScore.value < OptionsPtr->getLevelData(i).countScore)
                {
                    GKScore *myScoreValue = [[[GKScore alloc] initWithCategory:[NSString stringWithUTF8String:buf]] autorelease];
                    myScoreValue.value = OptionsPtr->getLevelData(i).countScore;
                    myScoreValue.context = OptionsPtr->getLevelData(i).countStar;
                    CCLOG("%s score %d star %d", buf, OptionsPtr->getLevelData(i).countScore, OptionsPtr->getLevelData(i).countStar);
                    [myScoreValue reportScoreWithCompletionHandler:^(NSError *error){
                        if(error != nil){
                            NSLog(@"Score Submission Failed");
                        } else {
                            NSLog(@"Score Submitted");
                        }
                    }];
                }
                else
                {
                    CCLOG("%s score %llu star %llu", buf, board.localPlayerScore.value, board.localPlayerScore.context);
                    OptionsPtr->setLevelData(i, board.localPlayerScore.context, board.localPlayerScore.value, OptionsPtr->getLevelData(i).levelType);
                    if (OptionsPtr->getLevelData(i).countScore > 0)
                        if (OptionsPtr->getCurrentLevel() < i +2)
                            OptionsPtr->restoreCurrentLevel(i + 2);
                }
                
                if (i == 83)
                    OptionsPtr->save();
            }
        }];
    }
    OptionsPtr->save();
}

void submitLevelDataToGC()
{
}
