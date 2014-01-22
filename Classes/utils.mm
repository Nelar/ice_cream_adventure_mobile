//
//  utils.cpp
//  IceCream
//
//  Created by Nelar on 30.10.13.
//
//

#include "utils.h"
#import <AdSupport/ASIdentifierManager.h>


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
