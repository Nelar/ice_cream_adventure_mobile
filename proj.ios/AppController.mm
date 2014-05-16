//
//  FourPicsOneWordAppController.mm
//  FourPicsOneWord
//
//  Created by Philipp Mayevski on 7/16/13.
//  Copyright __MyCompanyName__ 2013. All rights reserved.
//
#import <UIKit/UIKit.h>
#import "AppController.h"
#import "cocos2d.h"
#import "EAGLView.h"
#import "AppDelegate.h"
#import "RootViewController.h"
#import <FacebookSDK/FacebookSDK.h>
#import "cFacebook.h"

#include "SimpleAudioEngine.h"
//#include "GAI.h"
#import <Parse/Parse.h>
#import <Tapjoy/Tapjoy.h>
#import "AppsFlyerTracker.h"
#import <AdSupport/AdSupport.h>
#import <GameKit/GameKit.h>

using namespace CocosDenshion;

@implementation AppController
@synthesize window;
@synthesize viewController;

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    // Override point for customization after application launch.
    
    [Parse setApplicationId:@"tz3896PMwkJXsj0CMl3wVcgvCzqG1ozzikCXUvU7"
                  clientKey:@"q8ql52jER8QtmKJtlDpN67wQEX9FknqyqY3RSgZP"];
    
    [PFFacebookUtils initializeFacebook];
    
    [PFUser enableAutomaticUser];
    
    PFACL *defaultACL = [PFACL ACL];
    [defaultACL setPublicReadAccess:YES];
    [defaultACL setPublicWriteAccess:YES];
    
    [PFACL setDefaultACL:defaultACL withAccessForCurrentUser:YES];

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    EAGLView *__glView = [EAGLView viewWithFrame: [window bounds]
                                     pixelFormat: kEAGLColorFormatRGB565
                                     depthFormat: GL_DEPTH24_STENCIL8_OES
                              preserveBackbuffer: NO
                                      sharegroup: nil
                                   multiSampling: NO
                                 numberOfSamples: 0];
    [__glView setMultipleTouchEnabled:YES];

    // Use RootViewController manage EAGLView
    viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    viewController.wantsFullScreenLayout = YES;
    viewController.view = __glView;

    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:viewController];
    }
    
    [window makeKeyAndVisible];

    [[UIApplication sharedApplication] setStatusBarHidden: YES];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    self.appUsageCheckEnabled = YES;
    
    // Optional: automatically send uncaught exceptions to Google Analytics.
/*    [GAI sharedInstance].trackUncaughtExceptions = YES;
    
    // Optional: set Google Analytics dispatch interval to e.g. 20 seconds.
    [GAI sharedInstance].dispatchInterval = 20;
    
    // Optional: set Logger to VERBOSE for debug information.
    [[[GAI sharedInstance] logger] setLogLevel:kGAILogLevelVerbose];
    
    // Initialize tracker.
    id<GAITracker> tracker = [[GAI sharedInstance] trackerWithTrackingId:@"UA-44781394-4"];
*/
    [Tapjoy requestTapjoyConnect:@"9c1347c8-9037-465e-80aa-7f0ca2132e5a" secretKey:@"vA2FgPlWDpuT8E46WOZR"];
    
    cocos2d::CCApplication::sharedApplication()->run();
    
    if (application.applicationState != UIApplicationStateBackground) {
        BOOL preBackgroundPush = ![application respondsToSelector:@selector(backgroundRefreshStatus)];
        BOOL oldPushHandlerOnly = ![self respondsToSelector:@selector(application:didReceiveRemoteNotification:fetchCompletionHandler:)];
        BOOL noPushPayload = ![launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
        if (preBackgroundPush || oldPushHandlerOnly || noPushPayload) {
            [PFAnalytics trackAppOpenedWithLaunchOptions:launchOptions];
        }
    }
    
    [application registerForRemoteNotificationTypes:UIRemoteNotificationTypeBadge|
     UIRemoteNotificationTypeAlert|
     UIRemoteNotificationTypeSound];
    
    application.applicationIconBadgeNumber = 0;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(orientationDidChanged:) name:@"UIDeviceOrientationDidChangeNotification" object:nil];
    
    self.appUsageCheckEnabled = YES;
    if ([defaults objectForKey:@"AppUsageCheck"]) {
        self.appUsageCheckEnabled = [defaults boolForKey:@"AppUsageCheck"];
    }
    
    if (launchOptions)
    {
        NSDictionary *userInfo = launchOptions[UIApplicationLaunchOptionsRemoteNotificationKey];
        
        NSString *notif = [userInfo objectForKey:@"notif"];
        NSString *fbid = [userInfo objectForKey:@"fb"];
        int a = 0;
    }
    
    // Replace App_ID & Dev_Key with your values
    // App_ID = Your iTunes App ID, e.g. 34320065
    // Dev_Key = Your unique developer ID, which is accessible from your account, e.g. R5AKpQjiER6LzSQ6iX
    // For example:
    
    [AppsFlyerTracker sharedTracker].appsFlyerDevKey = @"YJq3qzRGdkR6ZQZHmkozQn";
    [AppsFlyerTracker sharedTracker].appleAppID = @"705058125";
        
    return YES;
}

- (void) onConversionDataRequestFailure:(NSError *)error
{
    int a;
}

- (BOOL) checkAppUsageTrigger {
    // Initialize the app active count
    NSInteger appActiveCount = 0;
    // Read the stored value of the counter, if it exists
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    if ([defaults objectForKey:@"AppUsedCounter"]) {
        appActiveCount = [defaults integerForKey:@"AppUsedCounter"];
    }
    
    // Increment the counter
    appActiveCount++;
    BOOL trigger = NO;
    // Only trigger the prompt if the facebook session is valid and
    // the counter is greater than a certain value, 3 in this sample
    if ([FBSession activeSession].isOpen && (appActiveCount = 3)) {
        trigger = YES;
        appActiveCount = 0;
    }
    // Save the updated counter
    [defaults setInteger:appActiveCount forKey:@"AppUsedCounter"];
    [defaults synchronize];
    return trigger;
}

-(void) orientationDidChanged:(NSNotification*)notification
{
    CGSize s;
    if (UIInterfaceOrientationIsLandscape(UIApplication.sharedApplication.statusBarOrientation)) {
        s = CGSizeMake(std::max<float>(UIScreen.mainScreen.bounds.size.width, UIScreen.mainScreen.bounds.size.height), std::min<float>(UIScreen.mainScreen.bounds.size.width, UIScreen.mainScreen.bounds.size.height));
    } else {
        s = CGSizeMake(std::min<float>(UIScreen.mainScreen.bounds.size.width, UIScreen.mainScreen.bounds.size.height), std::max<float>(UIScreen.mainScreen.bounds.size.width, UIScreen.mainScreen.bounds.size.height));
    }
    
    float scale = [UIScreen mainScreen].scale;
    
    cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
    director->getOpenGLView()->setFrameSize(s.width*scale, s.height*scale);
    director->getOpenGLView()->setDesignResolutionSize(s.width*scale, s.height*scale, kResolutionShowAll);
    ((AppDelegate*)cocos2d::CCApplication::sharedApplication())->applicationWillChangeOrientation();
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)newDeviceToken {
    PFInstallation *currentInstallation = [PFInstallation currentInstallation];
    [currentInstallation setDeviceTokenFromData:newDeviceToken];
    [currentInstallation saveInBackground];
    
    [PFPush storeDeviceToken:newDeviceToken];
    [PFPush subscribeToChannelInBackground:@"" target:self selector:@selector(subscribeFinished:error:)];
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
    if (error.code == 3010) {
        NSLog(@"Push notifications are not supported in the iOS Simulator.");
    } else {
        // show some alert or otherwise handle the failure to register.
        NSLog(@"application:didFailToRegisterForRemoteNotificationsWithError: %@", error);
	}
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
    [PFPush handlePush:userInfo];
    if (application.applicationState == UIApplicationStateInactive) {
        [PFAnalytics trackAppOpenedWithRemoteNotificationPayload:userInfo];
    }
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo fetchCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler {
    if (application.applicationState == UIApplicationStateInactive) {
        [PFAnalytics trackAppOpenedWithRemoteNotificationPayload:userInfo];
    }
    
    if (userInfo)
    {
        NSString *notif = [userInfo objectForKey:@"notif"];
        NSString *fbid = [userInfo objectForKey:@"fb"];
        int a = 0;
    }
}


- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    
    cocos2d::CCDirector::sharedDirector()->pause();
    SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
    [[AppsFlyerTracker sharedTracker] trackAppLaunch];
    [FBAppCall handleDidBecomeActive];
    
    cocos2d::CCDirector::sharedDirector()->resume();
    SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
  /*  PFQuery *pushQuery = [PFInstallation query];
    [pushQuery whereKey:@"deviceType" equalTo:@"ios"];
    
    PFPush* push = [PFPush push];
    [push setQuery:pushQuery];
    [push expireAfterTimeInterval:(60*60*48)];
    [push setMessage:@"Two days"];
    [push sendPushInBackground];

    */
    
    cocos2d::CCApplication::sharedApplication()->applicationDidEnterBackground();
    SimpleAudioEngine::sharedEngine()->pauseAllEffects();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    application.applicationIconBadgeNumber = 0;
    cocos2d::CCApplication::sharedApplication()->applicationWillEnterForeground();
    SimpleAudioEngine::sharedEngine()->resumeAllEffects();
}

- (void)applicationWillTerminate:(UIApplication *)application {
    [[FBSession activeSession] close];
}


#pragma mark -
#pragma mark Facebook integration

-(BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
    return [PFFacebookUtils handleOpenURL:url];
}

#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */
     cocos2d::CCDirector::sharedDirector()->purgeCachedData();
}



- (void)dealloc {
    [super dealloc];
}


@end

