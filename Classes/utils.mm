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
void goToLink(const char* link)
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:link]]];
}

void sendlocalNotification(float second)
{
    NSDate *setTime = [NSDate dateWithTimeIntervalSinceNow:((NSTimeInterval)second)];
        
    UILocalNotification *localNotif = [[UILocalNotification alloc] init];
    if (localNotif == nil)
        return;
    
    localNotif.fireDate = setTime;
    localNotif.timeZone = [NSTimeZone defaultTimeZone];
    
    localNotif.alertBody = @"Lives is full!";
    localNotif.alertAction = @"View";
    
    localNotif.soundName = UILocalNotificationDefaultSoundName;
    localNotif.applicationIconBadgeNumber = 1;
    
    [[UIApplication sharedApplication] scheduleLocalNotification:localNotif];
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
    Reachability* reachability = [Reachability reachabilityForInternetConnection];
    NetworkStatus remoteHostStatus = [reachability currentReachabilityStatus];
    
    if(remoteHostStatus == NotReachable)
        return false;
    else if (remoteHostStatus == ReachableViaWiFi || remoteHostStatus == ReachableViaWWAN)
        return true;
    
    return false;
}

void alertNetwork()
{
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"" message:[NSString stringWithUTF8String:CCLocalizedString("NETWORK_MESSAGE", NULL)]
                                                   delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil];
    [alert show];
    [alert release];
}
