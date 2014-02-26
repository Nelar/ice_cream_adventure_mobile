//
//  MMPInterface_ios.cpp
//  angrypetsmobile
//
//  Created by Nikolay Ischuk on 22.10.13.
//
//

#include "MMPInterface.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#import <Foundation/Foundation.h>
#import <ifaddrs.h>
#import <arpa/inet.h>
#import "Reachability.h"

#include <sys/sysctl.h>
#include <sys/utsname.h>

/*#include "GAI.h"
#include "GAITracker.h"
#include "GAIDictionaryBuilder.h"*/

namespace Core
{
    std::string MMPInterface::GetIsFirstTimeRunning()
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
    
    std::string MMPInterface::GetSystemVersion()
    {
        NSString *currSysVer = [[UIDevice currentDevice] systemVersion];
        return [currSysVer UTF8String];
    }
    
    void MMPInterface::gaiEvent(std::string category, std::string action)
    {
       /* id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];
        [tracker send:[[GAIDictionaryBuilder createEventWithCategory:[NSString stringWithUTF8String:category.c_str()]
                                                              action:[NSString stringWithUTF8String:action.c_str()]
                                                               label:nil
                                                               value:nil] build]];*/
    }
    
    void MMPInterface::gaiEvent(std::string category, std::string action, std::map<std::string, std::string>& userData)
    {
        int sizeUserData = userData.size();
        NSMutableDictionary* dict = [[NSMutableDictionary alloc] initWithCapacity:sizeUserData];
        
        for( std::map<std::string, std::string>::const_iterator it = userData.begin(), end = userData.end(); it != end; it++)
        {
            [dict setObject:[NSString stringWithUTF8String:it->first.c_str()] forKey:[NSString stringWithUTF8String:it->second.c_str()]];
        }
        /*
        id<GAITracker> tracker = [[GAI sharedInstance] defaultTracker];
        GAIDictionaryBuilder* dictBuilder = [GAIDictionaryBuilder createEventWithCategory:[NSString stringWithUTF8String:category.c_str()]
                                                                                    action:[NSString stringWithUTF8String:action.c_str()]
                                                                                     label:nil
                                                                                     value:nil];
        [dictBuilder setAll:dict];
        
        [tracker send: [dictBuilder build]];*/
    }
    
    std::string MMPInterface::GetDevice()
    {
        NSLog(@"[UIDevice currentDevice].model: %@",[UIDevice currentDevice].model);
        NSLog(@"[UIDevice currentDevice].description: %@",[UIDevice currentDevice].description);
        NSLog(@"[UIDevice currentDevice].localizedModel: %@",[UIDevice currentDevice].localizedModel);
        NSLog(@"[UIDevice currentDevice].name: %@",[UIDevice currentDevice].name);
        NSLog(@"[UIDevice currentDevice].systemVersion: %@",[UIDevice currentDevice].systemVersion);
        NSLog(@"[UIDevice currentDevice].systemName: %@",[UIDevice currentDevice].systemName);
        NSLog(@"[UIDevice currentDevice].batteryLevel: %f",[UIDevice currentDevice].batteryLevel);
        struct utsname systemInfo;
        uname(&systemInfo);
        NSLog(@"[NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding]: %@",[NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding]);
        NSString *platform = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
        
        return [platform UTF8String];
    }
    
    std::string MMPInterface::GetIP()
    {
        NSString *address = @"error";
        struct ifaddrs *interfaces = NULL;
        struct ifaddrs *temp_addr = NULL;
        int success = 0;
        // retrieve the current interfaces - returns 0 on success
        success = getifaddrs(&interfaces);
        if (success == 0) {
            // Loop through linked list of interfaces
            temp_addr = interfaces;
            while(temp_addr != NULL) {
                if(temp_addr->ifa_addr->sa_family == AF_INET) {
                    // Check if interface is en0 which is the wifi connection on the iPhone
                    if([[NSString stringWithUTF8String:temp_addr->ifa_name] isEqualToString:@"en0"]) {
                        // Get NSString from C String
                        address = [NSString stringWithUTF8String:inet_ntoa(((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr)];
                    }
                }
                temp_addr = temp_addr->ifa_next;
            }
        }
        // Free memory
        freeifaddrs(interfaces);
        return [address UTF8String];
    }
    
    std::string MMPInterface::GetNet()
    {
        Reachability *reachability = [Reachability reachabilityForInternetConnection];
        [reachability startNotifier];
        
        NetworkStatus status = [reachability currentReachabilityStatus];
        
        if(status == NotReachable)
        {
            return "no";
        }
        else if (status == ReachableViaWiFi)
        {
            return "wifi";
        }
        else if (status == ReachableViaWWAN) 
        {
            return "wwan";
        }
        
        return "no";
    }
    
    std::string MMPInterface::GetIsPushEnabled()
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
    
    std::string MMPInterface::GetIsJailbroken()
    {
        NSString *filePath = @"/Applications/Cydia.app";
        if ([[NSFileManager defaultManager] fileExistsAtPath:filePath])
        {
            return "true";
        }
        else
        {
            return "false";
        }
    }

}

#endif